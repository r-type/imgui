/* nuklear - v1.00 - public domain */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#include "libretro-core.h"
#include "softdraw.h"
Quad_helper clip;

#define MOUSE_RELATIVE 0 //0 = absolute
int gmx,gmy; // mouse
int mouse_wu=0,mouse_wd=0;
//KEYBOARD
char Key_Sate[512];
char Key_Sate2[512];
char old_Key_Sate[512];
int SUPRON=-1,CTRLON=-1,ALTON=-1,SHIFTON=-1;

#include <imgui.h>
#include "imgui_impl_retro.h"

bool show_test_window = true;
bool show_another_window = false;
ImVec4 clear_color;

bool softrender1=false;

#include <sys/time.h>

long GetTicks(void)
{ // in MSec
#ifndef _ANDROID_

#ifdef __CELLOS_LV2__

   //#warning "GetTick PS3\n"

   unsigned long	ticks_micro;
   uint64_t secs;
   uint64_t nsecs;

   sys_time_get_current_time(&secs, &nsecs);
   ticks_micro =  secs * 1000000UL + (nsecs / 1000);

   return ticks_micro/1000;
#else
   struct timeval tv;
   gettimeofday (&tv, NULL);
   return (tv.tv_sec*1000000 + tv.tv_usec)/1000;
#endif

#else

   struct timespec now;
   clock_gettime(CLOCK_MONOTONIC, &now);
   return (now.tv_sec*1000000 + now.tv_nsec/1000)/1000;
#endif

} 

int app_init()
{
   retroscreen=bmp;

   ImGui_ImplRetro_Init();
   clear_color = ImColor(114, 144, 154);

    // Load Fonts
    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
    //ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyClean.ttf", 13.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

   clip.x=0;
   clip.y=0;
   clip.h=rwidth;
   clip.w=rheight;

   memset(Key_Sate,0,512);
   memset(old_Key_Sate ,0, sizeof(old_Key_Sate));

   gmx=(rwidth/2)-1;
   gmy=(rheight/2)-1;

 return 0;
}

int app_free()
{
    // Cleanup
    ImGui_ImplRetro_Shutdown();

 return 0;
}


int app_event(){

        ImGui_ImplRetro_ProcessEvent();
 return 0;
}

int
app_main()
{
   
        ImGui_ImplRetro_NewFrame();

        // 1. Show a simple window
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
        {
            static float f = 0.0f;
            ImGui::Text("Hello, world!");
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            ImGui::ColorEdit3("clear color", (float*)&clear_color);
            if (ImGui::Button("Test Window")) show_test_window ^= 1;
            if (ImGui::Button("Another Window")) show_another_window ^= 1;

            if(softrender1)ImGui::Text("Segments render");
	    else        ImGui::Text("Standard render");
            if (ImGui::Button("Toggle SoftRender")){
		softrender1^= 1;
	    } 

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        // 2. Show another simple window, this time using an explicit Begin/End pair
        if (show_another_window)
        {
            ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello");
            ImGui::End();
        }

        // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
        if (show_test_window)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
            ImGui::ShowTestWindow(&show_test_window);
        }

	unsigned char r=(unsigned char)(255.0*clear_color.x+0.5);
	unsigned char g=(unsigned char)(255.0*clear_color.y+0.5);
	unsigned char b=(unsigned char)(255.0*clear_color.z+0.5);
	unsigned char a=(unsigned char)(255.0*clear_color.w+0.5);

        unsigned int col=(unsigned int)(r<<16 | g<<8 | b| a<<24);

   	fillrect(0,0,rwidth,rheight,col,clip);

        ImGui::Render();


    return 0;
}

