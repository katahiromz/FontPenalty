// NOTE: See Table 1. of https://msdn.microsoft.com/en-us/library/ms969909.aspx
static UINT
GetFontPenalty(const LOGFONTW *               LogFont,
               const OUTLINETEXTMETRICW *     Otm,
               const char *             style_name)
{
    ULONG   Penalty = 0;
    BYTE    Byte;
    LONG    Long;
    BOOL    fNeedScaling = FALSE;
    const BYTE UserCharSet = CharSetFromLangID(gusLanguageID);
    const TEXTMETRICW * TM = &Otm->otmTextMetrics;
    WCHAR* ActualNameW;

    ASSERT(Otm);
    ASSERT(LogFont);

    /* FIXME: IntSizeSynth Penalty 20 */
    /* FIXME: SmallPenalty Penalty 1 */
    /* FIXME: FaceNameSubst Penalty 500 */

    Byte = LogFont->lfCharSet;
    if (Byte == DEFAULT_CHARSET)
    {
        if (_wcsicmp(LogFont->lfFaceName, L"Marlett") == 0)
        {
            if (Byte == ANSI_CHARSET)
            {
                DPRINT("Warning: FIXME: It's Marlett but ANSI_CHARSET.\n");
            }
            /* We assume SYMBOL_CHARSET for "Marlett" font */
            Byte = SYMBOL_CHARSET;
        }
    }

    if (Byte != TM->tmCharSet)
    {
        if (Byte != DEFAULT_CHARSET && Byte != ANSI_CHARSET)
        {
            /* CharSet Penalty 65000 */
            /* Requested charset does not match the candidate's. */
            GOT_PENALTY("CharSet", 65000);
        }
        else
        {
            if (UserCharSet != TM->tmCharSet)
            {
                /* UNDOCUMENTED: Not user language */
                GOT_PENALTY("UNDOCUMENTED:NotUserLanguage", 100);

                if (ANSI_CHARSET != TM->tmCharSet)
                {
                    /* UNDOCUMENTED: Not ANSI charset */
                    GOT_PENALTY("UNDOCUMENTED:NotAnsiCharSet", 100);
                }
            }
        }
    }

    Byte = LogFont->lfOutPrecision;
    switch (Byte)
    {
        case OUT_DEFAULT_PRECIS:
            /* nothing to do */
            break;
        case OUT_DEVICE_PRECIS:
            if (!(TM->tmPitchAndFamily & TMPF_DEVICE) ||
                !(TM->tmPitchAndFamily & (TMPF_VECTOR | TMPF_TRUETYPE)))
            {
                /* OutputPrecision Penalty 19000 */
                /* Requested OUT_STROKE_PRECIS, but the device can't do it
                   or the candidate is not a vector font. */
                GOT_PENALTY("OutputPrecision", 19000);
            }
            break;
        default:
            if (TM->tmPitchAndFamily & (TMPF_VECTOR | TMPF_TRUETYPE))
            {
                /* OutputPrecision Penalty 19000 */
                /* Or OUT_STROKE_PRECIS not requested, and the candidate
                   is a vector font that requires GDI support. */
                GOT_PENALTY("OutputPrecision", 19000);
            }
            break;
    }

    Byte = (LogFont->lfPitchAndFamily & 0x0F);
    if (Byte == DEFAULT_PITCH)
        Byte = VARIABLE_PITCH;
    if (Byte == FIXED_PITCH)
    {
        if (TM->tmPitchAndFamily & _TMPF_VARIABLE_PITCH)
        {
            /* FixedPitch Penalty 15000 */
            /* Requested a fixed pitch font, but the candidate is a
               variable pitch font. */
            GOT_PENALTY("FixedPitch", 15000);
        }
    }
    if (Byte == VARIABLE_PITCH)
    {
        if (!(TM->tmPitchAndFamily & _TMPF_VARIABLE_PITCH))
        {
            /* PitchVariable Penalty 350 */
            /* Requested a variable pitch font, but the candidate is not a
               variable pitch font. */
            GOT_PENALTY("PitchVariable", 350);
        }
    }

    Byte = (LogFont->lfPitchAndFamily & 0x0F);
    if (Byte == DEFAULT_PITCH)
    {
        if (!(TM->tmPitchAndFamily & _TMPF_VARIABLE_PITCH))
        {
            /* DefaultPitchFixed Penalty 1 */
            /* Requested DEFAULT_PITCH, but the candidate is fixed pitch. */
            GOT_PENALTY("DefaultPitchFixed", 1);
        }
    }

    ActualNameW = (WCHAR*)((ULONG_PTR)Otm + (ULONG_PTR)Otm->otmpFamilyName);

    if (LogFont->lfFaceName[0])
    {
        BOOL Found = FALSE;

        /* localized family name */
        if (!Found)
        {
            Found = (_wcsicmp(LogFont->lfFaceName, ActualNameW) == 0);
        }
        /* localized full name */
        if (!Found)
        {
            ActualNameW = (WCHAR*)((ULONG_PTR)Otm + (ULONG_PTR)Otm->otmpFaceName);
            Found = (_wcsicmp(LogFont->lfFaceName, ActualNameW) == 0);
        }
        if (!Found)
        {
            /* FaceName Penalty 10000 */
            /* Requested a face name, but the candidate's face name
               does not match. */
            GOT_PENALTY("FaceName", 10000);
        }
    }

    Byte = (LogFont->lfPitchAndFamily & 0xF0);
    if (Byte != FF_DONTCARE)
    {
        if (Byte != (TM->tmPitchAndFamily & 0xF0))
        {
            /* Family Penalty 9000 */
            /* Requested a family, but the candidate's family is different. */
            GOT_PENALTY("Family", 9000);
        }
        if ((TM->tmPitchAndFamily & 0xF0) == FF_DONTCARE)
        {
            /* FamilyUnknown Penalty 8000 */
            /* Requested a family, but the candidate has no family. */
            GOT_PENALTY("FamilyUnknown", 8000);
        }
    }

    /* Is the candidate a non-vector font? */
    if (!(TM->tmPitchAndFamily & (TMPF_TRUETYPE | TMPF_VECTOR)))
    {
        /* Is lfHeight specified? */
        if (LogFont->lfHeight != 0)
        {
            if (labs(LogFont->lfHeight) < TM->tmHeight)
            {
                /* HeightBigger Penalty 600 */
                /* The candidate is a nonvector font and is bigger than the
                   requested height. */
                GOT_PENALTY("HeightBigger", 600);
                /* HeightBiggerDifference Penalty 150 */
                /* The candidate is a raster font and is larger than the
                   requested height. Penalty * height difference */
                GOT_PENALTY("HeightBiggerDifference", 150 * labs(TM->tmHeight - labs(LogFont->lfHeight)));

                fNeedScaling = TRUE;
            }
            if (TM->tmHeight < labs(LogFont->lfHeight))
            {
                /* HeightSmaller Penalty 150 */
                /* The candidate is a raster font and is smaller than the
                   requested height. Penalty * height difference */
                GOT_PENALTY("HeightSmaller", 150 * labs(TM->tmHeight - labs(LogFont->lfHeight)));

                fNeedScaling = TRUE;
            }
        }
    }

    switch (LogFont->lfPitchAndFamily & 0xF0)
    {
        case FF_ROMAN: case FF_MODERN: case FF_SWISS:
            switch (TM->tmPitchAndFamily & 0xF0)
            {
                case FF_DECORATIVE: case FF_SCRIPT:
                    /* FamilyUnlikely Penalty 50 */
                    /* Requested a roman/modern/swiss family, but the
                       candidate is decorative/script. */
                    GOT_PENALTY("FamilyUnlikely", 50);
                    break;
                default:
                    break;
            }
            break;
        case FF_DECORATIVE: case FF_SCRIPT:
            switch (TM->tmPitchAndFamily & 0xF0)
            {
                case FF_ROMAN: case FF_MODERN: case FF_SWISS:
                    /* FamilyUnlikely Penalty 50 */
                    /* Or requested decorative/script, and the candidate is
                       roman/modern/swiss. */
                    GOT_PENALTY("FamilyUnlikely", 50);
                    break;
                default:
                    break;
            }
        default:
            break;
    }

    if (LogFont->lfWidth != 0)
    {
        if (LogFont->lfWidth != TM->tmAveCharWidth)
        {
            /* Width Penalty 50 */
            /* Requested a nonzero width, but the candidate's width
               doesn't match. Penalty * width difference */
            GOT_PENALTY("Width", 50 * labs(LogFont->lfWidth - TM->tmAveCharWidth));

            if (!(TM->tmPitchAndFamily & (TMPF_TRUETYPE | TMPF_VECTOR)))
                fNeedScaling = TRUE;
        }
    }

    if (fNeedScaling)
    {
        /* SizeSynth Penalty 50 */
        /* The candidate is a raster font that needs scaling by GDI. */
        GOT_PENALTY("SizeSynth", 50);
    }

    if (!!LogFont->lfItalic != !!TM->tmItalic)
    {
        if (!LogFont->lfItalic && ItalicFromStyle(style_name))
        {
            /* Italic Penalty 4 */
            /* Requested font and candidate font do not agree on italic status,
               and the desired result cannot be simulated. */
            /* Adjusted to 40 to satisfy (Oblique Penalty > Book Penalty). */
            GOT_PENALTY("Italic", 40);
        }
        else if (LogFont->lfItalic && !ItalicFromStyle(style_name))
        {
            /* ItalicSim Penalty 1 */
            /* Requested italic font but the candidate is not italic,
               although italics can be simulated. */
            GOT_PENALTY("ItalicSim", 1);
        }
    }

    if (LogFont->lfOutPrecision == OUT_TT_PRECIS)
    {
        if (!(TM->tmPitchAndFamily & TMPF_TRUETYPE))
        {
            /* NotTrueType Penalty 4 */
            /* Requested OUT_TT_PRECIS, but the candidate is not a
               TrueType font. */
            GOT_PENALTY("NotTrueType", 4);
        }
    }

    Long = LogFont->lfWeight;
    if (LogFont->lfWeight == FW_DONTCARE)
        Long = FW_NORMAL;
    if (Long != TM->tmWeight)
    {
        /* Weight Penalty 3 */
        /* The candidate's weight does not match the requested weight. 
           Penalty * (weight difference/10) */
        GOT_PENALTY("Weight", 3 * (labs(Long - TM->tmWeight) / 10));
    }

    if (!LogFont->lfUnderline && TM->tmUnderlined)
    {
        /* Underline Penalty 3 */
        /* Requested font has no underline, but the candidate is
           underlined. */
        GOT_PENALTY("Underline", 3);
    }

    if (!LogFont->lfStrikeOut && TM->tmStruckOut)
    {
        /* StrikeOut Penalty 3 */
        /* Requested font has no strike-out, but the candidate is
           struck out. */
        GOT_PENALTY("StrikeOut", 3);
    }

    /* Is the candidate a non-vector font? */
    if (!(TM->tmPitchAndFamily & (TMPF_TRUETYPE | TMPF_VECTOR)))
    {
        if (LogFont->lfHeight != 0 && TM->tmHeight < LogFont->lfHeight)
        {
            /* VectorHeightSmaller Penalty 2 */
            /* Candidate is a vector font that is smaller than the
               requested height. Penalty * height difference */
            GOT_PENALTY("VectorHeightSmaller", 2 * labs(TM->tmHeight - LogFont->lfHeight));
        }
        if (LogFont->lfHeight != 0 && TM->tmHeight > LogFont->lfHeight)
        {
            /* VectorHeightBigger Penalty 1 */
            /* Candidate is a vector font that is bigger than the
               requested height. Penalty * height difference */
            GOT_PENALTY("VectorHeightBigger", 1 * labs(TM->tmHeight - LogFont->lfHeight));
        }
    }

    if (!(TM->tmPitchAndFamily & TMPF_DEVICE))
    {
        /* DeviceFavor Penalty 2 */
        /* Extra penalty for all nondevice fonts. */
        GOT_PENALTY("DeviceFavor", 2);
    }

    if (TM->tmAveCharWidth >= 5 && TM->tmHeight >= 5)
    {
        if (TM->tmAveCharWidth / TM->tmHeight >= 3)
        {
            /* Aspect Penalty 30 */
            /* The aspect rate is >= 3. It seems like a bad font. */
            GOT_PENALTY("Aspect", ((TM->tmAveCharWidth / TM->tmHeight) - 2) * 30);
        }
        else if (TM->tmHeight / TM->tmAveCharWidth >= 3)
        {
            /* Aspect Penalty 30 */
            /* The aspect rate is >= 3. It seems like a bad font. */
            GOT_PENALTY("Aspect", ((TM->tmHeight / TM->tmAveCharWidth) - 2) * 30);
        }
    }

    if (Penalty < 200)
    {
        DPRINT("WARNING: Penalty:%ld < 200: RequestedNameW:%ls, "
            "ActualNameW:%ls, lfCharSet:%d, lfWeight:%ld, "
            "tmCharSet:%d, tmWeight:%ld\n",
            Penalty, LogFont->lfFaceName, ActualNameW,
            LogFont->lfCharSet, LogFont->lfWeight,
            TM->tmCharSet, TM->tmWeight);
    }

    return Penalty;     /* success */
}
