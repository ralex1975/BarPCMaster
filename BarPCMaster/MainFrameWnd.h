#pragma once
#include "UIProblemList.h"


class CMainFrameWnd : public WindowImplBase
{
public:
	CMainFrameWnd(CDuiString strXMLPath);
	~CMainFrameWnd();

protected:
	virtual CDuiString	GetSkinFolder();
	virtual CDuiString	GetSkinFile();
	virtual LPCTSTR		GetWindowClassName(void) const;
	virtual CControlUI* CreateControl(LPCTSTR pstrClassName);

	virtual void		Notify(TNotifyUI& msg);
	virtual void		InitWindow();

	virtual LRESULT		MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	virtual LRESULT		OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	// CShadowUI*		m_pShadowUI;
	CProblemListUI*	m_pProblemList;
	CDuiString		m_strXMLPath;
};

