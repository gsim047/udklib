#include "tLongFile.h"
//#include "fFile.h"
#include "tTimer.h"
#include <sys/stat.h>

#include "fString.h"


/*
     hFile = CreateFile(szCertFile,
                   GENERIC_READ,
                   0,
                   NULL,
                   OPEN_EXISTING,
                   FILE_ATTRIBUTE_NORMAL,
                   NULL);
     if (hFile == INVALID_HANDLE_VALUE)
     {
       printf("Unable to open certificate file\n");
       __leave;
     }

     // Get file length
     dwSize = GetFileSize(hFile, NULL);
     if (dwSize == 0xFFFFFFFF)
     {
       printf("Unable to get size of certificate file\n");
       __leave;
     }

     // Allocate memory for encoded certificate
     pbEncodedCert = (LPBYTE)HeapAlloc(hHeap, 0, dwSize);
     if (!pbEncodedCert)
     {
       printf("Unable to allocate memory for encoded certificate\n");
       __leave;
     }

     // Read encoded certificate data
     bResult = ReadFile(hFile,
                    (LPVOID)pbEncodedCert,
                    dwSize,
                    &dwRead,
                    NULL);
     if (!bResult)
     {
       printf("Unable to read encoded certificate\n");
       __leave;
     }

     // Close file handle
     CloseHandle(hFile);
     hFile = INVALID_HANDLE_VALUE;

     // Convert multibyte store name to wide char string
     if (mbstowcs(szwStore, szStore, strlen(szStore)+1) == (size_t)-1)
     {
       printf("Unable to convert store to Unicode string\n");
       __leave;
     }

     // Open Certificate Store
     hStore = CertOpenStore(CERT_STORE_PROV_SYSTEM,
                      ENCODING,
                      0,
                      dwCertOpenFlags,
                      (LPVOID)szwStore);
     if (!hStore)
     {
       printf("CertOpenStore failed with %x\n", GetLastError());
       __leave;
     }

     // Add Certificate to store
     bResult = CertAddEncodedCertificateToStore(hStore,
                                     X509_ASN_ENCODING,
                                     pbEncodedCert,
                                     dwSize,
                                     CERT_STORE_ADD_REPLACE_EXISTING,
                                     &pCertContext);
     if (!bResult)
     {
       printf("CertAddEncodedCertificateToStore failed with %x\n", GetLastError());
       __leave;
     }

     {
        WCHAR szwContainer[160];
        WCHAR szwProvider[260];

        i = (int)mbstowcs(szwContainer, szContainer, (size_t)(sizeof(szContainer)+1));
        if (i == 0)
        {
            printf("MultiByteToWideChar failed with %d\n", GetLastError());
            __leave;
        }

        i = MultiByteToWideChar(0, 0, szProvider, -1, szwProvider, 260);
        if (i == 0)
        {
            printf("MultiByteToWideChar failed with %d\n", GetLastError());
            __leave;
        }

        ZeroMemory(&CertKeyInfo, sizeof(CertKeyInfo));
        CertKeyInfo.pwszContainerName = szwContainer;
        CertKeyInfo.pwszProvName = szwProvider;
        CertKeyInfo.dwProvType = dwProviderType;

        bResult = CryptGetKeyParam(hPubKey, KP_ALGID, (LPBYTE)&CertKeyInfo.dwKeySpec,
                            &dwSize, 0);
        if (!bResult)
        {
          printf("CryptGetKeyParam failed with %x\n", GetLastError());
          __leave;
        }

        // Set Key property for Certificate
        bResult = CertSetCertificateContextProperty(pCertContext,
                                         CERT_KEY_PROV_INFO_PROP_ID,
                                         0,
                                         (LPVOID)&CertKeyInfo);
        if (!bResult)
        {
          printf("CertSetCertificateContextProperty failed with %x\n", GetLastError());
          __leave;
        }
     }

     fReturn = TRUE;
   }
   _finally
   {
     // Clean up
     if (szContainer) RpcStringFree(&szContainer);
     if (hHash) CryptDestroyHash(hHash);
     if (hKey) CryptDestroyKey(hKey);
     if (hPubKey) CryptDestroyKey(hPubKey);
     if (hProv) CryptReleaseContext(hProv, 0);
     if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
     if (pbEncodedCert) HeapFree(hHeap, 0, pbEncodedCert);
     if (pbEncodedKey) HeapFree(hHeap, 0, pbEncodedKey);

*/






bool tLongFile::backup = false;


tLongFile::tLongFile(const string &fname, const string &mode) :
	_fname(fname),
	_mode(mode)
{
	for ( unsigned i = 0; i < 3; i++ ){
		f = CreateFile(_fname.c_str(),
                   GENERIC_READ,
                   0,
                   NULL,
                   OPEN_EXISTING,
                   FILE_ATTRIBUTE_NORMAL,
                   NULL);
/*
		f = CreateFile(_fname.c_str(),               // file to open
                       GENERIC_READ,          // open for reading
                       FILE_SHARE_READ,       // share for reading
                       NULL,                  // default security
                       OPEN_EXISTING,         // existing file only
                       FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
                       NULL);                 // no attr. template
*/


		if ( isOpen() ) break;
		tTimer::delay(100);
	}

	if ( !f ){
		setResult(1, nError::FILEError("Can't open file '" + fname + "'"));
	}
}// tLongFile::tLongFile


tLongFile::~tLongFile()
{
	if ( isOpen() )  CloseHandle(f);
}// tLongFile::~tLongFile


longlong tLongFile::size() const
{
	cR();
	//if ( f == NULL )  return 0;
	LARGE_INTEGER li;
	memset(&li, 0, sizeof(li));

	BOOL rs = GetFileSizeEx(f, &li);
	if ( rs == 0 ){
		return __INT64_C(0);
	}

	return li.QuadPart;
}// tLongFile::size


string tLongFile::read(size_t _size)
{
	cR();
	string ret;
	if ( !eof() ){
		//char *buf = new char[_size];
		HANDLE hHeap = GetProcessHeap();

		LPBYTE pbBuf = NULL;
		pbBuf = (LPBYTE)HeapAlloc(hHeap, 0, _size);
		if ( pbBuf ){ 
			if ( get(pbBuf, _size) )
				ret = string((const char*)pbBuf, _size);
			HeapFree(hHeap, 0, pbBuf);
		}
		//delete [] buf;
	}
	return ret;
}// tLongFile::read


bool tLongFile::get(char &c)
{
	cR();
	if ( !eof() ){
		return get(&c, sizeof(c));
	}
	return false;
}// tLongFile::get


bool tLongFile::get(unsigned &c)
{
	cR();
	if ( !eof() ){
		return get(&c, sizeof(c));
	}
	return false;
}// tLongFile::get


bool tLongFile::get(double &c)
{
	cR();
	if ( !eof() ){
		return get(&c, sizeof(c));
	}
	return false;
}// tLongFile::get


bool tLongFile::get(void *buf, size_t len)
{
	cR();
	if ( !eof() ){
//		printf("tLongFile::get - buf %s, len %d, pos %s\n", ToStringP(buf).c_str(), len, ToStringL(tell()).c_str());
		DWORD sz = 0;
		BOOL rs = ReadFile(f, buf, len, &sz, NULL);
//		printf("tLongFile::get - rs %d, sz %d\n", rs, sz);
//		if ( !rs ){
//			printf("errno: %d\n", GetLastError());
//		}
		return rs != FALSE && len == sz;
	}
//	printf("tLongFile::get - eof\n");
	return false;
}// tLongFile::get


bool tLongFile::write(const string &s)
{
	cR();
	//if ( f == NULL )  return false;
	return put(s.c_str(), s.size());
}// tLongFile::write

/*
bool tLongFile::write(char c)
{
	cR();
	return fwrite(&c, 1, 1, f) == 1;
}// tLongFile::write


bool tLongFile::write(unsigned c)
{
	cR();
	return fwrite(&c, 1, sizeof(c), f) == sizeof(c);
}// tLongFile::write


bool tLongFile::write(const void *buf, size_t len)
{
	cR();
	return fwrite(buf, 1, len, f) == len;
}// tLongFile::write
*/

bool tLongFile::put(const string &s)
{
	cR();
	//if ( f == NULL )  return false;
	return put(s.c_str(), s.size());
}// tLongFile::put


bool tLongFile::put(char c)
{
	cR();
	return put(&c, sizeof(c));
}// tLongFile::put


bool tLongFile::put(unsigned c)
{
	cR();
	return put(&c, sizeof(c));
}// tLongFile::put


bool tLongFile::put(double c)
{
	cR();
	return put(&c, sizeof(c));
}// tLongFile::put


bool tLongFile::put(const void *buf, size_t len)
{
	cR();
	DWORD sz = 0;
	BOOL rs = WriteFile(f, buf, len, &sz, NULL);
	return rs && (sz == len);
}// tLongFile::put


longlong tLongFile::tell() const
{
	cR();
	//if ( f == NULL )  return 0;
	LARGE_INTEGER sz, pt;
	sz.QuadPart = 0;
	BOOL rs = SetFilePointerEx(f, sz, &pt, FILE_CURRENT);
	if ( !rs ){
		return __INT64_C(0);
	}
	return (longlong)pt.QuadPart;
}// tLongFile::tell


bool tLongFile::seek(longlong shift, int whence) const
{
	cR();
	//if ( f == NULL )  return false;

	DWORD mode = (whence == SEEK_END) ? FILE_END : ((whence == SEEK_CUR) ? FILE_CURRENT : FILE_BEGIN);
	LARGE_INTEGER sz, pt;
	sz.QuadPart = shift;

	BOOL rs = SetFilePointerEx(f, sz, &pt, mode);
	return rs != FALSE;
}// tLongFile::seek


bool tLongFile::trunc()
{
	cR();

	return SetEndOfFile(f) != FALSE;
}// tLongFile::trunc


bool tLongFile::flush()
{
	cR();
	return FlushFileBuffers(f) != FALSE;
}// tLongFile::flush

/*
string tLongFile::LoadFile(const string &fname)
{
	tLongFile fi(fname, "rb");
	return fi.read(fi.size());
}// tLongFile::LoadFile


string tLongFile::LoadFileT(const string &fname)
{
	tLongFile fi(fname, "rt");
	return fi.read(fi.size());
}// tLongFile::LoadFileT
*/

void tLongFile::__back(const string &fname)
{
	if ( backup && tFILE::FileExists(fname) ){
		if ( !tFILE::FileBackup(fname) )  nError::Throw("Can't backup file [" + fname + "]");
	}
}// tLongFile::__back


bool tLongFile::SaveFile(const std::vector<string> &list, const string &fname)
{
	__back(fname);
	
	tLongFile f(fname, "wb");
	if ( !f.isOpen() )  return false;

	for ( size_t i = 0; i < list.size(); i++ ){
		if ( !f.write(list[i] + "\n") )  return false;
	}
	return true;
}// tLongFile::SaveFile


bool tLongFile::SaveFileT(const std::vector<string> &list, const string &fname)
{
	__back(fname);
	
	tLongFile f(fname, "wt");
	if ( !f.isOpen() )  return false;

	for ( size_t i = 0; i < list.size(); i++ ){
		if ( !f.write(list[i] + "\n") )  return false;
	}
	return true;
}// tLongFile::SaveFileT


bool tLongFile::SaveFile(const string &s, const string &fname)
{
	__back(fname);
	
	tLongFile f(fname, "wb");
	if ( !f.isOpen() )  return false;

	return f.write(s);
}// tLongFile::SaveFile


bool tLongFile::SaveFileT(const string &s, const string &fname)
{
	__back(fname);
	
	tLongFile f(fname, "wt");
	if ( !f.isOpen() )  return false;

	return f.write(s);
}// tLongFile::SaveFileT


bool tLongFile::copyTo(tLongFile &to, longlong siz)
{
	for ( longlong pos = 0; pos < siz; ){
		longlong ltor = 0x8000;
		size_t tor = 0x8000;

		if ( pos + ltor >= siz ){  
			ltor = siz - pos;
			tor = (size_t)ltor;
		}
		string s = read(tor);
		if ( s.size() != tor )  return false;
		pos += ltor;
		if ( !to.write(s) )  return false;
	}
	return true;
}// tLongFile::copyTo


bool tLongFile::copyTo(tFILE &to, size_t siz)
{
	for ( size_t pos = 0; pos < siz; ){
		size_t tor = 0x8000;

		if ( pos + tor >= siz ){  
			tor = siz - pos;
		}
		string s = read(tor);
		if ( s.size() != tor )  return false;
		pos += tor;
		if ( !to.write(s) )  return false;
	}
	return true;
}// tLongFile::copyTo




