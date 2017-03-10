#include "stdafx.h"
#include "Examination.h"
#include "ControlNames.h"
#include "MainFrameWnd.h"


CExamination::CExamination(CMainFrameWnd* pMainFrameWnd)
	: m_pMainFrameWnd(pMainFrameWnd)
	, m_bIsRunning(FALSE)
	, m_bIsStop(FALSE)
	, m_pExamationThread(nullptr)
{
}


CExamination::~CExamination()
{
}

BOOL CExamination::Start()
{
	BOOL bRet = FALSE;

	/*
		1、开始体检
		2、设置内循环 bIsStop 变量为 TRUE，让线程内部可以正常执行 while 循环
		3、启动体检线程
	*/
	m_bIsStop = FALSE;
	m_pExamationThread = new std::thread(BeginExamation, this);
	if (nullptr != m_pExamationThread)
	{
		bRet = TRUE;
	}

	return bRet;
}

BOOL CExamination::Stop()
{
	// 停止线程中正在进行体检的 while 循环
	m_bIsStop = TRUE;
	return TRUE;
}

BOOL CExamination::IsRunning()
{
	// 返回当前线程是否在运行中
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

	m_pMainFrameWnd->SetBtnText(_T("停止检测"));
	m_pMainFrameWnd->SetBtnBkColor(STOP_BTN_BKCOLOR);
	m_pMainFrameWnd->SetTipsText(_T("正在检查系统可清理垃圾文件..."));

	m_pMainFrameWnd->AddGroupToList(_T("清理"));
	for (int i = 0; i < 8 && m_bIsStop == FALSE; i++)
	{
		strText.Format(_T("清理垃圾第 %d 项测试内容"), i);
		m_pMainFrameWnd->AddItemToList(_T("清理"), strText.GetData());
		m_pMainFrameWnd->SetProgressValue(static_cast<int>((static_cast<float>(i) / 8) * 50));
		Sleep(300);
	}

	m_pMainFrameWnd->AddGroupToList(_T("加速"));
	for (int i = 0; i < 8 && m_bIsStop == FALSE; i++)
	{
		strText.Format(_T("系统加速第 %d 项测试内容"), i);
		m_pMainFrameWnd->AddItemToList(_T("加速"), strText.GetData());
		m_pMainFrameWnd->SetProgressValue(static_cast<int>((static_cast<float>(i) / 8) * 50) + 50);
		Sleep(300);
	}

	// 判断是否有可优化项
	if (TRUE)
	{
		m_pMainFrameWnd->SetTipsText(_T("有可优化项，请根据所需进行处理！"));
		m_pMainFrameWnd->SetBtnBkColor(START_BTN_BKCOLOR);
		m_pMainFrameWnd->SetBtnText(_T("立即处理"));
	}

	// 如果非手动停止，则设置进度条到100%
	if (!m_bIsStop)
	{
		m_pMainFrameWnd->SetProgressValue(100);
	}

	// 显示返回按钮
	m_pMainFrameWnd->ShowReturnButton();
	
	m_bIsRunning = FALSE;
}

