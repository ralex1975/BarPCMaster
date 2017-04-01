#include "stdafx.h"
#include "Examination.h"
#include "ControlNames.h"
#include "MainFrameWnd.h"

CExamination::CExamination(CMainFrameWnd* pMainFrameWnd)
	: m_pMainFrameWnd(pMainFrameWnd)
	, m_bIsRunning(FALSE)
	, m_bIsStop(FALSE)
	, m_pExamationThread(nullptr)
	, m_pDBMgr(nullptr)
{
}


CExamination::~CExamination()
{
	if (nullptr != m_pDBMgr)
	{
		delete m_pDBMgr;
		m_pDBMgr = nullptr;
	}
}

BOOL CExamination::Start()
{
	BOOL bRet = FALSE;

	if (nullptr == m_pDBMgr)
	{
		TCHAR szFilePath[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, szFilePath, MAX_PATH);
		PathRemoveFileSpec(szFilePath);
		PathAppend(szFilePath, SQLITE_DB_FOLDER);
		if (_taccess(szFilePath, 0) == -1)
		{
			CreateDirectory(szFilePath, NULL);
		}
		PathAppend(szFilePath, SQLITE_DB_FILENAME);

		m_pDBMgr = new CDBDataMgr(szFilePath);
		m_pDBMgr->Init();
	}

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
	BOOL		bSuccess = FALSE;
	UINT		uCount = 0;
	CDuiString	strText = _T("");

	// 设置为工作状态
	m_bIsRunning = TRUE;

	// 设置各个控件状态
	m_pMainFrameWnd->SetBtnText(_T("停止检测"));
	m_pMainFrameWnd->SetBtnBkColor(STOP_BTN_BKCOLOR);
	m_pMainFrameWnd->SetTipsText(_T("正在检查系统可清理垃圾文件..."));

	// 清空问题列表
	m_vecProblemList.clear();

	do 
	{
		// 获取所有数据
		bSuccess = m_pDBMgr->GetAllData(m_vecProblemList);
		if (!bSuccess)
		{
			break;
		}

		// 遍历开始扫描
		for (auto &stRowData : m_vecProblemList)
		{
			CDuiString	strGroup	= _T("");
			CDuiString	strTipsText = _T("");

			strTipsText.Format(_T("检查 %s ..."), stRowData.strDescription);
			m_pMainFrameWnd->SetTipsText(strTipsText);

			switch (stRowData.nType)
			{
			case EXT_TYPE_CLEAN:
				{
					if (!Clean(stRowData))
					{
						continue;
					}

					strGroup = _T("清理");
					break;
				}
			case EXT_TYPE_SPEEDUP:
				{
					if (!SpeedUp(stRowData))
					{
						continue;
					}

					strGroup = _T("加速");
					break;
				}
			}

			m_pMainFrameWnd->AddItemToList(strGroup, &stRowData);
			m_pMainFrameWnd->SetProgressValue(static_cast<int>((static_cast<float>(++uCount) / m_vecProblemList.size()) * 100));
		}
	} while (FALSE);
	

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
	
	LOG(INFO) << _T("Examine finished.");

	m_bIsRunning = FALSE;
}

BOOL CExamination::Clean(PROBLEMITEM& ProblemData)
{
	BOOL bRet = FALSE;

	// 判断操作类型
	switch (ProblemData.nOperation)
	{
	case EXT_OPT_FILES:
		{
			DWORD	 dwError = 0;
			LONGLONG llSize = 0;

			dwError = SearchDirectory(ProblemData.strPath, ProblemData.strKey, llSize);
			if (0 != dwError || 0 == llSize)
			{
				// 如果出现错误或者扫描的目录没有垃圾文件，那么跳出
				LOG(INFO) << _T("Has no files, dwError = ") << dwError
					<< _T(", llSize = ") << llSize;
				break;
			}

			// 修改显示文字，增加垃圾文件大小
			ProblemData.strDescription += _T(" (");
			ProblemData.strDescription += GetSizeString(llSize);
			ProblemData.strDescription += _T(")");
			LOG(INFO) << _T("扫描目录 ") << ProblemData.strPath.GetString()
				<< _T(" 有残留文件大小 ") << llSize << " Byte.";

			bRet = TRUE;
			break;
		}
	case EXT_OPT_REG64:
	case EXT_OPT_REG32:
		{
			DWORD dwError = 0;
			PBYTE pData = nullptr;

			dwError = SearchRegistry(ProblemData, pData);

			if (0 != dwError)
			{
				// 没有找到指定的注册表或内容不匹配
				LOG(INFO) << _T("Registry key not found, ")
					<< ProblemData.strPath << _T(", key = ")
					<< ProblemData.strKey;
				if (nullptr != pData)
				{
					delete[] pData;
					pData = nullptr;
				}
				break;
			}
			
			if (nullptr != pData)
			{
				delete[] pData;
				pData = nullptr;
			}
			bRet = TRUE;
			break;
		}
	}

	return bRet;
}

BOOL CExamination::SpeedUp(PROBLEMITEM & ProblemData)
{
	BOOL bRet = FALSE;

	switch (ProblemData.nOperation)
	{
	case EXT_OPT_FILES:
		{
			break;
		}
	case EXT_OPT_REG64:
	case EXT_OPT_REG32:
		{
			DWORD dwError = 0;

			dwError = SearchRegistryEx(ProblemData);
			if (0 != dwError)
			{
				// 没有找到指定的注册表或内容不匹配
				LOG(INFO) << _T("Registry key not found, ")
					<< ProblemData.strPath << _T(", key = ")
					<< ProblemData.strKey;
				break;
			}

			bRet = TRUE;
			break;
		}
	}

	return bRet;
}

DWORD CExamination::SearchDirectory(LPCTSTR szDirectory, LPCTSTR szType, LONGLONG& dwFileSize)
{
	DWORD dwError = 0;
	TCHAR szFindPath[MAX_PATH] = { 0 };
	WIN32_FIND_DATA ffd = { 0 };
	LARGE_INTEGER nFileSize;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	_tcscpy_s(szFindPath, szDirectory);
	PathAppend(szFindPath, _T("*.*"));

	hFind = FindFirstFile(szFindPath, &ffd);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		dwError = GetLastError();
		LOG(ERROR) << _T("Find first file return error, error code = ") << dwError;
	}
	else
	{
		do
		{
			// 排除回收站、自己目录、父目录
			if (0 == _tcscmp(ffd.cFileName, _T(".")) ||
				0 == _tcscmp(ffd.cFileName, _T("..")) ||
				0 == _tcscmp(ffd.cFileName, _T("$Recycle.Bin")))
			{
				continue;
			}

			_tcscpy(szFindPath, szDirectory);
			PathAppend(szFindPath, ffd.cFileName);

			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				dwError = SearchDirectory(szFindPath, szType, dwFileSize);
			}
			else
			{
				nFileSize.LowPart = ffd.nFileSizeLow;
				nFileSize.HighPart = ffd.nFileSizeHigh;

				if (_tcscmp(szType, _T("*")) == 0)
				{
					// 计算所有文件大小
					dwFileSize += nFileSize.QuadPart;
					LOG(INFO) << _T("Found file ") << szFindPath;
				}
				else
				{
					// 同后缀名的文件才计算大小
					if (_tcscmp(PathFindExtension(szType), PathFindExtension(ffd.cFileName)) == 0)
					{
						LOG(INFO) << _T("Found file ") << szFindPath;
						dwFileSize += nFileSize.QuadPart;
					}
				}
				
			}
		} while (FindNextFile(hFind, &ffd) != 0);
	}

	FindClose(hFind);
	return dwError;
}

DWORD CExamination::SearchRegistry(const PROBLEMITEM& ProblemData, __out LPBYTE& pData)
{
	DWORD	dwError = 0;
	HKEY	hKey = nullptr;
	HKEY	hPrimaryKey = nullptr;
	LONG	lRet = 0;
	DWORD	cbData = 0;
	PBYTE	pszBuffer = nullptr;

	do
	{
		// 找到第一个斜杠
		int nFind = ProblemData.strPath.Find(_T('\\'));
		if (0 == nFind)
		{
			dwError = ERROR_INVALID_KEY;
			break;
		}

		// 得到主键位置
		CDuiString strPrimaryKey = ProblemData.strPath.Mid(0, nFind);
		if (strPrimaryKey == _T("HKEY_LOCAL_MACHINE"))
		{
			hPrimaryKey = HKEY_LOCAL_MACHINE;
		}
		else if (strPrimaryKey == _T("HKEY_CURRENT_USER"))
		{
			hPrimaryKey = HKEY_CURRENT_USER;
		}
		else if (strPrimaryKey == _T("HKEY_CLASSES_ROOT"))
		{
			hPrimaryKey = HKEY_CLASSES_ROOT;
		}
		else if (strPrimaryKey == _T("HKEY_USERS"))
		{
			hPrimaryKey = HKEY_USERS;
		}
		else
		{
			dwError = ERROR_INVALID_KEY;
			LOG(ERROR) << _T("Invalid primary key: ") << strPrimaryKey;
			break;
		}

		// 得到子键位置
		CDuiString strSubKey = ProblemData.strPath.Mid(nFind + 1,
			ProblemData.strPath.GetLength());

		lRet = RegOpenKeyEx(hPrimaryKey, strSubKey.GetData(), 0,
			KEY_QUERY_VALUE |
			(ProblemData.nOperation == EXT_OPT_REG32 ? KEY_WOW64_32KEY : KEY_WOW64_64KEY),
			&hKey);
		if (ERROR_SUCCESS != lRet)
		{
			dwError = lRet;
			LOG(ERROR) << _T("Failed to open key ") << strPrimaryKey.GetData()
				<< _T("\\") << strSubKey.GetData() << _T(", error code = ")
				<< lRet;
			break;
		}

		lRet = RegQueryValueEx(hKey,
			ProblemData.strKey.GetString(),
			NULL,
			NULL,
			reinterpret_cast<LPBYTE>(pszBuffer),
			&cbData);
		if (ERROR_SUCCESS != lRet)
		{
			dwError = lRet;
			LOG(ERROR) << _T("Failed to query key ") << strPrimaryKey.GetData()
				<< _T("\\") << strSubKey.GetData() << _T(" -> ")
				<< ProblemData.strKey.GetString() << _T(", error code = ")
				<< lRet;
			break;
		}

		pszBuffer = new BYTE[cbData];
		pData = new BYTE[cbData];

		lRet = RegQueryValueEx(hKey,
			ProblemData.strKey.GetString(),
			NULL,
			NULL,
			reinterpret_cast<LPBYTE>(pszBuffer),
			&cbData);
		if (ERROR_SUCCESS != lRet)
		{
			dwError = lRet;
			LOG(ERROR) << _T("Failed to query key ") << strPrimaryKey.GetData()
				<< _T("\\") << strSubKey.GetData() << _T(" -> ")
				<< ProblemData.strKey.GetString() << _T(", error code = ")
				<< lRet;
			break;
		}

		// 将查询到的值传出
		CopyMemory(pData, pszBuffer, cbData);

		// 如果是字符串类型
		if (ProblemData.nKeyType == EXT_KEYTYPE_STRING)
		{
			if (_tcsicmp(reinterpret_cast<TCHAR*>(pszBuffer), ProblemData.strValue.GetString()) == 0)
			{
				LOG(INFO) << _T("");
				RegCloseKey(hKey);
				break;
			}
			else
			{
				RegCloseKey(hKey);
				dwError = ERROR_VALUE_ARE_NOT_EQUAL;
				LOG(ERROR) << pszBuffer << _T(" is not equal to ")
					<< ProblemData.strValue.GetString();
				break;
			}
		}

		// 如果是 DWORD 类型
		if (ProblemData.nKeyType == EXT_KEYTYPE_DWORD)
		{
			DWORD nTmpValue = *(reinterpret_cast<DWORD*>(pszBuffer));
			DWORD nValue = _ttoi(ProblemData.strValue.GetString());

			// 如果找到直接跳出
			if (nTmpValue == nValue)
			{
				LOG(INFO) << _T("Found value ") << nTmpValue << _T("==") << nValue;
				RegCloseKey(hKey);
				break;
			}
			else
			{
				RegCloseKey(hKey);
				dwError = ERROR_VALUE_ARE_NOT_EQUAL;
				LOG(ERROR) << nTmpValue << _T(" is not equal to ") << nValue;
				break;
			}
		}

		RegCloseKey(hKey);

	} while (FALSE);

	if (nullptr != pszBuffer)
	{
		delete[] pszBuffer;
	}

	return dwError;
}

DWORD CExamination::SearchRegistryEx(const PROBLEMITEM& ProblemData)
{
	DWORD	dwError = 0;
	BOOL	bRet = FALSE;
	PBYTE	pData = nullptr;

	dwError = SearchRegistry(ProblemData, pData);

	do 
	{
		// 返回值不存在的时候要当作需要加速处理
		if (ERROR_FILE_NOT_FOUND == dwError)
		{
			dwError = 0;
			break;
		}

		// 值存在的情况下要与默认值进行比对
		if (ProblemData.nKeyType == EXT_KEYTYPE_STRING)
		{
			// 对比字符串，如果符合条件则不需要优化，不符合则显示在界面上
			bRet = CompareString(pData, 
				ProblemData.strValue.GetString(),
				ProblemData.nComparison, 
				ProblemData.nCompareType);
			dwError = static_cast<DWORD>(bRet);
			break;
		}
		
		if (ProblemData.nKeyType == EXT_KEYTYPE_DWORD)
		{
			// 对比 DWORD 值，如果符合条件则不需要优化，不符合则显示在界面上
			bRet = CompareDWORD(pData,
				ProblemData.strValue.GetString(),
				ProblemData.nComparison,
				ProblemData.nCompareType);
			dwError = static_cast<DWORD>(bRet);
			break;
		}
	} while (FALSE);

	if (nullptr != pData)
	{
		delete[] pData;
		pData = nullptr;
	}

	return dwError;
}

BOOL CExamination::CompareString(const PBYTE pData, CDuiString strDefaultValue, int nComparison, int nCompareType)
{
	BOOL bRet = FALSE;

	// 从注册表取出的数据有可能是字符串格式的，所以要转换成数值类型再比对
	// 数据库中 CompareType 代表了要以什么方式对比
	// 如果对比方式是以字符串方式对比，那么直接 strcmp，返回相等则是需要清理的
	if (nCompareType == EXT_KEYTYPE_STRING)
	{
		if (_tcsicmp(reinterpret_cast<TCHAR*>(pData), strDefaultValue.GetData()) == 0)
		{
			bRet = TRUE;
		}
	}
	
	// 如果对比方式是以DWORD数值类型对比，那么强转数据再对比，根据数据库配置对比大小
	if (nCompareType == EXT_KEYTYPE_DWORD)
	{
		TCHAR*	pszData = reinterpret_cast<TCHAR*>(pData);
		DWORD	dwData = _ttoi(pszData);
		DWORD	dwDefaultValue = _ttoi(strDefaultValue.GetData());

		switch (nComparison)
		{
		case EXT_COMPT_LT:
			if (dwData < dwDefaultValue)
			{
				bRet = TRUE;
			}
			break;
		case EXT_COMPT_LTE:
			if (dwData <= dwDefaultValue)
			{
				bRet = TRUE;
			}
			break;
		case EXT_COMPT_EQUAL:
			if (dwData == dwDefaultValue)
			{
				bRet = TRUE;
			}
			break;
		case EXT_COMPT_GTE:
			if (dwData >= dwDefaultValue)
			{
				bRet = TRUE;
			}
			break;
		case EXT_COMPT_GT:
			if (dwData > dwDefaultValue)
			{
				bRet = TRUE;
			}
			break;
		}
	}

	return bRet;
}

BOOL CExamination::CompareDWORD(const PBYTE pData, CDuiString strDefaultValue, int nComparison, int nCompareType)
{
	BOOL bRet = FALSE;

	DWORD dwData = *(reinterpret_cast<DWORD*>(pData));
	DWORD dwDefaultValue = _ttoi(strDefaultValue.GetData());

	switch (nComparison)
	{
	case EXT_COMPT_LT:
		if (dwData < dwDefaultValue)
		{
			bRet = TRUE;
		}
		break;
	case EXT_COMPT_LTE:
		if (dwData <= dwDefaultValue)
		{
			bRet = TRUE;
		}
		break;
	case EXT_COMPT_EQUAL:
		if (dwData == dwDefaultValue)
		{
			bRet = TRUE;
		}
		break;
	case EXT_COMPT_GTE:
		if (dwData >= dwDefaultValue)
		{
			bRet = TRUE;
		}
		break;
	case EXT_COMPT_GT:
		if (dwData > dwDefaultValue)
		{
			bRet = TRUE;
		}
		break;
	}

	return bRet;
}

DWORD CExamination::ScanRegistry(const PROBLEMITEM& refProblemData)
{
	DWORD		dwError = 0;
	HKEY		hKey = nullptr;
	HKEY		hPrimaryKey = nullptr;
	LONG		lRet = 0;
	DWORD		cbData = 0;
	PBYTE		pszBuffer = nullptr;

	do 
	{
		// 找到第一个斜杠
		int nFind = refProblemData.strPath.Find(_T('\\'));
		if (0 == nFind)
		{
			dwError = -1;
			break;
		}

		// 得到主键位置
		CDuiString strPrimaryKey = refProblemData.strPath.Mid(0, nFind);
		if (strPrimaryKey == _T("HKEY_LOCAL_MACHINE"))
		{
			hPrimaryKey = HKEY_LOCAL_MACHINE;
		}
		else if (strPrimaryKey == _T("HKEY_CURRENT_USER"))
		{
			hPrimaryKey = HKEY_CURRENT_USER;
		}
		else if (strPrimaryKey == _T("HKEY_CLASSES_ROOT"))
		{
			hPrimaryKey = HKEY_CLASSES_ROOT;
		}
		else if (strPrimaryKey == _T("HKEY_USERS"))
		{
			hPrimaryKey = HKEY_USERS;
		}
		else
		{
			dwError = -1;
			LOG(ERROR) << _T("Invalid primary key: ") << strPrimaryKey;
			break;
		}

		// 得到子键位置
		CDuiString strSubKey = refProblemData.strPath.Mid(nFind + 1,
			refProblemData.strPath.GetLength());

		lRet = RegOpenKeyEx(hPrimaryKey, strSubKey.GetData(), 0,
			KEY_QUERY_VALUE | 
			(refProblemData.nOperation == EXT_OPT_REG32 ? KEY_WOW64_32KEY : KEY_WOW64_64KEY), 
			&hKey);
		if (ERROR_SUCCESS != lRet)
		{
			dwError = lRet;
			LOG(ERROR) << _T("Failed to open key ") << strPrimaryKey.GetData()
				<< _T("\\") << strSubKey.GetData() << _T(", error code = ")
				<< lRet;
			break;
		}

		lRet = RegQueryValueEx(hKey, 
			refProblemData.strKey.GetString(), 
			NULL, 
			NULL, 
			reinterpret_cast<LPBYTE>(pszBuffer),
			&cbData);
		if (ERROR_SUCCESS != lRet)
		{
			dwError = lRet;
			LOG(ERROR) << _T("Failed to query key ") << strPrimaryKey.GetData()
				<< _T("\\") << strSubKey.GetData() << _T(" -> ") 
				<< refProblemData.strKey.GetString() << _T(", error code = ")
				<< lRet;
			break;
		}

		pszBuffer = new BYTE[cbData];
		lRet = RegQueryValueEx(hKey,
			refProblemData.strKey.GetString(),
			NULL,
			NULL,
			reinterpret_cast<LPBYTE>(pszBuffer),
			&cbData);
		if (ERROR_SUCCESS != lRet)
		{
			dwError = lRet;
			LOG(ERROR) << _T("Failed to query key ") << strPrimaryKey.GetData()
				<< _T("\\") << strSubKey.GetData() << _T(" -> ")
				<< refProblemData.strKey.GetString() << _T(", error code = ")
				<< lRet;
			break;
		}

		// 如果是字符串类型
		if (refProblemData.nKeyType == EXT_KEYTYPE_STRING)
		{
			if (_tcsicmp(reinterpret_cast<TCHAR*>(pszBuffer), refProblemData.strValue.GetString()) == 0)
			{
				LOG(INFO) << _T("");
				break;
			}
			else
			{
				RegCloseKey(hKey);
				dwError = -1;
				LOG(ERROR) << pszBuffer << _T(" is not equal to ")
					<< refProblemData.strValue.GetString();
				if (nullptr != pszBuffer)
				{
					delete[] pszBuffer;
				}
				break;
			}
		}

		// 如果是 DWORD 类型
		if (refProblemData.nKeyType == EXT_KEYTYPE_DWORD)
		{
			DWORD nTmpValue = *(reinterpret_cast<DWORD*>(pszBuffer));
			DWORD nValue = _ttoi(refProblemData.strValue.GetString());

			if (refProblemData.nComparison == EXT_COMPT_LT)
			{
				// 小于条件，如果实际值大于等于参考值，那么条件不符
				if (nTmpValue < nValue)
				{
					dwError = -1;
				}
			}
			if (refProblemData.nComparison == EXT_COMPT_LTE)
			{
				if (nTmpValue <= nValue)
				{
					dwError = -1;
				}
			}
			if (refProblemData.nComparison == EXT_COMPT_EQUAL)
			{
				if (nTmpValue == nValue)
				{
					dwError = -1;
				}
			}
			if (refProblemData.nComparison == EXT_COMPT_GTE)
			{
				if (nTmpValue >= nValue)
				{
					dwError = -1;
				}
			}
			if (refProblemData.nComparison == EXT_COMPT_GT)
			{
				if (nTmpValue > nValue)
				{
					dwError = -1;
				}
			}
		}

		if (nullptr != pszBuffer)
		{
			delete[] pszBuffer;
		}

		RegCloseKey(hKey);

	} while (FALSE);

	return dwError;
}

DWORD CExamination::ScanRegistryEx(const PROBLEMITEM& refProblemData)
{
	return 0;
}

CDuiString CExamination::GetSizeString(LONGLONG llSize)
{
	CDuiString strSize = _T("0 MB");

	if (llSize <= 0)
	{
		return strSize;
	}

	if (llSize < 1024)
	{
		strSize.Format(_T("%I64d Byte"), llSize);
		return strSize;
	}

	llSize /= 1024;

	if (llSize < 1024)
	{
		strSize.Format(_T("%I64d KB"), llSize);
		return strSize;
	}

	llSize /= 1024;

	if (llSize < 1024)
	{
		strSize.Format(_T("%I64d MB"), llSize);
		return strSize;
	}

	llSize /= 1024;

	if (llSize < 1024)
	{
		strSize.Format(_T("%I64d GB"), llSize);
		return strSize;
	}

	return strSize;
}

