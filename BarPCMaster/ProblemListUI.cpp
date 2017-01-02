#include "StdAfx.h"
#include "ProblemListUI.h"

/************************************************************************/
/*       CProblemListGroupUI                                            */
/************************************************************************/
CProblemListGroupUI::CProblemListGroupUI()
{

}

void CProblemListGroupUI::SetProblemList(CProblemListUI* pProblemList)
{
	if (NULL != pProblemList)
	{
		m_pProblemList = pProblemList;
	}
}

/************************************************************************/
/*       CProblemListUI                                                 */
/************************************************************************/
CProblemListUI::CProblemListUI(CPaintManagerUI& PaintManager)
	:m_PaintManager(PaintManager)
{
	SetItemShowHtml(TRUE);
}


CProblemListUI::~CProblemListUI()
{
}

CControlUI* CProblemListUI::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass, szProblemListGroupUIInferFace) == 0)
	{
		return new CProblemListGroupUI();
	}
	else if (_tcsicmp(pstrClass, szProblemListItemUIInferFace) == 0)
	{
		return new CProblemListItemUI();
	}

	return NULL;
}

bool CProblemListUI::AddGroup(LPCTSTR szGroupName, int nIndex /*= -1*/)
{
	CProblemListGroupUI* pListGroup = NULL;
	if (!m_dlgGroupBuilder.GetMarkup()->IsValid())
	{
		pListGroup = static_cast<CProblemListGroupUI*>(m_dlgGroupBuilder.Create(_T("problem_list\\problem_list_group.xml"), (UINT)0, this, &m_PaintManager));
	}
	else
	{
		pListGroup = static_cast<CProblemListGroupUI*>(m_dlgGroupBuilder.Create(this, &m_PaintManager));
	}
	if (pListGroup == NULL)
	{
		return FALSE;
	}

	pListGroup->SetName(szGroupName);
	pListGroup->SetItemText(szGroupName);
	pListGroup->SetProblemList(this);

	if (nIndex == -1)
	{
		if (!Add(pListGroup))
		{
			delete pListGroup;
			return FALSE;
		}
	}
	else
	{
		if (!AddAt(pListGroup, nIndex))
		{
			delete pListGroup;
			return FALSE;
		}
	}

	return TRUE;
}

bool CProblemListUI::AddItem(LPCTSTR szGroupName, bool bAutoRefreshIndex /*= true*/, int nIndex /*= -1*/)
{
	CProblemListItemUI* pListElement = NULL;
	if (!m_dlgItemBuilder.GetMarkup()->IsValid())
	{
		pListElement = static_cast<CProblemListItemUI*>(m_dlgItemBuilder.Create(_T("problem_list\\problem_list_item.xml"), (UINT)0, this, &m_PaintManager));
	}
	else
	{
		pListElement = static_cast<CProblemListItemUI*>(m_dlgItemBuilder.Create(this, &m_PaintManager));
	}

	if (pListElement == NULL)
	{
		return NULL;
	}

	CLabelUI* pProblemText = static_cast<CLabelUI*>(m_PaintManager.FindSubControlByName(pListElement, _T("ProblemText")));
	if (NULL != pProblemText)
	{
		pProblemText->SetText(_T("Chrome 安装目录缓存文件"));
	}

	CProblemListGroupUI* pDefaultGroup = static_cast<CProblemListGroupUI*>(m_PaintManager.FindSubControlByName(this, szGroupName));
	if (NULL == pDefaultGroup)
	{
		return FALSE;
	}

	if (nIndex == -1)
	{
		if (!pDefaultGroup->Add(pListElement))
		{
			delete pListElement;
			return FALSE;
		}
	}
	else
	{
		if (!pDefaultGroup->AddAt(pListElement, nIndex))
		{
			delete pListElement;
			return FALSE;
		}
	}

	return TRUE;
}
