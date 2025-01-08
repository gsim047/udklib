#ifndef __tDbConnection
#define __tDbConnection 1

#ifndef __nError_h
#include "nError.h"
#endif

#ifndef __tDataRow_h
#include "tDataRow.h"
#endif

#ifndef __tMutex_h
#include "tMutex.h"
#endif

#ifndef __fPid_h
#include "fPid.h"
#endif

#ifndef __tResult_h
#include "tResult.h"
#endif

#include <map>
#include <typeinfo>




namespace nError
{
	class DbConnect : public Message {
	public:
		DbConnect(const string &info);
	};// class DbConnect

	class ConnectionFail : public DbConnect {
	public:
	  ConnectionFail(const string &info);
	};// class ConnectionFail

	class QueryFail : public DbConnect {
	public:
		QueryFail(const string &q) : DbConnect(q) {};
		QueryFail(const string &q, const string &info);
	};// class QueryFail

	class NotSupported : public DbConnect {
	public:
		NotSupported(const string &info);
	};// class NotSupported
}// namespace nError




// класс соединения с базой данных
class tDbConnection : public tRefCounter, public tMutex, protected tResult
{
public:
	enum{
		READ_COMMITTED = 0,
		SERIALIZABLE   = 1
	};

	typedef tDbConnection* (*__new_db) (const string &dbname);

public:
	tDbConnection(const string &dbname) :
		tid(getTid()), tr_level(READ_COMMITTED), trans(false), dbName(dbname) {};
	virtual ~tDbConnection();

public:
	virtual bool valid() { return !stopping() && !badCode() && !ConnectionIsBad(); };
public:
	// любой запрос, когда результат неважен
	virtual void MakeQuery(const string &query);
	// запрос insert - возвращает значение поля field для вставленной записи
	virtual string MakeInsert(const string &table,
	                          const string &field,
	                          const string &query);
	// запрос insert - возвращает значение поля field для вставленной записи
	virtual string MakeInsert(const string &table,
	                          const string &field,
	                          const tDataRow &row);
	// запрос insert
	virtual void MakeInsert(const string &table,
	                        const tDataRow &row);
	// запрос update - обновляет запись, для которой поле field=value
	virtual void MakeUpdate(const string &table,
	                        const string &field,
	                        int value,
	                        const tDataRow &row);
	virtual void MakeUpdate(const string &table,
	                        const string &field,
	                        const string &value,
	                        const tDataRow &row);

	// select - возвращает результат
	virtual tDataSet MakeSelect(const string &query);          // returns data
	virtual tDataSet MakeSelect(const string &table,
	                            const string &field,
	                            int value);
	virtual tDataSet MakeSelect(const string &table,
	                            const string &field,
	                            const string &value);
	static string addSC(const string &q);

	virtual bool Begin() = 0;
	virtual void Abort() = 0;
	virtual void Commit() = 0;

	// все запросы бросают nError::QueryFail при ошибке

	virtual int get_tr_level() const { return tr_level; };
	virtual void set_tr_level(int new_tr_level) { tr_level = new_tr_level; };

	virtual string getDbTypeName() const = 0;
	virtual string getDbName() const { return dbName; };

	virtual string TimeStamp(const string &s) { return s; }
	virtual string TimS(const string &s) { return s; }

protected:
	virtual bool ConnectionIsBad() = 0;
	virtual bool QueryExec(const string &) = 0;
	virtual bool SelectExec(const string &) = 0;
	virtual string GetFieldName(int) = 0;
	virtual string GetFieldValue(int, int) = 0;
	virtual int Cols() = 0;
	virtual int Rows() = 0;
	virtual void store_res() {};
	virtual void free_res() {};
	virtual string GetLastId(const string &table, const string &field) = 0;
	virtual string ErrorMessage() { return string(); };
	virtual bool Connect(const string &host, int port, const string &user, 
	                     const string &password, const string &db) = 0;

public:
	int tid;

protected:
	int tr_level;
	bool trans;
	string dbName;
};// class tDbConnection




class tDbConnFactory {
public:
	tDbConnFactory() {}

	virtual tDbConnection *make(const std::map<string, string> &param) = 0;
}; //class tDbConnFactory




#endif // __tDbConnection
