#include "tServerSocket.h"




tTcpSocket tServerSocket::accept(int tm)
{
	int cs = _accept(tm);
	if ( cs == -1 ) return tTcpSocket();
	return tTcpSocket(cs);
}// tServerSocket::accept


tSocket tServerSocket::accept0(int tm)
{
	int cs = _accept(tm);
	if ( cs == -1 ) return tSocket();
	return tSocket(cs);
}// tServerSocket::accept0

