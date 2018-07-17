#include "nError.h"
using namespace nError;




Message::~Message() throw() 
{
}// Message::~Message


void nError::Throw(const nError::Message &e) 
{
	Message m(e); 
	throw m; 
}// nError::Throw


void nError::Throw(const string &s)  
{
	Message m(s); 
	throw m; 
}// nError::Throw
