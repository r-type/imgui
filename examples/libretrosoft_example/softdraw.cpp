
#include "libretro-core.h"
#include "softdraw.h"

void FillTextureQuad(int x,int y,int w,int h,float u,float v,unsigned int col,Quad_helper myclip)
{
    //FIXME: scale font (only scale=1 for now)
    int pos=0;

    int startu= 0.5+ u*rfontx;
    int startv= 0.5+ v*rfonty;

    int stu=startu;
    int stv=startv;

    for (int16_t j=y; j<=y+h; j++){

	int stu=(int)startu;

	for (int16_t i=x; i<=x+w; i++){

		pos=stu+(stv*rfontx);

		if(i<myclip.x ||i>(myclip.h)||j<myclip.y ||j>(myclip.w) );
		else if((j*rwidth+i)<rwidth*rheight)
			if(pos<(rfontx*rfonty)-1)
				if(retrofont[pos]!=0)					
					retroscreen[j*rwidth+i]=col;

		stu++;
	}

	stv++;
    }

}

#include <cmath>
#include <ostream> 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class t> struct Vec2 {
	union {
		struct {t u, v;};
		struct {t x, y;};
		t raw[2];
	};
	Vec2() : u(0), v(0) {}
	Vec2(t _u, t _v) : u(_u),v(_v) {}
	inline Vec2<t> operator +(const Vec2<t> &V) const { return Vec2<t>(u+V.u, v+V.v); }
	inline Vec2<t> operator -(const Vec2<t> &V) const { return Vec2<t>(u-V.u, v-V.v); }
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(u*f, v*f); }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);
};
typedef Vec2<int> Vec2i;
template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

#include <algorithm> 
#include <utility>
#include <vector>

void triangle(ImDrawVert tt0,  ImDrawVert tt1,  ImDrawVert tt2,unsigned int color,Quad_helper myclip) {

	Vec2i t0=Vec2i(tt0.pos.x,tt0.pos.y);
	Vec2i t1=Vec2i(tt1.pos.x,tt1.pos.y);	
	Vec2i t2=Vec2i(tt2.pos.x,tt2.pos.y);

    if (t0.y==t1.y && t0.y==t2.y) return; // i dont care about degenerate triangles
    if (t0.y>t1.y) std::swap(t0, t1);
    if (t0.y>t2.y) std::swap(t0, t2);
    if (t1.y>t2.y) std::swap(t1, t2);

    int total_height = t2.y-t0.y;
    for (int i=0; i<total_height; i++) {
        bool second_half = i>t1.y-t0.y || t1.y==t0.y;
        int segment_height = second_half ? t2.y-t1.y : t1.y-t0.y;
        float alpha = (float)i/total_height;
        float beta  = (float)(i-(second_half ? t1.y-t0.y : 0))/segment_height; // be careful: with above conditions no division by zero here
        Vec2i A =               t0 + (t2-t0)*alpha;
        Vec2i B = second_half ? t1 + (t2-t1)*beta : t0 + (t1-t0)*beta;
        if (A.x>B.x) std::swap(A, B);
        for (int j=A.x; j<=B.x; j++) {
  		if(((t0.y+i)*rwidth+j)>rwidth*rheight);//printf(" (%d)",(t0.y+i)*rwidth+j);
		else if(j<myclip.x||j>(myclip.h) || (t0.y+i)<myclip.y || (t0.y+i)>(myclip.w) );
		else retroscreen[(t0.y+i)*rwidth+j]=color;// attention, due to int casts t0.y+i != A.y		
    	}
    }
}

// Bresenham's algorithm - thx wikpedia
#ifdef M16B
void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
#else
void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, unsigned int color) {
#endif
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
		retroscreen[(x0)*rwidth+y0]=color;
        } else {
		retroscreen[(y0)*rwidth+x0]=color;
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}
#ifdef M16B
void drawFastVLine(int16_t x, int16_t y,
        int16_t h, uint16_t color) {
#else
void drawFastVLine(int16_t x, int16_t y,
        int16_t h, unsigned int color) {
#endif
       writeLine(x, y, x, y+h-1, color);

}
#ifdef M16B
void fillrect(int x,int y,int w,int h,unsigned short int col){
#else
void fillrect(int x,int y,int w,int h,unsigned int col){
#endif
    for (int16_t i=x; i<x+w; i++) {
 	drawFastVLine(i, y, h, col);
    }
}

