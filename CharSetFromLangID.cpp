/* See https://msdn.microsoft.com/en-us/library/bb165625(v=vs.90).aspx */
static BYTE
CharSetFromLangID(LANGID LangID)
{
    /* FIXME: Add more and fix if wrong */
    switch (PRIMARYLANGID(LangID))
    {
        case LANG_CHINESE:
            switch (SUBLANGID(LangID))
            {
                case SUBLANG_CHINESE_TRADITIONAL:
                    return CHINESEBIG5_CHARSET;
                case SUBLANG_CHINESE_SIMPLIFIED:
                default:
                    break;
            }
            return GB2312_CHARSET;

        case LANG_CZECH: case LANG_HUNGARIAN: case LANG_POLISH:
        case LANG_SLOVAK: case LANG_SLOVENIAN: case LANG_ROMANIAN:
            return EASTEUROPE_CHARSET;

        case LANG_RUSSIAN: case LANG_BULGARIAN: case LANG_MACEDONIAN:
        case LANG_SERBIAN: case LANG_UKRAINIAN:
            return RUSSIAN_CHARSET;

        case LANG_ARABIC:       return ARABIC_CHARSET;
        case LANG_GREEK:        return GREEK_CHARSET;
        case LANG_HEBREW:       return HEBREW_CHARSET;
        case LANG_JAPANESE:     return SHIFTJIS_CHARSET;
        case LANG_KOREAN:       return JOHAB_CHARSET;
        case LANG_TURKISH:      return TURKISH_CHARSET;
        case LANG_THAI:         return THAI_CHARSET;
        case LANG_LATVIAN:      return BALTIC_CHARSET;
        case LANG_VIETNAMESE:   return VIETNAMESE_CHARSET;

        case LANG_ENGLISH: case LANG_BASQUE: case LANG_CATALAN:
        case LANG_DANISH: case LANG_DUTCH: case LANG_FINNISH:
        case LANG_FRENCH: case LANG_GERMAN: case LANG_ITALIAN:
        case LANG_NORWEGIAN: case LANG_PORTUGUESE: case LANG_SPANISH:
        case LANG_SWEDISH: default:
            return ANSI_CHARSET;
    }
}
