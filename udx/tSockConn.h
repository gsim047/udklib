#ifndef __tSockConn_h
#define __tSockConn_h 1

#ifndef __tConnection_h
#include "tConnection.h"
#endif

#ifndef __tSvSocket_h
#include "tSvSocket.h"
#endif




class tSocketConnection : public tConnection {
public:
	tSocketConnection(const tSSocket &S) : s(S) {};

	virtual bool send(tPk *p) const;

private:
	mutable tSSocket s;
};// class tSocketConnection




#endif // __tSockConn_h
