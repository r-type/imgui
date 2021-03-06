
DEBUG := 0

ifeq ($(platform),)
	platform = unix
	ifeq ($(shell uname -a),)
		platform = win
	else ifneq ($(findstring MINGW,$(shell uname -a)),)
		platform = win
	else ifneq ($(findstring Darwin,$(shell uname -a)),)
		platform = osx
		arch = intel
	ifeq ($(shell uname -p),powerpc)
		arch = ppc
	endif
	else ifneq ($(findstring win,$(shell uname -a)),)
		platform = win
	endif
endif

# system platform
system_platform = unix
ifeq ($(shell uname -a),)
	EXE_EXT = .exe
	system_platform = win
else ifneq ($(findstring Darwin,$(shell uname -a)),)
	system_platform = osx
	arch = intel
	ifeq ($(shell uname -p),powerpc)
		arch = ppc
	endif
	else ifneq ($(findstring MINGW,$(shell uname -a)),)
	system_platform = win
endif

TARGET_NAME = imgui

ifeq ($(platform), unix)
   TARGET := $(TARGET_NAME)_libretro.so
   fpic := -fPIC
   SHARED := -shared -Wl,--version-script=link.T -Wl,--no-undefined
   GL_LIB := -lGL
else ifneq (,$(findstring osx,$(platform)))
   TARGET := $(TARGET_NAME)_libretro.dylib
   fpic := -fPIC
   SHARED := -dynamiclib
   GL_LIB := -framework OpenGL
   CFLAGS += -DOSX
	ifeq ($(arch),ppc)
		CFLAGS += -D__ppc__ -DOSX_PPC
	endif
	OSXVER = `sw_vers -productVersion | cut -d. -f 2`
	OSX_LT_MAVERICKS = `(( $(OSXVER) <= 9)) && echo "YES"`
	fpic += -mmacosx-version-min=10.1
else ifeq ($(platform), android)
   CC = arm-linux-androideabi-gcc
   CXX =arm-linux-androideabi-g++
   AR = @arm-linux-androideabi-ar
   LD = @arm-linux-androideabi-g++ 
   TARGET := $(TARGET_NAME)_libretro_android.so
   fpic := -fPIC
	LDFLAGS := -lz -lm -llog
   SHARED :=  -Wl,--fix-cortex-a8 -shared -Wl,--version-script=link.T -Wl,--no-undefined
   CFLAGS += -DAND
else ifeq ($(platform), pi)
   TARGET := $(TARGET_NAME)_libretro.so
   fpic := -fPIC
   SHARED := -shared -Wl,--version-script=link.T -Wl,--no-undefined
   CFLAGS += -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/vmcs_host/linux
   GLES := 1
   LIBS += -L/opt/vc/lib
else ifneq (,$(findstring ios,$(platform)))
   TARGET := $(TARGET_NAME)_libretro_ios.dylib
   fpic := -fPIC
   SHARED := -dynamiclib

ifeq ($(IOSSDK),)
   IOSSDK := $(shell xcodebuild -version -sdk iphoneos Path)
endif

   GL_LIB := -framework OpenGLES
   DEFINES := -DIOS
   CFLAGS += -DGLES $(DEFINES)
   CC = cc -arch armv7 -isysroot $(IOSSDK)
ifeq ($(platform),ios9)
	CC     += -miphoneos-version-min=8.0
	CFLAGS += -miphoneos-version-min=8.0
else
	CC     += -miphoneos-version-min=5.0
	CFLAGS += -miphoneos-version-min=5.0
endif
else ifneq (,$(findstring qnx,$(platform)))
   TARGET := $(TARGET_NAME)_libretro_qnx.so
   fpic := -fPIC
   SHARED := -shared -Wl,--version-script=link.T
   GL_LIB := -lGL

   CC = qcc -Vgcc_ntoarmv7le
   AR = qcc -Vgcc_ntoarmv7le
   CFLAGS += -DGLES
   GL_LIB := -lGLESv2
   GLES := 1
else ifneq (,$(findstring armv,$(platform)))
   CC = gcc
   TARGET := $(TARGET_NAME)_libretro.so
   fpic := -fPIC
   SHARED := -shared -Wl,--version-script=link.T -Wl,--no-undefined
   CFLAGS += -I.
ifneq (,$(findstring gles,$(platform)))
   GLES := 1
else
   GL_LIB := -lGL
endif
ifneq (,$(findstring cortexa8,$(platform)))
   CFLAGS += -marm -mcpu=cortex-a8
else ifneq (,$(findstring cortexa9,$(platform)))
   CFLAGS += -marm -mcpu=cortex-a9
endif
   CFLAGS += -marm
ifneq (,$(findstring neon,$(platform)))
   CFLAGS += -mfpu=neon
endif
ifneq (,$(findstring softfloat,$(platform)))
   CFLAGS += -mfloat-abi=softfp
else ifneq (,$(findstring hardfloat,$(platform)))
   CFLAGS += -mfloat-abi=hard
endif
   CFLAGS += -DARM
# emscripten
else ifeq ($(platform), emscripten)
	TARGET := $(TARGET_NAME)_libretro_emscripten.bc
else ifeq ($(platform), wincross64)
	TARGET := $(TARGET_NAME)_libretro.dll
	AR = x86_64-w64-mingw32-ar
	CC = x86_64-w64-mingw32-gcc
	CXX = x86_64-w64-mingw32-g++ 
	SHARED := -shared -Wl,--no-undefined -Wl,--version-script=link.T
	LDFLAGS += -static-libgcc -static-libstdc++ 
	EXTRA_LDF := -lwinmm -Wl,--export-all-symbols
else
   CC = gcc
   TARGET := $(TARGET_NAME)_libretro.dll
   SHARED := -shared -static-libgcc -static-libstdc++ -s -Wl,--version-script=link.T -Wl,--no-undefined
   GL_LIB := -lopengl32
   CFLAGS += -I..
endif

ifeq ($(DEBUG), 1)
   CFLAGS += -O0 -g
else
   CFLAGS += -O2
endif


INCLUDES += -I. -I../ -I../.. -I./retro

OBJECTS := libretro.o  app.o softdraw.o imgui_impl_retro.o ../../imgui_demo.o ../../imgui.o ../../imgui_draw.o

CFLAGS += -Wall $(fpic) -std=c++14

ifeq ($(CORE), 1)
   CFLAGS += -DCORE
endif


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(fpic) $(SHARED) $(INCLUDES) -o $@ $(OBJECTS) $(LIBS) -lm $(EXTRA_GL_LIBS)

%.o: %.c
	$(CC) -I. $(INCLUDES) $(CFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) -I. $(INCLUDES) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: clean

