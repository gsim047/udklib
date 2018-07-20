// This file was created by MkExe ver. 1.0.15.59 at [2017-04-24 21:59:33]

#include "cMain.h"
#include <iostream>
using std::cout;
using std::endl;

#include "fString.h"
#include "tFile.h"
//#include "tDebug.h"
#include "tLept.h"
#include "leptonica/allheaders.h"
#include <math.h>


#ifndef __CYGWIN__
#ifdef __GNUG__

#define EXE_NAME ent

extern char _binary_ent_rc_start[];
extern char _binary_ent_rc_end[];

const char* getRCStart() { return _binary_ent_rc_start; }
const char* getRCEnd() { return _binary_ent_rc_end; }

#endif // __GNUG__
#endif // !__CYGWIN__


static void About()
{
	cout << "";    // some description
	//cout << endl << endl;
	cout << "Call :  " << config.getname() << " [key [...]] ..." << endl << endl;
	cout << "Keys :  " << endl;
	cout << "  -?              - this info" << endl;
	cout << "  --cfg:CfgName   - use config file 'CfgName' (default '" << config.getname("cfg") << "')" << endl;
	cout << endl;
	exit(0);
}// About


int Main()
{
	cout << "###.  ver " << GetVer() << "   (c) Gerasim#  [" 
	     << GetVer("Date") << " " << GetVer("Time") << "]" << endl << endl;
	if ( config.getBool("option", "?", false) )  About(); 

	//string fin = config.get("cmdline", "1");
	//if ( fin.empty() )  About();

	string dir_pix = config.get("path", "pix", "");
	if ( dir_pix.empty() ) nError::Throw("Path to foto is not defined!");
	printf("Path to jpg: [%s]\n", dir_pix.c_str());
	string dir_txt = config.get("path", "txt", "");
	if ( dir_txt.empty() ) nError::Throw("Path to text is not defined!");
	printf("Path to pts: [%s]\n", dir_txt.c_str());
	double entMin = config.get("option", "value", 0.);
	printf("Minimal entropy: %.2f\n", entMin);

	std::map<string, int> mm;

	// <--- Your code
	std::vector<string> pix = DirFileList(dir_txt, "*.pts");
	for ( size_t i = 0; i < pix.size(); i++ ){
		string s = pix[i];
		size_t pos = s.find("_det_");
		string fpix = s.substr(0, pos) + ".jpg";

		double ent = 0.;

		if ( mm.find(fpix) == mm.end() ){

			tPix px(dir_pix + "/" + fpix);
			printf("[%s] %dx%dx%d", fpix.c_str(), px.width(),	px.height(), px.depth());
	  
			if ( px.depth() == 8 ){
//				px = pixConvert8To32 ( px );
			}else{
				tPix px2 = pixConvertRGBToHSV ( NULL, px );
	  
				NUMA* hue = NULL;
				pixMakeHistoHS ( px2, 1, &hue, NULL );
				if ( hue ){
					int sz = hue->n;
					l_float32* arr = numaGetFArray ( hue, 0 );
					if ( arr ){
						l_float32 sum = 0.; //px.width() * px.height();
						for ( int i = 0; i < sz; i++ ){
							sum += arr[i];
						}
						for ( int i = 0; i < sz; i++ ){
							l_float32 pai = arr[i];
							if ( pai > 0.0 ){
								pai /= sum;
								ent += pai * log(pai);
							}
						}
						ent = -ent;
						printf("  -  ent: %lf\n", ent);
						delete [] arr;
					}
				}
			}
			mm[fpix] = (ent < entMin) ? 0 : 1;
		}
		if ( mm[fpix] == 0 ){
			printf(" -- deleting [%s]\n", s.c_str());
			tFILE::FileDelete(dir_txt + "/" + s);
		}
	}
	
	return 0;
}// Main
