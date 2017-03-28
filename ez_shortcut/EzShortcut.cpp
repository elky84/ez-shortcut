// EzShortcut.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "EzShortcut.h"
#include "EzShortcutDlg.h"

#include "DirectoryUtil.h"
#include "Tray.h"
#include "ProcessUtil.h"

#include "LogHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CEzShortcutApp

BEGIN_MESSAGE_MAP(CEzShortcutApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CEzShortcutApp ����

CEzShortcutApp::CEzShortcutApp()
{
}


// ������ CEzShortcutApp ��ü�Դϴ�.

CEzShortcutApp theApp;


// CEzShortcutApp �ʱ�ȭ

BOOL CEzShortcutApp::InitInstance()
{
	//#32770�� MFC���̾�α� �����츦 ã�� ����.
	HWND hWnd = light::Process::find_process_by_name(_T("#32770"), _T("EzShortcut"));
	if( NULL != hWnd )
	{
		if( TRUE == ::IsIconic(hWnd))
			::ShowWindow(hWnd, SW_SHOWDEFAULT);
		else
			::PostMessage(hWnd, WM_TRAYICON_MSG, 0, WM_LBUTTONDBLCLK);

		::SetForegroundWindow(hWnd);
		return FALSE;
	}

	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControls()�� �ʿ��մϴ�. 
	// InitCommonControls()�� ������� ������ â�� ���� �� �����ϴ�.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));

    light::Directory::set_current_directory(light::get_module_path()); // ���̳ʸ� ���� ��θ� ���� ���丮�� ��������.
    light::LogHelper::initialize();

	CEzShortcutDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ���⿡ ��ȭ ���ڰ� Ȯ���� ���� �������� ��� ó����
		// �ڵ带 ��ġ�մϴ�.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ���⿡ ��ȭ ���ڰ� ��Ҹ� ���� �������� ��� ó����
		// �ڵ带 ��ġ�մϴ�.
	}

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�
	// ���� ���α׷��� ���� �� �ֵ��� FALSE�� ��ȯ�մϴ�.
	return FALSE;
}
