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
void COxelonMCDlg::OnBnClickedAddFiles()
/* ----------------------------------------------------------------------------------------------- */
{

	SelectedFileNames=(TCHAR*)malloc(1024*1024*sizeof(TCHAR));
	_tcscpy(SelectedFileNames,_T("")); // without this initialization "open dialog" doesn't work in Win98
	TCHAR *TempFileName=(TCHAR*)malloc(1024);

	StartAddFile();
	InitOpenFileNameForAddFiles();

	if (GetOpenFileName(&OFNAF)==TRUE)
	{
		TCHAR *Pointer=SelectedFileNames;
		Pointer = _tcschr(Pointer+1,0);
		if (Pointer[1]==0)
		{	
			Pointer=_tcsrchr(SelectedFileNames,_T('\\'))+1;
			if (Pointer!=NULL) 	
			{  
				AddFile(Pointer,SelectedFileNames);
			}
		}
		else
		{
			_tcscpy(InputDirectory,SelectedFileNames);
			while (Pointer[1]!=0)
			{
				Pointer++;
				_tcscpy(TempFileName,InputDirectory);
				long len=_tcslen(TempFileName);
				if (len>0) if (TempFileName[len-1]!='\\') _tcscat(TempFileName,_T("\\"));
				_tcscat(TempFileName,Pointer);
				AddFile(Pointer,TempFileName);
				Pointer = _tcschr(Pointer,0);
			}
		}
	}
	List.InvalidateRect(NULL,TRUE);

	EndAddFile();
	free(SelectedFileNames);	
	free(TempFileName);

}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::StartAddFile()
/* ----------------------------------------------------------------------------------------------- */
{
	TempString = (char*)malloc(50*1024);
	Duration = (TCHAR*)malloc(50);
	Info = (TCHAR*)malloc(3000);
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::EndAddFile()
/* ----------------------------------------------------------------------------------------------- */
{
	free(Info);
	free(Duration);
	free(TempString);	
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::InitOpenFileNameForAddFiles()
/* ----------------------------------------------------------------------------------------------- */
{
	ZeroMemory(&OFNAF, sizeof(OPENFILENAME));
	
	OFNAF.lStructSize=sizeof(OPENFILENAME);
	OFNAF.hwndOwner=this->m_hWnd;
	OFNAF.hInstance=hInstance;
	OFNAF.lpstrFilter=_T("All Files\0*.*\0\0");
	OFNAF.nFilterIndex=1;
	OFNAF.lpstrFile=SelectedFileNames;
	OFNAF.nMaxFile=(1024*1024-2)*sizeof(TCHAR);
	OFNAF.lpstrFileTitle=NULL;
	OFNAF.nMaxFileTitle=NULL;
	OFNAF.lpstrTitle=_T("Add Media File(s) to Convert...");
	OFNAF.Flags=OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER | OFN_ALLOWMULTISELECT;
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::AddFile(TCHAR *FileName, TCHAR *FileNameWithPath)
/* ----------------------------------------------------------------------------------------------- */
{
	TCHAR FileSizeBuffer[50];
	unsigned __int64 FileSize, FileSizeMB;
	FileNames.Add(FileNameWithPath);
	ConvertedFileNames.Add(_T(""));

	int n=List.GetItemCount();
	List.InsertItem(n,_T("x"));
	List.SetItemText(n,1,FileName);

	FileSize = GetFileSize(FileNameWithPath);
	FileSizeMB = FileSize/(1024*1024);
	_ui64tot(FileSizeMB,FileSizeBuffer,10);
	_tcscat(FileSizeBuffer,_T(" MB"));
	List.SetItemText(n,3,FileSizeBuffer);
	List.SetItemText(n,4,_T(""));
	ErrorsArray.Add(_T("Unsupported media format."));

	if (FileSize > 1024)
	{
		GetFileDuration(FileNameWithPath,Duration,Info);
		InfoArray.Add(Info);
		List.SetItemText(n,2,Duration);
		if (_tcslen(Duration)>0)
		{
			List.SetItemText(n,5,_T("Ready"));
			Status.AddTail(ready);
			DurationSeconds.AddTail(TimeStringToSeconds(Duration));
			return;
		}
	}
	else InfoArray.Add(_T(""));

	List.SetItemText(n,5,_T("Unsupported"));
	Status.AddTail(errors);
	DurationSeconds.AddTail((unsigned long)0);
}


/* ----------------------------------------------------------------------------------------------- */
unsigned __int64 COxelonMCDlg::GetFileSize(TCHAR *FileName)
/* ----------------------------------------------------------------------------------------------- */
{
	unsigned __int64 FileSize;
	HANDLE FileHandle;
	WIN32_FIND_DATA FindData;

	FileHandle = FindFirstFile(FileName, &FindData);
	if (FileHandle != INVALID_HANDLE_VALUE)
	{
		FileSize = (unsigned __int64)FindData.nFileSizeHigh * (unsigned __int64)((unsigned __int64)0xFFFFFFFF+(unsigned __int64)1) + (unsigned __int64)FindData.nFileSizeLow;
		FindClose(FileHandle);
		return FileSize;
	}
	else
	{
		return 0;
	}
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::GetFileDuration(TCHAR *FileName, TCHAR *DurationBuffer, TCHAR *InfoBuffer)
/* ----------------------------------------------------------------------------------------------- */
{
	memset(TempString,0,50*1024);
	_tcscpy(DurationBuffer,_T(""));
	_tcscpy(InfoBuffer,_T(""));
	
	_tcscpy(CommandLine,_T("ffmpeg.exe -i \""));
	_tcscat(CommandLine,FileName);
	_tcscat(CommandLine,_T("\""));
	Spawner.SpawnProcess(CommandLine);
	Spawner.read(TempString,50*1024);
	
	char *Start = strstr(TempString,"Duration: ");
	if (Start==NULL) return;
	Start=Start+10;
	char *End = strstr(Start,".");
	if (End==NULL) return;
	if (End-Start>10) return;
		
	#ifdef  _UNICODE
	mbstowcs(DurationBuffer,Start,End-Start);
	#else 
	strncpy(DurationBuffer,Start,End-Start);
	#endif 
	DurationBuffer[End-Start]=0;
		
	Start = strstr(TempString, "    Stream #");
	if (Start==NULL) return;
	End = strstr(Start,"At least");
	if (End==NULL) return;

	#ifdef  _UNICODE
	mbstowcs(InfoBuffer,Start,End-Start);
	#else 
	strncpy(InfoBuffer,Start,End-Start);
	#endif 
	InfoBuffer[End-Start]=0;
		
}

/* ----------------------------------------------------------------------------------------------- */
unsigned long COxelonMCDlg::TimeStringToSeconds(TCHAR *TimeString)
/* ----------------------------------------------------------------------------------------------- */
{
	unsigned long Seconds=0;
	TCHAR *PointerMinute;
	TCHAR *PointerSecond;
	TCHAR *PointerMSecond;
	
	PointerMinute=_tcschr(TimeString,':');
	if (PointerMinute!=NULL)
	{
		PointerMinute[0]=0;
		Seconds=_ttol(TimeString)*60*60;
		PointerMinute[0]=':';
		
		PointerSecond=_tcschr(PointerMinute+1,':');
		if (PointerSecond!=NULL)
		{
			PointerSecond[0]=0;
			Seconds=Seconds + _ttol(PointerMinute+1)*60;
			PointerSecond[0]=':';

			Seconds=Seconds + _ttol(PointerSecond+1);
		}
	}
	
	return Seconds;
}
