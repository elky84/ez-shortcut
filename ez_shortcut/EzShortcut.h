#pragma once

#include "afxwin.h"

#include "resource.h"		// �� ��ȣ

class CEzShortcutApp : public CWinApp
{
public:
	CEzShortcutApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CEzShortcutApp theApp;
