#include "tPk.h"
#include "tDebug.h"
#include "tTimer.h"
//#include "tMutex.h"
#include "fString.h"


std::map<string, tPk::__new_pk> __tab_ioPk;


class __iniPk{
public:
	__iniPk() { initPackets(); };
};// class __iniPk

static __iniPk __iniPkx;



tByteArray tPk::put()
{
	tByteArray ba;
	if ( check() ){
		ba << name();
		put(ba);
	}
	return ba;
}// tPk::put


string tPk::name() const
{
	return getTypeName(typeid(*this).name());
}// tPk::name

/*
void tPk::logO(bool ok, int s) const
{
	string buf;
	if ( s > 0 ) buf += "s:" + ToString(s) + ", ";
	buf += _name() + " send " + (ok ? "ok" : "err");
	string buf2;
	if ( logOut(buf2, ok) ){
		if ( !buf2.empty() ) buf += ", " + buf2;
		debugLog(buf);
	}
}// tPk::logO
*/

tPk *tPk::exec(const string &login) const
{
	errorLog("tPk::exec: Can't process packet \"" + name() + "\" for user \"" + login + "\"");
	return NULL;
}// tPk::exec


tPk *pkReceive(tByteArray &ib, tConnection *conn)
{
	tPk *p = NULL;
	if ( ib.size() >= 4 ){
		string pk_name;
		ib >> pk_name;
		if ( __tab_ioPk.find(pk_name) != __tab_ioPk.end() ){
			if ( __tab_ioPk[pk_name] != NULL ){
				p = __tab_ioPk[pk_name]();
			}else{
				errorLog("*** input disabled ***  for packet \"" + pk_name + "\"");
			}
		}
		if ( p != NULL ){
			try{
				p->get(ib);
				debugLog("get packet \"" + p->name() + "\"");
				p->conn = conn;
				if ( ib.size() > 0 ){
					errorLog("??? ib.size() after tPk::get() = " + ToString(ib.size()));
				}
			}catch ( nError::ByteArrayEmpty & ){
				delete p;
				p = NULL;
			}
		}else{
			errorLog("<- Unhandled packet \"" + pk_name + "\"");
		}
	}
	if ( !p ) tTimer::delay(50);
	return p;
}// pkReceive

