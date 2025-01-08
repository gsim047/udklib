#include "tDbConnection.h"
#include "fString.h"
#include "tDebug.h"
//#include <stdio.h>




nError::DbConnect::DbConnect(const string &info) :
	Message("db connection error:" + info) 
{
}// :DbConnect::DbConnect



nError::ConnectionFail::ConnectionFail(const string &info) :
	DbConnect("connection fail - " + info) 
{
}// ConnectionFail::ConnectionFail



nError::QueryFail::QueryFail(const string &q, const string &info) :
	DbConnect("query '" + q + "' : " + info) 
{
}// QueryFail::QueryFail



nError::NotSupported::NotSupported(const string &info) :
	DbConnect("not supported feature - " + info) 
{
}// NotSupported::NotSupported




tDbConnection::~tDbConnection()
{
	stop();
}// tDbConnection::~tDbConnection


void tDbConnection::MakeQuery(const string &query)
{
	cR();
	//debugLog("tDbConnection::MakeQuery :" + query);
	//if ( !valid() ) return;
	//debugLog("tDbConnection::MakeQuery valid()");
	tMutexLock ll(*this);
	if ( !QueryExec(addSC(query)) ){
		nError::QueryFail e(query, ErrorMessage());
		throw e;
	}
}//tDbConnection::MakeQuery


tDataSet tDbConnection::MakeSelect(const string &query)
{
	cR();
	//if ( !valid() ) return data;
	tMutexLock ll(*this);
	if ( !SelectExec(addSC(query)) ) {
		nError::QueryFail e(query, ErrorMessage());
		throw e;
	}
	store_res();
	std::vector<string> f;
	for ( int j = 0; j < Cols(); j++ )  f.push_back(GetFieldName(j));
	tDataSet data(f);
	for ( int i = 0; i < Rows(); i++ ){
		tDataRow row(data);
		for ( int j = 0; j < Cols(); j++ ){
			row[GetFieldName(j)] = GetFieldValue(i, j);
		}
		data += row;
	}
	free_res();
	return data;
}// tDbConnection::MakeSelect


tDataSet tDbConnection::MakeSelect(const string &table,
                                   const string &field,
                                   int value)
{
	cR();
	//if ( !valid() ) return data;
	string query = "SELECT * FROM " + table + " WHERE " + field + "=" +
	               Quote(value) + ";";
	tMutexLock ll(*this);
	if ( !SelectExec(query) ) {
		nError::QueryFail e(query, ErrorMessage());
		throw e;
	}
	store_res();
	std::vector<string> f;
	for ( int j = 0; j < Cols(); j++ )  f.push_back(GetFieldName(j));
	tDataSet data(f);
	for ( int i = 0; i < Rows(); i++ ){
		tDataRow row(data);
		for ( int j = 0; j < Cols(); j++ ){
			row[GetFieldName(j)] = GetFieldValue(i, j);
		}
		data += row;
	}
	free_res();
	return data;
}// tDbConnection::MakeSelect


tDataSet tDbConnection::MakeSelect(const string &table,
                                   const string &field,
                                   const string &value)
{
	cR();
	//if ( !valid() ) return data;
	string query = "SELECT * FROM " + table + " WHERE " + field + "=" +
	               Quote(value) + ";";
	tMutexLock ll(*this);
	if ( !SelectExec(query) ) {
		nError::QueryFail e(query, ErrorMessage());
		throw e;
	}
	store_res();
	std::vector<string> f;
	for ( int j = 0; j < Cols(); j++ )  f.push_back(GetFieldName(j));
	tDataSet data(f);
	for ( int i = 0; i < Rows(); i++ ){
		tDataRow row(data);
		for ( int j = 0; j < Cols(); j++ ){
			row[GetFieldName(j)] = GetFieldValue(i, j);
		}
		data += row;
	}
	free_res();
	return data;
}// tDbConnection::MakeSelect


string tDbConnection::MakeInsert(const string &table,
                                 const string &field,
                                 const string &query)
{
	cR();
	//if ( !valid() ) return ret;
	string ret;
	tMutexLock ll(*this);
	if ( !SelectExec(addSC(query)) ){
		nError::QueryFail e(query, ErrorMessage());
		throw e;
	}
	return GetLastId(table, field);
}// tDbConnection::MakeInsert


string tDbConnection::MakeInsert(const string &table,
                                 const string &field,
                                 const tDataRow &row)
{
	cR();
	//if ( !valid() ) return ret;
	string ret;
	tMutexLock ll(*this);
	string query = "INSERT INTO " + table + " (";
	bool was = false;
	{
		for ( unsigned i = 0; i < row.size(); ++i ){
			if ( was )  query += ", ";
			was = true;
			query += row.getField(i);
		}
	}
	query += ") VALUES (";
	was = false;
	{
		for ( unsigned i = 0; i < row.size(); ++i ){
			if ( was )  query += ", ";
			was = true;
			query += row.getValue(i);
		}
	}
	query += ")";
	if ( !QueryExec(addSC(query)) ){
		nError::QueryFail e(query, ErrorMessage());
		throw e;
	}
	return GetLastId(table, field);
}// tDbConnection::MakeInsert


void tDbConnection::MakeInsert(const string &table,
                               const tDataRow &row)
{
	cR();
	//if ( !valid() ) return;
	tMutexLock ll(*this);

	string query = "INSERT INTO " + table + " (";
	bool was = false;
	{
		for ( unsigned i = 0; i < row.size(); ++i ){
			if ( was )  query += ",";
			was = true;
			query += row.getField(i);
		}
	}
	query += ") VALUES (";
	was = false;
	{
		for ( unsigned i = 0; i < row.size(); ++i ){
			if ( was )  query += ",";
			was = true;
			query += row.getValue(i);
		}
	}
	query += ")";
	if ( !QueryExec(addSC(query)) ){
		nError::QueryFail e(query, ErrorMessage());
		throw e;
	}
}// tDbConnection::MakeInsert


void tDbConnection::MakeUpdate(const string &table,
                               const string &field,
                               int value,
                               const tDataRow &row)
{
	cR();
	//if ( !valid() ) return;
	tMutexLock ll(*this);

	string query = "UPDATE " + table + " SET ";
	bool was = false;
	for ( unsigned i = 0; i < row.size(); ++i ){
		if ( was )  query += ", ";
		was = true;
		query += row.getField(i) + "=" + row.getValue(i);
	}
	query += " WHERE " + field + "=" + Quote(value);
	if ( !QueryExec(addSC(query)) ){
		nError::QueryFail e(query, ErrorMessage());
		throw e;
	}
}// tDbConnection::MakeUpdate


void tDbConnection::MakeUpdate(const string &table,
                               const string &field,
                               const string &value,
                               const tDataRow &row)
{
	cR();
	//if ( !valid() ) return;
	tMutexLock ll(*this);

	string query = "UPDATE " + table + " SET ";
	bool was = false;
	for ( unsigned i = 0; i < row.size(); ++i ){
		if ( was )  query += ", ";
		was = true;
		query += row.getField(i) + "=" + row.getValue(i);
	}
	query += " WHERE " + field + "=" + Quote(value);
	if ( !QueryExec(addSC(query)) ){
		nError::QueryFail e(query, ErrorMessage());
		throw e;
	}
}// tDbConnection::MakeUpdate


string tDbConnection::addSC(const string &q)
{
	string ret = q;
	if ( !ret.empty() ){
		if ( ret[ret.size() - 1] != ';' )  ret += ';';
	}
	return ret;
}// tDbConnection::addSC


