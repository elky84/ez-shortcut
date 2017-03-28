#pragma once

#include "afxwin.h"

#include "resource.h"		// 주 기호

class CEzShortcutApp : public CWinApp
{
public:
	CEzShortcutApp();

// 재정의
	public:
	virtual BOOL InitInstance();

// 구현

	DECLARE_MESSAGE_MAP()
};

extern CEzShortcutApp theApp;
