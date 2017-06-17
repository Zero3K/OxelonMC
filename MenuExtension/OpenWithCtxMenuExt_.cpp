// OpenWithCtxMenuExt.cpp : Implementation of COpenWithCtxMenuExt

#include "stdafx.h"
#include "OpenWithExt.h"
#include "OpenWithCtxMenuExt.h"

#pragma comment(lib,"shlwapi")


#define SZV sizeof(VideoMediaFormats)/sizeof(VideoMediaFormats[0])
#define SZA sizeof(AudioMediaFormats)/sizeof(AudioMediaFormats[0])


TCHAR AudioFormatParameters[][34] = {_T("ac3"),_T("adts"),           _T("aiff"),            _T("amr"),           _T("au"),         _T("flac"), _T("mmf"),_T("mp2"),                     _T("mp3"),                     _T("ogg"), _T("voc"),                 _T("wav") };
TCHAR AudioMediaFormats[][45]     = {_T("AC3"),_T("AAC (ADTS)"),     _T("AIFF (Audio IFF)"),_T("AMR (3GPP AMR)"),_T("AU (SUN AU)"),_T("FLAC"), _T("MMF"),_T("MP2 (MPEG Audio Layer 2)"),_T("MP3 (MPEG Audio Layer 3)"),_T("OGG"), _T("VOC (Creative Voice)"),_T("WAV") };

TCHAR VideoFormatParameters[][34] = {_T("3g2"), _T("3gp"),_T("asf"), _T("avi"),_T("dv"),           _T("dvd"),                   _T("flv"),_T("gif -pix_fmt rgb24 -loop_output 0"), _T("mov"), _T("mp4"), _T("mpeg"),                 _T("vob"),                          _T("mpegts"),                      _T("nut"),_T("psp"),      _T("rawvideo"), _T("rm"),_T("svcd"),           _T("swf"),        _T("vcd"),               _T("vob"),           _T("yuv4mpegpipe"), _T("mpeg1video"),       _T("mpeg2video"),       _T("m4v")                    };
TCHAR VideoMediaFormats[][45]     = {_T("3GP2"),_T("3GP"),_T("ASF"), _T("AVI"),_T("DV (DV Video)"),_T("DVD (MPEG2 PS DVD VOB)"),_T("FLV"),_T("GIF"),                               _T("MOV"), _T("MP4"), _T("MPEG1 (MPEG1 System)"), _T("MPEG2 PS (Program Stream)"),    _T("MPEG2 TS (Transport Stream)"), _T("NUT"),_T("PSP (MP4)"),_T("RAW Video"),_T("RM"),_T("SVCD (MPEG2 PS)"),_T("SWF (Flash)"),_T("VCD (MPEG1 System)"),_T("VOB (MPEG2 PS)"),_T("YUV4MPEG Pipe"),_T("MPEG1 Video Only"), _T("MPEG2 Video Only"), _T("M4V (MPEG4 Video Only)") };


/////////////////////////////////////////////////////////////////////////////
// COpenWithCtxMenuExt

HRESULT COpenWithCtxMenuExt::Initialize ( LPCITEMIDLIST pidlFolder,
                                          LPDATAOBJECT pDataObj,
                                          HKEY hProgID )
{
FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
STGMEDIUM stg = { TYMED_HGLOBAL };
HDROP     hDrop;

    // Look for CF_HDROP data in the data object.
    if ( FAILED( pDataObj->GetData ( &fmt, &stg )))
        {
        // Nope! Return an "invalid argument" error back to Explorer.
        return E_INVALIDARG;
        }

    // Get a pointer to the actual data.
    hDrop = (HDROP) GlobalLock ( stg.hGlobal );

    // Make sure it worked.
    if ( NULL == hDrop )
        return E_INVALIDARG;

    // Sanity check - make sure there is at least one filename.
    uNumFiles = DragQueryFile ( hDrop, 0xFFFFFFFF, NULL, 0 );

    if ( 0 == uNumFiles )
        {
        GlobalUnlock ( stg.hGlobal );
        ReleaseStgMedium ( &stg );
        return E_INVALIDARG;
        }

HRESULT hr = S_OK;


	SelectedFiles = NULL;
	SelectedFiles = (TCHAR**)malloc(uNumFiles*sizeof(TCHAR*));
	memset(SelectedFiles,0,uNumFiles*sizeof(TCHAR*));

	unsigned long i;

	for (i=0; i<uNumFiles; i++)
	{
		SelectedFiles[i] = (TCHAR*)malloc(2000*sizeof(TCHAR));

		if ( 0 == DragQueryFile ( hDrop, i, SelectedFiles[i], 2000 )) 
		{
			hr = E_INVALIDARG;
		}
		else
		{
			PathQuoteSpaces ( SelectedFiles[i] );
		}
	}

/*
    // Get the name of the first file and store it in our member variable m_szFile.
    if ( 0 == DragQueryFile ( hDrop, 0, m_szSelectedFile, MAX_PATH ))
        hr = E_INVALIDARG;
    else
        {
        // Quote the name if it contains spaces (needed so the cmd line is built properly)
        PathQuoteSpaces ( m_szSelectedFile );
        }
*/

    GlobalUnlock ( stg.hGlobal );
    ReleaseStgMedium ( &stg );

	OxelonBmp = LoadBitmap ( _Module.GetModuleInstance(),  MAKEINTRESOURCE(IDB_OXELONBITMAP) );

	// get module path with name, after that cut filename
	::GetModuleFileName(::GetModuleHandle(_T("menuext7.dll")),ModuleDirectory,2000*sizeof(TCHAR));
	*((TCHAR *)_tcsrchr(ModuleDirectory,_T('\\'))+1) = NULL;


    return hr;
}

HRESULT COpenWithCtxMenuExt::QueryContextMenu ( HMENU hmenu, UINT  uMenuIndex, 
                                                UINT  uidFirstCmd, UINT  uidLastCmd,
                                                UINT  uFlags )
{
    // If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
    if ( uFlags & CMF_DEFAULTONLY )
        return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 );

    // First, create and populate a submenu.
HMENU hSubmenuVideo = CreatePopupMenu();
UINT uID = uidFirstCmd;


	FirstID = uID;

	unsigned long i;

	for (i=0; i<SZV; i++)
	{
	    InsertMenu ( hSubmenuVideo, i, MF_BYPOSITION, uID++, VideoMediaFormats[i] );
	}

HMENU hSubmenuAudio = CreatePopupMenu();


	for (i=0; i<SZA; i++)
	{
	    InsertMenu ( hSubmenuAudio, i, MF_BYPOSITION, uID++, AudioMediaFormats[i] );
	}


    // Insert the submenu into the ctx menu provided by Explorer.
MENUITEMINFO mii = { sizeof(MENUITEMINFO) };

    mii.fMask = MIIM_SUBMENU | 0x040 |/* MIIM_STRING |*/ MIIM_ID;
    mii.wID = uID++;
    mii.hSubMenu = hSubmenuVideo;
    mii.dwTypeData = _T("Oxelon - Convert to Video");
	
    InsertMenuItem ( hmenu, uMenuIndex, TRUE, &mii );

	if ( NULL != OxelonBmp ) SetMenuItemBitmaps ( hmenu, uMenuIndex, MF_BYPOSITION, OxelonBmp, NULL );



    mii.fMask = MIIM_SUBMENU | 0x040 |/* MIIM_STRING |*/ MIIM_ID;
    mii.wID = uID++;
    mii.hSubMenu = hSubmenuAudio;
    mii.dwTypeData = _T("Oxelon - Convert to Audio");
	
    InsertMenuItem ( hmenu, uMenuIndex, TRUE, &mii );

	if ( NULL != OxelonBmp ) SetMenuItemBitmaps ( hmenu, uMenuIndex, MF_BYPOSITION, OxelonBmp, NULL );

    return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, uID - uidFirstCmd );
}

HRESULT COpenWithCtxMenuExt::GetCommandString ( UINT  idCmd,      UINT uFlags,
                                                UINT* pwReserved, LPSTR pszName,
                                                UINT  cchMax )
{
USES_CONVERSION;

    // Check idCmd, it must be 0 or 1 since we have two menu items.
    if ( idCmd > 1 )
        return E_INVALIDARG;

    // If Explorer is asking for a help string, copy our string into the
    // supplied buffer.
    if ( uFlags & GCS_HELPTEXT )
        {
        LPCTSTR szNotepadText = _T("Open the selected file in Notepad");
        LPCTSTR szIEText = _T("Open the selected file in Internet Explorer");
        LPCTSTR pszText = (0 == idCmd) ? szNotepadText : szIEText;

        if ( uFlags & GCS_UNICODE )
            {
            // We need to cast pszName to a Unicode string, and then use the
            // Unicode string copy API.
            lstrcpynW ( (LPWSTR) pszName, T2CW(pszText), cchMax );
            }
        else
            {
            // Use the ANSI string copy API to return the help string.
            lstrcpynA ( pszName, T2CA(pszText), cchMax );
            }

        return S_OK;
        }

    return E_INVALIDARG;
}

HRESULT COpenWithCtxMenuExt::InvokeCommand ( LPCMINVOKECOMMANDINFO pCmdInfo )
{
    // If lpVerb really points to a string, ignore this function call and bail out.
    if ( 0 != HIWORD( pCmdInfo->lpVerb ))
        return E_INVALIDARG;

 	unsigned long i;

	TCHAR DestinationDirectory[2000];
	TCHAR CommandLineOxelon[2100];

	if ( (LOWORD(pCmdInfo->lpVerb) >= 0) && (LOWORD(pCmdInfo->lpVerb) < SZV+SZA) )
	{

		TCHAR *CommandLineParameters = NULL;
		CommandLineParameters = (TCHAR*)malloc( (uNumFiles+2)*2000*sizeof(TCHAR) );
		_tcscpy(CommandLineParameters, _T("\""));

		_tcscpy(DestinationDirectory, SelectedFiles[0]);
		*((TCHAR *)_tcsrchr(DestinationDirectory,_T('\\'))+1) = NULL;

		_tcscpy(CommandLineOxelon,ModuleDirectory);
		_tcscat(CommandLineOxelon,_T("OxelonMC.exe"));
		

		if ( (LOWORD(pCmdInfo->lpVerb) >= 0) && (LOWORD(pCmdInfo->lpVerb) < SZV) )
		{
			_tcscat(CommandLineParameters, VideoFormatParameters[LOWORD(pCmdInfo->lpVerb)]);
		}

		if ( (LOWORD(pCmdInfo->lpVerb) >= SZV) && (LOWORD(pCmdInfo->lpVerb) < SZV+SZA) )
		{
			_tcscat(CommandLineParameters, AudioFormatParameters[LOWORD(pCmdInfo->lpVerb)-SZV]);
		}

		_tcscat(CommandLineParameters, _T("\" \""));
		_tcscat(CommandLineParameters, DestinationDirectory);

		for (i=0; i<uNumFiles; i++)
		{
			_tcscat(CommandLineParameters, _T("\" \""));
			_tcscat(CommandLineParameters,((TCHAR *)_tcsrchr(SelectedFiles[i],_T('\\'))+1));
		}

		_tcscat(CommandLineParameters, _T("\""));

		::MessageBox(NULL,CommandLineOxelon,CommandLineParameters,MB_OK);


        ShellExecute ( pCmdInfo->hwnd, _T("open"), CommandLineOxelon, CommandLineParameters, NULL, SW_SHOW );

		if (CommandLineParameters != NULL) free(CommandLineParameters);
	}



    for (i=0; i<uNumFiles; i++)
	{
		if (SelectedFiles[i]!=NULL) free(SelectedFiles[i]);
	}
	if (SelectedFiles!=NULL) free(SelectedFiles);

return S_OK;
	
}
