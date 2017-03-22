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

			// 判断操作类型
			switch (stRowData.nOperation)
			{
			case EXT_OPT_FILES:
				{
					DWORD	 dwError = 0;
					LONGLONG llSize = 0;

					dwError = ScanDirectory(stRowData.strPath, stRowData.strKey, llSize);
					if (0 != dwError || 0 == llSize)
					{
						// 如果出现错误或者扫描的目录没有垃圾文件，那么跳出
						LOG(INFO) << _T("Has no files, dwError = ") << dwError
							<< _T(", llSize = ") << llSize;
						continue;
					}

					stRowData.strDescription += _T(" (");
					stRowData.strDescription += GetSizeString(llSize);
					stRowData.strDescription += _T(")");
					LOG(INFO) << _T("扫描目录 ") << stRowData.strPath.GetData()
						<< _T(" 有残留文件大小 ") << llSize << " Byte.";
					break;
				}
			case EXT_OPT_REG64:
			case EXT_OPT_REG32:
				{
					DWORD dwError = 0;
					dwError = ScanRegistry(stRowData);
					if (0 != dwError)
					{
						// 没有找到指定的注册表或内容不匹配
						LOG(INFO) << _T("Registry key not found, ")
							<< stRowData.strPath << _T(", key = ")
							<< stRowData.strKey;
						continue;
					}
					break;
				}
			}

			switch (stRowData.nType)
			{
			case EXT_TYPE_CLEAN:
				strGroup = _T("清理");
				break;
			case EXT_TYPE_SPEEDUP:
				strGroup = _T("加速");
				break;
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

DWORD CExamination::ScanDirectory(LPCTSTR szDirectory, LPCTSTR szType, LONGLONG& dwFileSize)
{
	DWORD dwError = 0;
	TCHAR szFindPath[MAX_PATH] = { 0 };
	WIN32_FIND_DATA ffd = { 0 };
	LARGE_INTEGER nFileSize;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	_tcscpy_s(szFindPath, szDirectory);
	PathAppend(szFindPath, _T("*.*"));

	// LOG(INFO) << _T("Start Scan Directory ") << szFindPath;

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
				dwError = ScanDirectory(szFindPath, szType, dwFileSize);
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

DWORD CExamination::ScanRegistry(const PROBLEMITEM& refProblemData)
{
	DWORD		dwError = 0;
	HKEY		hKey = nullptr;
	HKEY		hPrimaryKey = nullptr;
	LONG		lRet = 0;

	do 
	{
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

		// 查询值
		DWORD cbData = 0;
		PBYTE pszBuffer = nullptr;

		lRet = RegQueryValueEx(hKey, 
			refProblemData.strKey.GetData(), 
			NULL, 
			NULL, 
			reinterpret_cast<LPBYTE>(pszBuffer),
			&cbData);
		if (ERROR_SUCCESS != lRet)
		{
			dwError = lRet;
			LOG(ERROR) << _T("Failed to query key ") << strPrimaryKey.GetData()
				<< _T("\\") << strSubKey.GetData() << _T(" -> ") 
				<< refProblemData.strKey.GetData() << _T(", error code = ")
				<< lRet;
			break;
		}

		pszBuffer = new BYTE[cbData];
		lRet = RegQueryValueEx(hKey,
			refProblemData.strKey.GetData(),
			NULL,
			NULL,
			reinterpret_cast<LPBYTE>(pszBuffer),
			&cbData);
		if (ERROR_SUCCESS != lRet)
		{
			dwError = lRet;
			LOG(ERROR) << _T("Failed to query key ") << strPrimaryKey.GetData()
				<< _T("\\") << strSubKey.GetData() << _T(" -> ")
				<< refProblemData.strKey.GetData() << _T(", error code = ")
				<< lRet;
			break;
		}

		// 如果是字符串类型
		if (refProblemData.strKeyType == EXT_KEYTYPE_STRING)
		{
			if (_tcsicmp(reinterpret_cast<TCHAR*>(pszBuffer), refProblemData.strValue.GetData()) == 0)
			{
				LOG(INFO) << _T("");
				break;
			}
			else
			{
				RegCloseKey(hKey);
				dwError = -1;
				LOG(ERROR) << pszBuffer << _T(" is not equal to ")
					<< refProblemData.strValue.GetData();
				if (nullptr != pszBuffer)
				{
					delete[] pszBuffer;
				}
				break;
			}
		}
		// 如果是 DWORD 类型
		if (refProblemData.strKeyType == EXT_KEYTYPE_DWORD)
		{
			DWORD nTmpValue = *(reinterpret_cast<DWORD*>(pszBuffer));
			DWORD nValue = _ttoi(refProblemData.strValue.GetData());

			if (refProblemData.nComparison == EXT_COMPT_LT)
			{
				if (nTmpValue >= nValue)
				{
					dwError = -1;
				}
			}
			if (refProblemData.nComparison == EXT_COMPT_LTE)
			{
				if (nTmpValue > nValue)
				{
					dwError = -1;
				}
			}
			if (refProblemData.nComparison == EXT_COMPT_EQUAL)
			{
				if (nTmpValue != nValue)
				{
					dwError = -1;
				}
			}
			if (refProblemData.nComparison == EXT_COMPT_GTE)
			{
				if (nTmpValue < nValue)
				{
					dwError = -1;
				}
			}
			if (refProblemData.nComparison == EXT_COMPT_GT)
			{
				if (nTmpValue <= nValue)
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

