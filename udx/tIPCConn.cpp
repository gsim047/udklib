#include "tIPCConn.h"

#include "tDebug.h"
//#include "tPk.h"



bool tIPCConnection::send(tPk *p) const
{
	if ( !p || !s.Alive() )  return false;

	s.setOutput(p->put(), addr);
	return true;
}// tIPCConnection::send


