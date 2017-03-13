#include "stdafx.h"
#include "DBDataMgr.h"


CDBDataMgr::CDBDataMgr(LPCTSTR lpPath)
	: m_pDB(nullptr)
	, m_strPath(lpPath)
{
}


CDBDataMgr::~CDBDataMgr()
{
	if (nullptr != m_pDB)
	{
		m_pDB->close();
		delete m_pDB;
		m_pDB = nullptr;
	}
}

void CDBDataMgr::Init()
{
	if (nullptr == m_pDB)
	{
		m_pDB = new CppSQLite3DB;
		m_pDB->open(m_strPath);
	}

	// 判断表是否存在，不存在则新建表
	if (!m_pDB->tableExists(SQLITE_DB_TABLE))
	{
		m_pDB->execDML(SQLITE_SQL_CREATE_TABLE);
	}
}

UINT CDBDataMgr::GetRowCount()
{
	if (m_strPath.IsEmpty())
	{
		return 0;
	}

	CDuiString strSQL = _T("");

	strSQL = _T("SELECT COUNT(*) FROM ");
	strSQL += SQLITE_DB_TABLE;
	strSQL += _T(";");

	return m_pDB->execScalar(strSQL);
}

BOOL CDBDataMgr::GetAllData(std::vector<EXAMINATION>& vecExaminationList)
{
	BOOL bRet = FALSE;

	try
	{
		CppSQLite3Query queryObj = m_pDB->execQuery(TEXT("SELECT * FROM EXAMINATION;"));

		while (!queryObj.eof())
		{
			EXAMINATION stRowData;

			stRowData.nID = queryObj.getIntField(_T("ID"));
			stRowData.strDescription = queryObj.getStringField(_T("DESCRIPTION"));
			stRowData.bStatus = queryObj.getIntField(_T("STATUS"));
			stRowData.nType = queryObj.getIntField(_T("TYPE"));
			stRowData.nOperation = queryObj.getIntField(_T("OPERATION"));
			stRowData.strPath = queryObj.getStringField(_T("PATH"));
			stRowData.strKey = queryObj.getStringField(_T("KEY"));
			stRowData.strKeyType = queryObj.getStringField(_T("KEYTYPE"));
			stRowData.strValue = queryObj.getStringField(_T("VALUE"));
			stRowData.nComparison = queryObj.getIntField(_T("COMPARISON"));
			stRowData.strNotes = queryObj.getStringField(_T("NOTES"));

			vecExaminationList.push_back(stRowData);

			queryObj.nextRow();
		}
		queryObj.finalize();

		bRet = TRUE;
	}
	catch (CppSQLite3Exception ex)
	{
		bRet = FALSE;
	}
	
	return bRet;
}
