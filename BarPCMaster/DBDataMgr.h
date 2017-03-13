#pragma once

#include "CppSQLite3U.h"

/*
数据库相关宏
*/
#define SQLITE_DB_FOLDER		_T("Data")
#define SQLITE_DB_FILENAME		_T("BarPCMaster.dat")
#define SQLITE_DB_TABLE			_T("EXAMINATION")
#define SQLITE_SQL_CREATE_TABLE	_T( \
"CREATE TABLE EXAMINATION(" \
"ID				INTEGER			PRIMARY KEY AUTOINCREMENT, " \
"DESCRIPTION	VARCHAR(50)		NOT NULL, " \
"STATUS			BOOL			NOT NULL, " \
"TYPE			INTEGER			NOT NULL, " \
"OPERATION		INTEGER			NOT NULL, " \
"PATH			VARCHAR(512)	NOT NULL, " \
"KEY			VARCHAR(256), " \
"KEYTYPE		VARCHAR(32), " \
"VALUE			VARCHAR(256), " \
"COMPARISON		INTEGER, " \
"NOTES			VARCHAR(256))")

typedef struct _tag_EXAMINATION 
{
	int				nID;
	CDuiString		strDescription;
	BOOL			bStatus;
	int				nType;
	int				nOperation;
	CDuiString		strPath;
	CDuiString		strKey;
	CDuiString		strKeyType;
	CDuiString		strValue;
	int				nComparison;
	CDuiString		strNotes;
} EXAMINATION, *PEXAMINATION;

class CDBDataMgr
{
public:
	CDBDataMgr(LPCTSTR lpPath);
	~CDBDataMgr();

	void	Init();
	UINT	GetRowCount();
	BOOL	GetAllData(std::vector<EXAMINATION>& vecExaminationList);

private:
	CppSQLite3DB*	m_pDB;
	CDuiString		m_strPath;
};

