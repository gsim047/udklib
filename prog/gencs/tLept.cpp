#include "tLept.h"

//#define LIBLEPT_IMPORTS 1

#include "leptonica/allheaders.h"
//#include <windows.h>


/*
static PIX * f_pixClone( PIX *pixs )
{
	if ( !leptDll_init() )  return NULL;
	return fp_pixClone( pixs );
}

static void f_pixDestroy( PIX **ppix )
{
	if ( !leptDll_valid() )  return;
	fp_pixDestroy( ppix );
}

static l_int32 f_pixWriteJpeg( const char *filename, PIX *pix, l_int32 quality, l_int32 progressive )
{
	if ( !leptDll_valid() )  return 1;
	return  fp_pixWriteJpeg( filename, pix, quality, progressive );
}

static BOX * f_boxCreate( l_int32 x, l_int32 y, l_int32 w, l_int32 h )
{
	if ( !leptDll_init() )  return NULL;
	return fp_boxCreate( x, y, w, h );
}// 

static void f_boxDestroy( BOX **pbox )
{
	if ( !leptDll_valid() )  return;
	fp_boxDestroy( pbox );
}

static PIX * f_pixClipRectangle( PIX *pixs, BOX *box, BOX **pboxc )
{
	if ( !leptDll_valid() )  return NULL;
	return fp_pixClipRectangle( pixs, box, pboxc );
}// 

static PIX * f_pixRead( const char *file )
{
	if ( !leptDll_init() )  return NULL;
	return fp_pixRead( file );
}

static l_int32 f_pixGetExtremeValue ( PIX *pixs, l_int32 factor, l_int32 type, l_int32 *prval, l_int32 *pgval, l_int32 *pbval, l_int32 *pgrayval )
{
	if ( !leptDll_valid() )  return 1;
	return fp_pixGetExtremeValue ( pixs, factor, type, prval, pgval, pbval, pgrayval );
}

static PIX * f_pixGammaTRC ( PIX *pixd, PIX *pixs, l_float32 gamma, l_int32 minval, l_int32 maxval )
{
	if ( !leptDll_valid() )  return NULL;
	return fp_pixGammaTRC ( pixd, pixs, gamma, minval, maxval );
}// 

static PIX * f_pixGetRGBComponent ( PIX *pixs, l_int32 color )
{
	if ( !leptDll_valid() )  return NULL;
	return fp_pixGetRGBComponent ( pixs, color );
}// 

static PIX * f_pixRotate90 ( PIX *pixs, l_int32 direction )
{
	if ( !leptDll_valid() )  return NULL;
	return fp_pixRotate90 ( pixs, direction );
}//

static l_int32 f_pixGetResolution ( PIX *pix, l_int32 *pxres, l_int32 *pyres )
{
	if ( !leptDll_valid() )  return 0;
	return fp_pixGetResolution ( pix, pxres, pyres );
}// 

static void f_l_getCurrentTime ( l_int32 *sec, l_int32 *usec )
{
	if ( !leptDll_valid() )  return;
	fp_l_getCurrentTime ( sec, usec );
}//

static l_int32 f_pixSetResolution ( PIX *pix, l_int32 xres, l_int32 yres )
{
	if ( !leptDll_valid() )  return 1;
	return fp_pixSetResolution ( pix, xres, yres );
}// 

static PIX * f_pixScale ( PIX *pixs, l_float32 scalex, l_float32 scaley )
{
	if ( !leptDll_valid() )  return NULL;
	return fp_pixScale ( pixs, scalex, scaley );
}// 
*/
// Конец раздела обёрток



tPix::tPix(Pix *px) : 
	pix(pixClone(px)) 
{
}


tPix::tPix(const string &file) : 
	pix( pixRead( file.c_str() ) ) 
{
	if ( pix )  pix->refcount += 1;
}


bool tPix::operator==(const tPix &s) const
{
  if ( &s == this )  return true;
  if ( pix == s.pix )  return true;
  return false;
}// tPix::operator==


void tPix::clear()
{
	pixDestroy(&pix);
}


tPix& tPix::operator=(Pix *px) 
{ 
	clear();
	if ( px != NULL ) 
		pix = pixClone(px); 
	return *this; 
}


tPix::tPix(const tPix &s) :
	pix(s.empty() ? NULL : pixClone(s.pix))
{
}


tPix& tPix::operator=(const tPix &s)
{
	if ( &s != this ){
		clear();
		if ( !s.empty() )
			pix = pixClone(s.pix);
	}
	return *this;
}


tPix::tPix(int width, int height, int depth) :
	pix(pixCreate (width, height, depth) )
{
}// 


int tPix::width() const { return empty() ? 0 : pix->w; }
int tPix::height() const { return empty() ? 0 : pix->h; }
int tPix::depth() const { return empty() ? 0 : pix->d; }


bool tPix::writeJpeg(const string &fname, int quality) const
{
	if ( empty() ) return false;
	return pixWriteJpeg( fname.c_str(), pix, quality, 0 ) == 0;
}// 


tPix tPix::clipRectangle(const tBox &box) const
{
	return pixClipRectangle(pix, box, NULL);
}// 


int tPix::getExtremeValue(int factor, bool minVal, int *prval, int *pgval, int *pbval, int *pgrayval ) const
{
	l_int32 rval, gval, bval, grayval;
	int type = (minVal ? L_SELECT_MIN : L_SELECT_MAX);
	int ret = pixGetExtremeValue(pix, factor, type, &rval, &gval, &bval, &grayval );
	if ( !ret ){
		if ( prval ) *prval = rval;
		if ( pgval ) *pgval = gval;
		if ( pbval ) *pbval = bval;
		if ( pgrayval ) *pgrayval = grayval;
	}
	return ret;
}


tPix tPix::gammaTRC(double gamma, int minval, int maxval) const
{
	return pixGammaTRC ( NULL, pix, gamma, minval, maxval );
}


tPix tPix::getRGBComponent(pixColor color) const
{
	int pcolor = COLOR_RED;
	if ( color == pix_COLOR_GREEN ){
		pcolor = COLOR_GREEN;
	}else if ( color == pix_COLOR_BLUE ){
		pcolor = COLOR_BLUE;
	}else if ( color == pix_ALPHA_CHANNEL ){
		pcolor = L_ALPHA_CHANNEL;
	}
	return pixGetRGBComponent ( pix, pcolor );
}


tPix tPix::rotate90(int direction) const
{
	return pixRotate90 ( pix, direction );
}


int tPix::getResolutionX()
{
	l_int32 pxres, pyres;
	if ( !pixGetResolution ( pix, &pxres, &pyres ) ) return pxres;
	return 0;
}// tPix::getResolutionX

int tPix::getResolutionY()
{
	l_int32 pxres, pyres;
	if ( !pixGetResolution ( pix, &pxres, &pyres ) ) return pyres;
	return 0;
}// tPix::getResolutionY

bool tPix::setResolution(int xres, int yres)
{
	return pixSetResolution ( pix, xres, yres ) == 0;
}// tPix::setResolution

tPix tPix::scale(double xscale, double yscale) const
{
	return pixScale ( pix, xscale, yscale );
}// tPix tPix::scale

int tPix::clearInRect(const tBox& bx)
{
	return pixClearInRect(pix, bx);
}// tPix::clearInRect




// класс tBox

tBox::tBox(int _x, int _y, int _w, int _h) :
	box(NULL)
{
	if ( _w > 0 && _h > 0 ){
		box = boxCreate(_x, _y, _w, _h);
	}
}// tBox::tBox

tBox::tBox(const tBox &s) :
	box(NULL)
{
	if ( !s.empty() ){
		box = boxCreate(s.x(), s.y(), s.w(), s.h());
	}
}// tBox::tBox


tBox &tBox::operator=(const tBox &s)
{
	if ( &s != this ){
		clear();
		box = boxCreate(s.x(), s.y(), s.w(), s.h());
	}
	return *this;
}


bool tBox::operator==(const tBox &s) const
{
  if ( &s == this )  return true;
  return false;
}// tBox::operator==


void tBox::clear()
{
	if ( box ){
		boxDestroy(&box);
		box = NULL;
	}
}


int tBox::x() const { return empty() ? 0 : box->x; }
int tBox::y() const { return empty() ? 0 : box->y; }
int tBox::w() const { return empty() ? 0 : box->w; }
int tBox::h() const { return empty() ? 0 : box->h; }


void tBox::setX(int newVal)
{
	if ( !empty() )  box->x = newVal;
}

void tBox::setY(int newVal)
{
	if ( !empty() )  box->y = newVal;
}

void tBox::setW(int newVal)
{
	if ( !empty() )  box->w = newVal;
}

void tBox::setH(int newVal)
{
	if ( !empty() )  box->h = newVal;
}




void dl_getCurrentTime ( int *sec, int *usec )
{
	l_int32 l_sec, l_usec;
	l_getCurrentTime ( &l_sec, &l_usec );
	if ( sec )  *sec = l_sec;
	if ( usec )  *usec = l_usec;
}// dl_getCurrentTime
