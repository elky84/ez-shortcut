#pragma once

#include "afxwin.h"

#include <vector>
#include "Str.h"
#include "Menu.h"

// CEzShortcutDlg ��ȭ ����
class CEzShortcutDlg : public CDialog
{
private:
	MAP_SHORTCUT_INFO m_map_ShortcutInfo;

// ����
public:
	CEzShortcutDlg(CWnd* pParent = NULL);	// ǥ�� ������
	~CEzShortcutDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_EZSHORTCUT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// ����
protected:
	// �޽��� �� �Լ��� �����߽��ϴ�.
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

private: //�ʱ�ȭ �޼ҵ�
	void Clear();

private: //�ڵ� ���� ���
	void AutoSave();

private: //�˾� �޴� �ݹ�
	bool OnExitPopup();

private: //Draw �ݹ�
	void OnDrawIcon();

private: // ���̾�α� ��Ʈ�� ����
	void InitInputDlg();
	
private: // ���� �����Ͱ���
	bool LoadShortCutList();
	bool SaveShortCutList();
	void RebuildShortcutList();

private: //���� ���� ����
	bool AddShortCutInfo(SHORTCUT_INFO& ShotcutInfo);
	bool RemoveShortCutInfo(const std::wstring& strShortCutName);
	SHORTCUT_INFO* GetShortCutInfo(const std::wstring& strShortCutName);
	void CopyShortCutInfo(const SHORTCUT_INFO& ShotcutInfo);
	void ClearShortCutInfo();

private:
	void EnableHotkeys(const BOOL bSwitch);

private: //Ʈ����
	void RegisterTrayIcon();
	void DeleteTrayIcon();

private: //��� ������
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
