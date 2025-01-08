#ifndef __tIp_h
#define __tIp_h 1

#pragma warning(disable:4786)

#include <string>
using std::string;




class tIp
{
public:
	enum {
		bad_ip  = 0xFFFFFFFF,     // 255.255.255.255
		max_ip  = 0xDFFFFF00,     // 223.255.255.0
		min_ip  = 0x01000000,     //   1.0.0.0
		a_start = 0x01000000,     //   1.0.0.0
		a_end   = 0x7F000000,     // 127.0.0.0
		b_start = 0x80000000,     // 128.0.0.0
		b_end   = 0xBFFF0000,     // 191.255.0.0
		c_start = 0xC0000000,     // 192.0.0.0
		c_end   = 0xDFFFFF00      // 223.255.255.0
	};

public:
	tIp() : ip(bad_ip) {};
	tIp(unsigned);
	tIp(const tIp &s) : ip(s.ip) {};
	tIp(const string &);
	virtual ~tIp() {};

	tIp &operator=(const tIp &s) { ip = s.ip; return *this; }

	bool operator==(const tIp &s) const { return ip == s.ip; }
	bool operator!=(const tIp &s) const { return ip != s.ip; }
	bool operator< (const tIp &s) const { return ip <  s.ip; }
	bool operator<=(const tIp &s) const { return ip <= s.ip; }
	bool operator> (const tIp &s) const { return ip >  s.ip; }
	bool operator>=(const tIp &s) const { return ip >= s.ip; }

	bool valid() const { return ip != bad_ip; };
	bool real() const;

	bool isA() const { return ip >= a_start && ip < a_end; }
	bool isB() const { return ip >= b_start && ip < b_end; }
	bool isC() const { return ip >= c_start && ip < c_end; }

	string toString() const;
	unsigned toNet() const;
	operator unsigned() const { return ip; };
	unsigned toHost() const { return ip; };

	tIp subnet() const;

	bool check() const { return check(ip); }

	static bool check(unsigned _ip) { return (_ip >= min_ip && _ip < max_ip); }

protected:
	unsigned ip; 
};// class tIp




class tIpMask : protected tIp
{
public:
	enum{
		mask_full = 0xFFFFFFFF,   // 255.255.255.255
		mask_a    = 0xFF000000,   // 255.0.0.0
		mask_b    = 0xFFFF0000,   // 255.255.0.0
		mask_c    = 0xFFFFFF00    // 255.255.255.0
	};

public:
	tIpMask() : tIp(mask_full) {};
	tIpMask(unsigned bits);            // 0-32
	tIpMask(const string &);           // e.g. "255.255.255.0"
	virtual ~tIpMask() {};

	bool valid() const;

	tIpMask &operator=(const tIpMask &s) { ip = s.ip; _bits = s._bits; return *this; }

	bool operator==(const tIpMask &s) const { return ip == s.ip && _bits == s._bits; }
	bool operator!=(const tIpMask &s) const { return !operator==(s); }
	bool operator< (const tIpMask &s) const { return ip <  s.ip; }

	unsigned bits() const { return _bits; }
	unsigned maskAnd() const;
	unsigned maskOr() const;

	string toString() const { return tIp::toString(); }
	unsigned toNet() const { return tIp::toNet(); }
	operator unsigned() const { return tIp::toHost(); };
	unsigned toHost() const { return tIp::toHost(); }

	tIp subnet(const tIp &) const;
	tIp first(const tIp &) const;
	tIp last(const tIp &) const;

protected:
	unsigned _bits;
};// class tIpMask




class tIpAddr
{
public:
	enum {
		bad_ip    = 0xFFFFFFFF,     // 255.255.255.255
		bad_port  = 0xFFFFFFFF
	};

public:
	tIpAddr() : ip(bad_ip), prt(bad_port) {};
	tIpAddr(unsigned Ip, unsigned Port) : ip(Ip), prt(Port) {};
	tIpAddr(const tIpAddr &s) : ip(s.ip), prt(s.prt) {};
	tIpAddr(const string &, unsigned);
	tIpAddr(const string &);
	virtual ~tIpAddr() {};

	tIpAddr &operator=(const tIpAddr &s) { ip = s.ip; prt = s.prt; return *this; }
	bool operator==(const tIpAddr &s) const { return ip == s.ip && prt == s.prt; }
	bool operator!=(const tIpAddr &s) const { return !operator==(s); }

	bool valid() const { return ip != bad_ip && prt != bad_port; };

	string Addr() const;
	unsigned addr() const { return ip; }
	unsigned netaddr() const;

	unsigned port() const { return prt; }
	unsigned netport() const;

protected:
	unsigned ip;
	unsigned prt; 
};// class tIpAddr




#endif // __tIp_h
