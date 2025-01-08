#include "tMyConnection.h"
#include "tConfiguration.h"
#include "tDebug.h"
#include <mysql.h>



static const char * const __MySql = { "MySql" };
static const char * const __MySql_name = { "dbname" };
static const char * const __MySql_host = { "dbhost" };
static const char * const __MySql_lhost = { "localhost" };
static const char * const __MySql_port = { "dbport" };
static const char * const __MySql_user = { "dbuser" };
static const char * const __MySql_pass = { "dbpassword" };



tMyConnection::tMyConnection() :
  tDbConnection(config.get(__MySql, __MySql_name))
{
	db = mysql_init((MYSQL*) 0);
	string dbhost = config.get(__MySql, __MySql_host, __MySql_lhost);
	int dbport = ToInt(config.get(__MySql, __MySql_port));
	string dbuser = config.get(__MySql, __MySql_user);
	string dbpassword = config.get(__MySql, __MySql_pass);
	string dbname = config.get(__MySql, __MySql_name);
	if ( !Connect(dbhost, dbport, dbuser, dbpassword, dbname) ){
		setResult(1, nError::ConnectionFail(ErrorMessage()));
		//mysql_close(db);
		//db = NULL;
		//debugLog("! tMyConnection::tMyConnection()");
		return;
	}
	debugLog("tMyConnection::tMyConnection()");
}// tMyConnection::tMyConnection


tMyConnection::tMyConnection(const string &dbname) :
  tDbConnection(dbname)
{
	db = mysql_init((MYSQL*) 0);
	string dbhost = config.get(__MySql, __MySql_host, __MySql_lhost);
	int dbport = ToInt(config.get(__MySql, __MySql_port));
	string dbuser = config.get(__MySql, __MySql_user);
	string dbpassword = config.get(__MySql, __MySql_pass);
	if ( !Connect(dbhost, dbport, dbuser, dbpassword, dbname) ){
		setResult(1, nError::ConnectionFail(ErrorMessage()));
		//debugLog("! tMyConnection::tMyConnection(const string &dbname)");
		//debugLog(badCode() ? "badCode" : "goodCode");
		return;
	}
	debugLog("tMyConnection::tMyConnection(const string &dbname)");
}// tMyConnection::tMyConnection


tMyConnection::tMyConnection(const string &dbname,
                             const string &login,
                             const string &passw) :
  tDbConnection(dbname)
{
	db = mysql_init((MYSQL*) 0);
	string dbhost = config.get(__MySql, __MySql_host, __MySql_lhost);
	int dbport = ToInt(config.get(__MySql,__MySql_port));
	if ( !Connect(dbhost, dbport, login, passw, dbname) ){
		setResult(1, nError::ConnectionFail(ErrorMessage()));
		//debugLog("! tMyConnection::tMyConnection(const string,const string,const string)");
		return;
	}
	debugLog("tMyConnection::tMyConnection(const string,const string,const string)");
}// tMyConnection::tMyConnection


tMyConnection::~tMyConnection()
{
	//debugLog("tMyConnection::~tMyConnection()");
	if ( db ) mysql_close(db);
}// tMyConnection::~tMyConnection


bool tMyConnection::Connect(const string &host, 
                            int port, 
                            const string &user, 
                            const string &password, 
                            const string &dbname)
{
	return mysql_real_connect(db, host.c_str(), user.c_str(), password.c_str(),
	                          dbname.c_str(), port, NULL, 0);
}// tMyConnection::Connect


void tMyConnection::store_res()
{
	cR();
	result = mysql_store_result(db);
	fields = mysql_fetch_fields(result);
}// tMyConnection::store_res


string tMyConnection::GetFieldValue(int i, int j)
{ 
	cR();
	string ret;
	mysql_data_seek(result, i);
	MYSQL_ROW row = mysql_fetch_row(result);
	if ( row ){
		unsigned long *lengths = mysql_fetch_lengths(result);;
		if ( lengths && lengths[j] > 0 && row[j] ){
			ret = string((char*)row[j], lengths[j]);
		}
	}
	return ret;
}// tMyConnection::GetFieldValue


string tMyConnection::TimeStamp(const string &s)
{
	return kill_all_of(s, " -:");
}// tMyConnection::TimeStamp


string tMyConnection::TimS(const string &s)
{
	return s.substr(0, 4) + "-" + s.substr(4, 2) + "-" + s.substr(6, 2) + " " +
	       s.substr(8, 2) + ":" + s.substr(10, 2) + ":" + s.substr(12, 2);
}// tMyConnection::TimS


bool tMyConnection::ConnectionIsBad() 
{
	return mysql_error(db)[0] != '\0'; 
}// tMyConnection::ConnectionIsBad


bool tMyConnection::QueryExec(const string &query) 
{
	cR();
	return !mysql_real_query(db, query.c_str(), query.size()); 
}// tMyConnection::QueryExec


string tMyConnection::GetFieldName(int i) 
{
	return string(fields[i].name); 
}// tMyConnection::GetFieldName


int tMyConnection::Cols() 
{
	return mysql_num_fields(result); 
}// tMyConnection::Cols


int tMyConnection::Rows() 
{
	return mysql_num_rows(result); 
}// tMyConnection::Rows


void tMyConnection::free_res() 
{
	mysql_free_result(result); 
}// tMyConnection::free_res


string tMyConnection::GetLastId(const string &, const string &) 
{
	return ToString(mysql_insert_id(db)); 
}// tMyConnection::GetLastId


string tMyConnection::ErrorMessage() 
{
	cR();
	char *err = mysql_error(db);
	string ret = (err) ? err : "";
	return ret;
}// tMyConnection::ErrorMessage


string tMyConnection::getDbTypeName() const 
{
	return "MySQL"; 
}// tMyConnection::getDbTypeName


bool tMyConnection::Begin()
{
	throw nError::NotSupported("Begin");
	//return false;                               // ???
}// tMyConnection::Begin


tMyConnection *sysMyConn = NULL;
