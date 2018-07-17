#include "tResult.h"
#include "fString.h"
#include <typeinfo>




tResult &tResult::operator=(const tResult &s)
{
	if ( &s != this ){
		code = s.code;
		msg = s.msg;
	}
	return *this;
}// tResult::operator=


void tResult::checkResult() const
{
	if ( code ){
		if ( !msg.empty() )  nError::Throw(msg);
		nError::Throw(getTypeName(typeid(*this).name()) + ": error " + ToString(code));
	}
}// tResult::checkResult
