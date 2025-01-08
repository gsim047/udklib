#include "tConnection.h"

#include "tDebug.h"
#include "tPk.h"



tConnection &tConnection::operator<<(tPk *p)
{
	//debugLog("tConnection << tPk*");
	send(p);
	if ( p ) delete p;
	return *this;
}// tConnection::operator<<


