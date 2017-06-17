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



#include "stdafx.h"

#include "OxelonMC.h"

#include "OxelonMCDlg.h"



void COxelonMCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE_LIST, List);	
	DDX_Control(pDX, IDC_WEBSITE, WebSiteLink);
}



BEGIN_MESSAGE_MAP(COxelonMCDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_MEDIA_FORMAT, &COxelonMCDlg::OnCbnSelchangeMediaFormat)
	ON_BN_CLICKED(IDC_CONVERSION, &COxelonMCDlg::OnBnClickedConversion)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_ADD_FILES, &COxelonMCDlg::OnBnClickedAddFiles)
	ON_BN_CLICKED(IDC_REMOVE_FILES, &COxelonMCDlg::OnBnClickedRemoveFiles)
	ON_BN_CLICKED(IDC_REMOVE_ALL, &COxelonMCDlg::OnBnClickedRemoveAll)
	ON_NOTIFY(NM_RCLICK, IDC_FILE_LIST, &COxelonMCDlg::OnNMRclickFileList)
	ON_BN_CLICKED(IDC_UP, &COxelonMCDlg::OnBnClickedUp)
	ON_BN_CLICKED(IDC_DOWN, &COxelonMCDlg::OnBnClickedDown)
	ON_BN_CLICKED(IDC_BROWSE, &COxelonMCDlg::OnBnClickedBrowse)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_VIDEO_CODEC, &COxelonMCDlg::OnCbnSelchangeVideoCodec)
	ON_CBN_SELCHANGE(IDC_AUDIO_CODEC, &COxelonMCDlg::OnCbnSelchangeAudioCodec)
	ON_CBN_SELCHANGE(IDC_FRAMES_SECOND, &COxelonMCDlg::OnCbnSelchangeFramesSecond)
	ON_CBN_SELCHANGE(IDC_VIDEO_BITRATE, &COxelonMCDlg::OnCbnSelchangeVideoBitrate)
	ON_CBN_SELCHANGE(IDC_CHANNELS, &COxelonMCDlg::OnCbnSelchangeChannels)
	ON_CBN_SELCHANGE(IDC_SAMPLING_RATE, &COxelonMCDlg::OnCbnSelchangeSamplingRate)
	ON_CBN_SELCHANGE(IDC_AUDIO_BITRATE, &COxelonMCDlg::OnCbnSelchangeAudioBitrate)
	ON_BN_CLICKED(IDCANCEL, &COxelonMCDlg::OnCancel)
	ON_BN_CLICKED(IDOK, &COxelonMCDlg::OnOk)
	ON_CBN_EDITCHANGE(IDC_FRAMES_SECOND, &COxelonMCDlg::OnCbnEditchangeFramesSecond)
	ON_CBN_EDITCHANGE(IDC_VIDEO_BITRATE, &COxelonMCDlg::OnCbnEditchangeVideoBitrate)
	ON_CBN_EDITCHANGE(IDC_SAMPLING_RATE, &COxelonMCDlg::OnCbnEditchangeSamplingRate)
	ON_CBN_EDITCHANGE(IDC_AUDIO_BITRATE, &COxelonMCDlg::OnCbnEditchangeAudioBitrate)
	ON_BN_CLICKED(IDC_SETTINGS, &COxelonMCDlg::OnBnClickedSettings)
	ON_BN_CLICKED(IDC_SHOW_ALL_PARAMETERS, &COxelonMCDlg::OnBnClickedShowAllParameters)
	ON_CBN_EDITCHANGE(IDC_WIDTH_HEIGHT, &COxelonMCDlg::OnCbnEditchangeWidthHeight)
	ON_CBN_SELCHANGE(IDC_WIDTH_HEIGHT, &COxelonMCDlg::OnCbnSelchangeWidthHeight)
	ON_NOTIFY(NM_DBLCLK, IDC_FILE_LIST, &COxelonMCDlg::OnNMDblclkFileList)
	ON_WM_DROPFILES()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



#include "OxelonMCDlgInitialization.h"

#include "InitializeInterface.h"

#include "LoadSaveConfiguration.h"

#include "Conversion.h"

#include "AddFiles.h"

#include "OxelonMCDlgMessages.h"



