#include "StdAfx.h"
#include "MainFrameWnd.h"
#include "ControlNames.h"


CMainFrameWnd::CMainFrameWnd(CDuiString strXMLPath)
	: m_strXMLPath(strXMLPath)
	, m_Examination(nullptr)
{
}


CMainFrameWnd::~CMainFrameWnd()
{
}

DuiLib::CDuiString CMainFrameWnd::GetSkinFolder()
{
	return m_PaintManager.GetInstancePath();
}

DuiLib::CDuiString CMainFrameWnd::GetSkinFile()
{
	return m_strXMLPath;
}

LPCTSTR CMainFrameWnd::GetWindowClassName(void) const
{
	return bpcMainFrameWnd;
}

CControlUI* CMainFrameWnd::CreateControl(LPCTSTR pstrClassName)
{
	if (_tcsicmp(pstrClassName, bpcProblemListUIInterFace) == 0)
	{
		return new CProblemListUI(m_PaintManager);
	}

	return NULL;
}

void CMainFrameWnd::Notify(TNotifyUI& msg)
{
	__super::Notify(msg);

	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == bpcBtnClose)
		{
			Close();
			PostQuitMessage(0);
		}
		if (msg.pSender->GetName() == bpcBtnReturn)
		{
			if (nullptr != m_Examination)
			{
				m_Examination->Back();
			}
		}
		if (msg.pSender->GetName() == bpcBtnExamination)
		{
			if (nullptr != m_Examination)
			{
				m_Examination->IsRunning() ? m_Examination->Stop() : MessageBox(NULL, _T("开始处理"), _T(""), MB_OK);
			}
		}
		if (msg.pSender->GetName() == bpcBtnStart)
		{
			if (nullptr != m_Examination)
			{
				m_Examination->Start();
			}
		}
	}
}

void CMainFrameWnd::InitWindow()
{
	m_Examination = new CExamination(m_PaintManager);
	if (nullptr != m_Examination)
	{
		m_Examination->Init();
	}
}

LRESULT CMainFrameWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if (uMsg == WM_KEYDOWN)
	{
		switch (wParam)
		{
		case VK_RETURN:
		case VK_ESCAPE:
			return FALSE;
		default:
			break;
		}
	}

	return FALSE;
}

LRESULT CMainFrameWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	__super::OnCreate(uMsg, wParam, lParam, bHandled);

	/*CShadowUI::Initialize(m_PaintManager.GetInstance());
	m_pShadowUI = new CShadowUI;
	m_pShadowUI->Create(m_hWnd);*/

#if 0
	RECT rcCorner = { 5,5,5,5 };
	RECT rcHoleOffset = { 0,0,0,0 };
	m_pWndShadow->SetImage(_T("bg_main_shadow.png"), rcCorner, rcHoleOffset);
#else
	/*
	m_pShadowUI->SetDarkness(50);
	m_pShadowUI->SetSize(10);
	m_pShadowUI->SetPosition(0, 0);
	*/
#endif

	return 0;
}
