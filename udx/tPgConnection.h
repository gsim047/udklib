#ifndef __tPgConnection
#define __tPgConnection 1

//#include <libpq-fe.h>
#include <libpq++.h>

#ifndef __tDbConnection_h
#include "tDbConnection.h"
#endif




// connection to PostgresSQL 
class tPgConnection : public tDbConnection
{
public:
	tPgConnection();
	tPgConnection(const string &dbname);
	tPgConnection(const string &db, const string &login, const string &passw);
	virtual ~tPgConnection() {};

public:
	bool Begin();
	void Abort();
	void Commit();

private:
	virtual bool ConnectionIsBad() { return db.ConnectionBad(); };
	bool QueryExec(const string &query);
	bool SelectExec(const string &query);
	string GetFieldName(int i);         // { return string(db.FieldName(i)); };
	string GetFieldValue(int i, int j); // { return string(db.GetValue(i, j)); };
	int Cols() { return db.Fields(); };
	int Rows() { return db.Tuples(); };
	string GetLastId(const string &table, const string &field);
	string ErrorMessage();              // { return db.ErrorMessage(); };
	bool Connect(const string &host, int port, const string &user, 
	             const string &password, const string &dbname);

	string getDbTypeName() const;       // { return "PgSQL"; };

private:
	PgDatabase db;
};// class tPgConnection


extern tPgConnection *sysPgConn;



#endif // __tPgConnection
