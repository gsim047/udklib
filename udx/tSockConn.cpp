#include "tSockConn.h"

#include "tDebug.h"
//#include "tPk.h"



bool tSocketConnection::send(tPk *p) const
{
	if ( !p || !s.Alive() )  return false;

	s.setOutput(p->put());
	return true;
}// tSocketConnection::send
