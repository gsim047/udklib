#include "tShMem.h"
#include "fString.h"
//#include "tDebug.h"

#ifdef __GNUG__
#include <sys/ipc.h>
#include <sys/shm.h>
#endif // __GNUG__


#ifdef _WIN32

tShMem::tShMem(unsigned keynumber, unsigned Size, bool Server) :
	name("shm-" + ToString(keynumber)),
	server(Server),
	size(Size)
{
	//debugLog("tShMem");
	if ( server ){
		m_hFileMap = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE,
		                               0, size, name.c_str());

	}else{
		m_hFileMap = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE,
		                             FALSE, name.c_str());
	}
	dwBaseMMF = (DWORD)MapViewOfFile(m_hFileMap, FILE_MAP_READ | FILE_MAP_WRITE,
	                                 0, 0, 0);
}// tShMem::tShMem


tShMem::~tShMem()
{
	//debugLog("~tShMem");
	if ( dwBaseMMF != 0 )  UnmapViewOfFile((LPVOID)dwBaseMMF);
	if ( m_hFileMap != NULL )  CloseHandle(m_hFileMap);
}// tShMem::~tShMem


bool tShMem::get(void *buf, unsigned offset, unsigned len)
{
	if ( !Alive() || offset + len > size )  return false;
	memmove(buf, (void *)(dwBaseMMF + offset), len);
	return true;
}// tShMem::get


bool tShMem::put(unsigned offset, const void *buf, unsigned len)
{
	if ( !Alive() || offset + len > size ) {
//  debugLog("tShMem::put return false");
		return false;
	}
// debugLog("tShMem::put memmove");
	memmove((void *)(dwBaseMMF + offset), buf, len);
	return true;
}// tShMem::put


void *tShMem::getbase() const
{
	return (void *)dwBaseMMF;
}// tShMem::getbase

#endif // _WIN32

#ifdef __GNUG__


tShMem::tShMem(unsigned keynumber, unsigned Size, bool Server) :
	name("shm-" + ToString(keynumber)),
	server(Server),
	size(Size),
	base(NULL)
{
	//debugLog("tShMem");
	if ( server ){
		id = shmget((key_t)keynumber, size, IPC_CREAT | /*IPC_EXCL |*/ 0666);
	}else{
		id = shmget((key_t)keynumber, size, 0666);
	}
	if ( id != -1 )  base = shmat(id, NULL, 0);
}// tShMem::tShMem


tShMem::~tShMem()
{
	//debugLog("~tShMem");
	if ( base != NULL )  shmdt(base);
	if ( id != -1 && server ){
		struct shmid_ds ds;
		shmctl(id, IPC_RMID, &ds);
	}
}// tShMem::~tShMem


bool tShMem::get(void *buf, unsigned offset, unsigned len)
{
// debugLog("tShMem::get");
	if ( !Alive() || offset + len > size )  return false;
	memmove(buf, (void *)((char *)base + offset), len);
	return true;
}// tShMem::get


bool tShMem::put(unsigned offset, const void *buf, unsigned len)
{
// debugLog("tShMem::put");
	if ( !Alive() || offset + len > size )  return false;
	memmove((void *)((char *)base + offset), buf, len);
	return true;
}// tShMem::put


void *tShMem::getbase() const
{
	return base;
}// tShMem::getbase

#endif // __GNUG__
