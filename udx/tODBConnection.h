#ifndef __tODBConnection_h
#define __tODBConnection_h 1

#ifndef __tDbConnection_h
#include "tDbConnection.h"
#endif // __tDbConnection_h

#ifndef WIN32
#define WIN32 1
#endif

#ifndef __SQL
#include <sql.h>
#endif




// connection to ODBC
class tODBConnection : public tDbConnection
{
public:
	tODBConnection();
	tODBConnection(const string &dbname);
	tODBConnection(const string &db, const string &login, const string &passw);
	virtual ~tODBConnection();

public:
	bool Begin() { throw nError::NotSupported("Begin"); return false; };
	void Abort() { throw nError::NotSupported("Abort"); };
	void Commit() { throw nError::NotSupported("Commit"); };

	virtual string TimeStamp(const string &s);
	virtual string TimS(const string &s);

private:
	bool ConnectionIsBad() { return !env || !dbc || !stmt; };
	bool QueryExec(const string &query);
	bool SelectExec(const string &query) { return QueryExec(query); }
	string GetFieldName(int i) { return fields[i]; };
	string GetFieldValue(int i, int j);
	int Cols() { return fields.size(); }
	int Rows() { return rows; }
	void store_res();
	void free_res();
	string GetLastId(const string &table, const string &field);
	string ErrorMessage();
	bool Connect(const string &host, int port, const string &user, 
	             const string &password, const string &dbname);

	string getDbTypeName() const;

private:
	SQLHENV env; //void *env;      // SQLHENV
	SQLHDBC dbc; //void *dbc;      // HDBC  
	SQLHSTMT stmt; //void *stmt;     // HSTMT 
	std::vector<string> fields;
	unsigned rows;
};// class tODBConnection


extern tODBConnection *sysODBConn;


#endif // __tODBConnection_h
