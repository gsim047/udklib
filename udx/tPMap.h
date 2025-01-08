#ifndef __tPMap_h
#define __tPMap_h 1

#pragma warning(disable:4786)

#include <map>
using std::map;




template <class Key, class T, class Compare = std::less<Key> >
class tPMap : public map<Key, T*, Compare>{
public:
	tPMap(const Compare& comp = Compare()) : map<Key, T*, Compare>(comp) {};
	virtual ~tPMap() { clearDel(); };

private:
	tPMap(const tPMap<Key, T, Compare>& x);
	tPMap<Key, T, Compare>& operator= (const tPMap<Key, T, Compare>& x);

public:
	void eraseDel(typename map<Key, T*, Compare>::iterator position) {
		delete position->second; this->map<Key, T*, Compare>::erase(position); };

	typename map<Key, T*, Compare>::size_type eraseDel(const Key &x) {
		map<Key, T*, Compare>::iterator i = find(x);
		if ( i != map<Key, T*, Compare>::end() ) delete i->second;
		return this->map<Key, T*, Compare>::erase(x); };

#ifdef __BORLANDC__
#pragma option push -w-inl
#endif
	void eraseDel(typename map<Key, T*, Compare>::iterator first,
	              typename map<Key, T*, Compare>::iterator last) {
		for ( map<Key, T*, Compare>::iterator i = first; i != last; ++i ){
			delete i->second;
		}
		this->map<Key, T*, Compare>::erase(first, last); };
#ifdef __BORLANDC__
#pragma option pop
#endif

	void clearDel() { eraseDel(begin(), end()); };

	/*
	T* operator[] (const Key& k) {
		map<Key, T*, Compare>::iterator i = find(k);
		if ( i != map<Key, T*, Compare>::end() ) return i->second;
		return NULL;
	};
	void insert(const Key& k, T *t) {
		map<Key, T*, Compare>::iterator i = find(k);
		if ( i != map<Key, T*, Compare>::end() ) delete i->second;
		this->map<Key, T*, Compare>::operator[](k) = t;;
	};
	*/
};// class tPMap




#endif // __tPMap

