#include "StdAfx.h"
#include "UIProblemList.h"

/************************************************************************/
/*                                                                      */
/************************************************************************/

CProblemListItemUI::CProblemListItemUI()
{

}

CProblemListItemUI::~CProblemListItemUI()
{

}

LPCTSTR CProblemListItemUI::GetClass() const
{
	return _T("TreeNodeUI");
}

LPVOID CProblemListItemUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcscmp(pstrName, bPCMasterProblemListItemUIInferFace) == 0)
	{
		return static_cast<CProblemListItemUI*>(this);
	}

	return CTreeNodeUI::GetInterface(pstrName);
}

/************************************************************************/
/*       CProblemListGroupUI                                            */
/************************************************************************/
CProblemListGroupUI::CProblemListGroupUI()
{

}

LPCTSTR CProblemListGroupUI::GetClass() const
{
	return _T("TreeNodeUI");
}

LPVOID CProblemListGroupUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcscmp(pstrName, bPCMasterProblemListGroupUIInferFace) == 0)
	{
		return static_cast<CProblemListGroupUI*>(this);
	}

	return CTreeNodeUI::GetInterface(pstrName);
}

void CProblemListGroupUI::SetItemText(LPCTSTR pstrValue)
{
	m_GroupName = pstrValue;
	CTreeNodeUI::SetItemText(pstrValue);
}

bool CProblemListGroupUI::Add(CProblemListItemUI* pControl, bool bAutoRefreshIndex /*= true*/)
{
	bool nRet = CTreeNodeUI::Add(pControl);

	if (bAutoRefreshIndex)
	{
		CDuiString text;

		/*int nIndex = GetTreeNodes().GetSize() + 1;
		text.Format(_T("%s[%d]"), m_GroupName.GetData(), GetTreeNodes().GetSize());
		CTreeNodeUI::SetItemText(text.GetData());*/
	}

	return nRet;
}

bool CProblemListGroupUI::AddAt(CProblemListItemUI* pControl, int iIndex, bool bAutoRefreshIndex /*= true*/)
{
	bool nRet = CTreeNodeUI::AddAt(pControl, iIndex);
	CTreeNodeUI::SetItemText(m_GroupName.GetData());

	return nRet;
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
	if (_tcsicmp(pstrClass, bPCMasterProblemListGroupUIInferFace) == 0)
	{
		return new CProblemListGroupUI();
	}
	else if (_tcsicmp(pstrClass, bPCMasterProblemListItemUIInferFace) == 0)
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

bool CProblemListUI::AddItem(LPCTSTR szGroupName, LPCTSTR szProblemText, bool bAutoRefreshIndex /*= true*/, int nIndex /*= -1*/)
{
	// 初始化 item 的数据
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

	// 给 Text 控件赋值
	CLabelUI* pProblemText = static_cast<CLabelUI*>(m_PaintManager.FindSubControlByName(pListElement, bPCMasterProblemItemTextControlName));
	if (NULL != pProblemText)
	{
		pProblemText->SetText(szProblemText);
	}

	// 根据传递进来的组名称搜索组控件对象
	CProblemListGroupUI* pDefaultGroup = static_cast<CProblemListGroupUI*>(m_pManager->FindSubControlByName(this, szGroupName));
	if (NULL == pDefaultGroup)
	{
		return FALSE;
	}

	// insert item to group
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
