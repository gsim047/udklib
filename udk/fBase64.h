#ifndef __fBase64_h
#define __fBase64_h

#include "nError.h"



namespace nError
{
	class BadMimeData : public Message {
	public:
		BadMimeData();
		BadMimeData(const string &s);
	};// class BadMimeData
}// namespace nError


string encode_base64(const string &str, bool app_eol = true);
string decode_base64(const string &str);


#endif // __fBase64_h
