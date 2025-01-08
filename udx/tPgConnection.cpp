#include "tPgConnection.h"
#include "tConfiguration.h"
#include "fString.h"
#include "tDebug.h"



tPgConnection::tPgConnection() :
	tDbConnection(config.get("PgDataBase","dbname")),
	db(string("dbname='" + config.get("PgDataBase","dbname") +
	          "' user='" + config.get("PgDataBase","dbuser") +
	          "' password='" + config.get("PgDataBase","dbpassword") +
	          "'").c_str())
{
	/*
	debugLog("tPgConnection::tPgConnection() " +
	  string(("dbname='" + config.get("PgDataBase","dbname") +
	   "' user='" + config.get("PgDataBase","dbuser") +
	   "' password='" + config.get("PgDataBase","dbpassword") + "'").c_str()));
	*/
	if ( !Connect("", 0, "", "", "") ){
		//debugLog("tPgConnection::tPgConnection db.ConnectionBad()`");
		setResult(1, nError::ConnectionFail(db.ErrorMessage()));
		return;
	}
}// tPgConnection::tPgConnection


tPgConnection::tPgConnection(const string &dbname) :
	tDbConnection(dbname),
	db(string("dbname='" + dbname +
	          "' user='" + config.get("PgDataBase","dbuser") +
	          "' password='" + config.get("PgDataBase","dbpassword") +
	          "'").c_str())
{
	/*
	debugLog("tPgConnection::tPgConnection(const string &dbname) " +
	  string(("dbname='" + dbname +
	   "' user='" + config.get("PgDataBase","dbuser") +
	   "' password='" + config.get("PgDataBase","dbpassword") + "'").c_str()));
	*/
	if ( !Connect("", 0, "", "", "") ){
	// debugLog("tPgConnection::tPgConnection db.ConnectionBad()`");
		setResult(1, nError::ConnectionFail(db.ErrorMessage()));
		return;
	}
}// tPgConnection::tPgConnection


tPgConnection::tPgConnection(const string &dbname,
                             const string &login,
                             const string &passw) :
	tDbConnection(dbname),
	db(string("dbname='" + dbname + "' user='" + login +
	          "' password='" + passw + "'").c_str())
{
	if ( !Connect("", 0, "", "", "") ){
		setResult(1, nError::ConnectionFail(db.ErrorMessage()));
		return;
	}
}// tPgConnection::tPgConnection


bool tPgConnection::Connect(const string &, int, const string &, 
                            const string &, const string &)
{
	return !db.ConnectionBad();
}// tPgConnection::Connect


string tPgConnection::GetLastId(const string& table, const string& field)
{
	cR();
	//if ( !valid() ) return "";
	string query = "SELECT "+field+" FROM "+table+" WHERE oid="+string(db.OidStatus())+";";
	if ( !db.ExecTuplesOk(query.c_str()) ){
		nError::QueryFail e(query, db.ErrorMessage());
		throw e;
	}
	if ( db.Tuples() <= 0 || db.Fields() <= 0 ){
		nError::QueryFail e(query);
		throw e;
	}
	return string(db.GetValue(0, 0));
}// tPgConnection::GetLastId


bool tPgConnection::QueryExec(const string &query)
{
	cR();
	//if ( !valid() ) return false;
	return db.ExecCommandOk(query.c_str());
}// tPgConnection::QueryExec


bool tPgConnection::SelectExec(const string &query)
{
	cR();
	//if ( !valid() ) return false;
	return db.ExecTuplesOk(query.c_str());
}// tPgConnection::SelectExec


bool tPgConnection::Begin()
{
	cR();
	if ( trans ) return false;
	//if ( !valid() ) return false;;
	tMutexLock ll(*this);
	db.ExecCommandOk("BEGIN;");
	if ( get_tr_level() == SERIALIZABLE ){
		db.ExecCommandOk("SET TRANSACTION ISOLATION LEVEL SERIALIZABLE;");
	}
	trans = true;
	return true;
}// tPgConnection::Begin


void tPgConnection::Commit()
{
	cR();
	if ( trans ){
		//if ( !valid() ) return;
		tMutexLock ll(*this);
		db.ExecCommandOk("COMMIT;");
		trans = false;
	}
}// tPgConnection::Commit


void tPgConnection::Abort()
{
	cR();
	if ( trans ){
		//if ( !valid() ) return;
		tMutexLock ll(*this);
		db.ExecCommandOk("ABORT;");
		trans = false;
	}
}// tPgConnection::Abort


string tPgConnection::GetFieldName(int i) 
{
	return db.FieldName(i); 
}// tPgConnection::GetFieldName


string tPgConnection::GetFieldValue(int i, int j) 
{
	return db.GetValue(i, j); 
}// tPgConnection::GetFieldValue


string tPgConnection::ErrorMessage() 
{
	return db.ErrorMessage(); 
}// tPgConnection::ErrorMessage


string tPgConnection::getDbTypeName() const 
{
	return "PgSQL"; 
}// tPgConnection::getDbTypeName



tPgConnection *sysPgConn = NULL;
