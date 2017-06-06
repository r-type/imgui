// ImGui libretro soft binding 
// In this binding, ImTextureID is used to store an OpenGL 'GLuint' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// If your context or own usage of OpenGL involve anything GL3/GL4, prefer using the code in sdl_opengl3_example.
// If you are not sure what that means, prefer using the code in sdl_opengl3_example.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#include "libretro-core.h"
#include "softdraw.h"

#include <imgui.h>
#include "imgui_impl_retro.h"

// Data
static double       g_Time = 0.0f;
static bool         g_MousePressed[3] = { false, false, false };
static float        g_MouseWheel = 0.0f;
static unsigned int       g_FontTexture = 0;

static struct Quad_helper myquad,myclip;

void RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count) {

	if (cmd_lists_count == 0)
	return;

	int vertex_index = 0;
	for (int n = 0; n < cmd_lists_count; n++) {

		const ImDrawList* cmd_list = cmd_lists[n];
                const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;

		for (int i = 0; i < cmd_list->CmdBuffer.size(); i++) {

			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[i];
			if (pcmd->UserCallback) {
				pcmd->UserCallback(cmd_list, pcmd);
			} 
			else {

				for (int k = 0; k <  pcmd->ElemCount; k+=3) {

    					bool text= !(cmd_list->VtxBuffer[idx_buffer[k]].uv.x==cmd_list->VtxBuffer[idx_buffer[k+1]].uv.x && 
						    cmd_list->VtxBuffer[idx_buffer[k]].uv.x==cmd_list->VtxBuffer[idx_buffer[k+2]].uv.x);
	 
					myclip.x=(int)pcmd->ClipRect.x;
					myclip.y=(int)pcmd->ClipRect.y;
					myclip.h=(int)pcmd->ClipRect.z;
					myclip.w=(int)pcmd->ClipRect.w;

    					if(text){

						static int cas=0;
	
						if(cas==0){
							cas=1;
						}
						else if(cas==1){
							cas=0;
	
							myquad.x=cmd_list->VtxBuffer[idx_buffer[k]].pos.x;
							myquad.y=cmd_list->VtxBuffer[idx_buffer[k]].pos.y;	
							myquad.w=cmd_list->VtxBuffer[idx_buffer[k+1]].pos.x - cmd_list->VtxBuffer[idx_buffer[k]].pos.x;
							myquad.h=cmd_list->VtxBuffer[idx_buffer[k+2]].pos.y - cmd_list->VtxBuffer[idx_buffer[k]].pos.y;
							myquad.u=cmd_list->VtxBuffer[idx_buffer[k]].uv.x;
							myquad.v=cmd_list->VtxBuffer[idx_buffer[k]].uv.y;
							myquad.col= (cmd_list->VtxBuffer[idx_buffer[k]].col & 0xFF00FF00) |
								(cmd_list->VtxBuffer[idx_buffer[k]].col & 0xFF)<<16 |
								(cmd_list->VtxBuffer[idx_buffer[k]].col >> 16) & 0xFF;
		
							FillTextureQuad(myquad.x,myquad.y,myquad.w,myquad.h,myquad.u,myquad.v,myquad.col,myclip);
						}
				
					}
					else{

						unsigned char *c = (unsigned char*)&cmd_list->VtxBuffer[idx_buffer[k+2]].col;
						unsigned int col=c[3]<<24 | c[0]<<16 | c[1]<<8 |c[2];

						ImDrawVert pt[3];
	
						pt[0]=cmd_list->VtxBuffer[idx_buffer[k+0]];
						pt[1]=cmd_list->VtxBuffer[idx_buffer[k+1]];
						pt[2]=cmd_list->VtxBuffer[idx_buffer[k+2]];


						triangle(pt[0],pt[1],pt[2],col,myclip);

					}

				}

			}
			idx_buffer += pcmd->ElemCount;
		}
	}
}

// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)

void ImGui_ImplRetro_RenderDrawLists(ImDrawData* draw_data)
{
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    ImGuiIO& io = ImGui::GetIO();
    int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    RenderDrawLists(draw_data->CmdLists, draw_data->CmdListsCount);

}

static const char* ImGui_ImplRetro_GetClipboardText(void*)
{
    return NULL;//SDL_GetClipboardText();
}

static void ImGui_ImplRetro_SetClipboardText(void*, const char* text)
{
    //SDL_SetClipboardText(text);
}

bool ImGui_ImplRetro_ProcessEvent()
{
    ImGuiIO& io = ImGui::GetIO();

    input_poll_cb();

    int i;
    for(i=0;i<320;i++)
        Key_Sate[i]=input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0,i) ? 0x80: 0;
   
    if(memcmp( Key_Sate,old_Key_Sate , sizeof(Key_Sate) ) )
	 for(i=0;i<320;i++)
		if(Key_Sate[i] && Key_Sate[i]!=old_Key_Sate[i]  )
        	{	

			if(i==RETROK_LSUPER /*|| i==RETROK_RSUPER*/ ){
				SUPRON=-SUPRON;io.KeySuper =true;
				continue;
			}else io.KeySuper =false;
			if(i==RETROK_LCTRL /*|| i==RETROK_RCTRL*/ ){
				CTRLON=-CTRLON;	io.KeyCtrl =true;			
				continue;
			}else io.KeyCtrl = false;
			if(i==RETROK_LSHIFT/* || i==RETROK_RSHIFT*/){
				SHIFTON=-SHIFTON;io.KeyShift =true;
				continue;
			}else io.KeyShift =false;
			if(i==RETROK_LALT){
				ALTON=-ALTON;io.KeyAlt =true;
				continue;
			}else io.KeyAlt =false;

			io.KeysDown[i] = true;
			if(i<256)io.AddInputCharacter((unsigned short)i);
        	}	
        	else if ( !Key_Sate[i] && Key_Sate[i]!=old_Key_Sate[i]  )
        	{
			if(i==RETROK_LSUPER /*|| i==RETROK_RSUPER*/ ){
//				SUPRON=-SUPRON;
				continue;
			}
			if(i==RETROK_LCTRL /*|| i==RETROK_RCTRL*/ ){
//				CTRLON=-CTRLON;					
				continue;
			}
			if(i==RETROK_LSHIFT/* || i==RETROK_RSHIFT*/){
//				SHITFON=-SHITFON;
				continue;
			}
			if(i==RETROK_LALT){
//				ALTON=-ALTON;
				continue;
			}

			io.KeysDown[i] = false;	
        	}	

   memcpy(old_Key_Sate,Key_Sate , sizeof(Key_Sate) );

/*
   io.KeyShift = SHIFTON?1:0;
   io.KeyCtrl =  CTRLON?1:0;
   io.KeyAlt =   ALTON?1:0;
   io.KeySuper = SUPRON?1:0;
*/

/*
     case SDL_TEXTINPUT:
        {
            io.AddInputCharactersUTF8(event->text.text);
            return true;
        }
*/

    return false;
}

int hanlde_mouse(){

   static int mmbL=0,mmbR=0,mmbM=0;

   int mouse_l,mouse_m,mouse_r;

   int16_t mouse_x,mouse_y;
   mouse_x=mouse_y=0;

   mouse_wu = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_WHEELUP);
   mouse_wd = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_WHEELDOWN);
   if(mouse_wu || mouse_wd)g_MouseWheel=mouse_wd?-1:1;

   mouse_l    = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT);
   mouse_r    = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT);
   mouse_m    = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_MIDDLE); 

   //relative
   if(MOUSE_RELATIVE){

     mouse_x = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
     mouse_y = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);

     gmx+=mouse_x;
     gmy+=mouse_y;
     if(gmx<0)gmx=0;
     if(gmx>rwidth-1)gmx=rwidth-1;
     if(gmy<0)gmy=0;
     if(gmy>rheight-1)gmy=rheight-1;

   }
   else{

     //absolute
     //FIXME FULLSCREEN no pointer
     int p_x = input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_X);
     int p_y = input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_Y);

     if(p_x!=0 && p_y!=0){
	int px=(int)((p_x+0x7fff)*rwidth/0xffff);
	int py=(int)((p_y+0x7fff)*rheight/0xffff);
	//printf("px:%d py:%d (%d,%d)\n",p_x,p_y,px,py);
	gmx=px;
	gmy=py;
     }

   }

   if(mmbL==0 && mouse_l){

	mmbL=1;		
	g_MousePressed[0] = true;

   }
   else if(mmbL==1 && !mouse_l) {

	mmbL=0;
	g_MousePressed[0] = false;
   }

   if(mmbR==0 && mouse_r){

      	mmbR=1;
	g_MousePressed[1] = true;	
   }
   else if(mmbR==1 && !mouse_r) {

      	mmbR=0;
	g_MousePressed[1] = false;
   }

   if(mmbM==0 && mouse_m){

      	mmbM=1;
	g_MousePressed[2] = true;	
   }
   else if(mmbM==1 && !mouse_m) {

      	mmbM=0;
	g_MousePressed[2] = false;
   }

    return false;
}

bool ImGui_ImplRetro_CreateDeviceObjects()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

    printf("texture:(%dx%d)\n",width, height);
    retrofont=(unsigned char*)malloc(width*height); 
    memcpy(retrofont,pixels,width*height);

    rfontx=width;
    rfonty=height;

    g_FontTexture=1;

    io.Fonts->TexID =(void *)NULL;

    return true;
}

void    ImGui_ImplRetro_InvalidateDeviceObjects()
{
    if (g_FontTexture)
    {
	if(retrofont)free(retrofont);
        ImGui::GetIO().Fonts->TexID = 0;
        g_FontTexture = 0;
    }
}

bool    ImGui_ImplRetro_Init()
{
    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = RETROK_TAB;                     // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
    io.KeyMap[ImGuiKey_LeftArrow] = RETROK_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = RETROK_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = RETROK_UP;
    io.KeyMap[ImGuiKey_DownArrow] = RETROK_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = RETROK_PAGEUP;
    io.KeyMap[ImGuiKey_PageDown] = RETROK_PAGEDOWN;
    io.KeyMap[ImGuiKey_Home] = RETROK_HOME;
    io.KeyMap[ImGuiKey_End] = RETROK_END;
    io.KeyMap[ImGuiKey_Delete] = RETROK_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = RETROK_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = RETROK_RETURN;
    io.KeyMap[ImGuiKey_Escape] = RETROK_ESCAPE;
    io.KeyMap[ImGuiKey_A] = RETROK_a;
    io.KeyMap[ImGuiKey_C] = RETROK_c;
    io.KeyMap[ImGuiKey_V] = RETROK_v;
    io.KeyMap[ImGuiKey_X] = RETROK_x;
    io.KeyMap[ImGuiKey_Y] = RETROK_y;
    io.KeyMap[ImGuiKey_Z] = RETROK_z;

    io.RenderDrawListsFn = ImGui_ImplRetro_RenderDrawLists;   // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
    io.SetClipboardTextFn = ImGui_ImplRetro_SetClipboardText;
    io.GetClipboardTextFn = ImGui_ImplRetro_GetClipboardText;
    io.ClipboardUserData = NULL;

    return true;
}

void ImGui_ImplRetro_Shutdown()
{
    ImGui_ImplRetro_InvalidateDeviceObjects();
    ImGui::Shutdown();
}

void ImGui_ImplRetro_NewFrame()
{
    if (!g_FontTexture)
        ImGui_ImplRetro_CreateDeviceObjects();

    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    int w=rwidth, h=rheight;
    int display_w=rwidth, display_h=rheight;

    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

    // Setup time step
    unsigned int time = GetTicks();
    double current_time = time / 1000.0;
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f/60.0f);
    g_Time = current_time;

    hanlde_mouse();

    io.MousePos = ImVec2((float)gmx, (float)gmy); 
    //io.MousePos = ImVec2(-1,-1);

    io.MouseDown[0] = g_MousePressed[0] ;//|| (mouseMask & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;		// If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
    io.MouseDown[1] = g_MousePressed[1];// || (mouseMask & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
    io.MouseDown[2] = g_MousePressed[2];// || (mouseMask & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
   // g_MousePressed[0] = g_MousePressed[1] = g_MousePressed[2] = false;

    io.MouseWheel = g_MouseWheel;
    g_MouseWheel = 0.0f;

    // Hide OS mouse cursor if ImGui is drawing it
    //SDL_ShowCursor(io.MouseDrawCursor ? 0 : 1);

    // Start the frame
    ImGui::NewFrame();
}
