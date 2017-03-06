#include "stdafx.h"
#include "Examination.h"
#include "ControlNames.h"


CExamination::CExamination(CPaintManagerUI& PaintManagerUI)
	: m_PaintManager(PaintManagerUI)
	, m_bIsRunning(FALSE)
	, m_bIsStop(FALSE)
	, m_pProblemListUI(nullptr)
	, m_pExaminationBtn(nullptr)
	, m_pReturnBtn(nullptr)
	, m_pTipsText(nullptr)
	, m_pExamationThread(nullptr)
	, m_pVLayoutMain(nullptr)
	, m_pVLayoutExamination(nullptr)
	, m_pProgressFront(nullptr)
{
}


CExamination::~CExamination()
{
}

VOID CExamination::Init()
{
	m_pProblemListUI = static_cast<CProblemListUI*>(m_PaintManager.FindControl(bpcProblemList));
	m_pExaminationBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(bpcBtnExamination));
	m_pReturnBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(bpcBtnReturn));
	m_pTipsText = static_cast<CTextUI*>(m_PaintManager.FindControl(bpcTextTooltip));
	m_pVLayoutMain = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(bpcTabLayoutMain));
	m_pVLayoutExamination = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(bpcTabLayoutExamination));
	m_pProgressFront = static_cast<CProgressUI*>(m_PaintManager.FindControl(bpcProgressFront));
}

VOID CExamination::Back()
{
	m_pReturnBtn->SetVisible(FALSE);
	m_pVLayoutExamination->SetVisible(FALSE);
	m_pVLayoutMain->SetVisible(TRUE);

	m_pTipsText->SetText(_T(""));
	m_pExaminationBtn->SetText(_T("立即处理"));
	m_pProblemListUI->RemoveAll();
}

BOOL CExamination::Start()
{
	BOOL bRet = FALSE;

	if (nullptr != m_pProblemListUI)
	{
		m_bIsStop = FALSE;
		m_pVLayoutMain->SetVisible(FALSE);
		m_pVLayoutExamination->SetVisible(TRUE);
		m_pExamationThread = new std::thread(BeginExamation, this);
		bRet = TRUE;
	}

	return bRet;
}

BOOL CExamination::Stop()
{
	m_bIsStop = TRUE;
	return TRUE;
}

BOOL CExamination::IsRunning()
{
	return m_bIsRunning;
}

VOID CExamination::BeginExamation(CExamination* pExamination)
{
	if (nullptr != pExamination)
	{
		pExamination->ExamationThreadInstance();
	}
}

VOID CExamination::ExamationThreadInstance()
{
	m_bIsRunning = TRUE;
	CDuiString strText;

	m_pExaminationBtn->SetText(_T("停止检测"));
	m_pExaminationBtn->SetBkColor(0xFFE7614E);

	m_pProblemListUI->RemoveAll();

	m_pTipsText->SetText(_T("正在检查系统可清理垃圾文件..."));

	m_pProblemListUI->AddGroup(_T("清理"));
	for (int i = 0; i < 8 && m_bIsStop == FALSE; i++)
	{
		strText.Format(_T("清理垃圾第 %d 项测试内容"), i);
		m_pProblemListUI->AddItem(_T("清理"), strText.GetData());
		m_pProgressFront->SetValue(static_cast<int>((static_cast<float>(i) / 8) * 50));
		Sleep(300);
	}

	m_pProblemListUI->AddGroup(_T("加速"));
	for (int i = 0; i < 8 && m_bIsStop == FALSE; i++)
	{
		strText.Format(_T("系统加速第 %d 项测试内容"), i);
		m_pProblemListUI->AddItem(_T("加速"), strText.GetData());
		m_pProgressFront->SetValue(static_cast<int>((static_cast<float>(i) / 8) * 50) + 50);
		Sleep(300);
	}

	m_pProgressFront->SetValue(100);
	m_pReturnBtn->SetVisible(TRUE);
	m_pTipsText->SetText(_T("有可优化项，请根据所需进行处理！"));
	m_pExaminationBtn->SetBkColor(0xFFED8033);
	m_pExaminationBtn->SetText(_T("立即处理"));
	m_bIsRunning = FALSE;
}

