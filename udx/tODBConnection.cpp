#include "tODBConnection.h"
#include <windows.h>
//#include <sql.h>             // ???
#include <sqlext.h>
#include "tConfiguration.h"
#include "tDebug.h"
//#include <stdio.h>


static bool SQLOk(int retcode)
{
	return retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO;
}// SQLOk




tODBConnection::tODBConnection() :
	env(NULL),
	dbc(NULL),
	stmt(NULL),
	rows(0),
	tDbConnection(config.get("ODBC", "dbname"))
{
	SQLAllocEnv(&env);
	SQLAllocConnect(env, &dbc);
	if ( !Connect("", 0, 
	              config.get("ODBC", "dbuser"),
	              config.get("ODBC", "dbpassword"),
	              config.get("ODBC", "dbname")) ){
		setResult(1, nError::ConnectionFail(ErrorMessage()));
		return;
	}
	//SQLSetConnectOption(dbc, SQL_AUTOCOMMIT, FALSE);
	SQLSetConnectOption(dbc, SQL_ODBC_CURSORS, SQL_CUR_USE_ODBC);
	SQLSetConnectOption(dbc, SQL_LOGIN_TIMEOUT, 3);
	SQLAllocStmt(dbc, &stmt);
	SQLSetStmtOption(stmt, SQL_CURSOR_TYPE, SQL_CURSOR_STATIC);
	//debugLog("tODBConnection::tODBConnection()");
}// tODBConnection::tODBConnection


tODBConnection::tODBConnection(const string &dbname) :
	env(NULL),
	dbc(NULL),
	stmt(NULL),
	rows(0),
	tDbConnection(dbname)
{
	SQLAllocEnv(&env);
	SQLAllocConnect(env, &dbc);
	if ( !Connect("", 0, 
	              config.get("ODBC", "dbuser"),
	              config.get("ODBC", "dbpassword"),
	              dbname) ){
		setResult(1, nError::ConnectionFail(ErrorMessage()));
		return;
	}
	//SQLSetConnectOption(dbc, SQL_AUTOCOMMIT, FALSE);
	SQLSetConnectOption(dbc, SQL_ODBC_CURSORS, SQL_CUR_USE_ODBC);
	SQLSetConnectOption(dbc, SQL_LOGIN_TIMEOUT, 3);
	SQLAllocStmt(dbc, &stmt);
	SQLSetStmtOption(stmt, SQL_CURSOR_TYPE, SQL_CURSOR_STATIC);
	//debugLog("tODBConnection::tODBConnection()");
}// tODBConnection::tODBConnection


tODBConnection::tODBConnection(const string &dbname,
                               const string &login,
                               const string &passw) :
	env(NULL),
	dbc(NULL),
	stmt(NULL),
	rows(0),
	tDbConnection(dbname)
{
	SQLAllocEnv(&env);
	SQLAllocConnect(env, &dbc);
	if ( !Connect("", 0, login, passw, dbname) ){
		setResult(1, nError::ConnectionFail(ErrorMessage()));
		return;
	}
	//SQLSetConnectOption(dbc, SQL_AUTOCOMMIT, FALSE);
	SQLSetConnectOption(dbc, SQL_ODBC_CURSORS, SQL_CUR_USE_ODBC);
	SQLSetConnectOption(dbc, SQL_LOGIN_TIMEOUT, 3);
	SQLAllocStmt(dbc, &stmt);
	SQLSetStmtOption(stmt, SQL_CURSOR_TYPE, SQL_CURSOR_STATIC);
	//debugLog("tODBConnection::tODBConnection()");
}// tODBConnection::tODBConnection


tODBConnection::~tODBConnection()
{
	if ( stmt ) SQLFreeStmt(stmt, SQL_DROP);
	if ( dbc ){
		SQLDisconnect(dbc);
		SQLFreeConnect(dbc);
	}
	if ( env ) SQLFreeEnv(env);
	//debugLog("tODBConnection::~tODBConnection()");
}// tODBConnection::~tODBConnection


bool tODBConnection::Connect(const string &,           // host
                            int,                       // port
                            const string &user, 
                            const string &password, 
                            const string &dbname)
{
	return SQLConnect(dbc, 
	                  (unsigned char *)dbname.c_str(), SQL_NTS, 
	                  (unsigned char *)user.c_str(), SQL_NTS, 
	                  (unsigned char *)password.c_str(), SQL_NTS) != SQL_ERROR;
}// tODBConnection::Connect


string tODBConnection::GetFieldValue(int i, int j)
{
	cR();
	//debugLog("tODBConnection::GetFieldValue(" + ToString(i) + ", " + ToString(j) + ")");
	string ret;
	UDWORD pcrow;
	UWORD rgfRowStatus;
	int code = SQLExtendedFetch(stmt, SQL_FETCH_ABSOLUTE, i + 1, &pcrow, &rgfRowStatus);
	if ( SQLOk(code) ){
		int code = SQLSetPos(stmt, 1, SQL_POSITION, SQL_LOCK_NO_CHANGE);
		if ( SQLOk(code) ){
			UCHAR val[4096];
			SDWORD pcbValue;
			code = SQLGetData(stmt, j + 1, SQL_C_CHAR, val, 4096, &pcbValue);
			//printf("i+1=%d,j+1=%d => code=%d [%s] %d\n", i+1, j+1, code, val, pcbValue);
			if ( SQLOk(code) && pcbValue > 0 )  ret = string((char *)val);
		}else{
			nError::Throw("tODBConnection::GetFieldValue(" + ToString(i) + ", " + ToString(j) + ") : " +
			               ErrorMessage());
		}
	}else{
		nError::Throw("tODBConnection::GetFieldValue(" + ToString(i) + ", " + ToString(j) + ") = " +
		               ErrorMessage());
	}
	return ret;
}// tODBConnection::GetFieldValue


string tODBConnection::TimeStamp(const string &s)
{
	return kill_all_of(s, " -:");
}// tODBConnection::TimeStamp


string tODBConnection::TimS(const string &s)
{
	return s.substr(0, 4) + "-" + s.substr(4, 2) + "-" + s.substr(6, 2) + " " +
	       s.substr(8, 2) + ":" + s.substr(10, 2) + ":" + s.substr(12, 2);
}// tODBConnection::TimS


string tODBConnection::ErrorMessage()
{
	UCHAR errmsg[256];
	SQLError(env, dbc, stmt, NULL, NULL, errmsg, sizeof(errmsg), NULL);
	return string((char *)errmsg);
}// tODBConnection::ErrorMessage


bool tODBConnection::QueryExec(const string &query) 
{
	cR();
	int res = SQLExecDirect(stmt, (unsigned char *)query.c_str(), SQL_NTS);
	//debugLog("tODBConnection::QueryExec: res=" + ToString(res) + " " + ErrorMessage());
	if ( res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO )  return false;
	return true;
}// tODBConnection::QueryExec


void tODBConnection::store_res()
{
	cR();
	fields.clear();

	SQLSMALLINT cols;
	if ( SQLNumResultCols(stmt, &cols) == SQL_SUCCESS ){
		for ( int i = 0; i < cols; i++ ){
			UCHAR colname[128];
			SWORD coltype;
			SWORD colnamelen;
			SWORD nullable;
			UDWORD collen; //[MAXCOLS];
			SWORD scale;
			SQLDescribeCol(stmt, i + 1, colname, (SWORD)sizeof(colname), &colnamelen,
			               &coltype, &collen, &scale, &nullable);
			string str((char *)colname);
			fields.push_back(str);
			//debugLog("tODBConnection::store_res: field " + str);
		}
	}

	rows = 0;
	for ( int j = 0;; j++ ){
		UDWORD pcrow;
		UWORD rgfRowStatus;
		int code = SQLExtendedFetch(stmt, SQL_FETCH_ABSOLUTE, j + 1, &pcrow, &rgfRowStatus);
		if ( code == SQL_ERROR || code == SQL_SUCCESS_WITH_INFO ) {
			nError::Throw("tODBConnection::store_res : " + ErrorMessage());
		}
		if ( !SQLOk(code) ){
			break;
		}
		rows++;
	}
	//debugLog("tODBConnection::store_res, rows=" + ToString(rows));
}// tODBConnection::store_res


void tODBConnection::free_res()
{
	cR();
	//debugLog("tODBConnection::free_res");
	SQLFreeStmt(stmt, SQL_CLOSE);
}// tODBConnection::free_res


string tODBConnection::GetLastId(const string &/*table*/, const string &/*field*/)
{
	return string("1");
}// tODBConnection::GetLastId           // ???


string tODBConnection::getDbTypeName() const
{
	return "ODBC";
}// tODBConnection::getDbTypeName




tODBConnection *sysODBConn = NULL;
