#pragma once
#define _CRT_SECURE_NO_WARNINGS


// Including cpp headers
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <map>
#include <functional>
#include <thread>

// Including external headers
#include <conio.h>
#include <ws2tcpip.h>
#include <wininet.h>
#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#include <gdiplus.h>
#include "KrimzLib/incl/OpenCL/cl.h"
#include "KrimzLib/incl/OpenGL/glad.h"
#include <gl/GLU.h>

// Including kl headers
#include "KrimzLib/type.h"
#include "KrimzLib/literal.h"
#include "KrimzLib/constant.h"
#include "KrimzLib/convert.h"
#include "KrimzLib/math.h"
#include "KrimzLib/random.h"
#include "KrimzLib/time.h"
#include "KrimzLib/string.h"
#include "KrimzLib/console.h"
#include "KrimzLib/array.h"
#include "KrimzLib/binary.h"
#include "KrimzLib/file.h"
#include "KrimzLib/thread.h"
#include "KrimzLib/image.h"
#include "KrimzLib/net.h"
#include "KrimzLib/opencl.h"
#include "KrimzLib/opengl.h"
#include "KrimzLib/window.h"
#include "KrimzLib/engine.h"

// Linking .libs
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

// Library intialiser
namespace kl {
	void InitLib() {
		random::SetSeed();
		time::LoadPCFrequency();
		console::EnableRGB();
		image::InitGdiPlus();
		net::InitWinSock();
	}
}