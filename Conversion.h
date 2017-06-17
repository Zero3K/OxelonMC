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
void COxelonMCDlg::OnBnClickedConversion()
/* ----------------------------------------------------------------------------------------------- */
{
	if (IsConverting==yes) {CancelConversion(); return;}
	
	if (_tcslen(Configuration.Folder)==0)
	{
		MessageBox(_T("Please, select the path for saving!"),_T("Oxelon Media Converter"),MB_OK|MB_ICONINFORMATION);
		return;
	}
	
	if (List.GetItemCount()==0)
	{
		MessageBox(_T("Please, add files!"),_T("Oxelon Media Converter"),MB_OK|MB_ICONINFORMATION);
		return;
	}	

	IsConverting=yes;
	SetDlgItemText(IDC_CONVERSION,_T("Cancel"));
	SetTimer(7,1000,0);
	SetTimer(9,30,0);
				
}


/* ----------------------------------------------------------------------------------------------- */
int Extract(char* SourceBuffer, TCHAR* DestinationBuffer, char* StartPattern, char* EndPattern)
/* ----------------------------------------------------------------------------------------------- */
{
	char *Start;
	char *End;

	if (StartPattern!=NULL)
	{
		Start = strstr(SourceBuffer,StartPattern);
		if (Start==NULL) return no;
		Start=Start+strlen(StartPattern);
	}
	else
	{
		Start = SourceBuffer;
	}
	
	if (EndPattern!=NULL)
	{
		End = strstr(Start,EndPattern);
		if (End==NULL) return no;
	}
	else
	{
		End = SourceBuffer + strlen(SourceBuffer);
	}
		
	#ifdef  _UNICODE
	mbstowcs(DestinationBuffer,Start,End-Start);
	#else 
	strncpy(DestinationBuffer,Start,End-Start);
	#endif 
	DestinationBuffer[End-Start]=0;
	
	return yes;
}

/* ----------------------------------------------------------------------------------------------- */
inline void CreatePipeAndProcess(LPTSTR CmdLine, HANDLE &ReadPipeHandle, HANDLE &WritePipeHandle, PROCESS_INFORMATION *pi)
/* ----------------------------------------------------------------------------------------------- */
{
  //create pipe for the console stdout 
  SECURITY_ATTRIBUTES sa; 
  ZeroMemory(&sa,sizeof(SECURITY_ATTRIBUTES)); 
  sa.nLength=sizeof(SECURITY_ATTRIBUTES); 
  sa.bInheritHandle=true; 
  sa.lpSecurityDescriptor=NULL; 
  CreatePipe(&ReadPipeHandle,&WritePipeHandle,&sa,0); 

  //Create a Console 
  STARTUPINFO si; 
  ZeroMemory(&si,sizeof(STARTUPINFO)); 
  si.cb=sizeof(STARTUPINFO); 
  si.dwFlags=STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES|STARTF_USESIZE; 
  si.wShowWindow=SW_HIDE; 
  si.hStdOutput=WritePipeHandle; 
  si.hStdError=WritePipeHandle; 
  si.dwXSize = 0;
  si.dwYSize = 0;

  ZeroMemory(pi,sizeof(PROCESS_INFORMATION)); 
  CreateProcess(NULL,CmdLine,NULL,NULL,true,0,NULL,NULL,&si,pi); 
}

/* ----------------------------------------------------------------------------------------------- */
// CloseHandles
/* ----------------------------------------------------------------------------------------------- */
#define CloseHandles                          \
        TerminateProcess(pi.hProcess,0);      \
        CloseHandle(pi.hThread);              \
        CloseHandle(pi.hProcess);             \
        CloseHandle(ReadPipeHandle);          \
        CloseHandle(WritePipeHandle);         \
        AmountOfProcesses--;


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::RetrieveConversionError(char *Data, unsigned long Index, int GoodFormat)
/* ----------------------------------------------------------------------------------------------- */
{
  TCHAR SizeBuffer[30];
  TCHAR TimePositionBuffer[30];
  TCHAR ErrorsBuffer[5000];
  if (GoodFormat==no)
  {
	 // error
	 Status.SetAt(Status.FindIndex(Index),errors);
	 ErrorsBuffer[0]=0;
	 if (Extract(Data, ErrorsBuffer, "mingw32-2)", NULL)==no) Extract(Data, ErrorsBuffer, NULL, NULL);
  	 ErrorsArray.SetAt(Index, ErrorsBuffer);
     List.SetItemText(Index,5,_T("Error"));
  }
  else
  {
	 // completed
	 Status.SetAt(Status.FindIndex(Index),completed);
     List.SetItemText(Index,5,_T("Completed"));
  }
  List.DeleteProgress(Index,5);
}  
  
/* ----------------------------------------------------------------------------------------------- */
int COxelonMCDlg::UpdateConversionProgress(char *Data, unsigned long Index)
/* ----------------------------------------------------------------------------------------------- */
{
  TCHAR SizeBuffer[30];
  unsigned long NewFileSize;
  double Percent;
  TCHAR TimePositionBuffer[30];

  if (Extract(Data,SizeBuffer,"size=","kB")==no ||
      Extract(Data,TimePositionBuffer,"time=",".")==no) return no;
  NewFileSize=_ttol(SizeBuffer)/1024;
  _ltot(NewFileSize,SizeBuffer,10);
  _tcscat(SizeBuffer,_T(" MB"));
  Percent = _ttol(TimePositionBuffer);
  Percent = (Percent / (double)DurationSeconds.GetAt(DurationSeconds.FindIndex(Index)))*100;
  List.SetItemText(Index,4,SizeBuffer);
  List.UpdateProgress(Index,5,(int)Percent);
  return yes;
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::ExecuteProgram(LPTSTR CmdLine, unsigned long Index)
/* ----------------------------------------------------------------------------------------------- */
{
  HANDLE ReadPipeHandle; 
  HANDLE WritePipeHandle;   
  PROCESS_INFORMATION pi; 
  int GoodFormat=no;
  CreatePipeAndProcess(CmdLine, ReadPipeHandle, WritePipeHandle, &pi);
  //Read from pipe 
  char Data[5000]; 
  for (;;) 
  {  
    DWORD BytesRead; 
    DWORD TotalBytes; 
    DWORD BytesLeft; 
    //Check for the presence of data in the pipe 
    PeekNamedPipe(ReadPipeHandle,Data,sizeof(Data),&BytesRead,&TotalBytes,&BytesLeft); 
    //If there are bytes, read them 
    if(BytesRead) 
    { 
      ReadFile(ReadPipeHandle,Data,sizeof(Data)-1,&BytesRead,NULL); 
      if (IsConverting==no) { CloseHandles  return; }
      Data[BytesRead]='\0'; 
      if (UpdateConversionProgress(Data, Index)==yes) GoodFormat=yes;
    } 
    else 
    { 
      //Is the console app terminated? 
      if(WaitForSingleObject(pi.hProcess,0)==WAIT_OBJECT_0) 
      {
		RetrieveConversionError(Data, Index, GoodFormat);
		break; 
	  }
    } 
  } 
  CloseHandles
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::CancelConversion()
/* ----------------------------------------------------------------------------------------------- */
{
	IsConverting=no;
	ConversionLastIndex=0;
	SetDlgItemText(IDC_CONVERSION,_T("Conversion"));
	KillTimer(7);
	KillTimer(9);
	ImageWindow->InvalidateRect(NULL,TRUE);

	unsigned long i;
	unsigned long Count = List.GetItemCount();

	if (Count>0)
	{
		for (i=0; i<Count; i++)
		{
			if (Status.GetAt(Status.FindIndex(i))==converting)
			{
				Status.SetAt(Status.FindIndex(i),ready);
				List.DeleteProgress(i,5);
			}
		}
	}
}


/* ----------------------------------------------------------------------------------------------- */
UINT ConversionThread(LPVOID pParam)  
/* ----------------------------------------------------------------------------------------------- */
{   
	unsigned char *Parameters = (unsigned char*)pParam;
	COxelonMCDlg *M;
	unsigned long i;
	TCHAR CommandLine[2048];
	memcpy(&M,Parameters,sizeof(void*));
	memcpy(&i,Parameters+sizeof(void*),sizeof(unsigned long));
	memcpy(CommandLine,Parameters+sizeof(void*)+sizeof(unsigned long),2048);
	
	M->ExecuteProgram(CommandLine,i);

    return 0;
};


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::LaunchConversionProcesses()
/* ----------------------------------------------------------------------------------------------- */
{
	unsigned long i;
	unsigned long Count = List.GetItemCount();

	if (Count>0) 
	{
		for (i=0; i<Count; i++)
		{
			if (Status.GetAt(Status.FindIndex(i))==ready)
			{
				Status.SetAt(Status.FindIndex(i),converting);
				List.SetProgress(i,5,TRUE);
				List.UpdateProgress(i,5,0);
				MakeCommandLine(i);
				
				unsigned char *Parameters = (unsigned char *)malloc(2*sizeof(void*)+sizeof(unsigned long)+2048);
				void* ThisObjectPointer = this;
				memcpy(Parameters,&ThisObjectPointer,sizeof(void*));
				memcpy(Parameters+sizeof(void*),&i,sizeof(unsigned long));
				memcpy(Parameters+sizeof(void*)+sizeof(unsigned long),CommandLine,2048);
				ConversionLastIndex=i;
				
				AfxBeginThread(ConversionThread,Parameters);
	
				AmountOfProcesses++;
				if (AmountOfProcesses==Configuration.ProcessesLimit) break;
			}
		}
	}
	if (AmountOfProcesses==0) CancelConversion();
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnTimer(UINT_PTR t)
/* ----------------------------------------------------------------------------------------------- */
{
	if (t==7) if (AmountOfProcesses < Configuration.ProcessesLimit && IsConverting==yes) LaunchConversionProcesses();
	
	if (t==9)
	{
		Plasma.Render((DWORD*)RenderTarget.GetDIBits(),100,75,100);
		CClientDC dc(ImageWindow);
		CPoint ptOrigin(0,0);
		RenderTarget.Draw(&dc,ptOrigin);
	}

	CDialog::OnTimer(t);
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::MakeCommandLine(unsigned long Index)
/* ----------------------------------------------------------------------------------------------- */
{
	_tcscpy(CommandLine,_T("ffmpeg.exe -i \""));
	_tcscat(CommandLine,FileNames.GetAt(Index));
	_tcscat(CommandLine,_T("\" -y -f "));
	_tcscat(CommandLine,FormatParameters[Configuration.IndexMediaFormat]);
	
	MakeCommandLineVideo();
	MakeCommandLineAudio(Index);
	MakeCommandLineDestinationFileName(Index);
	
//	AfxMessageBox(CommandLine);
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::MakeCommandLineVideo()
/* ----------------------------------------------------------------------------------------------- */
{
	TCHAR VB[30];
	if (Configuration.IndexVideoCodec>0)
	{
		if (Configuration.ShowAllParameters==yes)
		{
			_tcscat(CommandLine,_T(" -vcodec "));
			_tcscat(CommandLine,VideoParameters[Configuration.IndexVideoCodec]);
		}
		else
		{
			if (FPointers[Configuration.IndexMediaFormat]!=NULL)
			{
				if (_tcslen(FPointers[Configuration.IndexMediaFormat]->VideoParameters[Configuration.IndexVideoCodec-1])!=0)
				{
					_tcscat(CommandLine,_T(" -vcodec "));
					_tcscat(CommandLine,FPointers[Configuration.IndexMediaFormat]->VideoParameters[Configuration.IndexVideoCodec-1]);
				}
			}
		}

		if (_tcslen(Configuration.ParameterWidth)!=0 && _tcslen(Configuration.ParameterHeight)!=0)
		{
			_tcscat(CommandLine,_T(" -s "));
			_tcscat(CommandLine,Configuration.ParameterWidth);
			_tcscat(CommandLine,_T("x"));
			_tcscat(CommandLine,Configuration.ParameterHeight);
		}	
		if (_tcslen(Configuration.ParameterFPS)!=0)
		{
			_tcscat(CommandLine,_T(" -r "));
			_tcscat(CommandLine,Configuration.ParameterFPS);
		}
		if (_tcslen(Configuration.ParameterVideoBitRate)!=0)
		{
			_tcscat(CommandLine,_T(" -b "));
			int VideoBitrate = _ttol(Configuration.ParameterVideoBitRate) * 1000;
			_itot(VideoBitrate,VB,10);
			_tcscat(CommandLine,VB);
		}
	}
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::MakeCommandLineAudio(unsigned long Index)
/* ----------------------------------------------------------------------------------------------- */
{

	if (Configuration.IndexAudioCodec>0)
	{
		_tcscat(CommandLine,_T(" -acodec "));
		if (Configuration.ShowAllParameters==yes)
		{
			_tcscat(CommandLine,AudioParameters[Configuration.IndexAudioCodec]);
		}
		else
		{
			if (FPointers[Configuration.IndexMediaFormat]!=NULL)
			{
				_tcscat(CommandLine,FPointers[Configuration.IndexMediaFormat]->AudioParameters[Configuration.IndexAudioCodec-1]);
			}
		}
		
		MakeCommandLineAudioParameters(Index);
		
	}
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::MakeCommandLineAudioParameters(unsigned long Index)
/* ----------------------------------------------------------------------------------------------- */
{
	TCHAR AB[30];
	TCHAR CH[10];
	if ( NecessaryToResetAudioParameters(Index) == no)
	{
		if (_tcslen(Configuration.ParameterAudioBitRate)!=0)
		{
			double AudioBitrate;
			if (_tcscmp(&CommandLine[_tcslen(CommandLine)-6],_T("vorbis"))==0)
			{
				_tcscat(CommandLine,_T(" -aq "));
				AudioBitrate = _tstof(Configuration.ParameterAudioBitRate) / 4;
				_ltot((unsigned long)AudioBitrate,AB,10);
				_tcscat(CommandLine,AB);
			}
			else
			{
				_tcscat(CommandLine,_T(" -ab "));
				AudioBitrate = _tstof(Configuration.ParameterAudioBitRate) * 1000;
				_ltot((unsigned long)AudioBitrate,AB,10);
				_tcscat(CommandLine,AB);
			}
		}	
		if (Configuration.IndexChannels>=0)
		{
			if (Channels[Configuration.IndexChannels]!=-1)
			{
				if (Channels[Configuration.IndexChannels]==0)
				{
					_tcscat(CommandLine,_T(" -an"));
				}
				else
				{
					_tcscat(CommandLine,_T(" -ac "));
					_itot(Channels[Configuration.IndexChannels],CH,10);
					_tcscat(CommandLine,CH);
				}
			}
		}
		if (_tcslen(Configuration.ParameterSamplingRate)!=0)
		{
			_tcscat(CommandLine,_T(" -ar "));
			_tcscat(CommandLine,Configuration.ParameterSamplingRate);
		}	
	}
}

/* ----------------------------------------------------------------------------------------------- */
int COxelonMCDlg::NecessaryToResetAudioParameters(unsigned long Index)
/* ----------------------------------------------------------------------------------------------- */
{	
	// fix ffmpeg resampling error
	// because it doesn't allow other than 8000hz frequencies for 3g2 files
/*
	// amr_nb is 8khz by default, so params must always be set
	unsigned long Length = _tcslen(CommandLine);
	if ( _tcscmp(&CommandLine[Length-9], _T("libamr_nb")) == 0 ) return no;

	// if 3g2 extension then no params! let ffmpeg choose everything (without resampling)
	CString Extension;
	Length = FileNames.GetAt(Index).GetLength();
	long Position = FileNames.GetAt(Index).ReverseFind('.');
	if (Position != -1)
	{
		Extension = FileNames.GetAt(Index).Mid(Position+1, Length-Position-1);
	}
	if (Extension==_T("3g2") || 
		Extension==_T("3G2") || 
		Extension==_T("3GP2") || 
		Extension==_T("3gp2"))
	{
		return yes;
	}
	*/
	
	return no;
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::MakeCommandLineDestinationFileName(unsigned long Index)
/* ----------------------------------------------------------------------------------------------- */
{
	unsigned long Length = FileNames.GetAt(Index).GetLength();
	TCHAR *PointerBackSlash;
	TCHAR *PointerDot;
	
	TCHAR *FileName=(TCHAR*)malloc((Length+10)*2);
	_tcscpy(FileName,(LPCTSTR)FileNames.GetAt(Index));
	
	TCHAR ConvertedFileName[3000];
	
	_tcscat(CommandLine,_T(" \""));
	
	_tcscpy(ConvertedFileName,Configuration.Folder);
	
	if (_tcslen(Configuration.Folder)!=0) if (ConvertedFileName[_tcslen(ConvertedFileName)-1]!='\\') _tcscat(ConvertedFileName,_T("\\"));
	
	PointerBackSlash=_tcsrchr(FileName,_T('\\'));
	PointerDot=_tcsrchr(FileName,_T('.'));
	if (PointerDot==NULL || PointerDot<PointerBackSlash)
	{
		_tcscat(FileName, _T("."));
		_tcscat(FileName, Extensions[Configuration.IndexMediaFormat]);
	}
	else
	{
		PointerDot[1]=0;
		_tcscat(FileName, Extensions[Configuration.IndexMediaFormat]);
	}
	_tcscat(ConvertedFileName,PointerBackSlash+1);
	
	WIN32_FIND_DATA Win32FindData;
	HANDLE FileHandle;
	FileHandle = FindFirstFile(ConvertedFileName, &Win32FindData);
	if (FileHandle != INVALID_HANDLE_VALUE)
	{
		_tcscat(ConvertedFileName, _T("."));
		_tcscat(ConvertedFileName, Extensions[Configuration.IndexMediaFormat]);
		FindClose(FileHandle);
	}
	
	ConvertedFileNames.SetAt(Index,ConvertedFileName);
	_tcscat(CommandLine,ConvertedFileName);
	_tcscat(CommandLine,_T("\""));
	
	free(FileName);
}