// This file was created by MkExe ver. 1.0.15.59 at [2017-04-25 18:00:58]

#include "cMain.h"
#include <iostream>
using std::cout;
using std::endl;

#include "fString.h"
#include "tFile.h"
//#include "tDebug.h"
#include "fVector.h"
#include "tLept.h"
#include "leptonica/allheaders.h"
#include "tRdBuff.h"

#include <math.h>


#ifndef __CYGWIN__
#ifdef __GNUG__

#define EXE_NAME rsize

extern char _binary_rsize_rc_start[];
extern char _binary_rsize_rc_end[];

const char* getRCStart() { return _binary_rsize_rc_start; }
const char* getRCEnd() { return _binary_rsize_rc_end; }

#endif // __GNUG__
#endif // !__CYGWIN__



static string Bns = "\x01\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9";
static string Bs4 = ""; //"\xdd";
static string Bvs = "\xda\xdb\xdc\xdd\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xcc\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xfe";


/*
	1 - normal
	2 - flip H
	3 - rot 180
	4 - flip V
	5 - flip V + rot 90
	6 - rot 90
	7 - flipH + rot 90
	8 - rot 270
*/



static bool isSegment(tRdBuff &f, char &cc, size_t &sz)
{
	size_t pos = f.tell();
	if ( f.empty() ) return false;

	char c = (char)f.getBYTE();
	if ( c != '\xff' ){
		f.seek(pos);
		return false;
	}
	c = (char)f.getBYTE();
//	cout << "seg? " << ToStringHex((unsigned char)c) << endl;

	if ( Bns.find(c) != string::npos ){
		f.seek(pos);
		cc = c;
		sz = 2;
		return true;
	}
	if ( Bs4.find(c) != string::npos ){
		f.seek(pos);
		cc = c;
		sz = 6; // or 6??
		return true;
	}
	if ( Bvs.find(c) != string::npos ){
		cc = c;
		unsigned short t = f.getWORD(); 
		sz = __swap2(t) + 2;
		f.seek(pos);
		return true;
	}

	f.seek(pos);
	return true;
}// isSegment


static bool swp = false;

#pragma pack(push,1)

struct idf {
	u16 _tag = 0;
	u16 _type = 0;
	u32 _len = 0;
	u8 _v[4];

	u16 tag() const { return swp ? __swap2(_tag) : _tag; }
	u16 type() const { return swp ? __swap2(_type) : _type; }
	u32 len() const { return swp ? __swap4(_len) : _len; }

	u32 value() const { 
		u32 ret = 0;
		switch ( type() ){
		case 1:
			ret = _v[0];
			break;
		case 2:
			if ( swp ){
				ret = __swap4(*(u32*)_v);
			}else{
				ret = *(u32*)_v;
			}
			break;
		case 3:
			if ( swp ){
				ret = __swap2(*(u16*)_v);
			}else{
				ret = *(u16*)_v;
			}
			break;
		case 4:
			if ( swp ){
				ret = __swap4(*(u32*)_v);
			}else{
				ret = *(u32*)_v;
			}
			break;
		}
		return ret; //swp ? __swap4(_offset) : _offset; }
	}

	void show() const {
//		printf(" - [%04x %d %u/%X:%u/%X] ", (int)tag(), (int)type(), len(), len(), value(), value());
	}
};// struct idf

#pragma pack(pop)


static int getOri(tRdBuff &f, const string& name)
{
	int ori = 0;

	char seg;
	size_t segsize;
	int nn = 0;

	f.seek(0);
	size_t pos = f.tell();

	size_t jpgsize = 0;

	if ( isSegment(f, seg, segsize) ){
//		cout << "seg " << ToStringHex((unsigned char)seg) << " size:" << ToString(segsize) << " pos:" << ToStringHex(f.tell()) << endl;
		if ( seg == '\xd8' ){
			f.skip(segsize);
			while ( isSegment(f, seg, segsize) ){
//				cout << "seg " << ToStringHex((unsigned char)seg) << " size:" << ToString(segsize) << " pos:" << ToStringHex(f.tell()) << endl;
				
				if ( seg == '\xe1' ){
//					tFILE fo(name + "-E1-" + ToString(nn++) + ".bin", "wb");
//					if ( fo.isOpen() ){
						size_t pos = f.tell();
//						printf(" pos:%d %d ", (int) pos, (int)segsize);
						string out = f.get(segsize);
						f.seek(pos);

//						printf("[%s] [%s] ", out.substr(4, 4).c_str(), out.substr(10, 2).c_str());

						if ( out.substr(4, 4) == "Exif" ){
//							printf("Exif! ");
							string im = out.substr(10, 2);
							if ( im == "II" ){
								swp = false;
//								printf("i\n");
							}else if ( im == "MM" ){
								swp = true;
//								printf("m\n");
							}else{
//								printf("bad.. (\n");
							}

							{
								u32 idf1 = *(u32*)(out.c_str() + 14);
								if ( swp ) idf1 = __swap4(idf1);
//								printf(" : idf1 offset %d\n", idf1);

								u16 idfn = *(u16*)(out.c_str() + 18);
								if ( swp ) idfn = __swap2(idfn);
								int shift = idf1 + 12;
								for ( int i = 0; i < idfn; i++, shift += 12 ){
									idf* idfx = (idf*)(out.c_str() + shift);
									if ( idfx->tag() == 0 ) break;
									idfx->show();
									if ( idfx->tag() == 0x112 ){
										ori = idfx->value();
										break;
									}
									if ( idfx->type() == 2 ){
										u32 off = idfx->value();
										string o = out.substr(10 + off, idfx->len());
//										printf("{%s} ", o.c_str());
									}
									if ( idfx->type() == 7 && idfx->len() <= 4 ){
										string o = out.substr(shift + 8, idfx->len());
//										printf("{%s} ", o.c_str());
									}
//									printf("\n");
/*
									if ( idfx->tag() == 0x8769 ){
										u32 off = idfx->value();
										idfn = 200;
										shift = off;
//										printf(" ==> to Exif tags\n");
										continue;
									}
									if ( idfx->tag() == 0x8825 ){
										u32 off = idfx->value();
										idfn = 200;
										shift = off;
//										printf(" ==> to GPS tags\n");
										continue;
									}
*/
								}
//								printf("\n");

							}
						}

//						fo.write(out);
//					}
				}

				f.skip(segsize);
				if ( seg == '\xd9' ){
					break;
				}
				if ( seg == '\xda' ){
					if ( !f.find("\xff\xd9") ){
						break;
					}
//					cout << "pos_e: " << f.tell() << endl;
				}
			}
			jpgsize = f.tell();
		}
	}

//	cout << "size: " << jpgsize << endl;
	return ori;
}// getOri


static int fileOri(const string& fin)
{
	string ss = tFILE::LoadFile(fin);
	tRdBuff bf(ss);
	ss = "";
	swp = false;

	int ori = getOri(bf, fin.substr(0, fin.size()-4));
//	printf("== orientation: %d ==\n", ori);
	return ori;
}//


static void About()
{
	cout << "Check picture size and resize/rotate it";    // some description
	cout << endl << endl;
	cout << "Call :  " << config.getname() << " [key [...]] filename" << endl << endl;
	cout << "Keys :  " << endl;
	cout << "  -?              - this info" << endl;
	cout << "  --cfg:CfgName   - use config file 'CfgName' (default '" << config.getname("cfg") << "')" << endl;
	cout << "  --size:MaxSize  - resize pic, if width or height > MaxSize" << endl;
	cout << endl;
	exit(0);
}// About


int Main()
{
	cout << "###.  ver " << GetVer() << "   (c) Gerasim#  [" 
	     << GetVer("Date") << " " << GetVer("Time") << "]" << endl << endl;
	if ( config.getBool("option", "?", false) )  About();

	string fname = config.get("cmdline", "1");
	if ( fname.empty() )  About();

	int pxLen = config.get("option", "size", 0);
	if ( pxLen < 100 ) nError::Throw("Too small foto size!");

//	std::vector<string> pix = DirFileList(dir_pix, "*.jpg");
//	for ( size_t i = 0; i < pix.size(); i++ ){
		string fpix = ExtractFileNameExt(fname);
//		string fname = dir_pix + "/" + fpix;

		bool save = false;

		int ori = fileOri(fname);

		tPix px(fname);
		printf("[%s] %dx%dx%d  ", fpix.c_str(), px.width(),	px.height(), px.depth());

		if ( px.depth() > 8 && ori > 1 ){
			save = true;

			switch ( ori ){
			case 2:   // flip H
				printf("fh ");
				pixFlipLR ( px, px );
				break;
			case 3:
				printf("r* ");
				pixRotate180 ( px, px );
				break;
			case 4:
				printf("fv ");
				pixFlipTB ( px, px );
				break;
			case 5:
				printf("fv ");
				pixFlipTB ( px, px );
				{
					printf("r+");
					tPix pd = px.rotate90(1);
					px = pd;
				}
				break;
			case 6:
				{
					printf("r+");
					tPix pd = px.rotate90(1);
					px = pd;
				}
				break;
			case 7:
				printf("fh ");
				pixFlipLR ( px, px );
				{
					printf("r+");
					tPix pd = px.rotate90(1);
					px = pd;
				}
				break;
			case 8:
				{
					printf("r-");
					tPix pd = px.rotate90(-1);
					px = pd;
				}
				break;
			}
		}

		bool v = false;
		int len = px.width();
		if ( px.height() > px.width() ){
			v = true;
			len = px.height();
		}
		double scale = 1.;
		if ( len > pxLen ){
			scale = 1. * pxLen / len;
			tPix px2 = px.scale(scale, scale);
			printf(" ==> %dx%dx%d", px2.width(), px2.height(), px2.depth());
			px = px2;
			save = true;
		}

		if ( save ){
			px.writeJpeg(fname, 90);
		}
		printf("\n");
//	}
	
	return 0;
}// Main
