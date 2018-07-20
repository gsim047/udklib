#ifndef __tLept_h
#define __tLept_h 1

#include <string>
using std::string;


struct Pix;
struct Box;





// tBox - класс-обертка для структуры Box. 
//  реализует работу с прямоугольным фрагментом изображения
class tBox {
public:
	tBox() : box(NULL) {}
	tBox(Box *bx) : box(bx) {}
	tBox(int x, int y, int w, int h);
	tBox(const tBox &s);
	virtual ~tBox() { clear(); }

	bool empty() const { return !box; }

	bool operator==(const tBox &s) const;
	bool operator!=(const tBox &s) const { return !operator==(s); }

	tBox &operator=(const tBox &s);

	operator Box* () const { return box; }

	tBox& operator=(Box *bx) { clear(); box = bx; return *this; }

	int x() const;
	int y() const;
	int w() const;
	int h() const;

	void setX(int newVal);
	void setY(int newVal);
	void setW(int newVal);
	void setH(int newVal);

	void clear();

public:
	Box *box;
};// class tBox




// tPix - класс-обертка для структуры Pix
//  реализует основную работу с изображением в leptonica
class tPix {
public:
	enum pixColor {
		pix_COLOR_RED = 0,
		pix_COLOR_GREEN = 1,
		pix_COLOR_BLUE = 2,
		pix_ALPHA_CHANNEL = 3
	};

public:
	// конструкторы и деструктор
	tPix() : pix(NULL) {}
	tPix(Pix *px);
	tPix(const string &filename);
	tPix(int width, int height, int depth);
	virtual ~tPix() { clear(); }

    // задано ли изображение?
	bool empty() const { return !pix; }

	bool operator==(const tPix &s) const;
	bool operator!=(const tPix &s) const { return !operator==(s); }

	operator Pix* () const { return pix; }

    // ширина, высота и глубина цвета изображения
	int width() const;
	int height() const;
	int depth() const;

    // конструктор копии и оператор присваивания
	tPix(const tPix &s);
	tPix &operator=(const tPix &s);
	tPix& operator=(Pix *px);

    // очистить tPix
	void clear();

    // сохранить в файл в формате jpeg
	bool writeJpeg(const string &fname, int quality) const;

    // скопировать участок в отдельный объект
	tPix clipRectangle(const tBox &) const;

    // найти минимумы и максимумы
	int getExtremeValue(int factor, bool minVal, int *prval, int *pgval, int *pbval, int *pgrayval) const;
    // изменить контраст и яркость изображения
	tPix gammaTRC (double gamma, int minval, int maxval) const;
    // выделить один цветовой канал
	tPix getRGBComponent ( pixColor color ) const;
    // повернуть изображение на 90 градусов
	tPix rotate90 ( int direction ) const;

    // получить разрешение изображения
	int getResolutionX();
	int getResolutionY();
    // установить разрешение изображения
	bool setResolution(int xres, int yres);

    // изменить размер изображения
	tPix scale(double xscale, double yscale) const;

		// 
	int clearInRect (const tBox& );

protected:
	Pix *pix;
};// class tPix



// разные функции, которые используются в частности в Tesseract-е, в дополнении для Win32-port
void dl_getCurrentTime ( int *sec, int *usec );


#endif // __tLept_h
