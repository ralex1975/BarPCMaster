#include "StdAfx.h"
#include "MainFrameWnd.h"
#include "ControlNames.h"


CMainFrameWnd::CMainFrameWnd(CDuiString strXMLPath)
	: m_strXMLPath(strXMLPath)
	, m_Examination(nullptr)
	, m_pProblemListUI(nullptr)
	, m_pExaminationBtn(nullptr)
	, m_pReturnBtn(nullptr)
	, m_pTipsText(nullptr)
	, m_pVLayoutMain(nullptr)
	, m_pVLayoutExamination(nullptr)
	, m_pProgressFront(nullptr)
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
			ShowMainLayout();
			/*if (nullptr != m_Examination)
			{
				m_Examination->Back();
			}*/
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
			ShowExaminationLayout();

			if (nullptr != m_Examination)
			{
				m_Examination->Start();
			}
		}
	}
}

void CMainFrameWnd::InitWindow()
{
	// 初始化各个控件
	m_pProblemListUI = static_cast<CProblemListUI*>(m_PaintManager.FindControl(bpcProblemList));
	m_pExaminationBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(bpcBtnExamination));
	m_pReturnBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(bpcBtnReturn));
	m_pTipsText = static_cast<CTextUI*>(m_PaintManager.FindControl(bpcTextTooltip));
	m_pVLayoutMain = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(bpcTabLayoutMain));
	m_pVLayoutExamination = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(bpcTabLayoutExamination));
	m_pProgressFront = static_cast<CProgressUI*>(m_PaintManager.FindControl(bpcProgressFront));

	m_Examination = new CExamination(this);
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

BOOL CMainFrameWnd::AddGroupToList(LPCTSTR lpGroupName)
{
	return m_pProblemListUI->AddGroup(lpGroupName);
}

BOOL CMainFrameWnd::AddItemToList(LPCTSTR lpGroupName, LPCTSTR lpItemValue)
{
	return m_pProblemListUI->AddItem(lpGroupName, lpItemValue);
}

void CMainFrameWnd::SetProgressValue(int value)
{
	m_pProgressFront->SetValue(value);
}

void CMainFrameWnd::SetBtnText(LPCTSTR lpText)
{
	m_pExaminationBtn->SetText(lpText);
}

void CMainFrameWnd::SetBtnBkColor(DWORD dwColor)
{
	m_pExaminationBtn->SetBkColor(dwColor);
}

void CMainFrameWnd::SetTipsText(LPCTSTR lpTipsText)
{
	m_pTipsText->SetText(lpTipsText);
}

void CMainFrameWnd::ShowReturnButton(bool bIsShow)
{
	m_pReturnBtn->SetVisible(bIsShow);
}

void CMainFrameWnd::ShowMainLayout()
{
	m_pReturnBtn->SetVisible(FALSE);
	m_pVLayoutExamination->SetVisible(FALSE);
	m_pVLayoutMain->SetVisible(TRUE);
	m_pTipsText->SetText(_T(""));
	m_pExaminationBtn->SetText(_T("立即处理"));
	m_pProblemListUI->RemoveAll();
}

void CMainFrameWnd::ShowExaminationLayout()
{
	m_pVLayoutMain->SetVisible(FALSE);
	m_pVLayoutExamination->SetVisible(TRUE);
	m_pProblemListUI->RemoveAll();
}
