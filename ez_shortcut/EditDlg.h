#pragma once
#include "afxwin.h"


// CEditDlg dialog

struct SHORTCUT_INFO;

class CEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditDlg)

public:
	CEditDlg(SHORTCUT_INFO& ShotcutInfo, const MAP_SHORTCUT_INFO& map_ShortcutInfo, CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedButtonFileFind();
	afx_msg void OnBnClickedButtonFolderFind();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();

private:
	SHORTCUT_INFO& m_ShotcutInfo;
	CEdit m_editboxName;
	CEdit m_editboxPath;
	CEdit m_editboxParameter;
	CEdit m_editboxHotkey;

private:
	light::HotKey m_TempHotkey;

private:
	const MAP_SHORTCUT_INFO& m_map_ShortcutInfo;
};
