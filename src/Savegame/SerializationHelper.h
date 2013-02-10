/*
 * Copyright 2010-2012 OpenXcom Developers.
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

#ifndef OPENXCOM_SERHELP_H
#define OPENXCOM_SERHELP_H

#include <SDL_types.h>
#include <assert.h>

namespace OpenXcom
{

static Uint32 unserializeInt(Uint8 **buffer, Uint8 sizeKey)
{
	Uint32 ret = 0;
	switch(sizeKey)
	{
	case 1:
		ret = **buffer;
		break;
	case 2:
		ret = *(Uint16*)*buffer;
		break;
	case 3:
		assert(false); // no.
		break;
	case 4:
		ret = *(Uint32*)*buffer;
		break;
	default:
		assert(false); // get out.
	}

	*buffer += sizeKey;

	return ret;
}

static void serializeInt(Uint8 **buffer, Uint8 sizeKey, Uint32 value)
{
	switch(sizeKey)
	{
	case 1:
		**buffer = value;
		break;
	case 2:
		*(Uint16*)*buffer = value;
		break;
	case 3:
		assert(false); // no.
		break;
	case 4:
		*(Uint32*)*buffer = value;
		break;
	default:
		assert(false); // get out.
	}

	*buffer += sizeKey;
}

}

#endif
