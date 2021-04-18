#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "ModPortal.h"
#ifndef __NO_MODIO
#include <c/ModioC.h>
#endif
#include "Options.h"

namespace OpenXcom
{

namespace ModPortal
{

bool _init = false;
ModPortalLogin _login = LOGIN_NONE;

bool init()
{
#ifndef __NO_MODIO
	if (_init)
	{
		modioInstallDownloadedMods();
	}
	else
	{
		modioInit(MODIO_ENVIRONMENT_LIVE, 51, "68147f0659a3da8529f481e511bba9db", Options::getUserFolder().c_str());
		_init = true;
	}
#endif
	return _init;
}

void shutdown()
{
#ifndef __NO_MODIO
	if (_init)
	{
		modioShutdown();
	}
#endif
}

void process()
{
#ifndef __NO_MODIO
	if (_init)
	{
		modioProcess();
	}
#endif
}

std::vector<std::string> getInstalledMods()
{
	std::vector<std::string> mods;
#ifndef __NO_MODIO
	if (_init)
	{
		const u32 modCount = modioGetAllInstalledModsCount();
		if (modCount > 0)
		{
			ModioInstalledMod *installedMods = new ModioInstalledMod[modCount];
			modioGetAllInstalledMods(installedMods);
			for (size_t i = 0; i < modCount; ++i)
			{
				mods.push_back(installedMods[i].path);
				modioFreeInstalledMod(&installedMods[i]);
			}
			delete[] installedMods;
		}
	}
#endif
	return mods;
}

#ifndef __NO_MODIO
void onEmailRequest(void *status, ModioResponse response)
{
	ModPortalLogin *login = (ModPortalLogin *)status;
	if (response.code == 200)
	{
		*login = LOGIN_CODE;
	}
	else
	{
		*login = LOGIN_ERROR;
	}
}

void onEmailExchange(void *status, ModioResponse response)
{
	ModPortalLogin *login = (ModPortalLogin*)status;
	if (response.code == 200)
	{
		*login = LOGIN_SUCCESS;
	}
	else
	{
		*login = LOGIN_ERROR;
	}
}
#endif

void login(const std::string &username)
{
#ifndef __NO_MODIO
	if (_init)
	{
		ModPortalLogin status = getLoginStatus();
		if (status == LOGIN_NONE)
		{
			_login = LOGIN_PENDING;
			modioEmailRequest(&_login, username.c_str(), &onEmailRequest);
		}
		else if (status == LOGIN_CODE)
		{
			_login = LOGIN_PENDING;
			modioEmailExchange(&_login, username.c_str(), &onEmailExchange);
		}
	}
#endif
}

ModPortalLogin getLoginStatus()
{
#ifndef __NO_MODIO
	if (_init)
	{
		if (modioIsLoggedIn())
		{
			return LOGIN_SUCCESS;
		}
		else
		{
			return _login;
		}
	}
#endif
	return LOGIN_NONE;
}

void logout()
{
#ifndef __NO_MODIO
	if (_init)
	{
		modioLogout();
		_login = LOGIN_NONE;
	}
#endif
}

std::string getUsername()
{
#ifndef __NO_MODIO
	if (_init && modioIsLoggedIn())
	{
		ModioUser user = modioGetCurrentUser();
		if (user.username)
		{
			return modioGetCurrentUser().username;
		}
	}
#endif
	return std::string();
}

std::string getStatus()
{
#ifndef __NO_MODIO
	if (_init)
	{
		const u32 modCount = modioGetModDownloadQueueCount();
		if (modCount > 0)
		{
			std::ostringstream ss;
			ModioQueuedModDownload *downloadQueue = new ModioQueuedModDownload[modCount];
			modioGetModDownloadQueue(downloadQueue);
			int progress = 0;
			if (downloadQueue->current_progress > 0 && downloadQueue->total_size > 0)
			{
				progress = downloadQueue->current_progress / downloadQueue->total_size * 100.0;
			}
			ss << "Downloading " << modCount << " mods... " << progress << "%";
			for (size_t i = 0; i < modCount; ++i)
			{
				modioFreeQueuedModDownload(&downloadQueue[i]);
			}
			delete[] downloadQueue;
			return ss.str();
		}
		else
		{
			return "Mods updated";
		}
	}
#endif
	return "Offline";
}

}

}
