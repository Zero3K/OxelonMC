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



#pragma once

#define XLISTCTRLLIB_STATIC
#include "spawn.h"
#include "XListCtrl.h"
#include "XHyperLink.h"
#include "DIBSectionLite.h"
#include "CPlasmaRoutine.h"




class CConfiguration
{
public:
	TCHAR Folder[1024];

	int ShowAllParameters;
	unsigned long IndexMediaFormat;
	unsigned long IndexVideoCodec;
	unsigned long IndexAudioCodec;
	TCHAR ParameterWidth[20];
	TCHAR ParameterHeight[20];
	TCHAR ParameterFPS[20];
	TCHAR ParameterVideoBitRate[20];
	int IndexChannels;
	TCHAR ParameterSamplingRate[20];
	TCHAR ParameterAudioBitRate[20];
	
	unsigned long ProcessesLimit;
	int AlwaysOnTop;
	int DisableWebPage;
	int DisableContextMenu;
};


// COxelonMCDlg dialog
class COxelonMCDlg : public CDialog
{
// Construction
public:
	COxelonMCDlg(CWnd* pParent = NULL);	// standard constructor
	
/*********************************************************************************************/
	void InitializeInterface();
/*-------------------------------------------------------------------------------------------*/
		void AddMediaFormats();
/*-------------------------------------------------------------------------------------------*/
			TCHAR (*Formats)[45];
			CComboBox *MediaFormat;
/*-------------------------------------------------------------------------------------------*/
		void ClearAll();
/*-------------------------------------------------------------------------------------------*/
			void ClearVideoCodecParameters();
/*-------------------------------------------------------------------------------------------*/
				void EnableVideoCodecParameters();
/*-------------------------------------------------------------------------------------------*/
				void DisableVideoCodecParameters();
/*-------------------------------------------------------------------------------------------*/
			void ClearAudioCodecParameters();
/*-------------------------------------------------------------------------------------------*/
				void EnableAudioCodecParameters();
/*-------------------------------------------------------------------------------------------*/
				void DisableAudioCodecParameters();
/*-------------------------------------------------------------------------------------------*/
		void InitializeCodecs(int Init=no);
/*-------------------------------------------------------------------------------------------*/
			void AddVideoCodecParameters(int Init=no);
/*-------------------------------------------------------------------------------------------*/
			void AddAudioCodecParameters(int Init=no);
/*-------------------------------------------------------------------------------------------*/
		void AddVideoCodecs();
/*-------------------------------------------------------------------------------------------*/
			CComboBox *VideoCodec;
/*-------------------------------------------------------------------------------------------*/
		void AddAudioCodecs();
/*-------------------------------------------------------------------------------------------*/
			CComboBox *AudioCodec;
/*-------------------------------------------------------------------------------------------*/
		void AddVideoBitrates(int Init=no);
/*-------------------------------------------------------------------------------------------*/
			CComboBox *VideoBitrates;
/*-------------------------------------------------------------------------------------------*/
		void AddAudioBitrates(int Init=no);
/*-------------------------------------------------------------------------------------------*/
			CComboBox *AudioBitrates;
/*-------------------------------------------------------------------------------------------*/
		void AddFPS(int Init=no);
/*-------------------------------------------------------------------------------------------*/
			CComboBox *FPS;
/*-------------------------------------------------------------------------------------------*/
		void AddWidthHeight(int Init=no);
/*-------------------------------------------------------------------------------------------*/
			CComboBox *WidthHeightComboBox;
/*-------------------------------------------------------------------------------------------*/
		void AddAudioSamplingRates(int Init=no);
/*-------------------------------------------------------------------------------------------*/
			CComboBox *SamplingRate;
/*-------------------------------------------------------------------------------------------*/
		void AddChannels(int Init=no);
/*-------------------------------------------------------------------------------------------*/
			CComboBox *ChannelsComboBox;
/*-------------------------------------------------------------------------------------------*/
		void InitializeList();
/*-------------------------------------------------------------------------------------------*/
			CXListCtrl List;
			CStringArray FileNames;
			CStringArray ConvertedFileNames;
			CStringArray ErrorsArray;
			CStringArray InfoArray;
			CList<int,int> Status;
			CList<unsigned long,unsigned long> DurationSeconds;
			LVCOLUMN lvc;
/*-------------------------------------------------------------------------------------------*/
			void InsertFileListColumn(TCHAR *Text, int SubItem, int Size, int Alignment);
/*-------------------------------------------------------------------------------------------*/
		void InitializeOtherControls();
/*-------------------------------------------------------------------------------------------*/
			CDIBSectionLite RenderSource;
			CDIBSectionLite RenderTarget;
			CPlasmaRoutine Plasma;
			CWnd* ImageWindow;
			CXHyperLink WebSiteLink;
/*-------------------------------------------------------------------------------------------*/
		void InitializeGeneral();
/*-------------------------------------------------------------------------------------------*/
			TCHAR ModuleDirectory[2000];
/*-------------------------------------------------------------------------------------------*/
		void ParseCommandLine();
/*-------------------------------------------------------------------------------------------*/
			TCHAR *ApplicationCommandLine;
/*-------------------------------------------------------------------------------------------*/
			void MediaPluginsMessage();
/*********************************************************************************************/



/*********************************************************************************************/
	void LoadConfiguration();
/*-------------------------------------------------------------------------------------------*/
		CFile ConfigurationFile;
		TCHAR ConfigurationFileName[1024];
		CConfiguration Configuration;
/*-------------------------------------------------------------------------------------------*/
		void LoadData();
/*-------------------------------------------------------------------------------------------*/
	void SaveConfiguration();
/*-------------------------------------------------------------------------------------------*/
	void InitConfiguration();
/*********************************************************************************************/



/*********************************************************************************************/
	afx_msg void OnBnClickedConversion();
/*-------------------------------------------------------------------------------------------*/
		TCHAR CommandLine[2048];
		CSpawner Spawner;
		HINSTANCE hInstance;
		long IsConverting;
		unsigned long AmountOfProcesses;
		unsigned long ConversionLastIndex;
/*-------------------------------------------------------------------------------------------*/
		void CancelConversion();
/*-------------------------------------------------------------------------------------------*/
		afx_msg void OnTimer(UINT_PTR i);
/*-------------------------------------------------------------------------------------------*/
		void LaunchConversionProcesses();
/*-------------------------------------------------------------------------------------------*/
			void MakeCommandLine(unsigned long Index);
/*-------------------------------------------------------------------------------------------*/
				void MakeCommandLineVideo();
/*-------------------------------------------------------------------------------------------*/
				void MakeCommandLineAudio(unsigned long Index);
/*-------------------------------------------------------------------------------------------*/
					void MakeCommandLineAudioParameters(unsigned long Index);
/*-------------------------------------------------------------------------------------------*/
						int NecessaryToResetAudioParameters(unsigned long Index);
/*-------------------------------------------------------------------------------------------*/
				void MakeCommandLineDestinationFileName(unsigned long Index);
/*-------------------------------------------------------------------------------------------*/
			void ExecuteProgram(LPTSTR CmdLine, unsigned long Index);
/*-------------------------------------------------------------------------------------------*/
				int UpdateConversionProgress(char *Data, unsigned long Index);
/*-------------------------------------------------------------------------------------------*/
				void RetrieveConversionError(char *Data, unsigned long Index, int GoodFormat);
/*********************************************************************************************/



/*********************************************************************************************/
	afx_msg void OnBnClickedAddFiles();	
/*-------------------------------------------------------------------------------------------*/
		TCHAR *SelectedFileNames;
		TCHAR InputDirectory[1024];
/*-------------------------------------------------------------------------------------------*/
		void InitOpenFileNameForAddFiles();
/*-------------------------------------------------------------------------------------------*/
			OPENFILENAME OFNAF;
/*-------------------------------------------------------------------------------------------*/
		void StartAddFile();
/*-------------------------------------------------------------------------------------------*/
		void EndAddFile();
/*-------------------------------------------------------------------------------------------*/
		void AddFile(TCHAR *FileName, TCHAR *FileNameWithPath);
/*-------------------------------------------------------------------------------------------*/
			unsigned __int64 GetFileSize(TCHAR *FileName);
/*-------------------------------------------------------------------------------------------*/
			void GetFileDuration(TCHAR *FileName, TCHAR *DurationBuffer, TCHAR *InfoBuffer);
/*-------------------------------------------------------------------------------------------*/
			unsigned long TimeStringToSeconds(TCHAR *TimeString);
/*-------------------------------------------------------------------------------------------*/
				char *TempString;
				TCHAR *Duration;
				TCHAR *Info;
/*********************************************************************************************/






	afx_msg void OnBnClickedBrowse();
	afx_msg void OnCbnSelchangeMediaFormat();
	afx_msg void OnCbnSelchangeVideoCodec();
	afx_msg void OnCbnSelchangeAudioCodec();
	afx_msg void OnCbnSelchangeFramesSecond();
	afx_msg void OnCbnSelchangeVideoBitrate();
	afx_msg void OnCbnSelchangeChannels();
	afx_msg void OnCbnSelchangeSamplingRate();
	afx_msg void OnCbnSelchangeAudioBitrate();
	afx_msg void OnClose();
	afx_msg void OnBnClickedRemoveFiles();
	afx_msg void OnBnClickedRemoveAll();
	afx_msg void OnNMRclickFileList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedUp();
	afx_msg void OnBnClickedDown();
	
	void ExchangeItems(unsigned long x1, unsigned long x2);
	afx_msg void OnCancel();
	afx_msg void OnOk();
	afx_msg void OnCbnEditchangeFramesSecond();
	afx_msg void OnCbnEditchangeVideoBitrate();
	afx_msg void OnCbnEditchangeSamplingRate();
	afx_msg void OnCbnEditchangeAudioBitrate();

// Dialog Data
	enum { IDD = IDD_OXELONMC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSettings();
public:
	afx_msg void OnBnClickedShowAllParameters();
public:
	afx_msg void OnCbnEditchangeWidthHeight();
public:
	afx_msg void OnCbnSelchangeWidthHeight();
public:
	afx_msg void OnNMDblclkFileList(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
public:
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
