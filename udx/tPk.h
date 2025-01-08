#ifndef __tPk_h
#define __tPk_h 1

#ifndef __tConnection_h
#include "tConnection.h"
#endif

#ifndef __tByteArray_h
#include "tByteArray.h"
#endif

#include <typeinfo>




class tPk{
public:
	enum{
		CtoS = 1,
		StoC = 2,
		BotH = 3
	};

	typedef tPk* (*__new_pk) ();

protected:
	tPk() : conn(NULL) {};

public:
	virtual ~tPk() { if ( conn ) delete conn; };

private:
	tPk(const tPk &);
	tPk &operator=(const tPk &);

protected:
	virtual void clear() {};
	virtual bool check() const { return true; };

public:
	virtual string name() const;

	virtual void put(tByteArray &) {};
	virtual void get(tByteArray &) {};

	//virtual bool logIn (string &, bool) const { return true; };
	//virtual bool logOut(string &, bool) const { return true; };

	tByteArray put();

	virtual tPk *exec(const string &) const;

	tConnection *conn;

	template <class T> static tPk *makeNew() { return new T(); }
};// class tPk




tPk *pkReceive(tByteArray &, tConnection *conn = NULL);



extern std::map<string, tPk::__new_pk> __tab_ioPk;


#ifdef __BORLANDC__
template <class T> void registerPacket()
{
	__tab_ioPk[getTypeName(typeid(T).name())] = tPk::makeNew<T>;
}// registerPacket
#endif

template <class T> void registerPacket(tPk::__new_pk mT)
{
	__tab_ioPk[getTypeName(typeid(T).name())] = mT;
}// registerPacket


// must be present somewhere !!!
void initPackets();
/*
// something like :

#ifdef __BORLANDC__
void initPackets()
{
	registerPacket<tPk_Logged>  ();
	registerPacket<tPk_Ping>    ();
	registerPacket<tPk_Message> ();
}// initPackets
#else

tPk_Login   *mkNew_Login()   { return new tPk_Login(); }
tPk_Ping    *mkNew_Ping()    { return new tPk_Ping(); }
tPk_Message *mkNew_Message() { return new tPk_Message(); }

void initPackets()
{
	registerPacket<tPk_Logged>  (mkNew_Logged );
	registerPacket<tPk_Ping>    (mkNew_Ping   );
	registerPacket<tPk_Message> (mkNew_Message);
}// initPackets
#endif

*/



#endif // __tPk_h
