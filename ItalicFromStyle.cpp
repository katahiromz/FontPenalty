static BYTE
ItalicFromStyle(const char *style_name)
{
    if (style_name == NULL || style_name[0] == 0)
        return FALSE;
    if (strstr(style_name, "Italic") != NULL)
        return TRUE;
    if (strstr(style_name, "Oblique") != NULL)
        return TRUE;
    return FALSE;
}
