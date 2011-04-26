/*
 * Copyright 2010 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "CrossPlatform.h"
#ifdef _WIN32
#include <Windows.h>
#endif
#include <iostream>

namespace OpenXcom
{

/**
 * Displays a message box with an error message.
 * @param error Error message.
 */
void CrossPlatform::showError(const std::string &error)
{
#ifdef _WIN32
	MessageBoxA(NULL, error.c_str(), "OpenXcom Error", MB_ICONERROR | MB_OK);
#else
	std::cerr << "ERROR: " << error << std::endl;
#endif
}

/**
 * Displays a message box with an error message.
 * @param error Error message.
 */
void CrossPlatform::showError(const std::wstring &error)
{
#ifdef _WIN32
	MessageBoxW(NULL, error.c_str(), L"OpenXcom Error", MB_ICONERROR | MB_OK);
#else
	std::wcerr << L"ERROR: " << error << std::endl;
#endif
}

}
