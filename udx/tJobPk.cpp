#include "tJobPk.h"




void tJobPk::exec()
{
	if ( p ){
		tPk *d = p->exec(login);
		if ( d ){
			if ( p->conn )  *(p->conn) << d;
			else delete d;
		}
	}
}// tJobPk::exec


void tJobPk::addJob(tPk *p, const string &login)
{
	if ( p ){
		tJob::addJob(new tJobPk(p, login));
	}
}// tJobPk::addJob
