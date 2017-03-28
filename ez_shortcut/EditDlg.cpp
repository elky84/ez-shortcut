// EditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EzShortcut.h"
#include "EditDlg.h"
#include "afxdialogex.h"

#include "DirectoryUtil.h"
#include "FileUtil.h"

#include "Icon.h"

inline int CALLBACK MyCallbackFunc(HWND hwnd, UINT uMsg, LPARAM /*lParam*/, LPARAM lpData)
{
	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		break;
	}
	return 0;
}

// CEditDlg dialog

IMPLEMENT_DYNAMIC(CEditDlg, CDialogEx)

CEditDlg::CEditDlg(SHORTCUT_INFO& ShotcutInfo, const MAP_SHORTCUT_INFO& map_ShortcutInfo, CWnd* pParent /*=NULL*/)
	: CDialogEx(CEditDlg::IDD, pParent)
	, m_ShotcutInfo(ShotcutInfo)
	, m_map_ShortcutInfo(map_ShortcutInfo)
	, m_TempHotkey(ShotcutInfo.m_HotKey)
{
}

CEditDlg::~CEditDlg()
{
}

void CEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SHORTCUT_NAME, m_editboxName);
	DDX_Control(pDX, IDC_EDIT_TARGET_PROGRAM, m_editboxPath);
	DDX_Control(pDX, IDC_EDIT_PARAMETER, m_editboxParameter);
	DDX_Control(pDX, IDC_EDIT_HOTKEY, m_editboxHotkey);
}


BEGIN_MESSAGE_MAP(CEditDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_FILE_FIND, &CEditDlg::OnBnClickedButtonFileFind)
	ON_BN_CLICKED(IDC_BUTTON_FOLDER_FIND, &CEditDlg::OnBnClickedButtonFolderFind)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CEditDlg::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CEditDlg::OnBnClickedButtonCancel)
END_MESSAGE_MAP()

BOOL CEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_editboxName.SetWindowText(m_ShotcutInfo.m_strShortcutName.c_str());
	m_editboxPath.SetWindowText(m_ShotcutInfo.m_strProgramPath.c_str());
	m_editboxParameter.SetWindowText(m_ShotcutInfo.m_strParameter.c_str());
	m_editboxHotkey.SetWindowText(m_ShotcutInfo.m_HotKey.String().c_str());

	m_TempHotkey = m_ShotcutInfo.m_HotKey;

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

// CEditDlg message handlers


void CEditDlg::OnBnClickedButtonFileFind()
{
	CString strProgramPath;
	m_editboxPath.GetWindowText(strProgramPath);

	CFileDialog dlg(TRUE, NULL, NULL, OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY, _T("All Files(*.*)|*.*|Execute Files(*.exe)|*.exe||"));
	{
		dlg.m_ofn.lpstrInitialDir = strProgramPath.GetBuffer();
		if(dlg.DoModal() == IDCANCEL)
			return;
	}

	m_editboxPath.SetWindowText(dlg.GetPathName());
}

void CEditDlg::OnBnClickedButtonFolderFind()
{
	CString strProgramPath;
	m_editboxPath.GetWindowText(strProgramPath);

	TCHAR strPath[MAX_PATH] = _T("");

	BROWSEINFO browse;
	browse.hwndOwner = this->m_hWnd;
	browse.pidlRoot = NULL;
	browse.pszDisplayName = NULL;
	browse.lpszTitle = _T("Select Folder"); // 타이틀 지정
	browse.ulFlags = BIF_RETURNONLYFSDIRS;
	browse.lpfn = MyCallbackFunc; // 초기 경로를 설정하지 않으려면 NULL로 처리한다.

	// 초기 경로를 설정하려면 CallBack 함수를 연결 해주어야 한다.
	browse.lParam = (LPARAM)light::get_path_without_file_name(strProgramPath.GetBuffer()).c_str();

	LPITEMIDLIST pItemIdList = SHBrowseForFolder(&browse);
	if( NULL == pItemIdList ) 
		return;

	if( FALSE == SHGetPathFromIDList(pItemIdList,strPath)) 
		return;

	m_editboxPath.SetWindowText(strPath);
}

std::wstring WindowTextToTstring(CWnd& Wnd)
{
	CString strShortcutName;
	Wnd.GetWindowText(strShortcutName);
	return strShortcutName.GetBuffer();
}

void CEditDlg::OnBnClickedButtonOk()
{
	std::wstring strProgramPath = WindowTextToTstring(m_editboxPath);
	m_ShotcutInfo.m_strShortcutName = WindowTextToTstring(m_editboxName);
	m_ShotcutInfo.m_strProgramPath = strProgramPath;
	m_ShotcutInfo.m_strParameter = WindowTextToTstring(m_editboxParameter);
	m_ShotcutInfo.m_HotKey = m_TempHotkey;
	EndDialog(IDOK);
}

void CEditDlg::OnBnClickedButtonCancel()
{
	EndDialog(IDCANCEL);
}

BOOL CEditDlg::PreTranslateMessage(MSG* pMsg)
{
	CWnd* pFocusWnd = GetFocus();
	if( pFocusWnd != GetDlgItem(IDC_EDIT_HOTKEY) )
		return CDialog::PreTranslateMessage(pMsg);

	if(pMsg->message == WM_KEYUP || pMsg->message == WM_KEYDOWN)  
	{
		switch(pMsg->wParam)
		{
		case VK_CONTROL:
		case VK_SHIFT:
		case VK_MENU:
		case VK_LWIN:
		case VK_RWIN:
		case VK_LSHIFT:
		case VK_RSHIFT:
		case VK_RETURN:
			return FALSE;
		case VK_BACK:
			{
				m_TempHotkey = light::HotKey::INFORMATION();
				m_editboxHotkey.SetWindowText(_T(""));
			}
			return FALSE;
		default:
			break;
		}

		light::HotKey::MODIFIER dwModKey = 0;
		if( GetAsyncKeyState(VK_CONTROL) )
		{
			dwModKey |= MOD_CONTROL;
		}

		if( GetAsyncKeyState(VK_MENU) )
		{
			dwModKey |= MOD_ALT;
		}

		if( GetAsyncKeyState(VK_LWIN) || GetAsyncKeyState(VK_RWIN))
		{
			dwModKey |= MOD_WIN;
		}

		if( GetAsyncKeyState(VK_LSHIFT) || GetAsyncKeyState(VK_RSHIFT))
		{
			dwModKey |= MOD_SHIFT;
		}

		light::HotKey::INFORMATION HotKeyInformation = light::HotKey::INFORMATION(dwModKey, pMsg->wParam);
		for( MAP_SHORTCUT_INFO::const_iterator it = m_map_ShortcutInfo.begin(); it != m_map_ShortcutInfo.end(); ++it)
		{
			const SHORTCUT_INFO& Shortcutinfo = it->second;
			if(Shortcutinfo.m_HotKey == HotKeyInformation )
			{
				m_TempHotkey = light::HotKey::INFORMATION();
				m_editboxHotkey.SetWindowText(_T(""));
				return FALSE;
			}
		}

		m_TempHotkey.clear();
		m_TempHotkey = HotKeyInformation;
		m_editboxHotkey.SetWindowText( m_TempHotkey.String().c_str() );
	}
	return CDialog::PreTranslateMessage(pMsg);
}
