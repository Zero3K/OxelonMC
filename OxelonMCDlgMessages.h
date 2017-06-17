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
void COxelonMCDlg::OnSysCommand(UINT nID, LPARAM lParam)
/* ----------------------------------------------------------------------------------------------- */
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}  

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnPaint()
/* ----------------------------------------------------------------------------------------------- */
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
/* ----------------------------------------------------------------------------------------------- */
HCURSOR COxelonMCDlg::OnQueryDragIcon()
/* ----------------------------------------------------------------------------------------------- */
{
	return static_cast<HCURSOR>(m_hIcon);
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnBnClickedRemoveFiles()
/* ----------------------------------------------------------------------------------------------- */
{
	unsigned long x = (unsigned long)List.GetFirstSelectedItemPosition();
	if (x!=NULL && x>ConversionLastIndex)
	{
		x--;
		if (x>=0 && Status.GetAt(Status.FindIndex(x))<0)
		{
			List.DeleteItem(x);
			FileNames.RemoveAt(x);
			ConvertedFileNames.RemoveAt(x);
			InfoArray.RemoveAt(x);
			ErrorsArray.RemoveAt(x);
			Status.RemoveAt(Status.FindIndex(x));
			DurationSeconds.RemoveAt(DurationSeconds.FindIndex(x));
		}
	}
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnBnClickedRemoveAll()
/* ----------------------------------------------------------------------------------------------- */
{
	if (IsConverting==no)
	{
		List.DeleteAllItems();
		FileNames.RemoveAll();
		ConvertedFileNames.RemoveAll();
		InfoArray.RemoveAll();
		ErrorsArray.RemoveAll();
		Status.RemoveAll();
		DurationSeconds.RemoveAll();
	}
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnNMRclickFileList(NMHDR *pNMHDR, LRESULT *pResult)
/* ----------------------------------------------------------------------------------------------- */
{
	unsigned long x=(unsigned long)List.GetFirstSelectedItemPosition();
	
	if (x!=NULL)
	{
		x--;
		unsigned long Count = List.GetItemCount()-1;
		if (x<=Count)
		{
			if (Status.GetAt(Status.FindIndex(x))==errors)
			{
				MessageBox(ErrorsArray.GetAt(x),_T("Error Description"),MB_OK|MB_ICONWARNING);
			}

			if (Status.GetAt(Status.FindIndex(x))==ready || Status.GetAt(Status.FindIndex(x))==completed)
			{
				MessageBox(InfoArray.GetAt(x),_T("Media File Information"),MB_OK|MB_ICONINFORMATION);
			}
		}
	}
	
	*pResult = 0;
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnNMDblclkFileList(NMHDR *pNMHDR, LRESULT *pResult)
/* ----------------------------------------------------------------------------------------------- */
{
	unsigned long x=(unsigned long)List.GetFirstSelectedItemPosition();	

	if (x!=NULL)
	{
		x--;
		unsigned long Count = List.GetItemCount()-1;
		if (x<=Count)
		{
			if (Status.GetAt(Status.FindIndex(x))==completed)
			{
				::ShellExecute(NULL, _T("open"), ConvertedFileNames.GetAt(x), NULL, NULL, SW_SHOWNORMAL);
			}
			else
			{
				::ShellExecute(NULL, _T("open"), FileNames.GetAt(x), NULL, NULL, SW_SHOWNORMAL);
			}
		}
	}


	*pResult = 0;
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnBnClickedUp()
/* ----------------------------------------------------------------------------------------------- */
{
	unsigned long x = (unsigned long)List.GetFirstSelectedItemPosition();
	if (x!=NULL)
	{
		x--;
		unsigned long Count = List.GetItemCount()-1;
		if (x>0 && x<=Count)
		{
			ExchangeItems(x,x-1);
		}	
	}
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnBnClickedDown()
/* ----------------------------------------------------------------------------------------------- */
{
	unsigned long x = (unsigned long)List.GetFirstSelectedItemPosition();
	if (x!=NULL)
	{
		x--;
		unsigned long Count = List.GetItemCount()-1;
		if (x>=0 && x<Count)
		{
			ExchangeItems(x,x+1);
		}	
	}
}


#define SXCHG(a,b,bx,c) c = a.GetAt(b); a.SetAt(b, a.GetAt(bx)); a.SetAt(bx, c);
#define IXCHG(a,b,bx,c) c = a.GetAt(a.FindIndex(b)); a.SetAt(a.FindIndex(b), a.GetAt(a.FindIndex(bx))); a.SetAt(a.FindIndex(bx),c);
/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::ExchangeItems(unsigned long x1, unsigned long x2)
/* ----------------------------------------------------------------------------------------------- */
{
		CString TempString;
		int TempInteger;
		void* TempVoidPointer;
		unsigned long TempUnsignedLong;
		
		if (Status.GetAt(Status.FindIndex(x1))<0 && Status.GetAt(Status.FindIndex(x2))<0)
		{
			
			SXCHG(FileNames,x1,x2,TempString)
			SXCHG(ConvertedFileNames,x1,x2,TempString)
			SXCHG(InfoArray,x1,x2,TempString)
			SXCHG(ErrorsArray,x1,x2,TempString)
			IXCHG(Status,x1,x2,TempInteger)
			IXCHG(DurationSeconds,x1,x2,TempUnsignedLong)
			
			TCHAR *TS1 = (TCHAR*) malloc(1024);
			TCHAR *TS2 = (TCHAR*) malloc(1024);
		
			int AmountOfColumns = List.GetHeaderCtrl()->GetItemCount();
			int i;
			for (i=1; i<AmountOfColumns; i++)
			{
				memset(TS1,0,1024);
				memset(TS2,0,1024);
				List.GetItemText(x1,i,TS1, 1024);
				List.GetItemText(x2,i,TS2, 1024);
				List.SetItemText(x1,i,TS2);
				List.SetItemText(x2,i,TS1);
			}
			
			List.SetItemState(x1,NULL,LVIS_SELECTED);
			List.SetItemState(x2,LVIS_SELECTED,LVIS_SELECTED);

			free(TS1);
			free(TS2);
		}
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnBnClickedBrowse()
/* ----------------------------------------------------------------------------------------------- */
{

  CXFolderDialog dlg(Configuration.Folder);
  if (dlg.DoModal() == IDOK)
  {
	 _tcscpy(Configuration.Folder,(LPCTSTR)dlg.GetPath());
  }
  else
  {
	 Configuration.Folder[0]=0;
  }
  SetDlgItemText(IDC_PATH,Configuration.Folder);
}




/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnCbnSelchangeMediaFormat()
/* ----------------------------------------------------------------------------------------------- */
{
	Configuration.IndexMediaFormat = ((CComboBox*)GetDlgItem(IDC_MEDIA_FORMAT))->GetCurSel();
	if (Configuration.ShowAllParameters == no)
	{
		Configuration.IndexVideoCodec=0;
		Configuration.IndexAudioCodec=0;
		InitializeCodecs();
	}
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnCbnSelchangeVideoCodec()
/* ----------------------------------------------------------------------------------------------- */
{
	Configuration.IndexVideoCodec = ((CComboBox*)GetDlgItem(IDC_VIDEO_CODEC))->GetCurSel();
	if (Configuration.ShowAllParameters == no)
	{
		ClearVideoCodecParameters();
		if (Configuration.IndexVideoCodec!=0) AddVideoCodecParameters();
	}	
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnCbnSelchangeAudioCodec()
/* ----------------------------------------------------------------------------------------------- */
{
	Configuration.IndexAudioCodec = ((CComboBox*)GetDlgItem(IDC_AUDIO_CODEC))->GetCurSel();
	if (Configuration.ShowAllParameters == no)
	{
		ClearAudioCodecParameters();
		if (Configuration.IndexAudioCodec!=0) AddAudioCodecParameters();
	}	
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnCbnSelchangeFramesSecond()
/* ----------------------------------------------------------------------------------------------- */
{
	long i = ((CComboBox*)GetDlgItem(IDC_FRAMES_SECOND))->GetCurSel();
	((CComboBox*)GetDlgItem(IDC_FRAMES_SECOND))->GetLBText(i,Configuration.ParameterFPS);
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnCbnEditchangeFramesSecond()
/* ----------------------------------------------------------------------------------------------- */
{
	GetDlgItemText(IDC_FRAMES_SECOND, Configuration.ParameterFPS, 20);
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnCbnSelchangeVideoBitrate()
/* ----------------------------------------------------------------------------------------------- */
{
	long i = ((CComboBox*)GetDlgItem(IDC_VIDEO_BITRATE))->GetCurSel();
	((CComboBox*)GetDlgItem(IDC_VIDEO_BITRATE))->GetLBText(i,Configuration.ParameterVideoBitRate);
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnCbnEditchangeVideoBitrate()
/* ----------------------------------------------------------------------------------------------- */
{
	GetDlgItemText(IDC_VIDEO_BITRATE, Configuration.ParameterVideoBitRate, 20);
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnCbnSelchangeChannels()
/* ----------------------------------------------------------------------------------------------- */
{
	Configuration.IndexChannels = ((CComboBox*)GetDlgItem(IDC_CHANNELS))->GetCurSel();
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnCbnSelchangeSamplingRate()
/* ----------------------------------------------------------------------------------------------- */
{
	long i = ((CComboBox*)GetDlgItem(IDC_SAMPLING_RATE))->GetCurSel();
	((CComboBox*)GetDlgItem(IDC_SAMPLING_RATE))->GetLBText(i,Configuration.ParameterSamplingRate);
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnCbnEditchangeSamplingRate()
/* ----------------------------------------------------------------------------------------------- */
{
	GetDlgItemText(IDC_SAMPLING_RATE, Configuration.ParameterSamplingRate, 20);
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnCbnSelchangeAudioBitrate()
/* ----------------------------------------------------------------------------------------------- */
{
	long i = ((CComboBox*)GetDlgItem(IDC_AUDIO_BITRATE))->GetCurSel();
	((CComboBox*)GetDlgItem(IDC_AUDIO_BITRATE))->GetLBText(i,Configuration.ParameterAudioBitRate);
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnCbnEditchangeAudioBitrate()
/* ----------------------------------------------------------------------------------------------- */
{
	GetDlgItemText(IDC_AUDIO_BITRATE, Configuration.ParameterAudioBitRate, 20);
}





/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnCbnEditchangeWidthHeight()
/* ----------------------------------------------------------------------------------------------- */
{
	TCHAR S[20], W[20], H[20];
	unsigned long Width, Height;
	TCHAR *X;
	GetDlgItemText(IDC_WIDTH_HEIGHT, S, 20);

	X=_tcschr(S,'x');
	if (X!=NULL && X!=S && X[1]!=0)
	{
		*X=0;
		_tcscpy(W,S);
		_tcscpy(H,X+1);
		Width=_ttol(W);
		Height=_ttol(H);
		if (Width!=0 && Height!=0)
		{
			_ltot(Width,W,10);
			_ltot(Height,H,10);
			_tcscpy(Configuration.ParameterWidth,W);
			_tcscpy(Configuration.ParameterHeight,H);
		}
	}
	
	if (_tcslen(S)==0) 
	{
		*Configuration.ParameterWidth=0;
		*Configuration.ParameterHeight=0;
	}
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnCbnSelchangeWidthHeight()
/* ----------------------------------------------------------------------------------------------- */
{
	TCHAR S[20];
	TCHAR *X;
	long i = ((CComboBox*)GetDlgItem(IDC_WIDTH_HEIGHT))->GetCurSel();
	((CComboBox*)GetDlgItem(IDC_WIDTH_HEIGHT))->GetLBText(i,S);

	X=_tcschr(S,'x');
	if (X!=NULL)
	{
		*X=0;
		_tcscpy(Configuration.ParameterWidth,S);
		_tcscpy(Configuration.ParameterHeight,X+1);
	}
}




/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnClose()
/* ----------------------------------------------------------------------------------------------- */
{
	if (IsConverting==yes) {CancelConversion(); Sleep(2000); }
	SaveConfiguration();
	
	DWORD Flags = INTERNET_CONNECTION_LAN | INTERNET_CONNECTION_MODEM;
	if (Configuration.DisableWebPage==no)
	{
		if ( InternetGetConnectedState(&Flags,0) == TRUE)
		{
		ShellExecute(NULL, _T("open"), _T("http://www.oxelon.com"), NULL, NULL, SW_SHOWNORMAL);
		}
	}
	
	CDialog::OnClose();
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnLButtonDown(UINT nFlags, CPoint point)
/* ----------------------------------------------------------------------------------------------- */
{
/*	DWORD Flags = INTERNET_CONNECTION_LAN | INTERNET_CONNECTION_MODEM;
	if ( InternetGetConnectedState(&Flags,0) == TRUE)
	{
		ShellExecute(NULL, _T("open"), _T("http://www.oxelon.com"), NULL, NULL, SW_SHOWNORMAL);
	}	
*/		
	CDialog::OnLButtonDown(nFlags, point);
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnNcLButtonDown(UINT nHitTest, CPoint point)
/* ----------------------------------------------------------------------------------------------- */
{
	CDialog::OnNcLButtonDown(nHitTest, point);
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnCancel()
/* ----------------------------------------------------------------------------------------------- */
{
//	this->OnClose();
	exit(0);
}

/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnOk()
/* ----------------------------------------------------------------------------------------------- */
{
	OnBnClickedConversion();
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnBnClickedSettings()
/* ----------------------------------------------------------------------------------------------- */
{
	CSettings S;
	S.Main = this;
	S.DoModal();
}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnBnClickedShowAllParameters()
/* ----------------------------------------------------------------------------------------------- */
{
	if (((CButton*)GetDlgItem(IDC_SHOW_ALL_PARAMETERS))->GetCheck()==BST_CHECKED) 
	Configuration.ShowAllParameters = yes;
	else
	Configuration.ShowAllParameters = no;

	Configuration.IndexMediaFormat=0;
	Configuration.IndexVideoCodec=0;
	Configuration.IndexAudioCodec=0;
	MediaFormat=(CComboBox*)GetDlgItem(IDC_MEDIA_FORMAT);
	MediaFormat->SetCurSel(Configuration.IndexMediaFormat);

	InitializeCodecs();

}


/* ----------------------------------------------------------------------------------------------- */
void COxelonMCDlg::OnDropFiles(HDROP hDropInfo)
/* ----------------------------------------------------------------------------------------------- */
{
	UINT nFiles;
	TCHAR DropFileName[5000];
	CString CSDropFileName;

	StartAddFile();

	nFiles=DragQueryFile(hDropInfo,(UINT)-1,NULL,0);

	int sr=-1;

	for (UINT s=0;s<nFiles;s++)
	{
		ZeroMemory(DropFileName,5000);
		DragQueryFile(hDropInfo, s, (LPTSTR)DropFileName,5000);

		CSDropFileName=DropFileName;
		if ( (sr=CSDropFileName.ReverseFind('\\'))==-1)
		{
			sr=CSDropFileName.ReverseFind('/');
		}
		CSDropFileName=CSDropFileName.Right(CSDropFileName.GetLength()-sr-1);
		AddFile((TCHAR*)CSDropFileName.GetBuffer(),DropFileName);
	}	
	List.InvalidateRect(NULL,TRUE);

	EndAddFile();
	
	CDialog::OnDropFiles(hDropInfo);
}
