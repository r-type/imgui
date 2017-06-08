
#include <imgui.h>

struct Quad_helper {
   int x;
   int y;
   int w;
   int h;
   float u;
   float v;
   unsigned int col;
};

#ifdef M16B
extern void fillrect(int x,int y,int w,int h,unsigned short int col,Quad_helper myclip);
#else
extern void fillrect(int x,int y,int w,int h,unsigned int col,Quad_helper myclip);
#endif

extern bool softrender1;
extern void trianglex(ImDrawVert tt0,  ImDrawVert tt1,  ImDrawVert tt2,unsigned int color,Quad_helper myclip);
extern void triangle(ImDrawVert tt0,  ImDrawVert tt1,  ImDrawVert tt2,unsigned int color,Quad_helper myclip);
extern void FillTextureQuad(int x,int y,int w,int h,float u,float v,unsigned int col,Quad_helper myclip);
