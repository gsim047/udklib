#ifndef __tMyConnection_h
#define __tMyConnection_h 1

#ifndef __tDbConnection_h
#include "tDbConnection.h"
#endif // __tDbConnection_h



struct st_mysql;
struct st_mysql_res;
struct st_mysql_field;



// connection to MySQL
class tMyConnection : public tDbConnection
{
public:
	tMyConnection();
	tMyConnection(const string &dbname);
	tMyConnection(const string &db, const string &login, const string &passw);
	virtual ~tMyConnection();

public:
	bool Begin(); // { throw nError::NotSupported("Begin"); return false; }
	void Abort() { throw nError::NotSupported("Abort"); }
	void Commit() { throw nError::NotSupported("Commit"); }

	virtual string TimeStamp(const string &s);
	virtual string TimS(const string &s);

private:
	bool ConnectionIsBad();
	bool QueryExec(const string &query);
	bool SelectExec(const string &query) { return QueryExec(query); }
	string GetFieldName(int i);
	string GetFieldValue(int i, int j);
	int Cols();
	int Rows();
	void store_res();
	void free_res();
	string GetLastId(const string &table, const string &field);
	string ErrorMessage();
	bool Connect(const string &host, int port, const string &user, 
	             const string &password, const string &dbname);

	string getDbTypeName() const; // { return string("MySQL"); }

private:
	st_mysql *db;               // MYSQL
	st_mysql_res *result;       // MYSQL_RES
	st_mysql_field *fields;     // MYSQL_FIELD
};// class tMyConnection


extern tMyConnection *sysMyConn;



#endif // __tMyConnection_h
