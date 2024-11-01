static void APP_LAYER_VIT_ParseAudioPlayerIntent(void)
{
    VIT_Intent_st *pSpeechIntent = NULL;
    PL_INT16 tagsCnt;
    char *prompt = NULL;

    pSpeechIntent = &SpeechIntent;
    tagsCnt = pSpeechIntent->Slot_Tag_count - 1;

    /* VOLUME */
    if (!strcmp("Volume", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        if (pSpeechIntent->Slot_Tag_count == 1)
        {
            if (!strcmp("level", TAG_NAME(pSpeechIntent, tagsCnt)))
            {
                if (!strcmp("maximum", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("max", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = NULL;
                }
                else if (!strcmp("minimum", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("min", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = NULL;
                }
                else if (!strcmp("ten", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = NULL;
                }
                else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = NULL;
                }
                else if (!strcmp("thirty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = NULL;
                }
                else if (!strcmp("forty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = NULL;
                }
                else if (!strcmp("fifty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = NULL;
                }
                else if (!strcmp("sixty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = NULL;
                }
                else if (!strcmp("seventy", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = NULL;
                }
                else if (!strcmp("eighty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = NULL;
                }
                else if (!strcmp("ninety", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = NULL;
                }
            }
        }
        else if (pSpeechIntent->Slot_Tag_count == 2)
        {
            if (!strcmp("action", TAG_NAME(pSpeechIntent, tagsCnt)) &&
                !strcmp("level", TAG_NAME(pSpeechIntent, (tagsCnt - 1))))
            {
                if (!strcmp("increase", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("up", TAG_VALUE(pSpeechIntent, tagsCnt, 1)) ||
                    !strcmp("raise", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("ten", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = NULL;
                    }
                    else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = NULL;
                    }
                    else if (!strcmp("thirty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = NULL;
                    }
                    else if (!strcmp("forty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = NULL;
                    }
                    else if (!strcmp("fifty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = NULL;
                    }
                    else if (!strcmp("sixty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = NULL;
                    }
                    else if (!strcmp("seventy", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = NULL;
                    }
                    else if (!strcmp("eighty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = NULL;
                    }
                    else if (!strcmp("ninety", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = NULL;
                    }
                    else if (!strcmp("maximum", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) ||
                             !strcmp("max", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) ||
                             !strcmp("one", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = NULL;
                    }
                }
                else if (!strcmp("decrease", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("down", TAG_VALUE(pSpeechIntent, tagsCnt, 1)) ||
                         !strcmp("reduce", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("lower", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("ten", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = NULL;
                    }
                    else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = NULL;
                    }
                    else if (!strcmp("thirty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = NULL;
                    }
                    else if (!strcmp("forty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = NULL;
                    }
                    else if (!strcmp("fifty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = NULL;
                    }
                    else if (!strcmp("sixty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = NULL;
                    }
                    else if (!strcmp("seventy", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = NULL;
                    }
                    else if (!strcmp("eighty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = NULL;
                    }
                    else if (!strcmp("ninety", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = NULL;
                    }
                    else if (!strcmp("minimum", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) ||
                             !strcmp("min", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = NULL;
                    }
                }
            }
        }
    }

    /* MUSIC */
    else if (!strcmp("Music", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        if (pSpeechIntent->Slot_Tag_count == 1)
        {
            if (!strcmp("action", TAG_NAME(pSpeechIntent, tagsCnt)))
            {
                if (!strcmp("start", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = NULL;
                }
                else if (!strcmp("stop", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = NULL;
                }
                else if (!strcmp("pause", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = NULL;
                }
                else if (!strcmp("play", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                      prompt = NULL;
                }
                else if (!strcmp("next", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = NULL;
                }
                else if (!strcmp("previous", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = NULL;
                }
            }
        }
    }
#if ENABLE_STREAMER
    if (prompt != NULL)
    {
        APP_LAYER_PlayAudioFromFileSystem(prompt);
    }
    else
    {
        configPRINTF(("Intent processed successfully but no audio prompt associated to it found.\r\n"));
    }
#endif /* ENABLE_STREAMER */
}