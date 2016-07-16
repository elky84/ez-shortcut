#pragma once

#include "afxwin.h"

#include <vector>
#include "Str.h"
#include "Menu.h"

// CEzShortcutDlg 대화 상자
class CEzShortcutDlg : public CDialog
{
private:
	MAP_SHORTCUT_INFO m_map_ShortcutInfo;

// 생성
public:
	CEzShortcutDlg(CWnd* pParent = NULL);	// 표준 생성자
	~CEzShortcutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_EZSHORTCUT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// 구현
protected:
	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected:
	afx_msg void OnLbnSelchangeListShortcut();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonRemove();
	afx_msg void OnLbnDblclkListParameter();
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);

protected:
	afx_msg LRESULT OnClose(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTrayIconMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDestroy(WPARAM wParam, LPARAM lParam);

private:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL UpdateData(BOOL bSaveAndValidate);
	
private:
	bool OnEditShortcutDlg(SHORTCUT_INFO& ShortcutInfo);

private: //초기화 메소드
	void Clear();

private: //자동 저장 기능
	void AutoSave();

private: //팝업 메뉴 콜백
	bool OnExitPopup();

private: //Draw 콜백
	void OnDrawIcon();

private: // 다이얼로그 컨트롤 관련
	void InitInputDlg();
	
private: // 숏컷 데이터관련
	bool LoadShortCutList();
	bool SaveShortCutList();
	void RebuildShortcutList();

private: //숏컷 정보 관련
	bool AddShortCutInfo(SHORTCUT_INFO& ShotcutInfo);
	bool RemoveShortCutInfo(const std::wstring& strShortCutName);
	SHORTCUT_INFO* GetShortCutInfo(const std::wstring& strShortCutName);
	void CopyShortCutInfo(const SHORTCUT_INFO& ShotcutInfo);
	void ClearShortCutInfo();

private:
	void EnableHotkeys(const BOOL bSwitch);

private: //트레이
	void RegisterTrayIcon();
	void DeleteTrayIcon();

private: //상수 데이터
	static const int ICON_X = 8;
	static const int ICON_Y = 138;
	const std::wstring m_strXMLFileName;

private: 
	CListBox m_listboxShortCutInfo;
	BOOL m_bTrayIcon;
	light::Popup m_Popup;
	HICON m_hIcon;

public:
	afx_msg void OnBnClickedButtonEdit();
	CStatic m_staticHotKeyContent;
	CStatic m_staticShortcutNameContent;
	afx_msg void OnDropFiles(HDROP hDropInfo);
};
