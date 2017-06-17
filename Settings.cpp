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
#include "Settings.h"


// CSettings dialog

IMPLEMENT_DYNAMIC(CSettings, CDialog)

CSettings::CSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CSettings::IDD, pParent)
{

}

CSettings::~CSettings()
{
}

void CSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSettings, CDialog)
	ON_BN_CLICKED(IDOK, &CSettings::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSettings::OnBnClickedCancel)
END_MESSAGE_MAP()


// CSettings message handlers
#define GetCheck(x) ((CButton*)GetDlgItem(x))->GetCheck()
#define SetCheck(x,y) ((CButton*)GetDlgItem(x))->SetCheck(y)
#define CF Main->Configuration

/* ----------------------------------------------------------------------------------------------- */
BOOL CSettings::OnInitDialog()
/* ----------------------------------------------------------------------------------------------- */
{
	CDialog::OnInitDialog();
	TCHAR Buffer[20];
	
	if (CF.AlwaysOnTop==yes) SetCheck(IDC_ON_TOP, BST_CHECKED); else SetCheck(IDC_ON_TOP, BST_UNCHECKED);
	if (CF.DisableWebPage==yes) SetCheck(IDC_DISABLE_WEB_PAGE, BST_CHECKED); else SetCheck(IDC_DISABLE_WEB_PAGE, BST_UNCHECKED);
	if (CF.DisableContextMenu==yes) SetCheck(IDC_DISABLE_CONTEXT_MENU, BST_CHECKED); else SetCheck(IDC_DISABLE_CONTEXT_MENU, BST_UNCHECKED);
	_itot(CF.ProcessesLimit, Buffer, 10);
	SetDlgItemText(IDC_NUMBER_PROCESSES, Buffer);
	
    ::GetModuleFileName(::GetModuleHandle(_T("OxelonMC.exe")),ModuleDirectory,2000*sizeof(TCHAR));
    *((TCHAR *)_tcsrchr(ModuleDirectory,_T('\\'))+1) = NULL;


	return TRUE;  
}


/* ----------------------------------------------------------------------------------------------- */
void CSettings::OnBnClickedOk()
/* ----------------------------------------------------------------------------------------------- */
{
	TCHAR Buffer[20];
	if (GetCheck(IDC_ON_TOP)==BST_CHECKED) 
	{
		CF.AlwaysOnTop=yes; 
		::SetWindowPos(Main->m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE); 
	}
	else 
	{
		CF.AlwaysOnTop=no;
		::SetWindowPos(Main->m_hWnd,HWND_NOTOPMOST,0,0,0,0,SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE); 
	}
	if (GetCheck(IDC_DISABLE_WEB_PAGE)==BST_CHECKED) CF.DisableWebPage=yes; else CF.DisableWebPage=no;
	
	int ToUninstall = (GetCheck(IDC_DISABLE_CONTEXT_MENU)==BST_CHECKED && CF.DisableContextMenu==no);
	int ToInstall = (GetCheck(IDC_DISABLE_CONTEXT_MENU)==BST_UNCHECKED && CF.DisableContextMenu==yes);
	if (ToUninstall) { MenuExtensionDll = _T(" /u /s \""); CF.DisableContextMenu=yes; }
	if (ToInstall) { MenuExtensionDll = _T(" /s \""); CF.DisableContextMenu=no; }
	if (ToUninstall || ToInstall)
	{
		MenuExtensionDll += ModuleDirectory;
		MenuExtensionDll += _T("menuext.dll\"");
		ShellExecute(this->m_hWnd,_T("open"), _T("regsvr32.exe"), MenuExtensionDll, _T(""), SW_HIDE );
	}

	GetDlgItemText(IDC_NUMBER_PROCESSES,Buffer,20);
	CF.ProcessesLimit = _ttoi(Buffer);
	
	OnOK();
}

/* ----------------------------------------------------------------------------------------------- */
void CSettings::OnBnClickedCancel()
/* ----------------------------------------------------------------------------------------------- */
{
	OnCancel();
}
