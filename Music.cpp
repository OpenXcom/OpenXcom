/*
 * Copyright 2010 Daniel Albano
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
#include "Music.h"

Music::Music() : _music(0)
{
}

Music::~Music()
{
	Mix_FreeMusic(_music);
}

bool Music::load(string filename)
{
	struct stat info;
	if(stat(filename.c_str(), &info) != 0) 
	{
		return false;
	}
	_music = Mix_LoadMUS(filename.c_str());
	if(_music == 0) 
	{
		//throw Mix_GetError();
		return false;
	}
	return true;
}

void Music::play()
{
	if(Mix_PlayMusic(_music, -1) == -1) 
	{
		//throw Mix_GetError();
	}
}