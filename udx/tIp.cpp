#include "tIp.h"
#include <stdlib.h>
//#include "fString.h"

#ifdef __GNUG__
#include <arpa/inet.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#endif




tIp::tIp(unsigned Ip) :
	ip(bad_ip)
{
	if ( Ip >= min_ip && Ip < max_ip ){
		ip = Ip;
	}
}// tIp::tIp


tIp::tIp(const string &Ip) :
	ip(ntohl(inet_addr(Ip.c_str())))
{
}// tIp::tIp


bool tIp::real() const
{
	return (ip >= a_start && ip < a_end) ||
	       (ip >= b_start && ip < b_end) ||
	       (ip >= c_start && ip < c_end);
}// tIp::real


string tIp::toString() const
{
	struct in_addr ia;
	memset(&ia, 0, sizeof(ia));
	ia.s_addr = htonl(ip);
	string ret = inet_ntoa(ia);
	return ret;
	/*
	return ToString((unsigned)ip >> 24) + "." +
	       ToString(((unsigned)ip >> 16) & 0xFF) + "." +
	       ToString(((unsigned)ip >> 8) & 0xFF) + "." +
	       ToString((unsigned)ip & 0xFF); */
}// tIp::toString


unsigned tIp::toNet() const
{
	return htonl(ip);
}// tIp::toNet


tIp tIp::subnet() const
{
	tIp p;
	if ( isA() ){
		p = tIp(ip & 0xFF000000);
	}else if ( isB() ){
		p = tIp(ip & 0xFFFF0000);
	}else if ( isC() ){
		p = tIp(ip & 0xFFFFFF00);
	}
	return p;
}// tIp::subnet



static const unsigned bitMask[33] = {
	0,
	0x80000000, 0xC0000000, 0xE0000000, 0xF0000000,
	0xF8000000, 0xFC000000, 0xFE000000, 0xFF000000,
	0xFF800000, 0xFFC00000, 0xFFE00000, 0xFFF00000,
	0xFFF80000, 0xFFFC0000, 0xFFFE0000, 0xFFFF0000,
	0xFFFF8000, 0xFFFFC000, 0xFFFFE000, 0xFFFFF000,
	0xFFFFF800, 0xFFFFFC00, 0xFFFFFE00, 0xFFFFFF00,
	0xFFFFFF80, 0xFFFFFFC0, 0xFFFFFFE0, 0xFFFFFFF0,
	0xFFFFFFF8, 0xFFFFFFFC, 0xFFFFFFFE, 0xFFFFFFFF
};// bitMask



tIpMask::tIpMask(unsigned Bits) :
	tIp(0),
	_bits(Bits > 32 ? 32 : Bits)
{
	ip = (Bits > 32 ? bitMask[32] : bitMask[Bits]);
}// tIpMask::tIpMask


tIpMask::tIpMask(const string &m) :
	tIp(m),
	_bits(32)
{
	for ( unsigned i = ip; _bits && !(i & 1); i = i >> 1, _bits-- );
}// tIpMask::tIpMask


bool tIpMask::valid() const
{
	return ip == bitMask[_bits];
}// tIpMask::valid


unsigned tIpMask::maskAnd() const
{
	return bitMask[_bits];
}// tIpMask::maskAnd


unsigned tIpMask::maskOr() const
{
	return 0xFFFFFFFF ^ bitMask[_bits];
}// tIpMask::maskOr


tIp tIpMask::subnet(const tIp &i) const
{
	return tIp((i.toHost() & maskAnd()));
}// tIpMask::subnet


tIp tIpMask::first(const tIp &i) const
{
	return tIp((i.toHost() & maskAnd()) | 1);
}// tIpMask::first


tIp tIpMask::last(const tIp &i) const
{
	return tIp((i.toHost() & maskAnd()) | maskOr());
}// tIpMask::last




tIpAddr::tIpAddr(const string &Ip, unsigned Port) :
	ip(ntohl(inet_addr(Ip.c_str()))),
	prt(Port)
{
}// tIpAddr::tIpAddr


tIpAddr::tIpAddr(const string &Adr) :
	ip(bad_ip),
	prt(bad_port)
{
	size_t pos = Adr.find_first_of(":");
	if ( pos != string::npos ){
		string Ip = Adr.substr(0, pos);
		string Port = Adr.substr(pos + 1);
		ip = ntohl(inet_addr(Ip.c_str()));
		prt = strtoul(Port.c_str(), NULL, 10);
	}else{
		ip = ntohl(inet_addr(Adr.c_str()));
	}
}// tIpAddr::tIpAddr


string tIpAddr::Addr() const
{
	struct in_addr ia;
	memset(&ia, 0, sizeof(ia));
	ia.s_addr = ip;
	string ret = inet_ntoa(ia);
	return ret;
	/*
	return ToString((unsigned)ip >> 24) + "." +
	       ToString(((unsigned)ip >> 16) & 0xFF) + "." +
	       ToString(((unsigned)ip >> 8) & 0xFF) + "." +
	       ToString((unsigned)ip & 0xFF); */
}// tIpAddr::Addr

unsigned tIpAddr::netaddr() const
{
	return htonl(ip);
}// tIpAddr::netaddr

unsigned tIpAddr::netport() const
{
	return htons(prt);
}// tIpAddr::netport
