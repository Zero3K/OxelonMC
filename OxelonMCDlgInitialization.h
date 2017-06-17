/* 
 *	Copyright (C) 2008-2009 Oxelon
 *	http://www.oxelon.com
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *   
 */



#include "XFolderDialog.h"
#include "Settings.h"
#include "COMDEF.h"
#include <math.h>


#define converting 0
#define ready -1
#define errors -2
#define completed -3

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//----------------------------------------------------------------

#define COMMON_WIDTH_HEIGHT _T("128x96"), _T("160x120"), _T("176x144"), _T("220x176"), _T("240x192"), _T("320x240"), _T("352x240"), _T("352x288"), _T("384x288"), _T("480x480"), _T("480x576"), _T("640x368"), _T("640x480"), _T("704x480"), _T("704x576"), _T("720x480"), _T("720x540"), _T("720x576"), _T("1024x768"), _T("1280x720"), _T("1280x1024")
#define COMMON_FPS _T("25"), _T("11.988"), _T("12.5"), _T("14.985"), _T("15"), _T("23.98"), _T("29.97"), _T("30")
#define COMMON_VIDEOBITRATES _T("800")

class VideoSettings
{
public:
	TCHAR *WidthHeight[25];
	TCHAR *FPS[20];
	TCHAR *BitRate;
};

#define COMMON_CHANNEL 
#define COMMON_SAMPLING_RATES _T("48000"), _T("44100"), _T("32000"), _T("22050"), _T("16000"), _T("11025"), _T("8000"), _T("4000"), _T("3000"), _T("2000"), _T("1000")
#define COMMON_AUDIOBITRATES 

class AudioSettings
{
public:
	int ChannelsIndex;
	TCHAR *SamplingRate[20];
	TCHAR *BitRate[40];
};


#define FVCSIZE 25
#define FACSIZE 25

class FormatSettings
{
public:
	VideoSettings *VSettings[FVCSIZE];
	TCHAR *VideoParameters[FVCSIZE];
	TCHAR *VideoCodecs[FVCSIZE];
	AudioSettings *ASettings[FACSIZE];
	TCHAR *AudioParameters[FACSIZE];
	TCHAR *AudioCodecs[FACSIZE];
};

// ============================================================================
// 3GP2 VIDEO -----------------------------------------------------------------
// ============================================================================

VideoSettings V_COPY =
{
	{ _T(""), NULL },
	{ _T(""), NULL },
	{ _T("") }
};

VideoSettings V_3GPMPEG4 =
{
	{ _T("176x144"), _T("128x96"), _T("220x176"), _T("240x192"), _T("320x240"), _T("352x288"), NULL },
	{ COMMON_FPS, NULL },
	{ _T("144") }
};

VideoSettings V_3GPXVID =
{
	{ _T("176x144"), _T("128x96"), _T("220x176"), _T("240x192"), _T("320x240"), _T("352x288"), NULL },
	{ COMMON_FPS, NULL },
	{ _T("144") }
};

// 3GP2 AUDIO -----------------------------------------------------------------

AudioSettings A_COPY =
{
	{0},
	{_T(""), NULL},
	{_T(""), NULL}
};

AudioSettings A_3GPAAC =
{
	{3},
	{_T("22050"), _T("11025"), _T("12000"), _T("16000"), _T("24000"), _T("32000"), _T("44100"), _T("48000"), NULL},
	{_T("56"), _T("24"), _T("32"), _T("48"), _T("64"), _T("96"), _T("112"), _T("128"), NULL}
};


// 3GP2 FORMAT ----------------------------------------------------------------

FormatSettings F_3GP2 =
{
	{ &V_3GPMPEG4,   &V_3GPXVID,    &V_COPY,                   NULL},
	{_T("mpeg4"),    _T("libxvid"), _T("copy"),                NULL},
	{_T("MPEG4"),    _T("XviD"),    _T("Direct Stream Copy"),  NULL},
	{ &A_3GPAAC,    &A_COPY,                   NULL},
	{_T("libfaac"), _T("copy"),                NULL},
	{_T("AAC"),     _T("Direct Stream Copy"),  NULL}
};





// ============================================================================
// 3GP VIDEO ------------------------------------------------------------------
// ============================================================================

VideoSettings V_3GPH263 =
{
	{ _T("176x144"), _T("128x96"), _T("352x288"), NULL },
	{ _T("15"), _T("11.988"), _T("12.5"), _T("14.985"), NULL},
	{ _T("144") }
};


// 3GP AUDIO ------------------------------------------------------------------

AudioSettings A_AMRNB =
{
	{2},
	{_T("8000"), NULL},
	{_T("12.2"), _T("4.75"), _T("5.15"), _T("5.9"), _T("6.7"), _T("7.4"), _T("7.95"), _T("10.2"), NULL}
};

AudioSettings A_AMRWB =
{
	{2},
	{_T("16000"), NULL},
	{_T("23.85"), _T("6.6"), _T("8.85"), _T("12.65"), _T("14.25"), _T("15.85"), _T("18.25"), _T("19.85"), _T("23.05"), NULL}
};

// 3GP FORMAT ------------------------------------------------------------------


FormatSettings F_3GP =
{
	{ &V_3GPH263,    &V_3GPMPEG4,  &V_COPY,                   NULL},
	{_T("h263"),     _T("mpeg4"),  _T("copy"),                NULL},
	{_T("H263"),     _T("MPEG4"),  _T("Direct Stream Copy"),  NULL},
	{ &A_AMRNB,        &A_AMRWB,        &A_3GPAAC,    &A_COPY,                   NULL},
	{_T("libamr_nb"), _T("libamr_wb"), _T("libfaac"), _T("copy"),                NULL},
	{_T("AMR NB"),    _T("AMR WB"),    _T("AAC"),     _T("Direct Stream Copy"),  NULL}
};


// ============================================================================
//  AC3 AUDIO -----------------------------------------------------------------
// ============================================================================

AudioSettings A_AC3 =
{
	{3},   
	{_T("48000"), _T("44100"), _T("32000"), NULL},
	{_T("192"), _T("32"), _T("40"),  _T("48"),  _T("56"),  _T("64"),  _T("80"),  _T("96"),  _T("112"),  _T("128"),  _T("160"),  _T("224"),  _T("256"),  _T("320"),  _T("384"),  _T("448"), _T("512"), _T("576"), _T("640"),  NULL}
};

//  AC3 FORMAT ----------------------------------------------------------------

FormatSettings F_AC3 =
{
	{ NULL},
	{ NULL},
	{ NULL},
	{ &A_AC3,    &A_COPY,                   NULL},
	{_T("ac3"),  _T("copy"),                NULL},
	{_T("AC3"),  _T("Direct Stream Copy"),  NULL}
};



// ============================================================================
//  AAC AUDIO -----------------------------------------------------------------
// ============================================================================


AudioSettings A_AAC =
{
	{3},   
	{_T("44100"), _T("11025"), _T("12000"), _T("16000"), _T("22050"), _T("24000"), _T("32000"), _T("48000"), NULL},
	{_T("192"), _T("24"), _T("32"), _T("48"),  _T("56"),  _T("64"),  _T("96"),  _T("112"), _T("128"), _T("160"), _T("224"),  _T("256"),  NULL}
};

//  AAC FORMAT ----------------------------------------------------------------

FormatSettings F_AAC =
{
	{ NULL},
	{ NULL},
	{ NULL},
	{ &A_AAC,        &A_COPY,                   NULL},
	{_T("libfaac"),  _T("copy"),                NULL},
	{_T("AAC"),      _T("Direct Stream Copy"),  NULL}
};




// ============================================================================
//  AIFF AUDIO ----------------------------------------------------------------
// ============================================================================


AudioSettings A_PCMS16BE =
{
	{0},   
	{_T(""), COMMON_SAMPLING_RATES, NULL},
	{NULL}
};

AudioSettings A_PCMS16LE =
{
	{0},   
	{_T(""), COMMON_SAMPLING_RATES, NULL},
	{NULL}
};

AudioSettings A_PCMS24BE =
{
	{0},   
	{_T(""), COMMON_SAMPLING_RATES, NULL},
	{NULL}
};

AudioSettings A_PCMS24LE =
{
	{0},   
	{_T(""), COMMON_SAMPLING_RATES, NULL},
	{NULL}
};

AudioSettings A_PCMS32BE =
{
	{0},   
	{_T(""), COMMON_SAMPLING_RATES, NULL},
	{NULL}
};

AudioSettings A_PCMS32LE =
{
	{0},   
	{_T(""), COMMON_SAMPLING_RATES, NULL},
	{NULL}
};

AudioSettings A_PCMS8 =
{
	{0},   
	{_T(""), COMMON_SAMPLING_RATES, NULL},
	{NULL}
};

AudioSettings A_PCMU16BE =
{
	{0},   
	{_T(""), COMMON_SAMPLING_RATES, NULL},
	{NULL}
};

AudioSettings A_PCMU16LE =
{
	{0},   
	{_T(""), COMMON_SAMPLING_RATES, NULL},
	{NULL}
};

AudioSettings A_PCMU24BE =
{
	{0},   
	{_T(""), COMMON_SAMPLING_RATES, NULL},
	{NULL}
};

AudioSettings A_PCMU24LE =
{
	{0},   
	{_T(""), COMMON_SAMPLING_RATES, NULL},
	{NULL}
};

AudioSettings A_PCMU32BE =
{
	{0},   
	{_T(""), COMMON_SAMPLING_RATES, NULL},
	{NULL}
};

AudioSettings A_PCMU32LE =
{
	{0},   
	{_T(""), COMMON_SAMPLING_RATES, NULL},
	{NULL}
};

// AIFF FORMAT ---------------------------------------------------------------

FormatSettings F_AIFF =
{
	{ NULL},
	{ NULL},
	{ NULL},
	{ &A_PCMS16BE,                           &A_PCMS8,                &A_PCMS16LE,                           &A_PCMS24BE,                        &A_PCMS24LE,                             &A_PCMS32BE,                        &A_PCMS32LE,                           &A_COPY,                   NULL},
	{ _T("pcm_s16be"),                       _T("pcm_s8"),            _T("pcm_s16le"),                       _T("pcm_s24be"),                    _T("pcm_s24le"),                         _T("pcm_s32be"),                    _T("pcm_s32le"),                       _T("copy"),                NULL},
	{ _T("PCM Signed 16 Bit Big-Endian"),    _T("PCM Signed 8 Bit"),  _T("PCM Signed 16 Bit Little-Endian"), _T("PCM Signed 24 Bit Big-Endian"), _T("PCM Signed 24 Bit Little-Endian"),   _T("PCM Signed 32 Bit Big-Endian"), _T("PCM Signed 32 Bit Little-Endian"), _T("Direct Stream Copy"),  NULL}
};





// ============================================================================
//  PCM A LAW AUDIO -----------------------------------------------------------
// ============================================================================

AudioSettings A_PCMALAW =
{
	{0},   
	{_T(""), COMMON_SAMPLING_RATES, NULL},
	{NULL}
};

//  PCM A LAW FORMAT ----------------------------------------------------------

FormatSettings F_PCMALAW =
{
	{ NULL},
	{ NULL},
	{ NULL},
	{ &A_PCMALAW,            &A_COPY,                   NULL},
	{ _T("pcm_alaw"),        _T("copy"),                NULL},
	{ _T("ALAW (PCM A LAW)"),_T("Direct Stream Copy"),  NULL}
};




// ============================================================================
//  AMR FORMAT ----------------------------------------------------------------
// ============================================================================

FormatSettings F_AMR =
{
	{ NULL},
	{ NULL},
	{ NULL},
	{ &A_AMRNB,        &A_AMRWB,      &A_COPY,                   NULL},
	{_T("libamr_nb"), _T("libamr_wb"),_T("copy"),                NULL},
	{_T("AMR NB"),    _T("AMR WB"),   _T("Direct Stream Copy"),  NULL}
};




// ============================================================================
// ASF VIDEO ------------------------------------------------------------------
// ============================================================================

VideoSettings V_MSMPEG4 =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ COMMON_FPS, NULL },
	{ COMMON_VIDEOBITRATES }
};

VideoSettings V_MSMPEG4V2 =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ COMMON_FPS, NULL },
	{ COMMON_VIDEOBITRATES }
};


// ASF AUDIO ------------------------------------------------------------------

AudioSettings A_WMAV2 =
{
	{3},
	{_T("44100"), _T("22050"), _T("16000"), _T("11025"), _T("8000"), NULL},
	{_T("128"), _T("32"), _T("48"), _T("56"), _T("64"), _T("96"), _T("112"), _T("144"), _T("160"), _T("192"), NULL}
};

AudioSettings A_WMAV1 =
{
	{3},
	{_T("44100"), _T("22050"), _T("16000"), _T("11025"), _T("8000"), NULL},
	{_T("128"), _T("32"), _T("48"), _T("56"), _T("64"), _T("96"), _T("112"), _T("144"), _T("160"), _T("192"), NULL}
};

// ASF  FORMAT -----------------------------------------------------------------

FormatSettings F_ASF =
{
	{&V_MSMPEG4V2,     &V_MSMPEG4,    &V_COPY,                   NULL},
	{_T("msmpeg4v2"),  _T("msmpeg4"), _T("copy"),                NULL},
	{_T("MS MPEG4 V2"),_T("MS MPEG4"),_T("Direct Stream Copy"),  NULL},
	{&A_WMAV2,         &A_WMAV1,      &A_COPY,                   NULL},
	{_T("wmav2"),      _T("wmav1"),   _T("copy"),                NULL},
	{_T("WMA V2"),     _T("WMA V1"),  _T("Direct Stream Copy"),  NULL}
};






// ============================================================================
//  AU AUDIO ------------------------------------------------------------------
// ============================================================================

AudioSettings A_PCMMULAW =
{
	{0},   
	{_T(""), COMMON_SAMPLING_RATES, NULL},
	{NULL}
};


// AU FORMAT ------------------------------------------------------------------

FormatSettings F_AU =
{
	{ NULL},
	{ NULL},
	{ NULL},
	{&A_PCMALAW,     &A_PCMMULAW,     &A_PCMS16BE,                           &A_COPY,                   NULL},
	{_T("pcm_alaw"), _T("pcm_mulaw"), _T("pcm_s16be"),                       _T("copy"),                NULL},
	{_T("PCM ALAW"), _T("PCM MULAW"), _T("PCM Signed 16 Bit Big-Endian"),    _T("Direct Stream Copy"),  NULL}
};






// ============================================================================
// AVI VIDEO ------------------------------------------------------------------
// ============================================================================

VideoSettings V_MPEG4 =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ COMMON_FPS, NULL },
	{ COMMON_VIDEOBITRATES }
};

VideoSettings V_XVID =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ COMMON_FPS, NULL },
	{ COMMON_VIDEOBITRATES }
};

VideoSettings V_H261 =
{
	{ _T("352x288"), _T("176x144"), NULL },
	{ COMMON_FPS, NULL },
	{ _T("") }
};

VideoSettings V_H263 =
{
	{ _T("352x288"), _T("128x96"), _T("176x144"), _T("704x576"), _T("1408x1152"), NULL },
	{ COMMON_FPS, NULL },
	{ _T("") }
};

VideoSettings V_H263P =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ COMMON_FPS, NULL },
	{ _T("") }
};

VideoSettings V_H264 =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ COMMON_FPS, NULL },
	{ _T("1400") }
};

VideoSettings V_THEORA =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ COMMON_FPS, NULL },
	{ _T("") }
};

VideoSettings V_DVVIDEO =
{
	{ _T("720x576"), _T("720x480"), NULL },
	{ _T("25"), _T("29.97"), NULL },
	{ COMMON_VIDEOBITRATES }
};

VideoSettings V_SVQ1 =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ COMMON_FPS, NULL },
	{ COMMON_VIDEOBITRATES }
};

VideoSettings V_WMV1 =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ COMMON_FPS, NULL },
	{ COMMON_VIDEOBITRATES }
};

VideoSettings V_WMV2 =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ COMMON_FPS, NULL },
	{ COMMON_VIDEOBITRATES }
};

VideoSettings V_MJPEG =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ COMMON_FPS, NULL },
	{ _T("") }
};

VideoSettings V_FLV =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ COMMON_FPS, NULL },
	{ _T("") }
};

VideoSettings V_MPEG1VIDEO =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ COMMON_FPS, NULL },
	{ _T("") }
};

VideoSettings V_MPEG2VIDEO =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ COMMON_FPS, NULL },
	{ _T("") }
};

VideoSettings V_FFV1 =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ COMMON_FPS, NULL },
	{ COMMON_VIDEOBITRATES }
};

VideoSettings V_FFVHUFF =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ COMMON_FPS, NULL },
	{ COMMON_VIDEOBITRATES }
};

VideoSettings V_ZMBV =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ COMMON_FPS, NULL },
	{ COMMON_VIDEOBITRATES }
};

VideoSettings V_HUFFYUV =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ COMMON_FPS, NULL },
	{ COMMON_VIDEOBITRATES }
};


// AVI AUDIO ------------------------------------------------------------------

// MP3, AC3, MP2, VORBIS, PCM Signed 16 Bit Little-Endian, PCM Unsigned 8 Bit,
// ADPCM MS, ADPCM IMA WAV, WMA V1, WMA V2

AudioSettings A_MP3 =
{
	{3},   
	{_T("44100"), _T("48000"), _T("32000"), _T("22050"), _T("11025"), _T("8000"), NULL},
	{_T("128"), _T("32"), _T("40"),  _T("48"),  _T("56"),  _T("64"),  _T("80"),  _T("96"),  _T("112"), _T("160"), _T("192"), _T("224"),  _T("256"),  _T("320"),  _T("384"),  _T("448"),  NULL}
};

AudioSettings A_MP2 =
{
	{3},   
	{_T("44100"), _T("48000"), _T("32000"), _T("22050"), _T("11025"), _T("8000"), NULL},
	{_T("128"), _T("32"), _T("40"),  _T("48"),  _T("56"),  _T("64"),  _T("80"),  _T("96"),  _T("112"),  _T("160"), _T("192"), _T("224"),  _T("256"),  _T("320"),  _T("384"),  _T("448"),  NULL}
};

AudioSettings A_VORBIS =
{
	{3},   
	{_T("44100"), _T("48000"), _T("24000"), _T("22050"), NULL},
	{_T("192"), _T("32"), _T("40"),  _T("48"),  _T("56"),  _T("64"),  _T("80"),  _T("96"),  _T("112"),  _T("128"),  _T("160"),  _T("224"),  _T("256"),  _T("320"),  _T("384"),  _T("448"),  NULL}
};

AudioSettings A_PCMU8 =
{
	{0},   
	{_T(""), COMMON_SAMPLING_RATES, NULL},
	{NULL}
};

AudioSettings A_ADPCMMS =
{
	{0},   
	{_T(""), COMMON_SAMPLING_RATES, NULL},
	{NULL}
};

AudioSettings A_ADPCMIMAWAV =
{
	{0},   
	{_T(""), COMMON_SAMPLING_RATES, NULL},
	{NULL}
};

AudioSettings A_SONICLS =
{
	{0},   
	{_T(""), COMMON_SAMPLING_RATES, NULL},
	{NULL}
};


// AVI FORMAT -----------------------------------------------------------------

FormatSettings F_AVI =
{
	{&V_MPEG4,                  &V_XVID,      &V_MSMPEG4,     &V_MSMPEG4V2,      &V_H261,    &V_H263,    &V_H263P,    &V_H264,       &V_THEORA,       &V_DVVIDEO,     &V_SVQ1,                   &V_WMV1,    &V_WMV2,    &V_MJPEG,    &V_FLV,    &V_MPEG1VIDEO,     &V_MPEG2VIDEO,     &V_FFV1,    &V_FFVHUFF,     &V_ZMBV,    &V_HUFFYUV,    &V_COPY,                   NULL},
	{_T("mpeg4 -vtag XVID"),    _T("libxvid"),_T("msmpeg4"),  _T("msmpeg4v2"),   _T("h261"), _T("h263"), _T("h263p"), _T("libx264"), _T("libtheora"), _T("dvvideo"),  _T("svq1"),                _T("wmv1"), _T("wmv2"), _T("mjpeg"), _T("flv"), _T("mpeg1video"),  _T("mpeg2video"),  _T("ffv1"), _T("ffvhuff"),  _T("zmbv"), _T("huffyuv"), _T("copy"),                NULL},
	{_T("MPEG4"),               _T("XviD"),   _T("MS MPEG4"), _T("MS MPEG4 V2"), _T("H261"), _T("H263"), _T("H263+"), _T("H264"),    _T("Theora"),    _T("DV Video"), _T("SVQ1 Sorenson Video"), _T("WMV1"), _T("WMV2"), _T("MJPEG"), _T("FLV"), _T("MPEG1 Video"), _T("MPEG2 Video"), _T("FFV1"), _T("FFV HUFF"), _T("ZMBV"), _T("HuffYUV"), _T("Direct Stream Copy"),  NULL},
	{&A_MP3,           &A_AC3,    &A_MP2,    &A_VORBIS,    &A_PCMS16LE,                           &A_PCMU8,                 &A_ADPCMMS,     &A_ADPCMIMAWAV,      &A_WMAV2,         &A_WMAV1,      &A_COPY,                   NULL},
	{_T("libmp3lame"), _T("ac3"), _T("mp2"), _T("vorbis"), _T("pcm_s16le"),                       _T("pcm_u8"),             _T("adpcm_ms"), _T("adpcm_ima_wav"), _T("wmav2"),      _T("wmav1"),   _T("copy"),                NULL},
	{_T("MP3"),        _T("AC3"), _T("MP2"), _T("VORBIS"), _T("PCM Signed 16 Bit Little-Endian"), _T("PCM Unsigned 8 Bit"), _T("ADPCM MS"), _T("ADPCM IMA WAV"), _T("WMA V2"),     _T("WMA V1"),  _T("Direct Stream Copy"),  NULL}
};






// ============================================================================
// DV VIDEO -------------------------------------------------------------------
// ============================================================================

VideoSettings V_DV =
{
	{ _T(""), _T("720x576"), _T("720x480"), NULL },
	{ _T(""), _T("25"), _T("29.97"), NULL },
	{ _T("") }
};

// DV FORMAT ------------------------------------------------------------------

FormatSettings F_DV =
{
	{&V_DV,                                &V_DV,                              &V_COPY,                  NULL},
	{_T("dvvideo -target ntsc-dv "),       _T("dvvideo -target pal-dv "),      _T("copy"),               NULL},
	{_T("DV Video NTSC"),                  _T("DV Video PAL"),                 _T("Direct Stream Copy"), NULL},
	{NULL},
	{NULL},
	{NULL}
};


// ============================================================================
// DVD VOB MPEG2 PS VIDEO -----------------------------------------------------
// ============================================================================

VideoSettings V_MPEG2 =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ _T(""), COMMON_FPS, NULL },
	{ _T("") }
};


// DVD VOB MPEG2 PS FORMAT ----------------------------------------------------

FormatSettings F_DVD =
{
	{&V_MPEG2,                          &V_MPEG2,                           &V_COPY,                  NULL},
	{_T("mpeg2video -target ntsc-dvd "),_T("mpeg2video -target pal-dvd "),  _T("copy"),               NULL},
	{_T("MPEG2 Video NTSC"),            _T("MPEG2 Video PAL"),              _T("Direct Stream Copy"), NULL},
	{&A_AC3,    &A_MP2,    &A_COPY,                   NULL},
	{_T("ac3"), _T("mp2"), _T("copy"),                NULL},
	{_T("AC3"), _T("MP2"), _T("Direct Stream Copy"),  NULL}
};


// ============================================================================
// FFM FORMAT -----------------------------------------------------------------
// ============================================================================

FormatSettings F_FFM =
{
	{&V_MPEG1VIDEO,     &V_COPY,                  NULL},
	{_T("mpeg1video"),  _T("copy"),               NULL},
	{_T("MPEG1 Video"), _T("Direct Stream Copy"), NULL},
	{&A_MP2,    &A_COPY,                   NULL},
	{_T("mp2"), _T("copy"),                NULL},
	{_T("MP2"), _T("Direct Stream Copy"),  NULL}
};

// ============================================================================
// FLAC AUDIO -----------------------------------------------------------------
// ============================================================================

AudioSettings A_FLAC =
{
	{3},
	{_T("44100"), _T("8000"), _T("16000"), _T("22050"), _T("24000"), _T("32000"), _T("48000"), _T("96000"),  NULL},
	{_T(""), NULL}
};

// FLAC FORMAT ----------------------------------------------------------------


FormatSettings F_FLAC =
{
	{ NULL},
	{ NULL},
	{ NULL},
	{&A_FLAC,    &A_COPY,                   NULL},
	{_T("flac"), _T("copy"),                NULL},
	{_T("FLAC"), _T("Direct Stream Copy"),  NULL}
};


// ============================================================================
// FLV VIDEO ------------------------------------------------------------------
// ============================================================================

//enum {
//    FLV_CODECID_H263    = 2,
//    FLV_CODECID_SCREEN  = 3,
//    FLV_CODECID_VP6     = 4,
//    FLV_CODECID_VP6A    = 5,
//    FLV_CODECID_SCREEN2 = 6,
//}; 

// FLV AUDIO ------------------------------------------------------------------

AudioSettings A_FLVMP3 =
{
	{3},   
	{_T("44100"), _T("22050"), _T("11025"), NULL},
	{_T("192"), _T("32"), _T("40"),  _T("48"),  _T("56"),  _T("64"),  _T("80"),  _T("96"),  _T("112"),  _T("128"),  _T("160"),  _T("224"),  _T("256"),  _T("320"),  _T("384"),  _T("448"),  NULL}
};

AudioSettings A_FLVPCMS16BE =
{
	{3},   
	{_T("44100"), _T("22050"), _T("11025"), NULL},
	{NULL}
};

AudioSettings A_FLVPCMS16LE =
{
	{3},   
	{_T("44100"), _T("22050"), _T("11025"), NULL},
	{NULL}
};

AudioSettings A_ADPCMSWF =
{
	{0},   
	{_T(""), COMMON_SAMPLING_RATES, NULL},
	{NULL}
};

AudioSettings A_FLVADPCMSWF =
{
	{3},   
	{_T("44100"), _T("22050"), _T("11025"), NULL},
	{NULL}
};


//enum {
//    FLV_CODECID_PCM_BE              = 0,
//    FLV_CODECID_ADPCM               = 1 << FLV_AUDIO_CODECID_OFFSET,
//    FLV_CODECID_MP3                 = 2 << FLV_AUDIO_CODECID_OFFSET,
//    FLV_CODECID_PCM_LE              = 3 << FLV_AUDIO_CODECID_OFFSET,
//    FLV_CODECID_NELLYMOSER_8HZ_MONO = 5 << FLV_AUDIO_CODECID_OFFSET,
//    FLV_CODECID_NELLYMOSER          = 6 << FLV_AUDIO_CODECID_OFFSET,
//}; 

// FLV FORMAT -----------------------------------------------------------------

FormatSettings F_FLV =
{
	{&V_FLV,    &V_COPY,                  NULL},
	{_T("flv"), _T("copy"),               NULL},
	{_T("FLV"), _T("Direct Stream Copy"), NULL},
	{&A_FLVMP3,        &A_FLVPCMS16BE,                     &A_FLVPCMS16LE,                        &A_FLVADPCMSWF,  &A_COPY,                   NULL},
	{_T("libmp3lame"), _T("pcm_s16be"),                    _T("pcm_s16le"),                       _T("adpcm_swf"), _T("copy"),                NULL},
	{_T("MP3"),        _T("PCM Signed 16 Bit Big-Endian"), _T("PCM Signed 16 Bit Little-Endian"), _T("ADPCM SWF"), _T("Direct Stream Copy"),  NULL}
};



// ============================================================================
// GIF VIDEO ------------------------------------------------------------------
// ============================================================================

VideoSettings V_GIF =
{
	{ _T("64x48"), _T("32x24"), COMMON_WIDTH_HEIGHT, NULL },
	{ _T("10"), COMMON_FPS, NULL },
	{ _T("") }
};

// GIF FORMAT -----------------------------------------------------------------

FormatSettings F_GIF =
{
	{&V_GIF,    NULL},
	{_T(""),    NULL},
	{_T("GIF"), NULL},
	{NULL},
	{NULL},
	{NULL}
};


// ============================================================================
// H261 FORMAT ----------------------------------------------------------------
// ============================================================================

FormatSettings F_H261 =
{
	{&V_H261,    &V_COPY,                  NULL},
	{_T("h261"), _T("copy"),               NULL},
	{_T("H261"), _T("Direct Stream Copy"), NULL},
	{NULL},
	{NULL},
	{NULL}
};


// ============================================================================
// H263 FORMAT ----------------------------------------------------------------
// ============================================================================

FormatSettings F_H263 =
{
	{&V_H263,    &V_COPY,                  NULL},
	{_T("h263"), _T("copy"),               NULL},
	{_T("H263"), _T("Direct Stream Copy"), NULL},
	{NULL},
	{NULL},
	{NULL}
};




// ============================================================================
// H264 VIDEO -----------------------------------------------------------------
// ============================================================================
// H264 FORMAT ----------------------------------------------------------------




// ============================================================================
// M4V FORMAT -----------------------------------------------------------------
// ============================================================================

FormatSettings F_M4V =
{
	{&V_MPEG4,    &V_COPY,                  NULL},
	{_T("mpeg4"), _T("copy"),               NULL},
	{_T("MPEG4"), _T("Direct Stream Copy"), NULL},
	{NULL},
	{NULL},
	{NULL}
};

// ============================================================================
// MJPEG FORMAT ---------------------------------------------------------------
// ============================================================================

FormatSettings F_MJPEG =
{
	{&V_MJPEG,    &V_COPY,                  NULL},
	{_T("mjpeg"), _T("copy"),               NULL},
	{_T("MJPEG"), _T("Direct Stream Copy"), NULL},
	{NULL},
	{NULL},
	{NULL}
};




// ============================================================================
// MMF AUDIO -----------------------------------------------------------------
// ============================================================================

AudioSettings A_MMFPCMS16BE =
{
	{2},   
	{_T("22050"), _T("44100"), _T("11025"), _T("8000"), _T("4000"), NULL},
	{NULL}
};

AudioSettings A_MMFPCMS16LE =
{
	{2},   
	{_T("22050"), _T("44100"), _T("11025"), _T("8000"), _T("4000"), NULL},
	{NULL}
};


AudioSettings A_ADPCMYAMAHA =
{
	{2},   
	{_T("22050"), _T("44100"), _T("11025"), _T("8000"), _T("4000"), NULL},
	{NULL}
};  



// MMF FORMAT -----------------------------------------------------------------

FormatSettings F_MMF =
{
	{ NULL},
	{ NULL},
	{ NULL},
	{&A_ADPCMYAMAHA,     &A_MMFPCMS16BE,                      &A_MMFPCMS16LE,                        &A_COPY,                   NULL},
	{_T("adpcm_yamaha"), _T("pcm_s16be"),                     _T("pcm_s16le"),                       _T("copy"),                NULL},
	{_T("ADPCM YAMAHA"), _T("PCM Signed 16 Bit Big-Endian"),  _T("PCM Signed 16 Bit Little-Endian"), _T("Direct Stream Copy"),  NULL}
};


// ============================================================================
// MOV FORMAT -----------------------------------------------------------------
// ============================================================================

FormatSettings F_MOV =
{
	{&V_MPEG4,              &V_H263,               &V_H264,              &V_SVQ1,                              &V_XVID,                  &V_MJPEG,    &V_COPY,                  NULL},
	{_T("mpeg4"),           _T("h263"),            _T("libx264"),        _T("svq1"),                           _T("libxvid"),            _T("mjpeg"), _T("copy"),               NULL},
	{_T("Quicktime MPEG4"), _T("Quicktime H263"),  _T("Quicktime H264"), _T("Quicktime SVQ1 Sorenson Video"),  _T("Quicktime XviD"),     _T("MJPEG"), _T("Direct Stream Copy"), NULL},
	{&A_AAC,        &A_MP3,            &A_AMRNB,         &A_PCMALAW,     &A_PCMMULAW,        &A_PCMS16BE,                        &A_PCMS16LE,                           &A_COPY,                   NULL},
	{_T("libfaac"), _T("libmp3lame"),  _T("libamr_nb"),  _T("pcm_alaw"), _T("pcm_mulaw"),    _T("pcm_s16be"),                    _T("pcm_s16le"),                       _T("copy"),                NULL},
	{_T("AAC"),     _T("MP3"),         _T("AMR NB"),     _T("PCM ALAW"), _T("PCM MULAW"),    _T("PCM Signed 16 Bit Big-Endian"), _T("PCM Signed 16 Bit Little-Endian"), _T("Direct Stream Copy"),  NULL}
};

// ============================================================================
// MP2 FORMAT -----------------------------------------------------------------
// ============================================================================

FormatSettings F_MP2 =
{
	{ NULL},
	{ NULL},
	{ NULL},
	{ &A_MP2,      &A_COPY,                   NULL},
	{ _T("mp2"),   _T("copy"),                NULL},
	{ _T("MP2"),   _T("Direct Stream Copy"),  NULL}
};

// ============================================================================
// MP3 FORMAT -----------------------------------------------------------------
// ============================================================================

FormatSettings F_MP3 =
{
	{ NULL},
	{ NULL},
	{ NULL},
	{ &A_MP3,           &A_COPY,                   NULL},
	{ _T("libmp3lame"), _T("copy"),                NULL},
	{ _T("MP3"),        _T("Direct Stream Copy"),  NULL}
};



// ============================================================================
// MP4 FORMAT -----------------------------------------------------------------
// ============================================================================

FormatSettings F_MP4 =
{
	{&V_MPEG4,    &V_H263,    &V_H264,       &V_XVID,        &V_COPY,                  NULL},
	{_T("mpeg4"), _T("h263"), _T("libx264"), _T("libxvid"),  _T("copy"),               NULL},
	{_T("MPEG4"), _T("H263"), _T("H264"),    _T("XviD"),     _T("Direct Stream Copy"), NULL},
	{&A_AAC,        &A_AMRNB,        &A_AMRWB,        &A_MP3,            &A_PCMALAW,     &A_PCMS16BE,                        &A_COPY,                   NULL},
	{_T("libfaac"), _T("libamr_nb"), _T("libamr_wb"), _T("libmp3lame"),  _T("pcm_alaw"), _T("pcm_s16be"),                    _T("copy"),                NULL},
	{_T("AAC"),     _T("AMR NB"),    _T("AMR WB"),    _T("MP3"),         _T("PCM ALAW"), _T("PCM Signed 16 Bit Big-Endian"), _T("Direct Stream Copy"),  NULL}
};


// ============================================================================
// MPEG1 FORMAT ---------------------------------------------------------------
// ============================================================================

FormatSettings F_MPEG1 =
{
	{&V_MPEG1VIDEO,     &V_COPY,                   NULL},
	{_T("mpeg1video"),  _T("copy"),                NULL},
	{_T("MPEG1 Video"), _T("Direct Stream Copy"),  NULL},
	{&A_MP2,    &A_MP3,           &A_COPY,                   NULL},
	{_T("mp2"), _T("libmp3lame"), _T("copy"),                NULL},
	{_T("MP2"), _T("MP3"),        _T("Direct Stream Copy"),  NULL}
};


// ============================================================================
// MPEG2 FORMAT ---------------------------------------------------------------
// ============================================================================

FormatSettings F_MPEG2 =
{
	{&V_MPEG2VIDEO,     &V_COPY,                   NULL},
	{_T("mpeg2video"),  _T("copy"),                NULL},
	{_T("MPEG2 Video"), _T("Direct Stream Copy"),  NULL},
	{&A_AC3,    &A_MP2,    &A_MP3,           &A_COPY,                   NULL},
	{_T("ac3"), _T("mp2"), _T("libmp3lame"), _T("copy"),                NULL},
	{_T("AC3"), _T("MP2"), _T("MP3"),        _T("Direct Stream Copy"),  NULL}
};

// ============================================================================
// M1V FORMAT -----------------------------------------------------------------
// ============================================================================

FormatSettings F_M1V =
{
	{&V_MPEG1VIDEO,         &V_COPY,                   NULL},
	{_T("mpeg1video"),      _T("copy"),                NULL},
	{_T("MPEG1 Video"),     _T("Direct Stream Copy"),  NULL},
	{NULL},
	{NULL},
	{NULL}
};



// ============================================================================
// M2V FORMAT -----------------------------------------------------------------
// ============================================================================

FormatSettings F_M2V =
{
	{&V_MPEG2VIDEO,          &V_COPY,                   NULL},
	{_T("mpeg2video"),       _T("copy"),                NULL},
	{_T("MPEG2 Video"),      _T("Direct Stream Copy"),  NULL},
	{NULL},
	{NULL},
	{NULL}
};


// ============================================================================
// MPEG2 TS FORMAT ------------------------------------------------------------
// ============================================================================

FormatSettings F_MPEG2TS =
{
	{&V_MPEG2VIDEO,     &V_COPY,                   NULL},
	{_T("mpeg2video"),  _T("copy"),                NULL},
	{_T("MPEG2 Video"), _T("Direct Stream Copy"),  NULL},
	{&A_AC3,    &A_MP2,    &A_MP3,           &A_COPY,                   NULL},
	{_T("mp2"), _T("ac3"), _T("libmp3lame"), _T("copy"),                NULL},
	{_T("MP2"), _T("AC3"), _T("MP3"),        _T("Direct Stream Copy"),  NULL}
};


// ============================================================================
// MPJPEG FORMAT --------------------------------------------------------------
// ============================================================================

FormatSettings F_MPJPEG =
{
	{&V_MJPEG,    &V_COPY,                  NULL},
	{_T("mjpeg"), _T("copy"),               NULL},
	{_T("MJPEG"), _T("Direct Stream Copy"), NULL},
	{NULL},
	{NULL},
	{NULL}
};


// ============================================================================
// MULAW FORMAT ---------------------------------------------------------------
// ============================================================================

FormatSettings F_MULAW =
{
	{NULL},
	{NULL},
	{NULL},
	{&A_PCMMULAW,     &A_COPY,                   NULL},
	{_T("pcm_mulaw"), _T("copy"),                NULL},
	{_T("PCM MULAW"), _T("Direct Stream Copy"),  NULL}
};


// ============================================================================
// NUT FORMAT -----------------------------------------------------------------
// ============================================================================

FormatSettings F_NUT =
{
	{&V_MPEG4,       &V_XVID,      &V_MSMPEG4,     &V_MSMPEG4V2,      &V_H261,    &V_H263,    &V_H263P,    &V_THEORA,       &V_DVVIDEO,     &V_SVQ1,                   &V_WMV1,    &V_WMV2,    &V_MJPEG,    &V_FLV,    &V_MPEG1VIDEO,     &V_MPEG2VIDEO,     &V_FFV1,    &V_FFVHUFF,     &V_ZMBV,    &V_HUFFYUV,    &V_COPY,                   NULL},
	{_T("mpeg4"),    _T("libxvid"),_T("msmpeg4"),  _T("msmpeg4v2"),   _T("h261"), _T("h263"), _T("h263p"), _T("libtheora"), _T("dvvideo"),  _T("svq1"),                _T("wmv1"), _T("wmv2"), _T("mjpeg"), _T("flv"), _T("mpeg1video"),  _T("mpeg2video"),  _T("ffv1"), _T("ffvhuff"),  _T("zmbv"), _T("huffyuv"), _T("copy"),                NULL},
	{_T("MPEG4"),    _T("XviD"),   _T("MS MPEG4"), _T("MS MPEG4 V2"), _T("H261"), _T("H263"), _T("H263+"), _T("Theora"),    _T("DV Video"), _T("SVQ1 Sorenson Video"), _T("WMV1"), _T("WMV2"), _T("MJPEG"), _T("FLV"), _T("MPEG1 Video"), _T("MPEG2 Video"), _T("FFV1"), _T("FFV HUFF"), _T("ZMBV"), _T("HuffYUV"), _T("Direct Stream Copy"),  NULL},
	{&A_MP3,           &A_AC3,    &A_MP2,    &A_VORBIS,    &A_PCMS16LE,                           &A_PCMU8,                 &A_ADPCMMS,     &A_ADPCMIMAWAV,      &A_WMAV2,         &A_WMAV1,      &A_COPY,                   NULL},
	{_T("libmp3lame"), _T("ac3"), _T("mp2"), _T("vorbis"), _T("pcm_s16le"),                       _T("pcm_u8"),             _T("adpcm_ms"), _T("adpcm_ima_wav"), _T("wmav2"),      _T("wmav1"),   _T("copy"),                NULL},
	{_T("MP3"),        _T("AC3"), _T("MP2"), _T("VORBIS"), _T("PCM Signed 16 Bit Little-Endian"), _T("PCM Unsigned 8 Bit"), _T("ADPCM MS"), _T("ADPCM IMA WAV"), _T("WMA V2"),     _T("WMA V1"),  _T("Direct Stream Copy"),  NULL}
};


// ============================================================================
// OGG Audio FORMAT -----------------------------------------------------------------
// ============================================================================

FormatSettings F_OGG =
{
	{NULL},
	{NULL},
	{NULL},
	{&A_VORBIS,     &A_COPY,                   NULL},
	{_T("vorbis"),  _T("copy"),                NULL},
	{_T("VORBIS"),  _T("Direct Stream Copy"),  NULL}
};


// ============================================================================
// OGG Video and Audio FORMAT -----------------------------------------------------------------
// ============================================================================

FormatSettings F_OGGVA =
{
	{&V_THEORA,       &V_COPY,                   NULL},
	{_T("libtheora"), _T("copy"),                NULL},
	{_T("Theora"),    _T("Direct Stream Copy"),  NULL},
	{&A_VORBIS,     &A_COPY,                   NULL},
	{_T("vorbis"),  _T("copy"),                NULL},
	{_T("VORBIS"),  _T("Direct Stream Copy"),  NULL}
};



// ============================================================================
// PSP FORMAT -----------------------------------------------------------------
// ============================================================================

FormatSettings F_PSP =
{
	{&V_MPEG4,    &V_H263,    &V_H264,       &V_XVID,        &V_COPY,                  NULL},
	{_T("mpeg4"), _T("h263"), _T("libx264"), _T("libxvid"),  _T("copy"),               NULL},
	{_T("MPEG4"), _T("H263"), _T("H264"),    _T("XviD"),     _T("Direct Stream Copy"), NULL},
	{&A_AAC,        &A_AMRNB,        &A_AMRWB,        &A_MP3,            &A_PCMALAW,     &A_PCMS16BE,                        &A_COPY,                   NULL},
	{_T("libfaac"), _T("libamr_nb"), _T("libamr_wb"), _T("libmp3lame"),  _T("pcm_alaw"), _T("pcm_s16be"),                    _T("copy"),                NULL},
	{_T("AAC"),     _T("AMR NB"),    _T("AMR WB"),    _T("MP3"),         _T("PCM ALAW"), _T("PCM Signed 16 Bit Big-Endian"), _T("Direct Stream Copy"),  NULL}
};


// ============================================================================
// RAW VIDEO ------------------------------------------------------------------
// ============================================================================

VideoSettings V_RAWVIDEO =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ COMMON_FPS, NULL },
	{ _T("") }
};

// RAW FORMAT -----------------------------------------------------------------

FormatSettings F_RAWVIDEO =
{
	{&V_RAWVIDEO,    &V_COPY,                   NULL},
	{_T("rawvideo"), _T("copy"),                NULL},
	{_T("RAW Video"),_T("Direct Stream Copy"),  NULL},
	{NULL},
	{NULL},
	{NULL}
};


// ============================================================================
// RM VIDEO -------------------------------------------------------------------
// ============================================================================

VideoSettings V_RV10 =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ COMMON_FPS, NULL },
	{ _T("") }
};

VideoSettings V_RV20 =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ COMMON_FPS, NULL },
	{ _T("") }
};


// RM FORMAT ------------------------------------------------------------------

FormatSettings F_RM =
{
	{&V_RV10,             &V_RV20,              &V_COPY,                   NULL},
	{_T("rv10"),          _T("rv20"),           _T("copy"),                NULL},
	{_T("Real Video 10"), _T("Real Video 20"),  _T("Direct Stream Copy"),  NULL},
	{&A_AC3,    &A_AAC,         &A_COPY,                   NULL},
	{_T("ac3"), _T("libfaac"),  _T("copy"),                NULL},
	{_T("AC3"), _T("AAC"),      _T("Direct Stream Copy"),  NULL}
};


// ============================================================================
// SVCD VOB MPEG2 FORMAT ------------------------------------------------------
// ============================================================================

FormatSettings F_SVCD =
{
	{&V_MPEG2,                            &V_MPEG2,                            &V_COPY,                   NULL},
	{_T("mpeg2video -target ntsc-svcd "), _T("mpeg2video -target pal-svcd "),  _T("copy"),                NULL},
	{_T("MPEG2 Video NTSC"),              _T("MPEG2 Video PAL"),               _T("Direct Stream Copy"),  NULL},
	{&A_MP2,    &A_COPY,                   NULL},
	{_T("mp2"), _T("copy"),                NULL},
	{_T("MP2"), _T("Direct Stream Copy"),  NULL}
};

// ============================================================================
// SWF FORMAT -----------------------------------------------------------------
// ============================================================================

FormatSettings F_SWF =
{
	{&V_FLV,    &V_MJPEG,    &V_COPY,                  NULL},
	{_T("flv"), _T("mjpeg"), _T("copy"),               NULL},
	{_T("FLV"), _T("MJPEG"), _T("Direct Stream Copy"), NULL},
	{&A_FLVMP3,        /*&A_FLVPCMS16BE,                     &A_FLVPCMS16LE,                        &A_FLVADPCMSWF, */ &A_COPY,                   NULL},
	{_T("libmp3lame"), /*_T("pcm_s16be"),                    _T("pcm_s16le"),                       _T("adpcm_swf"),*/ _T("copy"),                NULL},
	{_T("MP3"),        /*_T("PCM Signed 16 Bit Big-Endian"), _T("PCM Signed 16 Bit Little-Endian"), _T("ADPCM SWF"),*/ _T("Direct Stream Copy"),  NULL}
};



// ============================================================================
// U16BE FORMAT ---------------------------------------------------------------
// ============================================================================

FormatSettings F_U16BE =
{
	{NULL},
	{NULL},
	{NULL},
	{&A_PCMU16BE,                           &A_COPY,                   NULL},
	{_T("pcm_u16be"),                       _T("copy"),                NULL},
	{_T("PCM Unsigned 16 Bit Big-Endian"),  _T("Direct Stream Copy"),  NULL}
};


// ============================================================================
// U16LE FORMAT ---------------------------------------------------------------
// ============================================================================

FormatSettings F_U16LE =
{
	{NULL},
	{NULL},
	{NULL},
	{&A_PCMU16LE,                             &A_COPY,                   NULL},
	{_T("pcm_u16le"),                         _T("copy"),                NULL},
	{_T("PCM Unsigned 16 Bit Little-Endian"), _T("Direct Stream Copy"),  NULL}
};



// ============================================================================
// U8 FORMAT ------------------------------------------------------------------
// ============================================================================

FormatSettings F_U8 =
{
	{NULL},
	{NULL},
	{NULL},
	{&A_PCMU8,                  &A_COPY,                   NULL},
	{_T("pcm_u8"),              _T("copy"),                NULL},
	{_T("PCM Unsigned 8 Bit"),  _T("Direct Stream Copy"),  NULL}
};


// ============================================================================
// VCD MPEG1 FORMAT -----------------------------------------------------------
// ============================================================================

VideoSettings V_MPEG1 =
{
	{ _T(""), COMMON_WIDTH_HEIGHT, NULL },
	{ _T(""), COMMON_FPS, NULL },
	{ _T("") }
};

FormatSettings F_VCD =
{
	{&V_MPEG1,                           &V_MPEG1,                            &V_COPY,                   NULL},
	{_T("mpeg1video -target ntsc-vcd "), _T("mpeg1video -target pal-vcd "),   _T("copy"),                NULL},
	{_T("MPEG1 Video NTSC"),             _T("MPEG1 Video PAL"),               _T("Direct Stream Copy"),  NULL},
	{&A_MP2,    &A_COPY,                   NULL},
	{_T("mp2"), _T("copy"),                NULL},
	{_T("MP2"), _T("Direct Stream Copy"),  NULL}
}; 


// ============================================================================
// VOB MPEG2 PS FORMAT --------------------------------------------------------
// ============================================================================

FormatSettings F_VOB =
{
	{&V_MPEG2VIDEO,     &V_COPY,                   NULL},
	{_T("mpeg2video"),  _T("copy"),                NULL},
	{_T("MPEG2 Video"), _T("Direct Stream Copy"),  NULL},
	{&A_AC3,    &A_MP2,    &A_MP3,           &A_COPY,                   NULL},
	{_T("ac3"), _T("mp2"), _T("libmp3lame"), _T("copy"),                NULL},
	{_T("AC3"), _T("MP2"), _T("MP3"),        _T("Direct Stream Copy"),  NULL}
};

// ============================================================================
// VOC FORMAT -----------------------------------------------------------------
// ============================================================================

FormatSettings F_VOC =
{
	{NULL},
	{NULL},
	{NULL},
	{&A_PCMS16LE,                           &A_PCMALAW,             &A_PCMMULAW,     &A_PCMU8,                  &A_COPY,                   NULL},
	{_T("pcm_s16le"),                       _T("pcm_alaw"),         _T("pcm_mulaw"), _T("pcm_u8"),              _T("copy"),                NULL},
	{_T("PCM Signed 16 Bit Little-Endian"), _T("PCM ALAW"),         _T("PCM MULAW"), _T("PCM Unsigned 8 Bit"),  _T("Direct Stream Copy"),  NULL}
};

// ============================================================================
// WAV AUDIO ------------------------------------------------------------------
// ============================================================================

// WAV FORMAT -----------------------------------------------------------------

FormatSettings F_WAV =
{
	{NULL},
	{NULL},
	{NULL},
	{&A_PCMS16LE,                           &A_PCMS24LE,                             &A_PCMS32LE,                           &A_PCMALAW,             &A_PCMMULAW,     &A_PCMU8,                  &A_ADPCMMS,     &A_ADPCMIMAWAV,      &A_ADPCMSWF,     &A_ADPCMYAMAHA,     &A_AAC,        &A_AC3,    &A_FLAC,    &A_MP2,    &A_MP3,            &A_SONICLS,    &A_VORBIS,    &A_WMAV1,      &A_WMAV2,         &A_COPY,                   NULL},
	{_T("pcm_s16le"),                       _T("pcm_s24le"),                         _T("pcm_s32le"),                       _T("pcm_alaw"),         _T("pcm_mulaw"), _T("pcm_u8"),              _T("adpcm_ms"), _T("adpcm_ima_wav"), _T("adpcm_swf"), _T("adpcm_yamaha"), _T("libfaac"), _T("ac3"), _T("flac"), _T("mp2"), _T("libmp3lame"),  _T("sonicls"), _T("vorbis"), _T("wmav1"),   _T("wmav2"),      _T("copy"),                NULL},
	{_T("PCM Signed 16 Bit Little-Endian"), _T("PCM Signed 24 Bit Little-Endian"),   _T("PCM Signed 32 Bit Little-Endian"), _T("PCM ALAW"),         _T("PCM MULAW"), _T("PCM Unsigned 8 Bit"),  _T("ADPCM MS"), _T("ADPCM IMA WAV"), _T("ADPCM SWF"), _T("ADPCM YAMAHA"), _T("AAC"),     _T("AC3"), _T("FLAC"), _T("MP2"), _T("MP3"),         _T("SONICLS"), _T("VORBIS"), _T("WMA V1"),  _T("WMA V2"),     _T("Direct Stream Copy"),  NULL}
};

// ============================================================================
// Y4M VIDEO ------------------------------------------------------------------
// ============================================================================

// Y4M FORMAT -----------------------------------------------------------------

FormatSettings F_YUV4MPEG =
{
	{NULL},
	{NULL},
	{NULL},
	{NULL},
	{NULL},
	{NULL}
};

// ============================================================================
// S16BE FORMAT ---------------------------------------------------------------
// ============================================================================

FormatSettings F_S16BE =
{
	{NULL},
	{NULL},
	{NULL},
	{&A_PCMS16BE,                           &A_COPY,                   NULL},
	{_T("pcm_s16be"),                       _T("copy"),                NULL},
	{_T("PCM Signed 16 Bit Big-Endian"),    _T("Direct Stream Copy"),  NULL}
};


// ============================================================================
// S16LE FORMAT ---------------------------------------------------------------
// ============================================================================

FormatSettings F_S16LE =
{
	{NULL},
	{NULL},
	{NULL},
	{&A_PCMS16LE,                           &A_COPY,                   NULL},
	{_T("pcm_s16le"),                       _T("copy"),                NULL},
	{_T("PCM Signed 16 Bit Little-Endian"), _T("Direct Stream Copy"),  NULL}
};


// ============================================================================
// S8 FORMAT ------------------------------------------------------------------
// ============================================================================

FormatSettings F_S8 =
{
	{NULL},
	{NULL},
	{NULL},
	{&A_PCMS8,                &A_COPY,                   NULL},
	{_T("pcm_s8"),            _T("copy"),                NULL},
	{_T("PCM Signed 8 Bit"),  _T("Direct Stream Copy"),  NULL}
};


#define SZF sizeof(MediaFormats)/sizeof(MediaFormats[0])

TCHAR FormatParameters[][34] = {_T("3g2"), _T("3gp"),_T("ac3"),_T("adts"),           _T("aiff"),            _T("amr"),           _T("asf"),/*_T("asf_stream"),*/ _T("au"),         _T("avi"),_T("dv"),           _T("dvd"),                   _T("ffm"),_T("flac"),_T("flv"),_T("gif -pix_fmt rgb24 -loop_output 0"), /*_T("h264"),*/ /*_T("image2"),*/          _T("mmf"),_T("mov"),_T("mp2"),                     _T("mp3"),                     _T("mp4"), _T("mpeg"),                 _T("vob"),                          _T("mpegts"),                      _T("nut"),_T("ogg -vn"),     _T("ogg"),                 _T("psp"),      _T("rawvideo"), _T("rm"),_T("svcd"),           _T("swf"),        _T("vcd"),               _T("vob"),           _T("voc"),                 _T("wav"),_T("yuv4mpegpipe"), _T("mpeg1video"),       _T("mpeg2video"),       _T("m4v"),                    _T("h261"), _T("h263"),  _T("mjpeg"),      _T("mpjpeg"),                      _T("u16be"),                                 _T("u16le"),                                    _T("u8"),                     _T("s16be"),                               _T("s16le"),                                  _T("s8"),                    _T("mulaw"),             _T("alaw")            };
TCHAR Extensions[][5]        = {_T("3g2"), _T("3gp"),_T("ac3"),_T("aac"),            _T("aiff"),            _T("amr"),           _T("asf"),/*_T("asf"),       */ _T("au"),         _T("avi"),_T("dv"),           _T("vob"),                   _T("ffm"),_T("flac"),_T("flv"),_T("gif"),                               /*_T("h264"),*/ /*_T(""),*/                _T("mmf"),_T("mov"),_T("mp2"),                     _T("mp3"),                     _T("mp4"), _T("mpg"),                  _T("mpg"),                          _T("mpg"),                         _T("nut"),_T("ogg"),         _T("ogg"),                 _T("mp4"),      _T("yuv"),      _T("rm"),_T("mpg"),            _T("swf"),        _T("mpg"),               _T("vob"),           _T("voc"),                 _T("wav"),_T("y4m"),          _T("m1v"),              _T("m2v"),              _T("m4v"),                    _T("h261"), _T("h263"),  _T("mjpg"),       _T("mjpg"),                        _T("raw"),                                   _T("raw"),                                      _T("raw"),                    _T("raw"),                                 _T("raw"),                                    _T("raw"),                   _T("raw"),               _T("raw")             };
TCHAR MediaFormats[][45]     = {_T("3GP2"),_T("3GP"),_T("AC3"),_T("AAC (ADTS)"),     _T("AIFF (Audio IFF)"),_T("AMR (3GPP AMR)"),_T("ASF"),/*_T("ASF STREAM"),*/ _T("AU (SUN AU)"),_T("AVI"),_T("DV (DV Video)"),_T("DVD (MPEG2 PS DVD VOB)"),_T("FFM"),_T("FLAC"),_T("FLV"),_T("GIF"),                               /*_T("H264"),*/ /*_T("Image2 sequence"),*/ _T("MMF"),_T("MOV"),_T("MP2 (MPEG Audio Layer 2)"),_T("MP3 (MPEG Audio Layer 3)"),_T("MP4"), _T("MPEG1 (MPEG1 System)"), _T("MPEG2 PS (Program Stream)"),    _T("MPEG2 TS (Transport Stream)"), _T("NUT"),_T("OGG Audio"),   _T("OGG Video and Audio"), _T("PSP (MP4)"),_T("RAW Video"),_T("RM"),_T("SVCD (MPEG2 PS)"),_T("SWF (Flash)"),_T("VCD (MPEG1 System)"),_T("VOB (MPEG2 PS)"),_T("VOC (Creative Voice)"),_T("WAV"),_T("YUV4MPEG Pipe"),_T("MPEG1 Video Only"), _T("MPEG2 Video Only"), _T("M4V (MPEG4 Video Only)"), _T("H261"), _T("H263"),  _T("MJPEG video"),_T("MPJPEG (Mime multipart JPEG)"),_T("U16BE (PCM UNSIGNED 16 BIT BIG ENDIAN)"),_T("U16LE (PCM UNSIGNED 16 BIT LITTLE ENDIAN)"),_T("U8 (PCM UNSIGNED 8 BIT)"),_T("S16BE (PCM SIGNED 16 BIT BIG ENDIAN)"),_T("S16LE (PCM SIGNED 16 BIT LITTLE ENDIAN)"),_T("S8 (PCM SIGNED 8 BIT)"), _T("MULAW (PCM MU LAW)"),_T("ALAW (PCM A LAW)")};
FormatSettings *FPointers[SZF]={&F_3GP2,   &F_3GP,   &F_AC3,   &F_AAC,               &F_AIFF,               &F_AMR,              &F_ASF,   /*&F_ASF,          */ &F_AU,            &F_AVI,   &F_DV,              &F_DVD,                      &F_FFM,   &F_FLAC,   &F_FLV,   &F_GIF,                                  /*NULL,*/       /*NULL,*/                  &F_MMF,   &F_MOV,   &F_MP2,                        &F_MP3,                        &F_MP4,    &F_MPEG1,                   &F_MPEG2,                           &F_MPEG2TS,                        &F_NUT,   &F_OGG,            &F_OGGVA,                  &F_PSP,         &F_RAWVIDEO,    &F_RM,   &F_SVCD,              &F_SWF,           &F_VCD,                  &F_VOB,              &F_VOC,                    &F_WAV,   &F_YUV4MPEG,        &F_M1V,                 &F_M2V,                 &F_M4V,                       &F_H261,    &F_H263,     &F_MJPEG,         &F_MPJPEG,                         &F_U16BE,                                    &F_U16LE,                                       &F_U8,                        &F_S16BE,                                  &F_S16LE,                                     &F_S8,                       &F_MULAW,                &F_PCMALAW            };

TCHAR VideoParameters[][20]  = {_T(""),        _T("copy"),               /*_T("asv1"), _T("asv2"),*/ _T("bmp"), _T("dvvideo"), _T("ffv1"), _T("ffvhuff"), _T("flashsv"), _T("flv"), _T("gif"), _T("h261"), _T("h263"), _T("h263p"), _T("libx264"), _T("huffyuv"), _T("jpegls"), _T("libtheora"), _T("libxvid"), _T("ljpeg"), _T("mjpeg"), _T("mpeg1video"), _T("mpeg2video"), _T("mpeg4"), _T("msmpeg4"), _T("msmpeg4v1"),  _T("msmpeg4v2"),  _T("pam"),_T("pbm"),_T("pgm"), _T("pgmyuv"), _T("png"), _T("ppm"), _T("rawvideo"), _T("rv10"),         _T("rv20"),           _T("sgi"), _T("snow"),_T("svq1"),                _T("targa"), _T("tiff"), _T("vp6"), _T("wmv1"), _T("wmv2"), _T("zlib"), _T("zmbv")};
TCHAR VideoCodecs[][20]      = {_T("Default"), _T("Direct Stream Copy"), /*_T("ASV1"), _T("ASV2"),*/ _T("BMP"), _T("DV Video"),_T("FFV1"), _T("FFV HUFF"),_T("FlashSV"), _T("FLV"), _T("GIF"), _T("H261"), _T("H263"), _T("H263+"), _T("H264"),    _T("HuffYUV"), _T("JPEGLS"), _T("Theora"),  _T("XviD"),    _T("LJPEG"), _T("MJPEG"), _T("MPEG1 Video"),_T("MPEG2 Video"),_T("MPEG4"), _T("MS MPEG4"),_T("MS MPEG4 V1"),_T("MS MPEG4 V2"),_T("PAM"),_T("PBM"),_T("PGM"), _T("PGM YUV"),_T("PNG"), _T("PPM"), _T("RAW Video"),_T("Real Video 10"),_T("Real Video 20"),  _T("SGI"), _T("SNOW"),_T("SVQ1 Sorenson Video"), _T("Targa"), _T("TIFF"), _T("VP6"), _T("WMV1"), _T("WMV2"), _T("ZLIB"), _T("ZMBV")};

TCHAR AudioParameters[][20]  = {_T(""),        _T("copy"),               _T("ac3"), /*_T("adpcm_4xm"), _T("adpcm_adx"), _T("adpcm_ct"), _T("adpcm_ea"), _T("adpcm_ima_dk3"), _T("adpcm_ima_dk4"), _T("adpcm_ima_qt"), _T("adpcm_ima_smjpeg"),*/ _T("adpcm_ima_wav"), /*_T("adpcm_ima_ws"),*/ _T("adpcm_ms"), /*_T("adpcm_sbpro_2"), _T("adpcm_sbpro_3"), _T("adpcm_sbpro_4"),*/ _T("adpcm_swf"), /*_T("adpcm_xa"),*/ _T("adpcm_yamaha"), _T("flac"), _T("g726"), _T("libamr_nb"), _T("libamr_wb"), _T("libfaac"), _T("libgsm"), _T("libgsm_ms"), _T("libmp3lame"), _T("mp2"), _T("pcm_alaw"), _T("pcm_mulaw"), _T("pcm_s16be"),                    _T("pcm_s16le"),                       _T("pcm_s24be"),                    _T("pcm_s24daud"),                      _T("pcm_s24le"),                       _T("pcm_s32be"),                    _T("pcm_s32le"),                       _T("pcm_s8"),           _T("pcm_u16be"),                      _T("pcm_u16le"),                         _T("pcm_u24be"),                      _T("pcm_u24le"),                         _T("pcm_u32be"),                      _T("pcm_u32le"),                         _T("pcm_u8"),              _T("sonic"), _T("sonicls"), _T("vorbis"), _T("wmav1"), _T("wmav2")};
TCHAR AudioCodecs[][40]      = {_T("Default"), _T("Direct Stream Copy"), _T("AC3"), /*_T("ADPCM 4XM"), _T("ADPCM ADX"), _T("ADPCM CT"), _T("ADPCM EA"), _T("ADPCM IMA DK3"), _T("ADPCM IMA DK4"), _T("ADPCM IMA QT"), _T("ADPCM IMA SMJPEG"),*/ _T("ADPCM IMA WAV"), /*_T("ADPCM IMA WS"),*/ _T("ADPCM MS"), /*_T("ADPCM SBPRO 2"), _T("ADPCM SBPRO 3"), _T("ADPCM SBPRO 4"),*/ _T("ADPCM SWF"), /*_T("ADPCM XA"),*/ _T("ADPCM YAMAHA"), _T("FLAC"), _T("G726"), _T("AMR NB"),    _T("AMR WB"),    _T("AAC"),     _T("GSM"),    _T("GSM MS"),    _T("MP3"),        _T("MP2"), _T("PCM ALAW"), _T("PCM MULAW"), _T("PCM Signed 16 Bit Big-Endian"), _T("PCM Signed 16 Bit Little-Endian"), _T("PCM Signed 24 Bit Big-Endian"), _T("PCM Signed 24 Bit D-Cinema Audio"), _T("PCM Signed 24 Bit Little-Endian"), _T("PCM Signed 32 Bit Big-Endian"), _T("PCM Signed 32 Bit Little-Endian"), _T("PCM Signed 8 Bit"), _T("PCM Unsigned 16 Bit Big-Endian"), _T("PCM Unsigned 16 Bit Little-Endian"), _T("PCM Unsigned 24 Bit Big-Endian"), _T("PCM Unsigned 24 Bit Little-Endian"), _T("PCM Unsigned 32 Bit Big-Endian"), _T("PCM Unsigned 32 Bit Little-Endian"), _T("PCM Unsigned 8 Bit"),  _T("SONIC"), _T("SONICLS"), _T("VORBIS"), _T("WMA V1"),_T("WMA V2")};


int Channels[]               = {-1,            0,              1,          2,            6};
TCHAR ChannelsText[][15]     = {_T("Default"), _T("No Audio"), _T("Mono"), _T("Stereo"), _T("Surround")};

TCHAR WidthHeight[][20]      = {COMMON_WIDTH_HEIGHT};

TCHAR FramesPerSecond[][10]  = {_T("11.988"), _T("12.5"), _T("14.985"), _T("15"), _T("23.98"), _T("25"), _T("29.97"), _T("30")};

TCHAR SamplingRates[][20]    = {_T("22050"), _T("24000"), _T("44100"), _T("48000")};








class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// COxelonMCDlg dialog

COxelonMCDlg::COxelonMCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COxelonMCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


/* ----------------------------------------------------------------------------------------------- */
BOOL COxelonMCDlg::OnInitDialog()
/* ----------------------------------------------------------------------------------------------- */
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	
	LoadConfiguration();
	InitializeInterface();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
