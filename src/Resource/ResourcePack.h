/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#ifndef OPENXCOM_RESOURCEPACK_H
#define OPENXCOM_RESOURCEPACK_H

#include <map>
#include <string>
#include <vector>
#include <SDL.h>

namespace OpenXcom
{

class Surface;
class SurfaceSet;
class Font;
class Palette;
class Music;
class SoundSet;
class Sound;

/**
 * Packs of external game media.
 * Resource packs contain all the game media that's
 * loaded externally, like graphics, fonts, languages,
 * audio and world map.
 * @note The game is still hardcoded to X-Com resources,
 * so for now this just serves to keep all the file loading
 * in one place.
 */
class ResourcePack
{
private:
	Music *_muteMusic;
	Sound *_muteSound;
	std::string _playingMusic;
protected:
	std::map<std::string, Palette*> _palettes;
	std::map<std::string, Font*> _fonts;
	std::map<std::string, Surface*> _surfaces;
	std::map<std::string, SurfaceSet*> _sets;
	std::map<std::string, SoundSet*> _sounds;
	std::map<std::string, Music*> _musics;
	std::vector<Uint16> _voxelData;
public:
	static int DOOR_OPEN;
	static int SLIDING_DOOR_OPEN;
	static int SLIDING_DOOR_CLOSE;
	static int SMALL_EXPLOSION;
	static int LARGE_EXPLOSION;
	static int EXPLOSION_OFFSET;
	static int ITEM_DROP;
	static int ITEM_THROW;
	static int ITEM_RELOAD;
	static int WALK_OFFSET;
	static int FLYING_SOUND;
	static int MALE_SCREAM[3];
	static int FEMALE_SCREAM[3];
	static int BUTTON_PRESS;
	static int WINDOW_POPUP[3];
	static int UFO_FIRE;
	static int UFO_HIT;
	static int UFO_CRASH;
	static int UFO_EXPLODE;
	static int INTERCEPTOR_HIT;
	static int INTERCEPTOR_EXPLODE;
	/// Create a new resource pack with a folder's contents.
	ResourcePack();
	/// Cleans up the resource pack.
	virtual ~ResourcePack();
	/// Gets a particular font.
	Font *getFont(const std::string &name) const;
	/// Gets a particular surface.
	Surface *getSurface(const std::string &name) const;
	/// Gets a particular surface set.
	SurfaceSet *getSurfaceSet(const std::string &name) const;
	/// Gets a particular music.
	Music *getMusic(const std::string &name) const;
	/// Gets a random music.
	Music *getRandomMusic(const std::string &name) const;
	/// Plays a particular music.
	void playMusic(const std::string &name, bool random = false);
	/// Gets a particular sound.
	Sound *getSound(const std::string &set, unsigned int sound) const;
	/// Gets a particular palette.
	Palette *getPalette(const std::string &name) const;
	/// Sets a new palette.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Gets list of voxel data.
	std::vector<Uint16> *getVoxelData();
	/// Returns a specific sound from either the land or underwater resource set.
	Sound *getSoundByDepth(unsigned int depth, unsigned int sound) const;
};

}

#endif
