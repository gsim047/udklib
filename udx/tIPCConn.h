#ifndef __tIPCConn_h
#define __tIPCConn_h 1

#ifndef __tConnection_h
#include "tConnection.h"
#endif

#ifndef __tIPC_h
#include "tIPC.h"
#endif




class tIPCConnection : public tConnection {
public:
	tIPCConnection(tSIPC &S, unsigned adr) : s(S), addr(adr) {};

	virtual bool send(tPk *p) const;

private:
	mutable tSIPC &s;
	unsigned addr;
};// class tIPCConnection




#endif // __tIPCConn_h
