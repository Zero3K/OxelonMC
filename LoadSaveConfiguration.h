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



/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::LoadConfiguration()
/* ----------------------------------------------------------------------------------------------- */
{
	// Get Application Data Directory
	#ifdef _UNICODE	
		SHGetFolderPath(NULL,CSIDL_APPDATA,NULL,0,ConfigurationFileName);	
		_tcscat(ConfigurationFileName, _T("\\OxelonMC"));
		
		WIN32_FIND_DATA Win32FindData;
		HANDLE FileHandle;
		FileHandle = FindFirstFile(ConfigurationFileName, &Win32FindData);
		if (FileHandle == INVALID_HANDLE_VALUE)
		{
			CreateDirectory(ConfigurationFileName,NULL);
		}
		else
		{
			FindClose(FileHandle);
		}
	
	#else
		::GetModuleFileName(::GetModuleHandle(NULL),ConfigurationFileName,1024);
		*(char *)strrchr(ConfigurationFileName,'\\')=NULL;
	#endif
	
	
	_tcscat(ConfigurationFileName, _T("\\omc.cfg"));
	
	if (ConfigurationFile.Open(ConfigurationFileName, CFile::modeRead)==yes) 
	{
//	    ConfigurationFile.Read(&Configuration,ConfigurationFile.GetLength());
	    LoadData();
	    ConfigurationFile.Close();
	}
	else
	{
		InitConfiguration();
	}
	
	SetDlgItemText(IDC_PATH,Configuration.Folder);
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::LoadData()
/* ----------------------------------------------------------------------------------------------- */
{
    ConfigurationFile.Read(&Configuration.Folder,sizeof(Configuration.Folder));
    ConfigurationFile.Read(&Configuration.ShowAllParameters,sizeof(int));
    ConfigurationFile.Read(&Configuration.IndexMediaFormat,sizeof(unsigned long));
    ConfigurationFile.Read(&Configuration.IndexVideoCodec,sizeof(unsigned long));
    ConfigurationFile.Read(&Configuration.IndexAudioCodec,sizeof(unsigned long));
    ConfigurationFile.Read(&Configuration.ParameterWidth,sizeof(Configuration.ParameterWidth));
    ConfigurationFile.Read(&Configuration.ParameterHeight,sizeof(Configuration.ParameterHeight));
    ConfigurationFile.Read(&Configuration.ParameterFPS,sizeof(Configuration.ParameterFPS));
    ConfigurationFile.Read(&Configuration.ParameterVideoBitRate,sizeof(Configuration.ParameterVideoBitRate));
    ConfigurationFile.Read(&Configuration.IndexChannels,sizeof(int));
    ConfigurationFile.Read(&Configuration.ParameterSamplingRate,sizeof(Configuration.ParameterSamplingRate));
    ConfigurationFile.Read(&Configuration.ParameterAudioBitRate,sizeof(Configuration.ParameterAudioBitRate));
    ConfigurationFile.Read(&Configuration.ProcessesLimit,sizeof(unsigned long));
    ConfigurationFile.Read(&Configuration.AlwaysOnTop,sizeof(int));
    ConfigurationFile.Read(&Configuration.DisableWebPage,sizeof(int));
    ConfigurationFile.Read(&Configuration.DisableContextMenu,sizeof(int));
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::InitConfiguration()
/* ----------------------------------------------------------------------------------------------- */
{
	Configuration.AlwaysOnTop=yes;
	Configuration.DisableWebPage=no;
	Configuration.DisableContextMenu=no;
	Configuration.ProcessesLimit=5;
	Configuration.ShowAllParameters=no;
	
	Configuration.Folder[0]=0;
	Configuration.IndexMediaFormat=1;
	Configuration.IndexVideoCodec=1;
	Configuration.IndexAudioCodec=1;
	Configuration.IndexChannels=2;
	_tcscpy(Configuration.ParameterWidth,_T("176"));
	_tcscpy(Configuration.ParameterHeight,_T("144"));
	_tcscpy(Configuration.ParameterFPS,_T("15"));
	_tcscpy(Configuration.ParameterVideoBitRate,_T("144"));
	_tcscpy(Configuration.ParameterSamplingRate,_T("8000"));
	_tcscpy(Configuration.ParameterAudioBitRate,_T("12.2"));
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::SaveConfiguration()
/* ----------------------------------------------------------------------------------------------- */
{
	if (ConfigurationFile.Open(ConfigurationFileName, CFile::modeCreate|CFile::modeWrite)==yes) 
	{
//	    ConfigurationFile.Write(&Configuration,sizeof(Configuration));
		ConfigurationFile.Write(&Configuration.Folder,sizeof(Configuration.Folder));
		ConfigurationFile.Write(&Configuration.ShowAllParameters,sizeof(int));
		ConfigurationFile.Write(&Configuration.IndexMediaFormat,sizeof(unsigned long));
		ConfigurationFile.Write(&Configuration.IndexVideoCodec,sizeof(unsigned long));
		ConfigurationFile.Write(&Configuration.IndexAudioCodec,sizeof(unsigned long));
		ConfigurationFile.Write(&Configuration.ParameterWidth,sizeof(Configuration.ParameterWidth));
		ConfigurationFile.Write(&Configuration.ParameterHeight,sizeof(Configuration.ParameterHeight));
		ConfigurationFile.Write(&Configuration.ParameterFPS,sizeof(Configuration.ParameterFPS));
		ConfigurationFile.Write(&Configuration.ParameterVideoBitRate,sizeof(Configuration.ParameterVideoBitRate));
		ConfigurationFile.Write(&Configuration.IndexChannels,sizeof(int));
		ConfigurationFile.Write(&Configuration.ParameterSamplingRate,sizeof(Configuration.ParameterSamplingRate));
		ConfigurationFile.Write(&Configuration.ParameterAudioBitRate,sizeof(Configuration.ParameterAudioBitRate));
		ConfigurationFile.Write(&Configuration.ProcessesLimit,sizeof(unsigned long));
		ConfigurationFile.Write(&Configuration.AlwaysOnTop,sizeof(int));
		ConfigurationFile.Write(&Configuration.DisableWebPage,sizeof(int));
		ConfigurationFile.Write(&Configuration.DisableContextMenu,sizeof(int));
		    
	    ConfigurationFile.Close();
	}
}




