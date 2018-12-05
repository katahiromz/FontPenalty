// FontPenalty.cpp
// Copyright (C) 2018 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// This file is public domain software.

#include <windows.h>
#include <cstdio>
#include <cassert>
#include <vector>
#include <string>
#include <iostream>

enum RET
{
    RET_SUCCESS = 0,
    RET_SHOWHELP,
    RET_SHOWVERSION,
    RET_INVALIDARG,
    RET_CANTCREATEFONT,
    RET_CANTGETMETRICS,
    RET_CANTSELECTFONT
};

HDC g_hDC;
const char *g_name = NULL;
const char *g_file = NULL;
const char *g_style = NULL;
BYTE g_charset = DEFAULT_CHARSET;
LONG g_lfHeight = 0;
BOOL g_show_metrics = FALSE;
LANGID g_langid = GetUserDefaultLangID();

void ShowVersion(void)
{
    printf("FontPenalty 0.2 %s by katahiromz\n", __DATE__);
}

void ShowHelp(void)
{
    ShowVersion();
    printf("Usage: FontPenalty [options]\n");
    printf("Options:\n");
    printf("--help                      Show this help\n");
    printf("--version                   Show version info\n");
    printf("--name \"font name\"          Set the target font name (default: \"\")\n");
    printf("--height HHH                Set request lfHeight value (default: 0)\n");
    printf("--charset ddd               Set request charset value (default: 0)\n");
    printf("--style \"Bold\"              Set actual style string\n");
    printf("--langid 0xXXXX             Set language ID (default: 0x%04X)\n", g_langid);
    printf("--file \"font_file.ttf\"      Set the target font file\n");
    printf("--metrics                   Show the metrics\n");
}

int ParseCommandLine(int argc, char **argv)
{
    printf("Command Line: FontPenalty ");
    for (int i = 1; i < argc; ++i)
    {
        if (strchr(argv[i], ' ') != NULL)
            printf("\"%s\" ", argv[i]);
        else
            printf("%s ", argv[i]);
    }
    printf("\n");

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--help") == 0)
        {
            ShowHelp();
            return RET_SHOWHELP;
        }
        if (strcmp(argv[i], "--version") == 0)
        {
            ShowVersion();
            return RET_SHOWVERSION;
        }
        if (strcmp(argv[i], "--name") == 0)
        {
            g_name = argv[i + 1];
            ++i;
            continue;
        }
        if (strcmp(argv[i], "--file") == 0)
        {
            g_file = argv[i + 1];
            ++i;
            continue;
        }
        if (strcmp(argv[i], "--charset") == 0)
        {
            g_charset = strtol(argv[i + 1], NULL, 0);
            ++i;
            continue;
        }
        if (strcmp(argv[i], "--height") == 0)
        {
            g_lfHeight = strtol(argv[i + 1], NULL, 0);
            ++i;
            continue;
        }
        if (strcmp(argv[i], "--metrics") == 0)
        {
            g_show_metrics = TRUE;
            continue;
        }
        if (strcmp(argv[i], "--style") == 0)
        {
            g_style = argv[i + 1];
            ++i;
            continue;
        }
        if (strcmp(argv[i], "--langid") == 0)
        {
            g_langid = strtol(argv[i + 1], NULL, 0);
            ++i;
            continue;
        }
        if (argv[i][0] == '-')
        {
            printf("ERROR: invalid option: %s\n", argv[i]);
            return RET_INVALIDARG;
        }
    }
    return 0;
}

void ShowMetrics(LPOUTLINETEXTMETRICW potm)
{
#define DO_OUT(x) std::cout << #x << ": " << (x) << std::endl
    DO_OUT(potm->otmSize);
    DO_OUT(potm->otmTextMetrics.tmHeight);
    DO_OUT(potm->otmTextMetrics.tmAscent);
    DO_OUT(potm->otmTextMetrics.tmDescent);
    DO_OUT(potm->otmTextMetrics.tmInternalLeading);
    DO_OUT(potm->otmTextMetrics.tmExternalLeading);
    DO_OUT(potm->otmTextMetrics.tmAveCharWidth);
    DO_OUT(potm->otmTextMetrics.tmMaxCharWidth);
    DO_OUT(potm->otmTextMetrics.tmWeight);
    DO_OUT(potm->otmTextMetrics.tmOverhang);
    DO_OUT(potm->otmTextMetrics.tmDigitizedAspectX);
    DO_OUT(potm->otmTextMetrics.tmDigitizedAspectY);
    DO_OUT(potm->otmTextMetrics.tmFirstChar);
    DO_OUT(potm->otmTextMetrics.tmLastChar);
    DO_OUT(potm->otmTextMetrics.tmDefaultChar);
    DO_OUT(potm->otmTextMetrics.tmBreakChar);
    DO_OUT((int)potm->otmTextMetrics.tmItalic);
    DO_OUT((int)potm->otmTextMetrics.tmUnderlined);
    DO_OUT((int)potm->otmTextMetrics.tmStruckOut);
    DO_OUT((int)potm->otmTextMetrics.tmPitchAndFamily);
    DO_OUT((int)potm->otmTextMetrics.tmCharSet);
    DO_OUT((int)potm->otmFiller);
    DO_OUT((int)potm->otmPanoseNumber.bFamilyType);
    DO_OUT((int)potm->otmPanoseNumber.bSerifStyle);
    DO_OUT((int)potm->otmPanoseNumber.bWeight);
    DO_OUT((int)potm->otmPanoseNumber.bProportion);
    DO_OUT((int)potm->otmPanoseNumber.bContrast);
    DO_OUT((int)potm->otmPanoseNumber.bStrokeVariation);
    DO_OUT((int)potm->otmPanoseNumber.bArmStyle);
    DO_OUT((int)potm->otmPanoseNumber.bLetterform);
    DO_OUT((int)potm->otmPanoseNumber.bMidline);
    DO_OUT((int)potm->otmPanoseNumber.bXHeight);
    DO_OUT(potm->otmfsSelection);
    DO_OUT(potm->otmfsType);
    DO_OUT(potm->otmsCharSlopeRise);
    DO_OUT(potm->otmsCharSlopeRun);
    DO_OUT(potm->otmItalicAngle);
    DO_OUT(potm->otmEMSquare);
    DO_OUT(potm->otmAscent);
    DO_OUT(potm->otmDescent);
    DO_OUT(potm->otmLineGap);
    DO_OUT(potm->otmsCapEmHeight);
    DO_OUT(potm->otmsXHeight);
    DO_OUT(potm->otmrcFontBox.left);
    DO_OUT(potm->otmrcFontBox.top);
    DO_OUT(potm->otmrcFontBox.right);
    DO_OUT(potm->otmrcFontBox.bottom);
    DO_OUT(potm->otmMacAscent);
    DO_OUT(potm->otmMacDescent);
    DO_OUT(potm->otmMacLineGap);
    DO_OUT(potm->otmusMinimumPPEM);
    DO_OUT(potm->otmptSubscriptSize.x);
    DO_OUT(potm->otmptSubscriptSize.y);
    DO_OUT(potm->otmptSubscriptOffset.x);
    DO_OUT(potm->otmptSubscriptOffset.y);
    DO_OUT(potm->otmptSuperscriptSize.x);
    DO_OUT(potm->otmptSuperscriptSize.y);
    DO_OUT(potm->otmptSuperscriptOffset.x);
    DO_OUT(potm->otmptSuperscriptOffset.y);
    DO_OUT(potm->otmsStrikeoutSize);
    DO_OUT(potm->otmsStrikeoutPosition);
    DO_OUT(potm->otmsUnderscoreSize);
    DO_OUT(potm->otmsUnderscorePosition);
}

#define gusLanguageID g_langid
#define ASSERT assert
#define DPRINT printf

/* TPMF_FIXED_PITCH is confusing; brain-dead api */
#ifndef _TMPF_VARIABLE_PITCH
    #define _TMPF_VARIABLE_PITCH    TMPF_FIXED_PITCH
#endif

#define GOT_PENALTY(penalty_name, penalty_value) do { \
    DPRINT("Penalty '%s': %d (%s: Line %d)\n", (penalty_name), (penalty_value), #penalty_value, __LINE__); \
    Penalty += (penalty_value); \
} while (0)

#include "ItalicFromStyle.cpp"
#include "CharSetFromLangID.cpp"
#include "GetFontPenalty.cpp"

int DoFont(HFONT hFont)
{
    ShowVersion();

    if (HGDIOBJ hFontOld = SelectObject(g_hDC, hFont))
    {
        union
        {
            OUTLINETEXTMETRICW otm;
            char buf[512];
        };
        otm.otmSize = sizeof(otm);
        if (!GetOutlineTextMetricsW(g_hDC, sizeof(buf), &otm))
        {
            printf("RET_CANTGETMETRICS\n");
            return RET_CANTGETMETRICS;
        }

        if (g_show_metrics)
        {
            ShowMetrics(&otm);
        }

        LOGFONTW lf;
        ZeroMemory(&lf, sizeof(lf));
        lf.lfHeight = g_lfHeight;
        lf.lfCharSet = g_charset;
        if (g_name)
            MultiByteToWideChar(CP_ACP, 0, g_name, -1, lf.lfFaceName, LF_FACESIZE);

        CHAR szFace[LF_FACESIZE];
        GetTextFaceA(g_hDC, LF_FACESIZE, szFace);
        printf("requested font: \"%s\"\n", g_name ? g_name : "");
        printf("actual font: \"%s\"\n", szFace);

        UINT nPenalty = GetFontPenalty(&lf, &otm, g_style);
        printf("Penalty: %u\n", nPenalty);

        SelectObject(g_hDC, hFontOld);
        return 0;
    }

    printf("RET_CANTSELECTFONT\n");
    return RET_CANTSELECTFONT;
}

int JustDoIt(int argc, char **argv)
{
    if (int ret = ParseCommandLine(argc, argv))
    {
        switch (ret)
        {
        case RET_SHOWHELP:
        case RET_SHOWVERSION:
            return 0;
        default:
            break;
        }
        return ret;
    }

    LOGFONTA lf;
    ZeroMemory(&lf, sizeof(lf));
    lf.lfCharSet = g_charset;
    lf.lfHeight = g_lfHeight;
    if (g_name)
        lstrcpynA(lf.lfFaceName, g_name, LF_FACESIZE);

    if (HFONT hFont = CreateFontIndirectA(&lf))
    {
        if (int ret = DoFont(hFont))
            return ret;

        DeleteObject(hFont);
        return 0;
    }

    printf("RET_CANTCREATEFONT\n");
    return RET_CANTCREATEFONT;
}

int main(int argc, char **argv)
{
    int ret = 0;
    if (g_hDC = CreateCompatibleDC(NULL))
    {
        ret = JustDoIt(argc, argv);
        DeleteDC(g_hDC);
        g_hDC = NULL;
    }
    return ret;
}
