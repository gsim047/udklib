#ifndef __tPList_h
#define __tPList_h 1

#ifndef __tX_del_h
#include "tX_del.h"
#endif

#include <list>
using std::list;




template <class T>
class tPList : public list<T*>{
public:
	tPList() : list<T*>() {};
	tPList(typename list<T*>::size_type n) : list<T*>(n) {};
	virtual ~tPList() { clear(); };

private:
	tPList(const tPList<T>& x);
	tPList<T>& operator= (const tPList<T>& x);

	void assign(typename list<T*>::iterator first, typename list<T*>::iterator last);
	void assign(typename list<T*>::size_type, T *t); 

public:
	void erase(typename list<T*>::iterator pos) {
	  delete (*pos); this->list<T*>::erase(pos); };

	void erase(typename list<T*>::iterator first, typename list<T*>::iterator last) {
	  __tX_del<T> fx;
	  std::for_each(first, last, fx);
//   for ( list<T*>::iterator i = first; i != last; ++i )  delete (*i);
	  this->list<T*>::erase(first, last); 
	};

	void clear() { erase(begin(), end()); };

};// class tPList




#endif // __tPList

