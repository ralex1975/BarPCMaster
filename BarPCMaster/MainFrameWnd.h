#pragma once
#include "ProblemListUI.h"
#include "ControlNames.h"


class CMainFrameWnd : public WindowImplBase
{
public:
	CMainFrameWnd();
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
	CWndShadow*		m_pWndShadow;

	CProblemListUI*	m_pProblemList;
};

