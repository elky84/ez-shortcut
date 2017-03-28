// EzShortcutDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "EzShortcut.h"
#include "EzShortcutDlg.h"

#include <afxtempl.h>

#include "LogHelper.h"

#include "Tray.h"
#include "Icon.h"

#include "DirectoryUtil.h"

#include "EditDlg.h"

#include "XMLParser.h"
#include "XMLWriter.h"

#include <boost/bind.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CEzShortcutDlg 대화 상자
CEzShortcutDlg::CEzShortcutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEzShortcutDlg::IDD, pParent)
	, m_bTrayIcon(FALSE)
	, m_Popup(m_hWnd)
	, m_strXMLFileName(_T("EzShortcut.xml"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_Popup.Add(_T("&Exit"), boost::bind(&CEzShortcutDlg::OnExitPopup, this));
}

CEzShortcutDlg::~CEzShortcutDlg()
{
}

void CEzShortcutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SHORTCUT, m_listboxShortCutInfo);
	DDX_Control(pDX, IDC_STATIC_HOTKEY_CONTENT, m_staticHotKeyContent);
	DDX_Control(pDX, IDC_STATIC_SHORTCUT_NAME_CONTENT, m_staticShortcutNameContent);
}

BEGIN_MESSAGE_MAP(CEzShortcutDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_LBN_SELCHANGE(IDC_LIST_SHORTCUT, OnLbnSelchangeListShortcut)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnBnClickedButtonRemove)
	ON_MESSAGE(WM_CLOSE, OnClose)
	ON_MESSAGE(WM_TRAYICON_MSG, OnTrayIconMsg)
	ON_MESSAGE(WM_DESTROY, OnDestroy)
	ON_LBN_DBLCLK(IDC_LIST_SHORTCUT, &CEzShortcutDlg::OnLbnDblclkListParameter)
	ON_WM_HOTKEY()
	ON_BN_CLICKED(IDC_BUTTON_EDIT, &CEzShortcutDlg::OnBnClickedButtonEdit)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CEzShortcutDlg 메시지 처리기

BOOL CEzShortcutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	CoInitialize(NULL);

	LoadShortCutList();
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CEzShortcutDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == SC_MINIMIZE)
	{
		RegisterTrayIcon();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CEzShortcutDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);

		OnDrawIcon();
	}
	else
	{
		CDialog::OnPaint();
		OnDrawIcon();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CEzShortcutDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CEzShortcutDlg::OnDrawIcon()
{
	CString strShortcutName;
	m_staticShortcutNameContent.GetWindowText(strShortcutName);

	SHORTCUT_INFO* ShortcutInfo = GetShortCutInfo(strShortcutName.GetBuffer());
	if( ShortcutInfo == NULL)
	{
		return;
	}

	CClientDC dc(this);
	light::Icon icon(ShortcutInfo->m_strProgramPath);
	if( icon.Large())
	{
		dc.DrawIcon(ICON_X, ICON_Y, icon.Large());
	}
	else
	{
		if( _tcsncmp(_T("http"), ShortcutInfo->m_strProgramPath.c_str(), 4) == 0)
			dc.DrawIcon(ICON_X, ICON_Y, AfxGetApp()->LoadIcon(IDI_WEB_ICON));
		else if( true == light::FileUtil::is_exist(ShortcutInfo->m_strProgramPath))
			dc.DrawIcon(ICON_X, ICON_Y, GetIcon(TRUE));
		else
			dc.DrawIcon(ICON_X, ICON_Y, AfxGetApp()->LoadIcon(IDI_FOLDER_ICON));
	}
}

void CEzShortcutDlg::OnLbnSelchangeListShortcut()
{
	int nCursel = m_listboxShortCutInfo.GetCurSel();
	if(nCursel < 0)
		return;

	CString strShortCutName;
	m_listboxShortCutInfo.GetText(nCursel, strShortCutName);

	SHORTCUT_INFO* ShortCutInfo = GetShortCutInfo(strShortCutName.GetBuffer());
	if( NULL == ShortCutInfo )
		return;

	CopyShortCutInfo(*ShortCutInfo);
}

bool CEzShortcutDlg::OnEditShortcutDlg(SHORTCUT_INFO& ShortcutInfo)
{
	EnableHotkeys(FALSE);

	CEditDlg dlg(ShortcutInfo, m_map_ShortcutInfo);
	int nRet = dlg.DoModal();

	EnableHotkeys(TRUE);

	if( nRet == IDCANCEL )
	{
		return false;
	}

	if( true == ShortcutInfo.m_strShortcutName.empty() )
	{
		AfxMessageBox(_T("This application must have shortcut-name. Please type shortcut-name."));
		return false;
	}

	if( true == ShortcutInfo.m_strProgramPath.empty() )
	{
		AfxMessageBox(_T("This application must have program-path. Please type program-path."));
		return false;
	}

	return true;
}

void CEzShortcutDlg::OnBnClickedButtonAdd()
{
	SHORTCUT_INFO ShortcutInfo(GetSafeHwnd());
	if( true == OnEditShortcutDlg(ShortcutInfo) ) 
	{
		AddShortCutInfo(ShortcutInfo);
		RebuildShortcutList();
		AutoSave();
	}
}

void CEzShortcutDlg::OnBnClickedButtonRemove()
{
	int nCursel = m_listboxShortCutInfo.GetCurSel();
	if(nCursel < 0)
	{
		AfxMessageBox(_T("You are not select Shotcut."));
		return;
	}

	CString strShortCutName;
	m_listboxShortCutInfo.GetText(nCursel, strShortCutName);

	if( false == RemoveShortCutInfo( strShortCutName.GetBuffer() ) )
		return;

	RebuildShortcutList();

	InitInputDlg();

	AutoSave();
}

void CEzShortcutDlg::InitInputDlg()
{
	m_staticShortcutNameContent.SetWindowText(_T(""));
	m_staticHotKeyContent.SetWindowText(_T(""));
}

void CEzShortcutDlg::CopyShortCutInfo(const SHORTCUT_INFO& ShortCutInfo)
{
	m_staticShortcutNameContent.SetWindowText(ShortCutInfo.m_strShortcutName.c_str());
	m_staticHotKeyContent.SetWindowText( ShortCutInfo.m_HotKey.String().c_str());

	UpdateData(FALSE);
}

bool CEzShortcutDlg::LoadShortCutList()
{
	light::XMLParser reader;
	std::wstring str = light::Directory::get_current_directory();

	if(false == reader.read_file(m_strXMLFileName) )
	{
		LOG_ERROR(_T("%s Load Failed CFG. %s"), __FUNCTIONW__, m_strXMLFileName.c_str());
		return false;
	}

	Clear();

	if( false == reader.execute(_T("/ShortCutList")) )
		return false;

	
	if( false == reader.bind_elem(_T("ShortCut")) )
		return false;

	std::wstring strProgramPath, strShortcutName, strParam, strHotKey;
	if( false == reader.bind_attrib(_T("Path"), strProgramPath) )
		return false;

	if( false == reader.bind_attrib(_T("Name"), strShortcutName) )
		return false;
	
	if( false == reader.bind_attrib(_T("Parameter"), strParam) )
		return false;

	if( false == reader.bind_attrib(_T("HotKey"), strHotKey) )
		return false;

	while(reader.next())
	{
		SHORTCUT_INFO ShortCutInfo(GetSafeHwnd(), strShortcutName, strParam, strProgramPath);
		ShortCutInfo.m_HotKey.Decode(strHotKey);

		m_map_ShortcutInfo.insert(MAP_SHORTCUT_INFO::value_type(ShortCutInfo.m_strShortcutName, ShortCutInfo));
	}

	RebuildShortcutList();
	return true;
}

bool CEzShortcutDlg::SaveShortCutList()
{
	light::XMLWriter generator;
	if(false == generator.open(m_strXMLFileName, L"ShortCutList"))
		return false;

	for(MAP_SHORTCUT_INFO::iterator it = m_map_ShortcutInfo.begin(); it != m_map_ShortcutInfo.end(); ++it)
	{
		generator.elem(L"ShortCut");

		SHORTCUT_INFO& ShortCutInfo = it->second;

		generator.attribute(L"Path", ShortCutInfo.m_strProgramPath);
		generator.attribute(L"Name", ShortCutInfo.m_strShortcutName);
		generator.attribute(L"Parameter", ShortCutInfo.m_strParameter);
		generator.attribute(L"HotKey", ShortCutInfo.m_HotKey.Encode());

		generator.elem_end();
	}
	return true;
}

void CEzShortcutDlg::AutoSave()
{
	SaveShortCutList();
}

void CEzShortcutDlg::RebuildShortcutList()
{
	m_listboxShortCutInfo.ResetContent();

	for(MAP_SHORTCUT_INFO::iterator it = m_map_ShortcutInfo.begin(); it != m_map_ShortcutInfo.end(); ++it)
	{
		SHORTCUT_INFO& ShortCutInfo = it->second;
		m_listboxShortCutInfo.AddString(ShortCutInfo.m_strShortcutName.c_str());
	}
}

LRESULT CEzShortcutDlg::OnClose(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	int nRet = AfxMessageBox(_T("Are you really quit?"), MB_YESNO);
	if(nRet == IDYES)
	{
		EndDialog(0);
	}
	return 0;
}

LRESULT CEzShortcutDlg::OnTrayIconMsg(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	if(lParam == WM_LBUTTONDBLCLK)
	{  
		DeleteTrayIcon();
	}
	else if(lParam == WM_RBUTTONDOWN)
	{ 
		m_Popup.Show();
	}
	return 0;
}

void CEzShortcutDlg::RegisterTrayIcon()
{
	light::Tray::set(m_hWnd, m_hIcon, _T("EzShortcut"));
	m_bTrayIcon = TRUE;
}

void CEzShortcutDlg::DeleteTrayIcon()
{
	light::Tray::Delete(m_hWnd);
	m_bTrayIcon = FALSE;
}

BOOL CEzShortcutDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return CDialog::OnCommand(wParam, lParam);
}

bool CEzShortcutDlg::OnExitPopup()
{
	PostMessage(WM_QUIT);
	return true;
}

LRESULT CEzShortcutDlg::OnDestroy(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	if(m_bTrayIcon)
	{
		light::Tray::Release(m_hWnd);
	}
	ClearShortCutInfo();
	return 0;
}

BOOL CEzShortcutDlg::UpdateData(BOOL bSaveAndValidate)
{
	{
		/// 아이콘 영역 리프레시
		RECT rect = {ICON_X, ICON_Y, ICON_X + 32, ICON_Y + 32};
		InvalidateRect(&rect);
	}

	return __super::UpdateData(bSaveAndValidate);
}

void CEzShortcutDlg::OnLbnDblclkListParameter()
{
	CString strShortcutName;
	m_staticShortcutNameContent.GetWindowText(strShortcutName);

	SHORTCUT_INFO* ShortcutInfo = GetShortCutInfo(strShortcutName.GetBuffer());
	if( ShortcutInfo == NULL)
	{
		LOG_ERROR(_T("%s() Critical Error. ShotcutName[%s]"), __FUNCTIONW__, strShortcutName.GetBuffer());
		return;
	}

	::ShellExecute(NULL, _T("open"), ShortcutInfo->m_strProgramPath.c_str(), ShortcutInfo->m_strParameter.c_str(), light::get_path_without_file_name(ShortcutInfo->m_strProgramPath).c_str(), SW_SHOWNORMAL);
}

void CEzShortcutDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	for(MAP_SHORTCUT_INFO::iterator it = m_map_ShortcutInfo.begin(); it != m_map_ShortcutInfo.end(); ++it)
	{
		SHORTCUT_INFO& ShortCutInfo = it->second;
		if( ShortCutInfo.m_HotKey == light::HotKey::INFORMATION(nKey1, nKey2) )
		{
			::ShellExecute(NULL, _T("open"), ShortCutInfo.m_strProgramPath.c_str(), ShortCutInfo.m_strParameter.c_str(), light::get_path_without_file_name(ShortCutInfo.m_strProgramPath).c_str(), SW_SHOWNORMAL);
		}
	}

	CDialog::OnHotKey(nHotKeyId, nKey1, nKey2);
}

BOOL CEzShortcutDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_F4)
	{
		OnClose(pMsg->wParam, pMsg->lParam);
		return TRUE;
	}

	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		OnLbnDblclkListParameter();
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CEzShortcutDlg::Clear()
{
	ClearShortCutInfo();
	m_listboxShortCutInfo.ResetContent();
	InitInputDlg();
}

void CEzShortcutDlg::ClearShortCutInfo()
{
	for(MAP_SHORTCUT_INFO::iterator it = m_map_ShortcutInfo.begin(); it != m_map_ShortcutInfo.end(); ++it)
	{
		SHORTCUT_INFO& ShortCutInfo = it->second;
		ShortCutInfo.m_HotKey.Clear();
	}
}

SHORTCUT_INFO* CEzShortcutDlg::GetShortCutInfo(const std::wstring& strShortCutName)
{
	MAP_SHORTCUT_INFO::iterator it = m_map_ShortcutInfo.find(strShortCutName);
	if( m_map_ShortcutInfo.end() == it )
		return NULL;

	return &it->second;
}

bool CEzShortcutDlg::AddShortCutInfo(SHORTCUT_INFO& ShortcutInfo)
{
	if( ShortcutInfo.m_HotKey.Information().Valid() )
		ShortcutInfo.m_HotKey.set(ShortcutInfo.m_HotKey.Information());

	m_map_ShortcutInfo.insert(MAP_SHORTCUT_INFO::value_type( ShortcutInfo.m_strShortcutName, ShortcutInfo));
	return true;
}

bool CEzShortcutDlg::RemoveShortCutInfo(const std::wstring& strShortCutName)
{
	SHORTCUT_INFO* ShortCutInfo = GetShortCutInfo(strShortCutName);
	if( NULL == ShortCutInfo )
		return false;

	if( ShortCutInfo->m_HotKey.Information().Valid() )
	{
		ShortCutInfo->m_HotKey.Clear();
	}

	m_map_ShortcutInfo.erase(strShortCutName);
	return true;
}

void CEzShortcutDlg::OnBnClickedButtonEdit()
{
	int nCursel = m_listboxShortCutInfo.GetCurSel();
	if(nCursel < 0)
	{
		AfxMessageBox(_T("You are not select Shortcut."));
		return;
	}

	CString strShortCutName;
	m_listboxShortCutInfo.GetText(nCursel, strShortCutName);

	SHORTCUT_INFO* pShortCutInfo = GetShortCutInfo(strShortCutName.GetBuffer());
	if( NULL == pShortCutInfo)
	{
		AfxMessageBox(_T("Not find shortcut Information"));
		return;
	}

	if( false == OnEditShortcutDlg( *pShortCutInfo ) )
	{
		return;
	}

	if( _tcsicmp(strShortCutName.GetBuffer(), pShortCutInfo->m_strShortcutName.c_str()) != 0 )
	{
		AddShortCutInfo(*pShortCutInfo);
		RemoveShortCutInfo(strShortCutName.GetBuffer());
		RebuildShortcutList();
	}

	OnLbnSelchangeListShortcut();
	AutoSave();
}

void CEzShortcutDlg::OnDropFiles(HDROP hDropInfo)
{
	TCHAR szPathName[MAX_PATH];

	// 드롭된 파일의 갯수
	int nFiles=DragQueryFile(hDropInfo, 0xFFFFFFFF, szPathName, MAX_PATH);
	if( nFiles != 1)
	{
		AfxMessageBox(_T("This application is not support multi-file drag & drop"));
		return;
	}

	for(int index=0 ; index < nFiles ; index++)
	{
		DragQueryFile(hDropInfo, index, szPathName, MAX_PATH);     // 파일의 경로 얻어옴
		std::wstring strExt = light::get_file_ext(szPathName);

		if( strExt == _T("lnk" ))
		{
			IShellLink *link;
			IPersistFile *pfile;
			BSTR szLinkPath;
			CString szPrePath;
			TCHAR szBuffer[MAX_PATH];

			CString fname = szPathName;
			if(SUCCEEDED(CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&link))) 
			{
				link->QueryInterface(IID_IPersistFile, (void **)&pfile);
				szLinkPath = fname.AllocSysString();
				pfile->Load(szLinkPath, STGM_READ);
				SysFreeString(szLinkPath);
				link->Resolve(NULL, NULL);
				link->GetPath(szBuffer, MAX_PATH, NULL, 0);

				// 리스트 박스나 메세지 박스로 해당 경로 값을 불러온다. (szBuffer)
				_tcsncpy_s(szPathName , szBuffer, _TRUNCATE);
				pfile->Release();
				link->Release();
			}
		}
		
		SHORTCUT_INFO ShortcutInfo(GetSafeHwnd());
		ShortcutInfo.m_strShortcutName = light::get_file_name_without_ext(szPathName);
		ShortcutInfo.m_strProgramPath = szPathName;
		if( true == OnEditShortcutDlg(ShortcutInfo) )
		{
			AddShortCutInfo(ShortcutInfo);
			RebuildShortcutList();
			AutoSave();
		}
	}

	DragFinish(hDropInfo);

	CDialog::OnDropFiles(hDropInfo);
}

void CEzShortcutDlg::EnableHotkeys(const BOOL bSwitch)
{
	for(MAP_SHORTCUT_INFO::iterator it = m_map_ShortcutInfo.begin(); it != m_map_ShortcutInfo.end(); ++it)
	{
		SHORTCUT_INFO& ShortCutInfo = it->second;
		if( TRUE == bSwitch )
		{
			ShortCutInfo.m_HotKey.set();
		}
		else
		{
			ShortCutInfo.m_HotKey.clear();
		}
	}
}
