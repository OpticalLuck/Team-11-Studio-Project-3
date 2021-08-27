#pragma once

#include <commdlg.h>
#include <GLFW/glfw3.h>
#include <string>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "../GameControl/Settings.h"
#include <tchar.h>
#include <ostream>
#include <iostream>

namespace FileDialog
{
	LPCSTR strFilter[] =
	{
		// _T("Bitmap Files (*. Bmp)\0 * .bmp\0")
		// _T("JPEG Files (*. Jpeg; * jpg)\0 * .jpeg; *. Jpg\0")
		"PNG Files (*. PNG)\0 * .png\0",
		"TGA Files (*. TGA)\0 * .tga\0",
		"All Files (*. *)\0 *. *\0",
	};

	std::string OpenFile(LPCSTR* filter = strFilter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		CHAR strFileTitle[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)CSettings::GetInstance()->pWindow);
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFileTitle = strFileTitle;
		ofn.nMaxFileTitle = sizeof(strFileTitle);
		ofn.lpstrFilter = *filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
		else
		{
			return std::string();
		}
	}
}