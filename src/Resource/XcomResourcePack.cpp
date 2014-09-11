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
#include "XcomResourcePack.h"
#include <sstream>
#include "../Engine/CrossPlatform.h"
#include "../Engine/Palette.h"
#include "../Engine/Font.h"
#include "../Engine/Surface.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Language.h"
#include "../Engine/Music.h"
#include "../Engine/GMCat.h"
#include "../Engine/SoundSet.h"
#include "../Engine/Sound.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Savegame/Node.h"
#include "../Battlescape/Position.h"
#include "../Ruleset/MapDataSet.h"
#include "../Engine/ShaderDraw.h"
#include "../Engine/ShaderMove.h"
#include "../Engine/Exception.h"
#include "../Engine/Logger.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/SoundDefinition.h"
#include "../Ruleset/ExtraSprites.h"
#include "../Ruleset/ExtraSounds.h"
#include "../Engine/AdlibMusic.h"

namespace OpenXcom
{

namespace
{
	
struct HairBleach
{
	static const Uint8 ColorGroup = 15<<4;
	static const Uint8 ColorShade = 15;

	static const Uint8 Hair = 9 << 4;
	static const Uint8 Face = 6 << 4;
	static inline void func(Uint8& src, const Uint8& cutoff, int, int, int)
	{
		if(src > cutoff && src <= Face + 15)
		{
			src = Hair + (src & ColorShade) - 6; //make hair color like male in xcom_0.pck
		}
	}
};

}
	
/**
 * Initializes the resource pack by loading all the resources
 * contained in the original game folder.
 * @param extraSprites List of mod extra sprites.
 * @param extraSounds List of mod extra sounds.
 */
XcomResourcePack::XcomResourcePack(Ruleset *rules) : ResourcePack()
{
	// Load palettes
	const char *pal[] = {"PAL_GEOSCAPE", "PAL_BASESCAPE", "PAL_GRAPHS", "PAL_UFOPAEDIA", "PAL_BATTLEPEDIA"};
	for (size_t i = 0; i < sizeof(pal) / sizeof(pal[0]); ++i)
	{
		std::string s = "GEODATA/PALETTES.DAT";
		_palettes[pal[i]] = new Palette();
		_palettes[pal[i]]->loadDat(CrossPlatform::getDataFile(s), 256, Palette::palOffset(i));
	}
	{
		std::string s1 = "GEODATA/BACKPALS.DAT";
		std::string s2 = "BACKPALS.DAT";
		_palettes[s2] = new Palette();
		_palettes[s2]->loadDat(CrossPlatform::getDataFile(s1), 128);
	}
	
	// Correct Battlescape palette
	{
		std::string s1 = "GEODATA/PALETTES.DAT";
		std::string s2 = "PAL_BATTLESCAPE";
		_palettes[s2] = new Palette();
		_palettes[s2]->loadDat(CrossPlatform::getDataFile(s1), 256, Palette::palOffset(4));

		// Last 16 colors are a greyish gradient
		SDL_Color gradient[] = {{140, 152, 148, 255},
								{132, 136, 140, 255},
								{116, 124, 132, 255},
								{108, 116, 124, 255},
								{92, 104, 108, 255},
								{84, 92, 100, 255},
								{76, 80, 92, 255},
								{56, 68, 84, 255},
								{48, 56, 68, 255},
								{40, 48, 56, 255},
								{32, 36, 48, 255},
								{24, 28, 32, 255},
								{16, 20, 24, 255},
								{8, 12, 16, 255},
								{3, 4, 8, 255},
								{3, 3, 6, 255}};
		for (size_t i = 0; i < sizeof(gradient)/sizeof(gradient[0]); ++i)
		{
			SDL_Color *color = _palettes[s2]->getColors(Palette::backPos + 16 + i);
			*color = gradient[i];
		}
	}

	// Load fonts
	YAML::Node doc = YAML::LoadFile(CrossPlatform::getDataFile("Language/Font.dat"));
	Font::setIndex(Language::utf8ToWstr(doc["chars"].as<std::string>()));
	for (YAML::const_iterator i = doc["fonts"].begin(); i != doc["fonts"].end(); ++i)
	{
		std::string id = (*i)["id"].as<std::string>();
		Font *font = new Font();
		font->load(*i);
		_fonts[id] = font;
	}

	// Load surfaces
	{
		std::ostringstream s;
		s << "GEODATA/" << "INTERWIN.DAT";
		_surfaces["INTERWIN.DAT"] = new Surface(160, 556);
		_surfaces["INTERWIN.DAT"]->loadScr(CrossPlatform::getDataFile(s.str()));
	}

	std::string geograph = CrossPlatform::getDataFolder("GEOGRAPH/");
	std::vector<std::string> scrs = CrossPlatform::getFolderContents(geograph, "SCR");
	for (std::vector<std::string>::iterator i = scrs.begin(); i != scrs.end(); ++i)
	{
		std::string path = geograph + *i;
		std::transform(i->begin(), i->end(), i->begin(), toupper);
		_surfaces[*i] = new Surface(320, 200);
		_surfaces[*i]->loadScr(path);
	}
	std::vector<std::string> bdys = CrossPlatform::getFolderContents(geograph, "BDY");
	for (std::vector<std::string>::iterator i = bdys.begin(); i != bdys.end(); ++i)
	{
		std::string path = geograph + *i;
		std::transform(i->begin(), i->end(), i->begin(), toupper);
		_surfaces[*i] = new Surface(320, 200);
		_surfaces[*i]->loadBdy(path);
	}

	// bigger geoscape background
	int newWidth = 320 - 64, newHeight = 200;
	Surface *newGeo = new Surface(newWidth*3, newHeight*3);
	Surface *oldGeo = _surfaces["GEOBORD.SCR"];
	for (int x = 0; x < newWidth; ++x)
	{
		for (int y = 0; y < newHeight; ++y)
		{
			newGeo->setPixel(newWidth+x, newHeight+y, oldGeo->getPixel(x, y));
			newGeo->setPixel(newWidth-x-1, newHeight+y, oldGeo->getPixel(x, y));
			newGeo->setPixel(newWidth*3-x-1, newHeight+y, oldGeo->getPixel(x, y));
			
			newGeo->setPixel(newWidth+x, newHeight-y-1, oldGeo->getPixel(x, y));
			newGeo->setPixel(newWidth-x-1, newHeight-y-1, oldGeo->getPixel(x, y));
			newGeo->setPixel(newWidth*3-x-1, newHeight-y-1, oldGeo->getPixel(x, y));
			
			newGeo->setPixel(newWidth+x, newHeight*3-y-1, oldGeo->getPixel(x, y));
			newGeo->setPixel(newWidth-x-1, newHeight*3-y-1, oldGeo->getPixel(x, y));
			newGeo->setPixel(newWidth*3-x-1, newHeight*3-y-1, oldGeo->getPixel(x, y));
		}
	}
	_surfaces["ALTGEOBORD.SCR"] = newGeo;

	// here we create an "alternate" background surface for the base info screen.
	_surfaces["ALTBACK07.SCR"] = new Surface(320, 200);
	_surfaces["ALTBACK07.SCR"]->loadScr(CrossPlatform::getDataFile("GEOGRAPH/BACK07.SCR"));
	for (int y = 172; y >= 152; --y)
		for (int x = 5; x <= 314; ++x)
			_surfaces["ALTBACK07.SCR"]->setPixel(x, y+4, _surfaces["ALTBACK07.SCR"]->getPixel(x,y));
	for (int y = 147; y >= 134; --y)
		for (int x = 5; x <= 314; ++x)
			_surfaces["ALTBACK07.SCR"]->setPixel(x, y+9, _surfaces["ALTBACK07.SCR"]->getPixel(x,y));
	for (int y = 132; y >= 109; --y)
		for (int x = 5; x <= 314; ++x)
			_surfaces["ALTBACK07.SCR"]->setPixel(x, y+10, _surfaces["ALTBACK07.SCR"]->getPixel(x,y));

	std::vector<std::string> spks = CrossPlatform::getFolderContents(geograph, "SPK");
	for (std::vector<std::string>::iterator i = spks.begin(); i != spks.end(); ++i)
	{
		std::string path = geograph + *i;
		std::transform(i->begin(), i->end(), i->begin(), toupper);
		_surfaces[*i] = new Surface(320, 200);
		_surfaces[*i]->loadSpk(path);
	}

	// Load intro
	std::string ufointro = CrossPlatform::getDataFolder("UFOINTRO/");
	std::vector<std::string> lbms = CrossPlatform::getFolderContents(ufointro, "LBM");
	for (std::vector<std::string>::iterator i = lbms.begin(); i != lbms.end(); ++i)
	{
		std::string path = ufointro + *i;
		std::transform(i->begin(), i->end(), i->begin(), toupper);
		_surfaces[*i] = new Surface(320, 200);
		_surfaces[*i]->loadImage(path);
	}

	// Load surface sets
	std::string sets[] = {"BASEBITS.PCK",
						  "INTICON.PCK",
						  "TEXTURE.DAT"};

	for (size_t i = 0; i < sizeof(sets)/sizeof(sets[0]); ++i)
	{
		std::ostringstream s;
		s << "GEOGRAPH/" << sets[i];

		std::string ext = sets[i].substr(sets[i].find_last_of('.')+1, sets[i].length());
		if (ext == "PCK")
		{
			std::string tab = CrossPlatform::noExt(sets[i]) + ".TAB";
			std::ostringstream s2;
			s2 << "GEOGRAPH/" << tab;
			_sets[sets[i]] = new SurfaceSet(32, 40);
			_sets[sets[i]]->loadPck(CrossPlatform::getDataFile(s.str()), CrossPlatform::getDataFile(s2.str()));
		}
		else
		{
			_sets[sets[i]] = new SurfaceSet(32, 32);
			_sets[sets[i]]->loadDat(CrossPlatform::getDataFile(s.str()));
		}
	}
	_sets["SCANG.DAT"] = new SurfaceSet(4, 4);
	std::ostringstream scang;
	scang << "GEODATA/" << "SCANG.DAT";
	_sets["SCANG.DAT"]->loadDat (CrossPlatform::getDataFile(scang.str()));

	if (!Options::mute)
	{
#ifndef __NO_MUSIC
		// Load musics
		std::string mus[] = {"GMDEFEND",
							 "GMENBASE",
							 "GMGEO1",
							 "GMGEO2",
							 "GMINTER",
							 "GMINTRO1",
							 "GMINTRO2",
							 "GMINTRO3",
							 "GMLOSE",
							 "GMMARS",
							 "GMNEWMAR",
							 "GMSTORY",
							 "GMTACTIC",
							 "GMWIN"};
		int tracks[] = {3, 6, 0, 18, 2, 19, 20, 21, 10, 9, 8, 12, 17, 11};
		float tracks_normalize[] = {0.76f, 0.83f, 1.19f, 1.0f, 0.74f, 0.8f, 0.8f, 0.8f, 1.0f, 0.92f, 0.81f, 1.0f, 1.14f, 0.84f};

		// Check which music version is available
		CatFile *adlibcat = 0, *aintrocat = 0;
		GMCatFile *gmcat = 0;

		std::string musicAdlib = "SOUND/ADLIB.CAT", musicIntro = "SOUND/AINTRO.CAT", musicGM = "SOUND/GM.CAT";
		if (CrossPlatform::fileExists(CrossPlatform::getDataFile(musicAdlib)))
		{
			adlibcat = new CatFile(CrossPlatform::getDataFile(musicAdlib).c_str());
			if (CrossPlatform::fileExists(CrossPlatform::getDataFile(musicIntro)))
			{
				aintrocat = new CatFile(CrossPlatform::getDataFile(musicIntro).c_str());
			}
		}
		if (CrossPlatform::fileExists(CrossPlatform::getDataFile(musicGM)))
		{
			gmcat = new GMCatFile(CrossPlatform::getDataFile(musicGM).c_str());
		}

		// Try the preferred format first, otherwise use the default priority
		MusicFormat priority[] = {Options::preferredMusic, MUSIC_FLAC, MUSIC_OGG, MUSIC_MP3, MUSIC_MOD, MUSIC_WAV, MUSIC_ADLIB, MUSIC_MIDI};

		for (size_t i = 0; i < sizeof(mus)/sizeof(mus[0]); ++i)
		{
			Music *music = 0;
			for (size_t j = 0; j < sizeof(priority)/sizeof(priority[0]) && music == 0; ++j)
			{
				music = loadMusic(priority[j], mus[i], tracks[i], tracks_normalize[i], adlibcat, aintrocat, gmcat);
			}
			if (!music)
			{
				throw Exception(mus[i] + " not found");
			}
			_musics[mus[i]] = music;
		}
		delete gmcat;
		delete adlibcat;
		delete aintrocat;

		// Ok, now try to load the optional musics
		std::string musOptional[] = {"GMGEO3",
									 "GMGEO4",
									 "GMGEO5",
									 "GMGEO6",
									 "GMGEO7",
									 "GMGEO8",
									 "GMGEO9",
									 "GMTACTIC2",
									 "GMTACTIC3",
									 "GMTACTIC4",
									 "GMTACTIC5",
									 "GMTACTIC6",
									 "GMTACTIC7",
									 "GMTACTIC8",
									 "GMTACTIC9"};

		for (size_t i = 0; i < sizeof(musOptional)/sizeof(musOptional[0]); ++i)
		{
			Music *music = 0;
			for (size_t j = 0; j < sizeof(priority) / sizeof(priority[0]) && music == 0; ++j)
			{
				music = loadMusic(priority[j], musOptional[i], 0, 0, 0, 0, 0);
			}
			if (music)
			{
				_musics[musOptional[i]] = music;
			}
		}
#endif		
		
		if (rules->getSoundDefinitions()->empty())
		{
			// Load sounds
			std::string catsId[] = {"GEO.CAT", "BATTLE.CAT"};
			std::string catsDos[] = {"SOUND2.CAT", "SOUND1.CAT"};
			std::string catsWin[] = {"SAMPLE.CAT", "SAMPLE2.CAT"};

			// Try the preferred format first, otherwise use the default priority
			std::string *cats[] = {0, catsWin, catsDos};
			if (Options::preferredSound == SOUND_14)
				cats[0] = catsWin;
			else if (Options::preferredSound == SOUND_10)
				cats[1] = catsDos;

			Options::currentSound = SOUND_AUTO;
			for (size_t i = 0; i < sizeof(catsId) / sizeof(catsId[0]); ++i)
			{
				SoundSet *sound = 0;
				for (size_t j = 0; j < sizeof(cats) / sizeof(cats[0]) && sound == 0; ++j)
				{
					bool wav = true;
					if (cats[j] == 0)
						continue;
					else if (cats[j] == catsDos)
						wav = false;
					std::ostringstream s;
					s << "SOUND/" << cats[j][i];
					std::string file = CrossPlatform::getDataFile(s.str());
					if (CrossPlatform::fileExists(file))
					{
						sound = new SoundSet();
						sound->loadCat(file, wav);
						Options::currentSound = (wav) ? SOUND_14 : SOUND_10;
					}
				}
				if (sound == 0)
				{
					throw Exception(catsWin[i] + " not found");
				}
				else
				{
					_sounds[catsId[i]] = sound;
				}
			}
		}
		else
		{
			for (std::map<std::string, SoundDefinition*>::const_iterator i = rules->getSoundDefinitions()->begin();
				i != rules->getSoundDefinitions()->end(); ++i)
			{
				std::ostringstream s;
				s << "SOUND/" << (*i).second->getCATFile();
				std::string file = CrossPlatform::getDataFile(s.str());
				if (CrossPlatform::fileExists(file))
				{
					if (_sounds.find((*i).first) == _sounds.end())
					{
						_sounds[(*i).first] = new SoundSet();
					}
					for (std::vector<int>::const_iterator j = (*i).second->getSoundList().begin(); j != (*i).second->getSoundList().end(); ++j)
					{
						_sounds[(*i).first]->loadCatbyIndex(file, *j);
					}
				}
				else
				{
					s << " not found";
					throw Exception(s.str());
				}
			}
		}
		

		if (CrossPlatform::fileExists(CrossPlatform::getDataFile("SOUND/INTRO.CAT")))
		{
			SoundSet *s = _sounds["INTRO.CAT"] = new SoundSet();
			s->loadCat(CrossPlatform::getDataFile("SOUND/INTRO.CAT"), false);
		}

		if (CrossPlatform::fileExists(CrossPlatform::getDataFile("SOUND/SAMPLE3.CAT")))
		{
			SoundSet *s = _sounds["SAMPLE3.CAT"] = new SoundSet();
			s->loadCat(CrossPlatform::getDataFile("SOUND/SAMPLE3.CAT"), true);
		}
	}

	TextButton::soundPress = getSound("GEO.CAT", ResourcePack::BUTTON_PRESS);
	Window::soundPopup[0] = getSound("GEO.CAT", ResourcePack::WINDOW_POPUP[0]);
	Window::soundPopup[1] = getSound("GEO.CAT", ResourcePack::WINDOW_POPUP[1]);
	Window::soundPopup[2] = getSound("GEO.CAT", ResourcePack::WINDOW_POPUP[2]);

	loadBattlescapeResources(); // TODO load this at battlescape start, unload at battlescape end?
	

	// we create extra rows on the soldier stat screens by shrinking them all down one pixel.
	// this is done after loading them, but BEFORE loading the extraSprites, in case a modder wants to replace them.

	// first, let's do the base info screen
	// erase the old lines, copying from a +2 offset to account for the dithering
	for (int y = 91; y < 199; y += 12)
		for (int x = 0; x < 149; ++x)
			_surfaces["BACK06.SCR"]->setPixel(x, y, _surfaces["BACK06.SCR"]->getPixel(x,y+2));
	// drawn new lines, use the bottom row of pixels as a basis
	for (int y = 89; y < 199; y += 11)
		for (int x = 0; x < 149; ++x)
			_surfaces["BACK06.SCR"]->setPixel(x, y, _surfaces["BACK06.SCR"]->getPixel(x,199));
	// finally, move the top of the graph up by one pixel, offset for the last iteration again due to dithering.
	for (int y = 72; y < 80; ++y)
		for (int x = 0; x < 320; ++x)
		{
			_surfaces["BACK06.SCR"]->setPixel(x, y, _surfaces["BACK06.SCR"]->getPixel(x,y + (y == 79 ? 2 : 1)));
		}

	// now, let's adjust the battlescape info screen.
	// erase the old lines, no need to worry about dithering on this one.
	for (int y = 39; y < 199; y += 10)
		for (int x = 0; x < 169; ++x)
			_surfaces["UNIBORD.PCK"]->setPixel(x, y, _surfaces["UNIBORD.PCK"]->getPixel(x,30));
	// drawn new lines, use the bottom row of pixels as a basis
	for (int y = 190; y > 37; y -= 9)
		for (int x = 0; x < 169; ++x)
			_surfaces["UNIBORD.PCK"]->setPixel(x, y, _surfaces["UNIBORD.PCK"]->getPixel(x,199));
	// move the top of the graph down by eight pixels to erase the row we don't need (we actually created ~1.8 extra rows earlier)
	for (int y = 37; y > 29; --y)
		for (int x = 0; x < 320; ++x)
		{
			_surfaces["UNIBORD.PCK"]->setPixel(x, y, _surfaces["UNIBORD.PCK"]->getPixel(x,y-8));
			_surfaces["UNIBORD.PCK"]->setPixel(x, y-8, 0);
		}

	Log(LOG_INFO) << "Loading extra resources from ruleset...";
	std::ostringstream s;
	std::vector< std::pair<std::string, ExtraSprites *> >extraSprites = rules->getExtraSprites();
	for (std::vector< std::pair<std::string, ExtraSprites *> >::const_iterator i = extraSprites.begin(); i != extraSprites.end(); ++i)
	{
		std::string sheetName = i->first;
		ExtraSprites *spritePack = i->second;
		bool subdivision = (spritePack->getSubX() != 0 && spritePack->getSubY() != 0);
		if (spritePack->getSingleImage())
		{
			if (_surfaces.find(sheetName) == _surfaces.end())
			{
				Log(LOG_DEBUG) << "Creating new single image: " << sheetName;
				_surfaces[sheetName] = new Surface(spritePack->getWidth(), spritePack->getHeight());
			}
			else
			{
				Log(LOG_DEBUG) << "Adding/Replacing single image: " << sheetName;
				delete _surfaces[sheetName];
				_surfaces[sheetName] = new Surface(spritePack->getWidth(), spritePack->getHeight());
			}
			s.str("");
			s << CrossPlatform::getDataFile(spritePack->getSprites()->operator[](0));
			_surfaces[sheetName]->loadImage(s.str());
		}
		else
		{
			bool adding = false;
			if (_sets.find(sheetName) == _sets.end())
			{
				Log(LOG_DEBUG) << "Creating new surface set: " << sheetName;
				adding = true;
				 if (subdivision)
				 {
					_sets[sheetName] = new SurfaceSet(spritePack->getSubX(), spritePack->getSubY());
				 }
				 else
				 {
					_sets[sheetName] = new SurfaceSet(spritePack->getWidth(), spritePack->getHeight());
				 }
			}
			else
			{
				Log(LOG_DEBUG) << "Adding/Replacing items in surface set: " << sheetName;
			}
			
			if (subdivision)
			{
				int frames = (spritePack->getWidth() / spritePack->getSubX())*(spritePack->getHeight() / spritePack->getSubY());
				Log(LOG_DEBUG) << "Subdividing into " << frames << " frames.";
			}

			for (std::map<int, std::string>::iterator j = spritePack->getSprites()->begin(); j != spritePack->getSprites()->end(); ++j)
			{
				int startFrame = j->first;
				std:: string fileName = j->second;
				s.str("");
				if (fileName.substr(fileName.length() - 1, 1) == "/")
				{
					Log(LOG_DEBUG) << "Loading surface set from folder: " << fileName << " starting at frame: " << startFrame;
					int offset = startFrame;
					std::ostringstream folder;
					folder << CrossPlatform::getDataFolder(fileName);
					std::vector<std::string> contents = CrossPlatform::getFolderContents(folder.str());
					for (std::vector<std::string>::iterator k = contents.begin();
						k != contents.end(); ++k)
					{
						if (!isImageFile((*k).substr((*k).length() -4, (*k).length())))
							continue;
						try
						{
							s.str("");
							s << folder.str() << CrossPlatform::getDataFile(*k);
							if (_sets[sheetName]->getFrame(offset))
							{
								Log(LOG_DEBUG) << "Replacing frame: " << offset;
								_sets[sheetName]->getFrame(offset)->loadImage(s.str());
							}
							else
							{
								if (adding)
								{
									_sets[sheetName]->addFrame(offset)->loadImage(s.str());
								}
								else
								{
									Log(LOG_DEBUG) << "Adding frame: " << offset + spritePack->getModIndex();
									_sets[sheetName]->addFrame(offset + spritePack->getModIndex())->loadImage(s.str());
								}
							}
							offset++;
						}
						catch (Exception &e)
						{
							Log(LOG_WARNING) << e.what();
						}
					}
				}
				else
				{
					if (spritePack->getSubX() == 0 && spritePack->getSubY() == 0)
					{
						s << CrossPlatform::getDataFile(fileName);
						if (_sets[sheetName]->getFrame(startFrame))
						{
							Log(LOG_DEBUG) << "Replacing frame: " << startFrame;
							_sets[sheetName]->getFrame(startFrame)->loadImage(s.str());
						}
						else
						{
							Log(LOG_DEBUG) << "Adding frame: " << startFrame << ", using index: " << startFrame + spritePack->getModIndex();
							_sets[sheetName]->addFrame(startFrame + spritePack->getModIndex())->loadImage(s.str());
						}
					}
					else
					{
						Surface *temp = new Surface(spritePack->getWidth(), spritePack->getHeight());
						s.str("");
						s << CrossPlatform::getDataFile(spritePack->getSprites()->operator[](startFrame));
						temp->loadImage(s.str());
						int xDivision = spritePack->getWidth() / spritePack->getSubX();
						int yDivision = spritePack->getHeight() / spritePack->getSubY();
						int offset = startFrame;

						for (int y = 0; y != yDivision; ++y)
						{
							for (int x = 0; x != xDivision; ++x)
							{
								if (_sets[sheetName]->getFrame(offset))
								{
									Log(LOG_DEBUG) << "Replacing frame: " << offset;
									_sets[sheetName]->getFrame(offset)->clear();
									// for some reason regular blit() doesn't work here how i want it, so i use this function instead.
									temp->blitNShade(_sets[sheetName]->getFrame(offset), 0 - (x * spritePack->getSubX()), 0 - (y * spritePack->getSubY()), 0);
								}
								else
								{
									if (adding)
									{
										// for some reason regular blit() doesn't work here how i want it, so i use this function instead.
										temp->blitNShade(_sets[sheetName]->addFrame(offset), 0 - (x * spritePack->getSubX()), 0 - (y * spritePack->getSubY()), 0);
									}
									else
									{
										Log(LOG_DEBUG) << "Adding frame: " << offset + spritePack->getModIndex();
										// for some reason regular blit() doesn't work here how i want it, so i use this function instead.
										temp->blitNShade(_sets[sheetName]->addFrame(offset + spritePack->getModIndex()), 0 - (x * spritePack->getSubX()), 0 - (y * spritePack->getSubY()), 0);
									}
								}
								++offset;
							}
						}
						delete temp;
					}
				}
			}
		}
	}

	// copy constructor doesn't like doing this directly, so let's make a second handobs file the old fashioned way.
	// handob2 is used for all the left handed sprites.
	_sets["HANDOB2.PCK"] = new SurfaceSet(_sets["HANDOB.PCK"]->getWidth(), _sets["HANDOB.PCK"]->getHeight());
	std::map<int, Surface*> *handob = _sets["HANDOB.PCK"]->getFrames();
	for (std::map<int, Surface*>::const_iterator i = handob->begin(); i != handob->end(); ++i)
	{
		Surface *surface1 = _sets["HANDOB2.PCK"]->addFrame(i->first);
		Surface *surface2 = i->second;
		surface1->setPalette(surface2->getPalette());
		surface2->blit(surface1);
	}
	
	std::vector< std::pair<std::string, ExtraSounds *> >extraSounds = rules->getExtraSounds();
	for (std::vector< std::pair<std::string, ExtraSounds *> >::const_iterator i = extraSounds.begin(); i != extraSounds.end(); ++i)
	{
		std::string setName = i->first;
		ExtraSounds *soundPack = i->second;
		if (_sounds.find(setName) == _sounds.end())
		{
			Log(LOG_DEBUG) << "Creating new sound set: " << setName << ", this will likely have no in-game use.";
			_sounds[setName] = new SoundSet();
		}
		else Log(LOG_DEBUG) << "Adding/Replacing items in sound set: " << setName;
		for (std::map<int, std::string>::iterator j = soundPack->getSounds()->begin(); j != soundPack->getSounds()->end(); ++j)
		{
			int startSound = j->first;
			std::string fileName = j->second;
			s.str("");
			if (fileName.substr(fileName.length() - 1, 1) == "/")
			{
				Log(LOG_DEBUG) << "Loading sound set from folder: " << fileName << " starting at index: " << startSound;
				int offset = startSound;
				std::ostringstream folder;
				folder << CrossPlatform::getDataFolder(fileName);
				std::vector<std::string> contents = CrossPlatform::getFolderContents(folder.str());
				for (std::vector<std::string>::iterator k = contents.begin();
					k != contents.end(); ++k)
				{
					try
					{
						s.str("");
						s << folder.str() << CrossPlatform::getDataFile(*k);
						if (_sounds[setName]->getSound(offset))
						{
							_sounds[setName]->getSound(offset)->load(s.str());
						}
						else
						{
							_sounds[setName]->addSound(offset + soundPack->getModIndex())->load(s.str());
						}
						offset++;
					}
					catch (Exception &e)
					{
						Log(LOG_WARNING) << e.what();
					}
				}
			}
			else
			{
				s << CrossPlatform::getDataFile(fileName);
				if (_sounds[setName]->getSound(startSound))
				{
					Log(LOG_DEBUG) << "Replacing index: " << startSound;
					_sounds[setName]->getSound(startSound)->load(s.str());
				}
				else
				{
					Log(LOG_DEBUG) << "Adding index: " << startSound;
					_sounds[setName]->addSound(startSound + soundPack->getModIndex())->load(s.str());
				}
			}
		}
	}
}

/**
 *
 */
XcomResourcePack::~XcomResourcePack()
{
}

/**
 * Loads the resources required by the Battlescape.
 */
void XcomResourcePack::loadBattlescapeResources()
{
	// Load Battlescape ICONS
	std::ostringstream s;
	s << "UFOGRAPH/" << "SPICONS.DAT";
	_sets["SPICONS.DAT"] = new SurfaceSet(32, 24);
	_sets["SPICONS.DAT"]->loadDat(CrossPlatform::getDataFile(s.str()));

	s.str("");
	std::ostringstream s2;
	s << "UFOGRAPH/" << "CURSOR.PCK";
	s2 << "UFOGRAPH/" << "CURSOR.TAB";
	_sets["CURSOR.PCK"] = new SurfaceSet(32, 40);
	_sets["CURSOR.PCK"]->loadPck(CrossPlatform::getDataFile(s.str()), CrossPlatform::getDataFile(s2.str()));

	s.str("");
	s2.str("");
	s << "UFOGRAPH/" << "SMOKE.PCK";
	s2 << "UFOGRAPH/" << "SMOKE.TAB";
	_sets["SMOKE.PCK"] = new SurfaceSet(32, 40);
	_sets["SMOKE.PCK"]->loadPck(CrossPlatform::getDataFile(s.str()), CrossPlatform::getDataFile(s2.str()));
	
	s.str("");
	s2.str("");
	s << "UFOGRAPH/" << "HIT.PCK";
	s2 << "UFOGRAPH/" << "HIT.TAB";
	_sets["HIT.PCK"] = new SurfaceSet(32, 40);
	_sets["HIT.PCK"]->loadPck(CrossPlatform::getDataFile(s.str()), CrossPlatform::getDataFile(s2.str()));

	s.str("");
	s2.str("");
	s << "UFOGRAPH/" << "X1.PCK";
	s2 << "UFOGRAPH/" << "X1.TAB";
	_sets["X1.PCK"] = new SurfaceSet(128, 64);
	_sets["X1.PCK"]->loadPck(CrossPlatform::getDataFile(s.str()), CrossPlatform::getDataFile(s2.str()));

	s.str("");
	_sets["MEDIBITS.DAT"] = new SurfaceSet(52, 58);
	s << "UFOGRAPH/" << "MEDIBITS.DAT";
	_sets["MEDIBITS.DAT"]->loadDat (CrossPlatform::getDataFile(s.str()));

	s.str("");
	_sets["DETBLOB.DAT"] = new SurfaceSet(16, 16);
	s << "UFOGRAPH/" << "DETBLOB.DAT";
	_sets["DETBLOB.DAT"]->loadDat (CrossPlatform::getDataFile(s.str()));

	// Load Battlescape Terrain (only blacks are loaded, others are loaded just in time)
	std::string bsets[] = {"BLANKS.PCK"};

	for (size_t i = 0; i < sizeof(bsets)/sizeof(bsets[0]); ++i)
	{
		std::ostringstream s;
		s << "TERRAIN/" << bsets[i];
		std::string tab = CrossPlatform::noExt(bsets[i]) + ".TAB";
		std::ostringstream s2;
		s2 << "TERRAIN/" << tab;
		_sets[bsets[i]] = new SurfaceSet(32, 40);
		_sets[bsets[i]]->loadPck(CrossPlatform::getDataFile(s.str()), CrossPlatform::getDataFile(s2.str()));
	}

	// Load Battlescape units
	std::string units = CrossPlatform::getDataFolder("UNITS/");
	std::vector<std::string> usets = CrossPlatform::getFolderContents(units, "PCK");
	for (std::vector<std::string>::iterator i = usets.begin(); i != usets.end(); ++i)
	{
		std::string path = units + *i;
		std::string tab = CrossPlatform::getDataFile("UNITS/" + CrossPlatform::noExt(*i) + ".TAB");
		std::transform(i->begin(), i->end(), i->begin(), toupper);
		if (*i != "BIGOBS.PCK")
			_sets[*i] = new SurfaceSet(32, 40);
		else
			_sets[*i] = new SurfaceSet(32, 48);
		_sets[*i]->loadPck(path, tab);
	}
	// incomplete chryssalid set: 1.0 data: stop loading.
	if (_sets.find("CHRYS.PCK") != _sets.end() && !_sets["CHRYS.PCK"]->getFrame(225))
	{
		Log(LOG_FATAL) << "Version 1.0 data detected";
		throw Exception("Invalid CHRYS.PCK, please patch your X-COM data to the latest version");
	}
	s.str("");
	s << "GEODATA/" << "LOFTEMPS.DAT";
	MapDataSet::loadLOFTEMPS(CrossPlatform::getDataFile(s.str()), &_voxelData);

	std::string scrs[] = {"TAC00.SCR"};

	for (size_t i = 0; i < sizeof(scrs)/sizeof(scrs[0]); ++i)
	{
		std::ostringstream s;
		s << "UFOGRAPH/" << scrs[i];
		_surfaces[scrs[i]] = new Surface(320, 200);
		_surfaces[scrs[i]]->loadScr(CrossPlatform::getDataFile(s.str()));
	}
	

	std::string lbms[] = {"D0.LBM",
						  "D1.LBM",
						  "D2.LBM",
						  "D2.LBM"};
	std::string pals[] = {"PAL_BATTLESCAPE",
						  "PAL_BATTLESCAPE_1",
						  "PAL_BATTLESCAPE_2",
						  "PAL_BATTLESCAPE_3"};

	SDL_Color backPal[] =  {{0, 5, 4, 255},
							{0, 10, 34, 255},
							{2, 9, 24, 255},
							{2, 0, 24, 255}};

	for (size_t i = 0; i < sizeof(lbms)/sizeof(lbms[0]); ++i)
	{
		std::ostringstream s;
		s << "UFOGRAPH/" << lbms[i];
		if (CrossPlatform::fileExists(CrossPlatform::getDataFile(s.str())))
		{
			if (!i)
			{
				delete _palettes["PAL_BATTLESCAPE"];
			}
			Surface *tempSurface = new Surface(1, 1);
			tempSurface->loadImage(CrossPlatform::getDataFile(s.str()));
			_palettes[pals[i]] = new Palette();
			SDL_Color *colors = tempSurface->getPalette();
			colors[255] = backPal[i];
			_palettes[pals[i]]->setColors(colors, 256);
			delete tempSurface;
		}
	}

	std::string spks[] = {"TAC01.SCR",
						  "DETBORD.PCK",
						  "DETBORD2.PCK",
						  "ICONS.PCK",
						  "MEDIBORD.PCK",
						  "SCANBORD.PCK",
						  "UNIBORD.PCK"};

	for (size_t i = 0; i < sizeof(spks)/sizeof(spks[0]); ++i)
	{
		std::ostringstream s;
		s << "UFOGRAPH/" << spks[i];
		if (CrossPlatform::fileExists(CrossPlatform::getDataFile(s.str())))
		{
			_surfaces[spks[i]] = new Surface(320, 200);
			_surfaces[spks[i]]->loadSpk(CrossPlatform::getDataFile(s.str()));
		}
	}

	
	std::string ufograph = CrossPlatform::getDataFolder("UFOGRAPH/");
	std::vector<std::string> bdys = CrossPlatform::getFolderContents(ufograph, "BDY");
	for (std::vector<std::string>::iterator i = bdys.begin(); i != bdys.end(); ++i)
	{
		std::string path = ufograph + *i;
		std::transform(i->begin(), i->end(), i->begin(), toupper);
		*i = (*i).substr(0, (*i).length() - 3);
		if ((*i).substr(0, 3) == "MAN")
		{
			*i = *i + "SPK";
		}
		else if (*i == "TAC01.")
		{
			*i = *i + "SCR";
		}
		else
		{
			*i = *i + "PCK";
		}
		_surfaces[*i] = new Surface(320, 200);
		_surfaces[*i]->loadBdy(path);
	}

	// Load Battlescape inventory
	std::vector<std::string> invs = CrossPlatform::getFolderContents(ufograph, "SPK");
	for (std::vector<std::string>::iterator i = invs.begin(); i != invs.end(); ++i)
	{
		std::string path = ufograph + *i;
		std::transform(i->begin(), i->end(), i->begin(), toupper);
		_surfaces[*i] = new Surface(320, 200);
		_surfaces[*i]->loadSpk(path);
	}

	//"fix" of hair color of male personal armor
	if (Options::battleHairBleach)
	{
		if (_sets.find("XCOM_1.PCK") != _sets.end())
		{
			SurfaceSet *xcom_1 = _sets["XCOM_1.PCK"];

			for (int i = 0; i < 16; ++i)
			{
				//chest frame
				Surface *surf = xcom_1->getFrame(4 * 8 + i);
				ShaderMove<Uint8> head = ShaderMove<Uint8>(surf);
				GraphSubset dim = head.getBaseDomain();
				surf->lock();
				dim.beg_y = 6;
				dim.end_y = 9;
				head.setDomain(dim);
				ShaderDraw<HairBleach>(head, ShaderScalar<Uint8>(HairBleach::Face + 5));
				dim.beg_y = 9;
				dim.end_y = 10;
				head.setDomain(dim);
				ShaderDraw<HairBleach>(head, ShaderScalar<Uint8>(HairBleach::Face + 6));
				surf->unlock();
			}

			for (int i = 0; i < 3; ++i)
			{
				//fall frame
				Surface *surf = xcom_1->getFrame(264 + i);
				ShaderMove<Uint8> head = ShaderMove<Uint8>(surf);
				GraphSubset dim = head.getBaseDomain();
				dim.beg_y = 0;
				dim.end_y = 24;
				dim.beg_x = 11;
				dim.end_x = 20;
				head.setDomain(dim);
				surf->lock();
				ShaderDraw<HairBleach>(head, ShaderScalar<Uint8>(HairBleach::Face + 6));
				surf->unlock();
			}
		}
	}
}

/**
 * Determines if an image file is an acceptable format for the game.
 * @param extension Image file extension.
 * @return True/false
 */
bool XcomResourcePack::isImageFile(std::string extension)
{
	std::transform(extension.begin(), extension.end(), extension.begin(), toupper);

	return (
		// arbitrary limitation: let's not use these ones (although they're officially supported by sdl)
		/*
		extension == ".ICO" ||
		extension == ".CUR" ||
		extension == ".PNM" ||
		extension == ".PPM" ||
		extension == ".PGM" ||
		extension == ".PBM" ||
		extension == ".XPM" ||
		extension == "ILBM" ||
		// excluding jpeg to avoid inevitable issues due to compression
		extension == ".JPG" ||
		extension == "JPEG" ||
		*/
		extension == ".BMP" ||
		extension == ".LBM" ||
		extension == ".IFF" ||
		extension == ".PCX" ||
		extension == ".GIF" ||
		extension == ".PNG" ||
		extension == ".TGA" ||
		extension == ".TIF" ||
		extension == "TIFF");
}

/**
 * Loads the specified music file format.
 * @param fmt Format of the music.
 * @param file Filename of the music.
 * @param track Track number of the music, if stored in a CAT.
 * @param volume Volume modifier of the music, if stored in a CAT.
 * @param adlibcat Pointer to ADLIB.CAT if available.
 * @param aintrocat Pointer to AINTRO.CAT if available.
 * @param gmcat Pointer to GM.CAT if available.
 * @return Pointer to the music file, or NULL if it couldn't be loaded.
 */
Music *XcomResourcePack::loadMusic(MusicFormat fmt, const std::string &file, int track, float volume, CatFile *adlibcat, CatFile *aintrocat, GMCatFile *gmcat)
{
	/* MUSIC_AUTO, MUSIC_FLAC, MUSIC_OGG, MUSIC_MP3, MUSIC_MOD, MUSIC_WAV, MUSIC_ADLIB, MUSIC_MIDI */
	static const std::string exts[] = {"", "flac", "ogg", "mp3", "mod", "wav", "", "mid"};
	Music *music = 0;
	try
	{
		// Try Adlib music
		if (fmt == MUSIC_ADLIB)
		{
			if (adlibcat && Options::audioBitDepth == 16)
			{
				music = new AdlibMusic(volume);
				if (track < adlibcat->getAmount())
				{
					music->load(adlibcat->load(track, true), adlibcat->getObjectSize(track));
				}
				// separate intro music
				else if (aintrocat)
				{
					track -= adlibcat->getAmount();
					music->load(aintrocat->load(track, true), aintrocat->getObjectSize(track));
				}
			}
		}
		// Try MIDI music
		else if (fmt == MUSIC_MIDI)
		{
			// DOS MIDI
			if (gmcat)
			{
				music = gmcat->loadMIDI(track);
			}
			// Windows MIDI
			else
			{
				std::ostringstream s;
				s << "SOUND/" << file << "." << exts[fmt];
				if (CrossPlatform::fileExists(CrossPlatform::getDataFile(s.str())))
				{
					music = new Music();
					music->load(CrossPlatform::getDataFile(s.str()));
				}
			}
		}
		// Try digital tracks
		else
		{
			std::ostringstream s;
			s << "SOUND/" << file << "." << exts[fmt];
			if (CrossPlatform::fileExists(CrossPlatform::getDataFile(s.str())))
			{
				music = new Music();
				music->load(CrossPlatform::getDataFile(s.str()));
			}
		}
	}
	catch (Exception &e)
	{
		Log(LOG_INFO) << e.what();
		if (music) delete music;
		music = 0;
	}
	return music;
}

}
