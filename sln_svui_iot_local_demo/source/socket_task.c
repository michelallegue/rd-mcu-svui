/*
 * Copyright 2022-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if ENABLE_WIFI_AUDIO_DUMP
#include "socket_task.h"

#include "lwip/sys.h"
#include "lwip/netif.h"
#include "sln_mic_config.h"
#include "local_sounds_task.h"

#define TCP_SERVER_CONNECTIONS_MAX  2

#if DUMP_ALL_STREAMS
#define AUDIO_BUFFER_OUTPUT_SIZE \
    ((PCM_SAMPLE_COUNT * PCM_SAMPLE_SIZE_BYTES) + (PCM_SINGLE_CH_SMPL_COUNT * PCM_SAMPLE_SIZE_BYTES * 2))
#define DUMP_QUEUE_SLOTS 5
SDK_ALIGN(static int16_t __attribute__((section(".bss.$SRAM_OC_NON_CACHEABLE")))
          s_audioStream[PCM_SINGLE_CH_SMPL_COUNT * (SLN_MIC_COUNT + 2)],
          8);
#else
#define AUDIO_BUFFER_OUTPUT_SIZE (PCM_SINGLE_CH_SMPL_COUNT * PCM_SAMPLE_SIZE_BYTES)
#define DUMP_QUEUE_SLOTS 25
SDK_ALIGN(static int16_t __attribute__((section(".bss.$SRAM_OC_NON_CACHEABLE")))
          s_audioStream[PCM_SINGLE_CH_SMPL_COUNT],
          8);
#endif /* DUMP_ALL_STREAMS */

typedef struct
{
    int sck;
    int sck_accepted[TCP_SERVER_CONNECTIONS_MAX];
    int af;
    int sck_type;
    int is_server;
    struct sockaddr_in ipv4;
    struct sockaddr_in6 ipv6;
} example_sockinfo_t;

volatile int run;
static example_sockinfo_t s_sockinfo;

static struct sockaddr_storage sender_addr;
static socklen_t sender_addr_len  = sizeof(sender_addr);

static bool s_startUDPForward     = false;
static bool s_startTCPForward     = false;
static int s_sck                  = 0;

QueueHandle_t g_xAudioDumpQueue        = NULL;
TaskHandle_t  audioDumpWiFiTaskHandle = NULL;

extern volatile long unsigned int g_processedFrames;

static int ip_port_str_to_sockaddr(const char *ip_str,
                                   const char *port_str,
                                   struct sockaddr_in *ipv4,
                                   struct sockaddr_in6 *ipv6)
{
    int ret;
    int af;
    int port;

    /* Convert port */
    port = atoi(port_str);
    if (port > 0xffff)
    {
        configPRINTF(("Port '%s' is not lower than 65536\r\n", port_str));
        return -1;
    }
    if (port <= 0)
    {
        configPRINTF(("Port '%s' is not greater than 0\r\n", port_str));
        return -1;
    }

    /* Convert IP */
    af = AF_INET;
    memset(ipv4, 0, sizeof(struct sockaddr_in));
    ipv4->sin_len    = sizeof(struct sockaddr_in);
    ipv4->sin_family = af;
    ipv4->sin_port   = htons(port);
    ret              = inet_pton(af, ip_str, &ipv4->sin_addr.s_addr);
    if (ret != 1)
    {
        /* Address is not valid IPv4 address. Lets try treat it as IPv6 */

        af = AF_INET6;
        memset(ipv6, 0, sizeof(struct sockaddr_in6));
        ipv6->sin6_len      = sizeof(struct sockaddr_in6);
        ipv6->sin6_family   = af;
        ipv6->sin6_port     = htons(port);
        ipv6->sin6_scope_id = netif_get_index(netif_default);

        LOCK_TCPIP_CORE();
        ret = inet_pton(af, ip_str, &ipv6->sin6_addr.s6_addr);
        UNLOCK_TCPIP_CORE();

        if (ret != 1)
        {
            configPRINTF(("'%s' is not valid IPv4 nor IPv6 address.\r\n", ip_str));
            return -1;
        }

#if LWIP_IPV6_SCOPES == 1
        /* IPv6 string can contain scope id, check if any netif matches */
        ip6_addr_t addr;
        LOCK_TCPIP_CORE();
        ip6addr_aton(ip_str, &addr);
        UNLOCK_TCPIP_CORE();
        if (addr.zone > 0)
        {
            ipv6->sin6_scope_id = addr.zone;
        }
#endif
    }

    return af;
}

static int set_receive_timeout(int sck)
{
    struct timeval timeout = {.tv_usec = 50 * 1000, .tv_sec = 0};

    int err = lwip_setsockopt(sck, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    if (err)
    {
        configPRINTF(("Setting socket receive timeout failed (%d).\r\n", err));
    }

    return err;
}

static void echo_udp(int sck)
{
    int err;
    uint8_t inBuf[10];

    err = set_receive_timeout(sck);
    if (err)
    {
        return;
    }

    while (1)
    {
        ssize_t bytes = recvfrom(sck, &inBuf, sizeof(inBuf), 0, (struct sockaddr *)&sender_addr, &sender_addr_len);

        if (bytes > 0)
        {
            if ('s' == inBuf[0])
            {
                s_sck = sck;
                s_startUDPForward = true;
            }
            else if ('c' == inBuf[0])
            {
                s_startUDPForward = false;
            }
        }
        else if (!run)
        {
            // end was called.
            return;
        }
        else if (errno == EWOULDBLOCK)
        {
            // Timeout is here to allow check if we should continue so call read again.
        }
        else
        {
            configPRINTF(("Socket terminated. (errno=%d).\r\n", errno));
            return;
        }
    }
}

static void echo_loop_tcp(int sck)
{
    int err;
    uint8_t inBuf[10];

    err = set_receive_timeout(sck);
    if (err)
    {
        return;
    }

    while (1)
    {
        ssize_t bytes = read(sck, &inBuf, sizeof(inBuf));
        if (bytes > 0)
        {
            if ('s' == inBuf[0])
            {
                s_sck = sck;
                s_startTCPForward = true;
            }
            else if ('c' == inBuf[0])
            {
                s_startTCPForward = false;
            }
        }
        else if (!run)
        {
            // end was called.
            return;
        }
        else if (errno == EWOULDBLOCK)
        {
            // Timeout is here to allow check if we should continue so call read again.
        }
        else
        {
            configPRINTF(("Connection terminated. (errno=%d).\r\n", errno));
            return;
        }
    }
}

static void finish_thread(int sck)
{
    if (sck != -1)
    {
        close(sck);
        sck = -1;
    }
    run = 0;

//    shell_task_set_mode(SHELL_MODE_DEFAULT);

    vTaskDelete(NULL);
}

static void tcp_connect_thread(void *arg)
{
    example_sockinfo_t *sockinfo = (example_sockinfo_t *)arg;

    int err;

    configPRINTF(("Connecting...\r\n"));
    if (sockinfo->af == AF_INET)
    {
        err = connect(sockinfo->sck, (struct sockaddr *)&sockinfo->ipv4, sizeof(sockinfo->ipv4));
    }
    else
    {
        err = connect(sockinfo->sck, (struct sockaddr *)&sockinfo->ipv6, sizeof(sockinfo->ipv6));
    }
    if (err)
    {
        configPRINTF(("Connecting failed. errno=%d\r\n", errno));
    }
    else
    {
        configPRINTF(("Connected.\r\n"));
        echo_loop_tcp(sockinfo->sck);
    }

    finish_thread(sockinfo->sck);
}

static void tcp_server_thread(void *arg)
{
    int sck = *((int *)arg);

    echo_loop_tcp(sck);

    finish_thread(sck);
}

static void tcp_listen_thread(void *arg)
{
    example_sockinfo_t *sockinfo = (example_sockinfo_t *)arg;

    int ret;

    if (sockinfo->af == AF_INET)
    {
        ret = bind(sockinfo->sck, (struct sockaddr *)&sockinfo->ipv4, sizeof(sockinfo->ipv4));
    }
    else
    {
        ret = bind(sockinfo->sck, (struct sockaddr *)&sockinfo->ipv6, sizeof(sockinfo->ipv6));
    }
    if (ret < 0)
    {
        configPRINTF(("bind() failed (errno=%d)\r\n", errno));
    }
    else
    {
        configPRINTF(("Waiting for incoming connection...\r\n"));
        listen(sockinfo->sck, 0); // zero to use the smallest connection backlog possible (see tcp_backlog_set)
        fcntl(sockinfo->sck, F_SETFL, O_NONBLOCK);

        run                  = 1;
        int accepted_sck_cnt = 0;
        while (1)
        {
            if (accepted_sck_cnt >= TCP_SERVER_CONNECTIONS_MAX)
            {
                // Reached maximum connections.
                accepted_sck_cnt -= TCP_SERVER_CONNECTIONS_MAX;
                vTaskDelay(pdMS_TO_TICKS(50));
                continue;
            }
            sockinfo->sck_accepted[accepted_sck_cnt] = accept(sockinfo->sck, NULL, 0);
            if (sockinfo->sck_accepted[accepted_sck_cnt] < 0)
            {
                // Nothing to accept. Wait 50ms and try it again.
                vTaskDelay(pdMS_TO_TICKS(50));
            }
            else
            {
                configPRINTF(("\r\nAccepted connection\r\n"));
                run                  = 1;

                // Create thread that serves this connection
                sys_thread_t thread =
                    sys_thread_new("tcp_server_thread", tcp_server_thread,
                                   (void *)&(sockinfo->sck_accepted[accepted_sck_cnt]), 1024, DEFAULT_THREAD_PRIO);

                if (thread == NULL)
                {
                    configPRINTF(("Can not create TCP connection server thread\r\n"));
                    close(sockinfo->sck_accepted[accepted_sck_cnt]);
                    sockinfo->sck_accepted[accepted_sck_cnt] = -1;
                }
                else
                {
                    accepted_sck_cnt += 1;
                }
            }
        }
        // Listen thread stopped, clean up connections
        accepted_sck_cnt = 0;
    }

    finish_thread(sockinfo->sck);
}

static void udp_thread(void *arg)
{
    example_sockinfo_t *sockinfo = (example_sockinfo_t *)arg;
    int ret;

    if (sockinfo->af == AF_INET)
    {
        ret = bind(sockinfo->sck, (struct sockaddr *)&sockinfo->ipv4, sizeof(sockinfo->ipv4));
    }
    else
    {
        ret = bind(sockinfo->sck, (struct sockaddr *)&sockinfo->ipv6, sizeof(sockinfo->ipv6));
    }
    if (ret < 0)
    {
        configPRINTF(("bind() failed (errno=%d)\r\n", errno));
    }
    else
    {
        configPRINTF(("Waiting for datagrams\r\n"));
        echo_udp(sockinfo->sck);
    }

    finish_thread(sockinfo->sck);
}

int socket_task_init(int is_tcp, const char *ip_str, const char *port_str)
{
    example_sockinfo_t *sockinfo = &s_sockinfo;
    void (*thread_func_ptr)(void *);

    const int is_server = (ip_str == NULL);
    sockinfo->is_server = is_server;

    sockinfo->af = ip_port_str_to_sockaddr((is_server) ? "::" : ip_str, port_str, &sockinfo->ipv4, &sockinfo->ipv6);
    if (sockinfo->af < 0)
    {
        return -1;
    }

    sockinfo->sck_type = (is_tcp) ? SOCK_STREAM : SOCK_DGRAM;

    configPRINTF(("Creating new socket.\r\n"));
    sockinfo->sck = socket(sockinfo->af, sockinfo->sck_type, 0);
    if (sockinfo->sck < 0)
    {
        configPRINTF(("Socket creation failed. (%d)\r\n", sockinfo->sck));
        return -1;
    }

    if (is_tcp)
    {
        if (is_server)
        {
            thread_func_ptr = (void (*)(void *))tcp_listen_thread;
        }
        else
        {
            thread_func_ptr = (void (*)(void *))tcp_connect_thread;
        }
    }
    else
    {
        thread_func_ptr = (void (*)(void *))udp_thread;
    }

    run = 1;

    sys_thread_t thread = sys_thread_new("socket_thread", thread_func_ptr, (void *)sockinfo, 1024, DEFAULT_THREAD_PRIO);

    if (thread == NULL)
    {
        configPRINTF(("Can not create socket thread\r\n"));
        close(sockinfo->sck);
        sockinfo->sck = -1;
    }

    return sockinfo->sck;
}

void socket_task_terminate(void)
{
    run = 0;
    configPRINTF(("\r\nAll socket tasks terminated\r\n"));
}

void socket_task_print_ips(void)
{
    struct netif *netif_;
    NETIF_FOREACH(netif_)
    {
        configPRINTF(("************************************************\r\n"));
		configPRINTF((" Interface name   : %s%d\r\n", netif_->name, netif_->num));
        configPRINTF((" IPv4 Address     : %s\r\n", ip4addr_ntoa(netif_ip4_addr(netif_))));
        configPRINTF((" IPv4 Subnet mask : %s\r\n", ip4addr_ntoa(netif_ip4_netmask(netif_))));
        configPRINTF((" IPv4 Gateway     : %s\r\n", ip4addr_ntoa(netif_ip4_gw(netif_))));
        for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++)
        {
            const char *str_ip = "-";
            if (ip6_addr_isvalid(netif_ip6_addr_state(netif_, i)))
            {
                str_ip = ip6addr_ntoa(netif_ip6_addr(netif_, i));
            }
            configPRINTF((" IPv6 Address%d    : %s\r\n", i, str_ip));
        }
        configPRINTF(("************************************************\r\n"));
    }
}

void AUDIO_DUMP_WIFI_Task(void *arg)
{
    static uint8_t *s_wifiBuffer = NULL;

    while (!g_xAudioDumpQueue)
        vTaskDelay(10);

    if (s_wifiBuffer == NULL)
    {
        configPRINTF(("Audio Dump over WiFi Started\r\n"));
        s_wifiBuffer = (uint8_t *)pvPortMalloc(AUDIO_BUFFER_OUTPUT_SIZE);
        memset(s_wifiBuffer, 0, AUDIO_BUFFER_OUTPUT_SIZE);
    }

    if (s_wifiBuffer == NULL)
    {
        configPRINTF(("ERROR: Failed to allocate memory for s_wifiBuffer of %d!\r\n", AUDIO_BUFFER_OUTPUT_SIZE));
    }
    else
    {
        /* Forward data to messeage sender ip */
        while (s_startUDPForward || s_startTCPForward)
        {
            if (xQueueReceive(g_xAudioDumpQueue, s_wifiBuffer, 100) != pdPASS)
            {
#if ENABLE_STREAMER && !ENABLE_AEC
            /* If AEC is disabled, temporarily bypass audio dump while streaming audio. */
                if (LOCAL_SOUNDS_isPlaying())
                {
                    continue;
                }
#endif /* ENABLE_STREAMER && !ENABLE_AEC */
                configPRINTF(("Could not receive from the queue\r\n"));
            }
            if (s_startUDPForward)
            {
                sendto(s_sck, s_wifiBuffer, AUDIO_BUFFER_OUTPUT_SIZE, 0, (struct sockaddr *)&sender_addr, sender_addr_len);
            }
            else if (s_startTCPForward)
            {
                write(s_sck, s_wifiBuffer, AUDIO_BUFFER_OUTPUT_SIZE);
            }
        }

        vTaskDelay(100);

        vPortFree(s_wifiBuffer);
        s_wifiBuffer = NULL;
        configPRINTF(("Audio Dump over WiFi Canceled\r\n"));
    }

    if (g_xAudioDumpQueue != NULL)
    {
        vQueueDelete(g_xAudioDumpQueue);
    }

    //set handlers to NULL so they'll be re-created when a new dump starts
    g_xAudioDumpQueue        = NULL;
    audioDumpWiFiTaskHandle = NULL;

    vTaskDelete(NULL);
}

void AUDIO_DUMP_ForwardDataOverWiFi(int16_t *micStream, int16_t *ampStream, void *cleanStream)
{
    uint32_t u32Element = 0;

    if (s_startUDPForward || s_startTCPForward)
    {
        if (g_xAudioDumpQueue == NULL)
        {
            g_xAudioDumpQueue = xQueueCreate(DUMP_QUEUE_SLOTS, AUDIO_BUFFER_OUTPUT_SIZE);
            if (g_xAudioDumpQueue == NULL)
            {
                configPRINTF(("Failed to create DumpQueue!\r\n"));
            }
            // reset g_processedFrames to match the exported audio
            g_processedFrames = 0;
        }

        if (audioDumpWiFiTaskHandle == NULL)
        {
            if (xTaskCreate(AUDIO_DUMP_WIFI_Task, "audio_dump_wifi_task", 350, NULL, configMAX_PRIORITIES - 3,
                                &audioDumpWiFiTaskHandle) != pdPASS)
            {
                configPRINTF(("xTaskCreate AUDIO_DUMP_WIFI_Task failed!\r\n"));
            }
        }

#if DUMP_ALL_STREAMS
        /* Prepare and send clean data to Audio Dump task */
        memcpy(&s_audioStream[u32Element], (uint8_t *)micStream, PCM_SAMPLE_COUNT * PCM_SAMPLE_SIZE_BYTES);
        u32Element += PCM_SAMPLE_COUNT;
        if (ampStream != NULL)
        {
            memcpy(&s_audioStream[u32Element], (uint8_t *)ampStream, PCM_SINGLE_CH_SMPL_COUNT * PCM_SAMPLE_SIZE_BYTES);
        }
        else
        {
            memset(&s_audioStream[u32Element], 0, PCM_SINGLE_CH_SMPL_COUNT * PCM_SAMPLE_SIZE_BYTES);
        }
        u32Element += PCM_SINGLE_CH_SMPL_COUNT;
        memcpy(&s_audioStream[u32Element], (uint8_t *)cleanStream, PCM_SINGLE_CH_SMPL_COUNT * PCM_SAMPLE_SIZE_BYTES);
        u32Element += PCM_SINGLE_CH_SMPL_COUNT;
#else
        memcpy(&s_audioStream[u32Element], (uint8_t *)cleanStream, PCM_SINGLE_CH_SMPL_COUNT * PCM_SAMPLE_SIZE_BYTES);
        u32Element += PCM_SINGLE_CH_SMPL_COUNT;
#endif /* DUMP_ALL_STREAMS */

        if (xQueueSendToBack(g_xAudioDumpQueue, s_audioStream, 0) != pdPASS)
        {
            configPRINTF(("Failed to push frames to audio dump queue\r\n"));
        }
    }
}
#endif /* ENABLE_WIFI_AUDIO_DUMP */
