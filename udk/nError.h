#ifndef __nError_h
#define __nError_h 1

#include <exception>
#include <string>
using std::string;



// error classes & functions
namespace nError
{
	class Message : public string, public std::exception  // Base error class
	{
	public:
		Message(const string &s) : string(s) {};
		virtual ~Message() throw(); // {};
	};// class Message

	//inline void Throw(const Message &e) { Message m(e); throw m; };
	//inline void Throw(const string &s)  { Message m(s); throw m; };
	void Throw(const Message &e);
	void Throw(const string &s);
}// namespace nError



#endif // __nError_h
