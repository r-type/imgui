#ifndef LIBRETRO_CORE_H
#define LIBRETRO_CORE_H 1

#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <libretro.h>

//LOG
#if  defined(__ANDROID__) || defined(ANDROID)
#include <android/log.h>
#define LOG_TAG "RetroArch.imgui"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#else
#define LOGI printf
#endif

//TYPES

#define UINT16 uint16_t
#define UINT32 uint32_t
#define uint32 uint32_t
#define uint8 uint8_t

#define MOUSE_RELATIVE 0 //0 = absolute
#define BASE_WIDTH 1280
#define BASE_HEIGHT 720

#define MAX_WIDTH 2048
#define MAX_HEIGHT 2048

#ifdef FRONTEND_SUPPORTS_RGB565
#define M16B
#endif

#define WINDOW_SIZE (BASE_HEIGHT*BASE_WIDTH)

#ifdef FRONTEND_SUPPORTS_RGB565
	extern uint16_t *retroscreen;
	extern uint16_t bmp[WINDOW_SIZE];
	#define PIXEL_BYTES 1
	#define PIXEL_TYPE UINT16
	#define PITCH 2	
#else
	extern unsigned int *retroscreen;
	extern unsigned int bmp[WINDOW_SIZE];
	#define PIXEL_BYTES 2
	#define PIXEL_TYPE UINT32
	#define PITCH 4	
#endif 

//STRUCTURES
typedef struct{
     signed short x, y;
     unsigned short w, h;
} retro_Rect;

typedef struct{
     unsigned char *pixels;
     unsigned short w, h,pitch;
} retro_Surface;

typedef struct{
     unsigned char r,g,b;
} retro_pal;

//VARIABLES
extern int pauseg; 
extern int CROP_WIDTH;
extern int CROP_HEIGHT;
extern int VIRTUAL_WIDTH;
extern int retrow;
extern unsigned int rwidth; 
extern int retroh;
extern unsigned int rheight;
extern int gmx,gmy; // mouse
extern int mouse_wu,mouse_wd;
extern char Key_Sate[512];
extern char old_Key_Sate[512];
extern int SUPRON,CTRLON,ALTON,SHIFTON;
extern unsigned char *retrofont;
extern int rfontx,rfonty;

extern long GetTicks(void);
extern retro_input_poll_t input_poll_cb;
extern retro_input_state_t input_state_cb;

#endif
