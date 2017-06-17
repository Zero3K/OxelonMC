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
void COxelonMCDlg::InitializeInterface()
/* ----------------------------------------------------------------------------------------------- */
{
	AddMediaFormats();
	
	InitializeCodecs(yes);
	InitializeList();
	InitializeOtherControls();
	InitializeGeneral();
	ParseCommandLine();
	
	IsConverting=no;
	AmountOfProcesses=0;
	ConversionLastIndex=0;
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::ClearAll()
/* ----------------------------------------------------------------------------------------------- */
{
	VideoCodec = (CComboBox*)GetDlgItem(IDC_VIDEO_CODEC);
	VideoCodec->ResetContent();
	AudioCodec = (CComboBox*)GetDlgItem(IDC_AUDIO_CODEC);
	AudioCodec->ResetContent();
	
	ClearVideoCodecParameters();
	ClearAudioCodecParameters();
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::ClearVideoCodecParameters()
/* ----------------------------------------------------------------------------------------------- */
{
	WidthHeightComboBox = (CComboBox*)GetDlgItem(IDC_WIDTH_HEIGHT);
	WidthHeightComboBox->ResetContent();
	FPS=(CComboBox*)GetDlgItem(IDC_FRAMES_SECOND);
	FPS->ResetContent();
	VideoBitrates=(CComboBox*)GetDlgItem(IDC_VIDEO_BITRATE);
	VideoBitrates->SetEditSel(0, -1);
	VideoBitrates->Clear();
	
	if (Configuration.ShowAllParameters==no)
	{
		if (FPointers[Configuration.IndexMediaFormat] != NULL)
		{
			if (FPointers[Configuration.IndexMediaFormat]->VSettings[0] == NULL)
			DisableVideoCodecParameters();
			else
			EnableVideoCodecParameters();
		}
		else DisableVideoCodecParameters();
	}
	else EnableVideoCodecParameters();
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::DisableVideoCodecParameters()
/* ----------------------------------------------------------------------------------------------- */
{
	VideoCodec = (CComboBox*)GetDlgItem(IDC_VIDEO_CODEC);
	WidthHeightComboBox = (CComboBox*)GetDlgItem(IDC_WIDTH_HEIGHT);
	FPS=(CComboBox*)GetDlgItem(IDC_FRAMES_SECOND);
	VideoBitrates=(CComboBox*)GetDlgItem(IDC_VIDEO_BITRATE);
	WidthHeightComboBox->EnableWindow(FALSE);
	FPS->EnableWindow(FALSE);
	VideoBitrates->EnableWindow(FALSE);
	VideoCodec->EnableWindow(FALSE);
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::EnableVideoCodecParameters()
/* ----------------------------------------------------------------------------------------------- */
{
	VideoCodec = (CComboBox*)GetDlgItem(IDC_VIDEO_CODEC);
	WidthHeightComboBox = (CComboBox*)GetDlgItem(IDC_WIDTH_HEIGHT);
	FPS=(CComboBox*)GetDlgItem(IDC_FRAMES_SECOND);
	VideoBitrates=(CComboBox*)GetDlgItem(IDC_VIDEO_BITRATE);
	WidthHeightComboBox->EnableWindow(TRUE);
	FPS->EnableWindow(TRUE);
	VideoBitrates->EnableWindow(TRUE);
	VideoCodec->EnableWindow(TRUE);
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::ClearAudioCodecParameters()
/* ----------------------------------------------------------------------------------------------- */
{
	SamplingRate = (CComboBox*)GetDlgItem(IDC_SAMPLING_RATE);	
	SamplingRate->ResetContent();
	AudioBitrates = (CComboBox*)GetDlgItem(IDC_AUDIO_BITRATE);
	AudioBitrates->ResetContent();
	if (Configuration.IndexAudioCodec==0)
	{
		ChannelsComboBox = (CComboBox*)GetDlgItem(IDC_CHANNELS);
		ChannelsComboBox->SetCurSel(0);
	}

	if (Configuration.ShowAllParameters==no)
	{
		if (FPointers[Configuration.IndexMediaFormat] != NULL)
		{
			if (FPointers[Configuration.IndexMediaFormat]->ASettings[0] == NULL)
			DisableAudioCodecParameters();
			else
			EnableAudioCodecParameters();
		} 
		else DisableAudioCodecParameters();
	} 
	else EnableAudioCodecParameters();	
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::EnableAudioCodecParameters()
/* ----------------------------------------------------------------------------------------------- */
{
	AudioCodec = (CComboBox*)GetDlgItem(IDC_AUDIO_CODEC);
	SamplingRate=(CComboBox*)GetDlgItem(IDC_SAMPLING_RATE);	
	AudioBitrates=(CComboBox*)GetDlgItem(IDC_AUDIO_BITRATE);
	ChannelsComboBox=(CComboBox*)GetDlgItem(IDC_CHANNELS);
	SamplingRate->EnableWindow(TRUE);
	AudioBitrates->EnableWindow(TRUE);
	ChannelsComboBox->EnableWindow(TRUE);
	AudioCodec->EnableWindow(TRUE);
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::DisableAudioCodecParameters()
/* ----------------------------------------------------------------------------------------------- */
{
	AudioCodec = (CComboBox*)GetDlgItem(IDC_AUDIO_CODEC);
	SamplingRate=(CComboBox*)GetDlgItem(IDC_SAMPLING_RATE);	
	AudioBitrates=(CComboBox*)GetDlgItem(IDC_AUDIO_BITRATE);
	ChannelsComboBox=(CComboBox*)GetDlgItem(IDC_CHANNELS);
	SamplingRate->EnableWindow(FALSE);
	AudioBitrates->EnableWindow(FALSE);
	ChannelsComboBox->EnableWindow(FALSE);
	AudioCodec->EnableWindow(FALSE);
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::InitializeCodecs(int Init)
/* ----------------------------------------------------------------------------------------------- */
{
	ClearAll();
	
	AddVideoCodecs();
	AddVideoCodecParameters(Init);	
	
	AddAudioCodecs();
	AddAudioCodecParameters(Init);
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::AddVideoCodecParameters(int Init)
/* ----------------------------------------------------------------------------------------------- */
{
	AddVideoBitrates(Init);	
	AddFPS(Init);
	AddWidthHeight(Init);
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::AddAudioCodecParameters(int Init)
/* ----------------------------------------------------------------------------------------------- */
{
	AddAudioBitrates(Init);
	AddAudioSamplingRates(Init);
	AddChannels(Init);
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::AddMediaFormats()
/* ----------------------------------------------------------------------------------------------- */
{
	MediaFormat=(CComboBox*)GetDlgItem(IDC_MEDIA_FORMAT);
	TCHAR S[55];

	int i;
	int AmountOfFormats = sizeof(MediaFormats)/sizeof(MediaFormats[0]);	
	for (i=0; i<AmountOfFormats; i++)
	{
		_tcscpy(S,MediaFormats[i]);	
		_tcscat(S,_T(" (.")); 
		_tcscat(S,Extensions[i]); 
		_tcscat(S,_T(")"));
		MediaFormat->AddString(S);
	}
	MediaFormat->SetCurSel(Configuration.IndexMediaFormat);
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::AddVideoCodecs()
/* ----------------------------------------------------------------------------------------------- */
{
	VideoCodec=(CComboBox*)GetDlgItem(IDC_VIDEO_CODEC);

	if (Configuration.ShowAllParameters==yes)
	{
		int i;
		int AmountOfCodecs = sizeof(VideoCodecs)/sizeof(VideoCodecs[0]);	
		VideoCodec->AddString(VideoCodecs[0]);
		for (i=1; i<AmountOfCodecs; i++)
		{
			VideoCodec->AddString(VideoCodecs[i]);
		}
	}
	else
	{
		VideoCodec->AddString(_T("Default"));
		if (FPointers[Configuration.IndexMediaFormat]!=NULL)
		{
			int j=0;
			if (FPointers[Configuration.IndexMediaFormat]->VSettings[0]!=NULL && Configuration.IndexVideoCodec==0) Configuration.IndexVideoCodec++;
			while (FPointers[Configuration.IndexMediaFormat]->VSettings[j]!=NULL)
			{
				VideoCodec->AddString(FPointers[Configuration.IndexMediaFormat]->VideoCodecs[j]);
				j++;
			}
		}
	}
	VideoCodec->SetCurSel(Configuration.IndexVideoCodec);
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::AddAudioCodecs()
/* ----------------------------------------------------------------------------------------------- */
{
	AudioCodec=(CComboBox*)GetDlgItem(IDC_AUDIO_CODEC);

	if (Configuration.ShowAllParameters==yes)
	{
		int i;
		int AmountOfCodecs = sizeof(AudioCodecs)/sizeof(AudioCodecs[0]);	
		AudioCodec->AddString(AudioCodecs[0]);
		for (i=1; i<AmountOfCodecs; i++)
		{
			AudioCodec->AddString(AudioCodecs[i]);
		}
	}
	else
	{
		AudioCodec->AddString(_T("Default"));
		if (FPointers[Configuration.IndexMediaFormat]!=NULL)
		{
			int j=0;
			if (FPointers[Configuration.IndexMediaFormat]->ASettings[0]!=NULL && Configuration.IndexAudioCodec==0) Configuration.IndexAudioCodec++;
			while (FPointers[Configuration.IndexMediaFormat]->ASettings[j]!=NULL)
			{
				AudioCodec->AddString(FPointers[Configuration.IndexMediaFormat]->AudioCodecs[j]);
				j++;
			}
		}	
	}
	AudioCodec->SetCurSel(Configuration.IndexAudioCodec);
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::AddVideoBitrates(int Init)
/* ----------------------------------------------------------------------------------------------- */
{
	VideoBitrates=(CComboBox*)GetDlgItem(IDC_VIDEO_BITRATE);

	TCHAR B[15];
	int Bitrate=64;
	while (Bitrate<10000)
	{
		_itot(Bitrate,B,10);
		VideoBitrates->AddString(B);
		Bitrate = Bitrate + 32;
		if (Bitrate>1000) Bitrate = Bitrate + 32;
		if (Bitrate>3000) Bitrate = Bitrate + 32;
		if (Bitrate>5000) Bitrate = Bitrate + 32;
	}

	if (Configuration.ShowAllParameters==no)
	{
		if (FPointers[Configuration.IndexMediaFormat]!=NULL)
		{
			if (FPointers[Configuration.IndexMediaFormat]->VSettings[Configuration.IndexVideoCodec-1]!=NULL)
			{
				if (Init==no) 
				{
					VideoBitrates->SetWindowText(FPointers[Configuration.IndexMediaFormat]->VSettings[Configuration.IndexVideoCodec-1]->BitRate);
				}
			}
		}
	}
	
	if (Init==no) 
	{
		OnCbnEditchangeVideoBitrate();
	}

}



/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::AddFPS(int Init)
/* ----------------------------------------------------------------------------------------------- */
{
	FPS=(CComboBox*)GetDlgItem(IDC_FRAMES_SECOND);

	if (Configuration.ShowAllParameters==yes)
	{
		int i;
		int AmountOfFPS = sizeof(FramesPerSecond)/sizeof(FramesPerSecond[0]);	
		FPS->AddString(FramesPerSecond[0]);
		for (i=1; i<AmountOfFPS; i++)
		{
			FPS->AddString(FramesPerSecond[i]);
		}
	}
	else
	{
		if (FPointers[Configuration.IndexMediaFormat]!=NULL)
		{
			if (FPointers[Configuration.IndexMediaFormat]->VSettings[Configuration.IndexVideoCodec-1]!=NULL)
			{
				int j=0;
				while (FPointers[Configuration.IndexMediaFormat]->VSettings[Configuration.IndexVideoCodec-1]->FPS[j] != NULL)
				{
					FPS->AddString(FPointers[Configuration.IndexMediaFormat]->VSettings[Configuration.IndexVideoCodec-1]->FPS[j]);
					j++;
				}
				if (Init==no) 
				{
					FPS->SetCurSel(0);
				}
			}			
		}			
	}
	
	if (Init==no) 
	{
		OnCbnEditchangeFramesSecond();
	}
	
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::AddWidthHeight(int Init)
/* ----------------------------------------------------------------------------------------------- */
{
	WidthHeightComboBox = (CComboBox*)GetDlgItem(IDC_WIDTH_HEIGHT);


	if (Configuration.ShowAllParameters==yes)
	{
		int i;
		int AmountOfWidthHeight = sizeof(WidthHeight)/sizeof(WidthHeight[0]);
		for (i=0; i<AmountOfWidthHeight; i++)
		{
			WidthHeightComboBox->AddString(WidthHeight[i]);
		}
	}
	else
	{
		if (FPointers[Configuration.IndexMediaFormat]!=NULL)
		{
			if (FPointers[Configuration.IndexMediaFormat]->VSettings[Configuration.IndexVideoCodec-1]!=NULL)
			{
				int j=0;
				while (FPointers[Configuration.IndexMediaFormat]->VSettings[Configuration.IndexVideoCodec-1]->WidthHeight[j] != NULL)
				{
					WidthHeightComboBox->AddString(FPointers[Configuration.IndexMediaFormat]->VSettings[Configuration.IndexVideoCodec-1]->WidthHeight[j]);
					j++;
				}
				if (Init==no) 
				{
					WidthHeightComboBox->SetCurSel(0);
				}
			}
		}
	}

	if (Init==no) 
	{
		OnCbnEditchangeWidthHeight();
	}
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::AddAudioBitrates(int Init)
/* ----------------------------------------------------------------------------------------------- */
{
	AudioBitrates=(CComboBox*)GetDlgItem(IDC_AUDIO_BITRATE);
	
	if (Configuration.ShowAllParameters==yes)
	{
		TCHAR B[15];
		int Bitrate=32;
		
		while (Bitrate<=320)
		{
			_itot(Bitrate,B,10);
			AudioBitrates->AddString(B);
			Bitrate = Bitrate + 16;
		}
	}
	else
	{
		if (FPointers[Configuration.IndexMediaFormat]!=NULL)
		{
			if (FPointers[Configuration.IndexMediaFormat]->ASettings[Configuration.IndexAudioCodec-1]!=NULL)
			{
				int j=0;
				while (FPointers[Configuration.IndexMediaFormat]->ASettings[Configuration.IndexAudioCodec-1]->BitRate[j] != NULL)
				{
					AudioBitrates->AddString(FPointers[Configuration.IndexMediaFormat]->ASettings[Configuration.IndexAudioCodec-1]->BitRate[j]);
					j++;
				}
				if (Init==no) 
				{
					AudioBitrates->SetCurSel(0);
				}
			}
		}
	}

	if (Init==no) 
	{
		OnCbnEditchangeAudioBitrate();
	}
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::AddAudioSamplingRates(int Init)
/* ----------------------------------------------------------------------------------------------- */
{
	SamplingRate=(CComboBox*)GetDlgItem(IDC_SAMPLING_RATE);

	if (Configuration.ShowAllParameters==yes)
	{
		int i;
		int AmountOfSamplingRates = sizeof(SamplingRates)/sizeof(SamplingRates[0]);	
		SamplingRate->AddString(SamplingRates[0]);
		for (i=1; i<AmountOfSamplingRates; i++)
		{
			SamplingRate->AddString(SamplingRates[i]);
		}
	}
	else
	{
		if (FPointers[Configuration.IndexMediaFormat]!=NULL)
		{
			if (FPointers[Configuration.IndexMediaFormat]->ASettings[Configuration.IndexAudioCodec-1]!=NULL)
			{
				int j=0;
				while (FPointers[Configuration.IndexMediaFormat]->ASettings[Configuration.IndexAudioCodec-1]->SamplingRate[j] != NULL)
				{
					SamplingRate->AddString(FPointers[Configuration.IndexMediaFormat]->ASettings[Configuration.IndexAudioCodec-1]->SamplingRate[j]);
					j++;
				}
				if (Init==no) 
				{
					SamplingRate->SetCurSel(0);
				}
			}
		}
	}

	if (Init==no) 
	{
		OnCbnEditchangeSamplingRate();
	}
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::AddChannels(int Init)
/* ----------------------------------------------------------------------------------------------- */
{
	ChannelsComboBox = (CComboBox*)GetDlgItem(IDC_CHANNELS);
	
	if (ChannelsComboBox->GetCount()==0)
	{
		int i;
		int Amount = sizeof(ChannelsText)/sizeof(ChannelsText[0]);
		for (i=0; i<Amount; i++)
		{
			ChannelsComboBox->AddString(ChannelsText[i]);
		}
	}

	if (Configuration.ShowAllParameters==no)
	{
		if (FPointers[Configuration.IndexMediaFormat]!=NULL)
		{
			if (FPointers[Configuration.IndexMediaFormat]->ASettings[Configuration.IndexAudioCodec-1]!=NULL)
			{
				if (Init==no) 
				{
					ChannelsComboBox->SetCurSel(FPointers[Configuration.IndexMediaFormat]->ASettings[Configuration.IndexAudioCodec-1]->ChannelsIndex);
				}
			}
		}
	}

	if (Init==no) 
	{
		OnCbnSelchangeChannels();
	}
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::InitializeList()
/* ----------------------------------------------------------------------------------------------- */
{
	DWORD dwExStyle = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	List.SetExtendedStyle(dwExStyle);
	List.SetExtendedStyleX(dwExStyle);
	
	memset(&lvc, 0, sizeof(lvc));
	lvc.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

	InsertFileListColumn(_T(""), 0, 0, LVCFMT_LEFT);
	InsertFileListColumn(_T("File Name"), 1, 180, LVCFMT_LEFT);
	InsertFileListColumn(_T("Time"), 2, 60, LVCFMT_LEFT);
	InsertFileListColumn(_T("Size"), 3, 70, LVCFMT_LEFT);
	InsertFileListColumn(_T("New Size"), 4, 70, LVCFMT_LEFT);
	InsertFileListColumn(_T("Status"), 5, 80, LVCFMT_CENTER);
	List.EnableToolTips(TRUE);
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::InsertFileListColumn(TCHAR *Text, int SubItem, int Size, int Alignment)
/* ----------------------------------------------------------------------------------------------- */
{
	lvc.pszText = Text;	
	lvc.iSubItem = SubItem; 
	lvc.cx = Size;
	lvc.fmt = Alignment;
	List.InsertColumn(SubItem, &lvc);
	List.SetColumn(SubItem,&lvc);
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::InitializeOtherControls()
/* ----------------------------------------------------------------------------------------------- */
{
	TCHAR WH[20];
	if (_tcslen(Configuration.ParameterWidth)!=0 && _tcslen(Configuration.ParameterHeight)!=0)
	{
		_tcscpy(WH,Configuration.ParameterWidth);
		_tcscat(WH,_T("x"));
		_tcscat(WH,Configuration.ParameterHeight);
		SetDlgItemText(IDC_WIDTH_HEIGHT,WH);
	}
	
	SetDlgItemText(IDC_FRAMES_SECOND,Configuration.ParameterFPS);
	SetDlgItemText(IDC_VIDEO_BITRATE,Configuration.ParameterVideoBitRate);
	((CComboBox*)GetDlgItem(IDC_CHANNELS))->SetCurSel(Configuration.IndexChannels);
	SetDlgItemText(IDC_SAMPLING_RATE,Configuration.ParameterSamplingRate);
	SetDlgItemText(IDC_AUDIO_BITRATE,Configuration.ParameterAudioBitRate);
	
	CPictureHolder TmpPicture;
	TmpPicture.CreateFromBitmap(IDB_W);
	RenderSource.Create32BitFromPicture(&TmpPicture,100,75);
	RenderTarget.Create32BitFromPicture(&TmpPicture,100,75);	
	Plasma.Create(100,75);
	Plasma.m_iAlpha = 50;
	ImageWindow=GetDlgItem(IDC_IMAGE_W);
	
	WebSiteLink.SetURL(_T("http://www.oxelon.com"));
	
	if (Configuration.AlwaysOnTop==yes)
	::SetWindowPos(this->m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE); 
	else
	::SetWindowPos(this->m_hWnd,HWND_TOP,0,0,0,0,SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE); 
	
	if (Configuration.ShowAllParameters==yes) 
	((CButton*)GetDlgItem(IDC_SHOW_ALL_PARAMETERS))->SetCheck(BST_CHECKED);
	else
	((CButton*)GetDlgItem(IDC_SHOW_ALL_PARAMETERS))->SetCheck(BST_UNCHECKED);
}



/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::InitializeGeneral()
/* ----------------------------------------------------------------------------------------------- */
{
    ::GetModuleFileName(::GetModuleHandle(_T("OxelonMC.exe")),ModuleDirectory,2000*sizeof(TCHAR));
    *((TCHAR *)_tcsrchr(ModuleDirectory,_T('\\'))+1) = NULL;
}



/* ----------------------------------------------------------------------------------------------- */
TCHAR* Extract(TCHAR *SourceString, TCHAR *StartPattern, TCHAR *EndPattern, TCHAR *&ReturnEndPatternPosition)
/* ----------------------------------------------------------------------------------------------- */
{
	TCHAR *Start, *End;
	
	int StartPatternLength = _tcslen(StartPattern);
	
	Start = _tcsstr(SourceString, StartPattern);
	if (Start == NULL) return NULL;
	Start = Start + StartPatternLength;
	
	End = _tcsstr(Start+1, EndPattern);
	if (End == NULL) return NULL;
	if (ReturnEndPatternPosition != NULL) ReturnEndPatternPosition = End;
	
	TCHAR *SubString = (TCHAR*)malloc((End - Start + 1) * sizeof(TCHAR));
	_tcsncpy(SubString, Start, End - Start);
	SubString[End - Start] = _T('\0');
	
	return SubString;
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::ParseCommandLine()
/* ----------------------------------------------------------------------------------------------- */
{
	TCHAR *Start = ApplicationCommandLine, *DestinationFormat = NULL, *Path = NULL, *FileName = NULL;
	StartAddFile();
	DestinationFormat = Extract(Start, _T("\""), _T("\""), Start);

	if (DestinationFormat != NULL) 
	{
		int i;
		for (i=0; i<SZF; i++)
		{
			if (_tcscmp(FormatParameters[i], DestinationFormat) == 0) 
			{
				Configuration.IndexMediaFormat = i;
				MediaFormat->SetCurSel(Configuration.IndexMediaFormat);
				OnCbnSelchangeMediaFormat();
				break;
			}
		}
		
		Path = Extract(Start+1, _T("\""), _T("\""), Start);
		if (Path != NULL)
		{
			unsigned long PathLength = _tcslen(Path);
			FileName = Extract(Start+1, _T("\""), _T("\""), Start);
			while (FileName != NULL)
			{
				unsigned long FileNameLength = _tcslen(FileName);
				TCHAR *FileNameWithPath = (TCHAR*)malloc((PathLength + FileNameLength + 1)*sizeof(TCHAR));
				_tcscpy(FileNameWithPath,Path);
				_tcscat(FileNameWithPath,FileName);
				AddFile(FileName, FileNameWithPath);
				free(FileName);
				free(FileNameWithPath);
				FileName = Extract(Start+1, _T("\""), _T("\""), Start);
			}
		}
	}
	else MediaPluginsMessage();
	
	List.InvalidateRect(NULL,TRUE);
	EndAddFile();
	if (DestinationFormat != NULL) free(DestinationFormat);
	if (Path != NULL) free(Path);
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::MediaPluginsMessage()
/* ----------------------------------------------------------------------------------------------- */
{
	TCHAR AVCodecDLL[3000];
	_tcscpy(AVCodecDLL, ModuleDirectory);
	_tcscat(AVCodecDLL, _T("avcodec-52.dll"));
	long Size=GetFileSize(AVCodecDLL);
	
	if (Size < 8000000)
	{
		MessageBox(_T("Please, download and install free Oxelon Media Plugins from this web page:\n\nhttp://www.oxelon.com/media_plugins.html\n\nIt contains important video/audio codecs:\n\nMPEG-4 XviD (the best codec for AVI containers),\n\nAC3 (for decoding DVD (MPEG2) audio streams),\n\nAAC (popular high-quality audio codec for 3GP, Quicktime MOV, MP4),\n\nx264 (for encoding H.264/MPEG-4 AVC video streams),\n\nSoftware scaler and postprocessing.\n"),_T("Important Information"),MB_OK|MB_ICONINFORMATION);
		DWORD Flags = INTERNET_CONNECTION_LAN | INTERNET_CONNECTION_MODEM;
		if ( InternetGetConnectedState(&Flags,0) == TRUE)
		{
			ShellExecute(NULL, _T("open"), _T("http://www.oxelon.com/media_plugins.html"), NULL, NULL, SW_SHOWNORMAL);
		}
	}
}