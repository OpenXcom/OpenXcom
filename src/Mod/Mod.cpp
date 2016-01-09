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
#include "Mod.h"
#include <algorithm>
#include <sstream>
#include <climits>
#include "../Engine/CrossPlatform.h"
#include "../Engine/FileMap.h"
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
#include "MapDataSet.h"
#include "RuleMusic.h"
#include "../Engine/ShaderDraw.h"
#include "../Engine/ShaderMove.h"
#include "../Engine/Exception.h"
#include "../Engine/Logger.h"
#include "SoundDefinition.h"
#include "ExtraSprites.h"
#include "ExtraSounds.h"
#include "../Engine/AdlibMusic.h"
#include "../fmath.h"
#include "../Engine/RNG.h"
#include "../Engine/Options.h"
#include "../Battlescape/Pathfinding.h"
#include "SoldierNamePool.h"
#include "RuleCountry.h"
#include "RuleRegion.h"
#include "RuleBaseFacility.h"
#include "RuleCraft.h"
#include "RuleCraftWeapon.h"
#include "RuleItem.h"
#include "RuleUfo.h"
#include "RuleTerrain.h"
#include "MapScript.h"
#include "RuleSoldier.h"
#include "RuleCommendations.h"
#include "AlienRace.h"
#include "AlienDeployment.h"
#include "Armor.h"
#include "ArticleDefinition.h"
#include "RuleInventory.h"
#include "RuleResearch.h"
#include "RuleManufacture.h"
#include "ExtraStrings.h"
#include "RuleInterface.h"
#include "RuleMissionScript.h"
#include "../Geoscape/Globe.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Region.h"
#include "../Savegame/Base.h"
#include "../Savegame/Country.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/Craft.h"
#include "../Savegame/Transfer.h"
#include "../Ufopaedia/Ufopaedia.h"
#include "../Savegame/AlienStrategy.h"
#include "../Savegame/GameTime.h"
#include "../Savegame/SoldierDiary.h"
#include "UfoTrajectory.h"
#include "RuleAlienMission.h"
#include "MCDPatch.h"
#include "StatString.h"
#include "RuleGlobe.h"
#include "RuleVideo.h"
#include "RuleConverter.h"

namespace OpenXcom
{

int Mod::DOOR_OPEN;
int Mod::SLIDING_DOOR_OPEN;
int Mod::SLIDING_DOOR_CLOSE;
int Mod::SMALL_EXPLOSION;
int Mod::LARGE_EXPLOSION;
int Mod::EXPLOSION_OFFSET;
int Mod::SMOKE_OFFSET;
int Mod::UNDERWATER_SMOKE_OFFSET;
int Mod::ITEM_DROP;
int Mod::ITEM_THROW;
int Mod::ITEM_RELOAD;
int Mod::WALK_OFFSET;
int Mod::FLYING_SOUND;
int Mod::BUTTON_PRESS;
int Mod::WINDOW_POPUP[3];
int Mod::UFO_FIRE;
int Mod::UFO_HIT;
int Mod::UFO_CRASH;
int Mod::UFO_EXPLODE;
int Mod::INTERCEPTOR_HIT;
int Mod::INTERCEPTOR_EXPLODE;
int Mod::GEOSCAPE_CURSOR;
int Mod::BASESCAPE_CURSOR;
int Mod::BATTLESCAPE_CURSOR;
int Mod::UFOPAEDIA_CURSOR;
int Mod::GRAPHS_CURSOR;
int Mod::DAMAGE_RANGE;
int Mod::EXPLOSIVE_DAMAGE_RANGE;
int Mod::FIRE_DAMAGE_RANGE[2];
std::string Mod::DEBRIEF_MUSIC_GOOD;
std::string Mod::DEBRIEF_MUSIC_BAD;
int Mod::DIFFICULTY_COEFFICIENT[5];

void Mod::resetGlobalStatics()
{
	Mod::DOOR_OPEN = 3;
	Mod::SLIDING_DOOR_OPEN = 20;
	Mod::SLIDING_DOOR_CLOSE = 21;
	Mod::SMALL_EXPLOSION = 2;
	Mod::LARGE_EXPLOSION = 5;
	Mod::EXPLOSION_OFFSET = 0;
	Mod::SMOKE_OFFSET = 8;
	Mod::UNDERWATER_SMOKE_OFFSET = 0;
	Mod::ITEM_DROP = 38;
	Mod::ITEM_THROW = 39;
	Mod::ITEM_RELOAD = 17;
	Mod::WALK_OFFSET = 22;
	Mod::FLYING_SOUND = 15;
	Mod::BUTTON_PRESS = 0;
	Mod::WINDOW_POPUP[0] = 1;
	Mod::WINDOW_POPUP[1] = 2;
	Mod::WINDOW_POPUP[2] = 3;
	Mod::UFO_FIRE = 8;
	Mod::UFO_HIT = 12;
	Mod::UFO_CRASH = 10;
	Mod::UFO_EXPLODE = 11;
	Mod::INTERCEPTOR_HIT = 10;
	Mod::INTERCEPTOR_EXPLODE = 13;
	Mod::GEOSCAPE_CURSOR = 252;
	Mod::BASESCAPE_CURSOR = 252;
	Mod::BATTLESCAPE_CURSOR = 144;
	Mod::UFOPAEDIA_CURSOR = 252;
	Mod::GRAPHS_CURSOR = 252;
	Mod::DAMAGE_RANGE = 100;
	Mod::EXPLOSIVE_DAMAGE_RANGE = 50;
	Mod::FIRE_DAMAGE_RANGE[0] = 5;
	Mod::FIRE_DAMAGE_RANGE[1] = 10;
	Mod::DEBRIEF_MUSIC_GOOD = "GMMARS";
	Mod::DEBRIEF_MUSIC_BAD = "GMMARS";

	Globe::OCEAN_COLOR = Palette::blockOffset(12);
	Globe::COUNTRY_LABEL_COLOR = 239;
	Globe::LINE_COLOR = 162;
	Globe::CITY_LABEL_COLOR = 138;
	Globe::BASE_LABEL_COLOR = 133;

	TextButton::soundPress = 0;

	Window::soundPopup[0] = 0;
	Window::soundPopup[1] = 0;
	Window::soundPopup[2] = 0;

	Pathfinding::red = 3;
	Pathfinding::yellow = 10;
	Pathfinding::green = 4;

	DIFFICULTY_COEFFICIENT[0] = 0;
	DIFFICULTY_COEFFICIENT[1] = 1;
	DIFFICULTY_COEFFICIENT[2] = 2;
	DIFFICULTY_COEFFICIENT[3] = 3;
	DIFFICULTY_COEFFICIENT[4] = 4;
}

/**
 * Creates an empty mod.
 */
Mod::Mod() : _costEngineer(0), _costScientist(0), _timePersonnel(0), _initialFunding(0), _turnAIUseGrenade(3), _turnAIUseBlaster(3), _startingTime(6, 1, 1, 1999, 12, 0, 0), _facilityListOrder(0), _craftListOrder(0), _itemListOrder(0), _researchListOrder(0),  _manufactureListOrder(0), _ufopaediaListOrder(0), _invListOrder(0), _modOffset(0)
{
	_muteMusic = new Music();
	_muteSound = new Sound();
	_globe = new RuleGlobe();
	_converter = new RuleConverter();
	_statAdjustment[0].aimAndArmorMultiplier = 0.5;
	_statAdjustment[0].growthMultiplier = 0;
	for (int i = 1; i != 5; ++i)
	{
		_statAdjustment[i].aimAndArmorMultiplier = 1.0;
		_statAdjustment[i].growthMultiplier = i;
	}
}

/**
 * Deletes all the mod data from memory.
 */
Mod::~Mod()
{
	delete _muteMusic;
	delete _muteSound;
	delete _globe;
	delete _converter;
	for (std::map<std::string, Font*>::iterator i = _fonts.begin(); i != _fonts.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, SurfaceSet*>::iterator i = _sets.begin(); i != _sets.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, Palette*>::iterator i = _palettes.begin(); i != _palettes.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, Music*>::iterator i = _musics.begin(); i != _musics.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, SoundSet*>::iterator i = _sounds.begin(); i != _sounds.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleCountry*>::iterator i = _countries.begin(); i != _countries.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleRegion*>::iterator i = _regions.begin(); i != _regions.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleBaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleCraft*>::iterator i = _crafts.begin(); i != _crafts.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleCraftWeapon*>::iterator i = _craftWeapons.begin(); i != _craftWeapons.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleItem*>::iterator i = _items.begin(); i != _items.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleUfo*>::iterator i = _ufos.begin(); i != _ufos.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleTerrain*>::iterator i = _terrains.begin(); i != _terrains.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, MapDataSet*>::iterator i = _mapDataSets.begin(); i != _mapDataSets.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleSoldier*>::iterator i = _soldiers.begin(); i != _soldiers.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, Unit*>::iterator i = _units.begin(); i != _units.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, AlienRace*>::iterator i = _alienRaces.begin(); i != _alienRaces.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, AlienDeployment*>::iterator i = _alienDeployments.begin(); i != _alienDeployments.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, Armor*>::iterator i = _armors.begin(); i != _armors.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, ArticleDefinition*>::iterator i = _ufopaediaArticles.begin(); i != _ufopaediaArticles.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleInventory*>::iterator i = _invs.begin(); i != _invs.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleResearch *>::const_iterator i = _research.begin(); i != _research.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleManufacture *>::const_iterator i = _manufacture.begin(); i != _manufacture.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, UfoTrajectory *>::const_iterator i = _ufoTrajectories.begin(); i != _ufoTrajectories.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleAlienMission *>::const_iterator i = _alienMissions.begin(); i != _alienMissions.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, MCDPatch *>::const_iterator i = _MCDPatches.begin(); i != _MCDPatches.end(); ++i)
	{
		delete i->second;
	}
	for (std::vector< std::pair<std::string, ExtraSprites *> >::const_iterator i = _extraSprites.begin(); i != _extraSprites.end(); ++i)
	{
		delete i->second;
	}
	for (std::vector< std::pair<std::string, ExtraSounds *> >::const_iterator i = _extraSounds.begin(); i != _extraSounds.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, ExtraStrings *>::const_iterator i = _extraStrings.begin(); i != _extraStrings.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleInterface *>::const_iterator i = _interfaces.begin(); i != _interfaces.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, std::vector<MapScript*> >::iterator i = _mapScripts.begin(); i != _mapScripts.end(); ++i)
	{
		for (std::vector<MapScript*>::iterator j = (*i).second.begin(); j != (*i).second.end(); ++j)
		{
			delete *j;
		}
	}
	for (std::map<std::string, RuleVideo *>::const_iterator i = _videos.begin(); i != _videos.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleMusic *>::const_iterator i = _musicDefs.begin(); i != _musicDefs.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleMissionScript*>::const_iterator i = _missionScripts.begin(); i != _missionScripts.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, SoundDefinition*>::const_iterator i = _soundDefs.begin(); i != _soundDefs.end(); ++i)
	{
		delete i->second;
	}
	for (std::vector<StatString*>::const_iterator i = _statStrings.begin(); i != _statStrings.end(); ++i)
	{
		delete (*i);
	}
	for (std::map<std::string, RuleCommendations *>::const_iterator i = _commendations.begin(); i != _commendations.end(); ++i)
	{
		delete i->second;
	}
}

/**
 * Gets a specific rule element by ID.
 * @param id String ID of the rule element.
 * @param name Human-readable name of the rule type.
 * @param map Map associated to the rule type.
 * @return Pointer to the rule element, or NULL if not found.
 */
template <typename T>
T *Mod::getRule(const std::string &id, const std::string &name, const std::map<std::string, T*> &map) const
{
	if (id.empty())
	{
		return 0;
	}
	typename std::map<std::string, T*>::const_iterator i = map.find(id);
	if (map.end() != i)
	{
		return i->second;
	}
	else
	{
//		if (id != Armor::NONE)
//		{
//			Log(LOG_WARNING) << name << " " << id << " not found";
//		}
		return 0;
	}
}

/**
 * Returns a specific font from the mod.
 * @param name Name of the font.
 * @return Pointer to the font.
 */
Font *Mod::getFont(const std::string &name) const
{
	return getRule(name, "Font", _fonts);
}

/**
 * Returns a specific surface from the mod.
 * @param name Name of the surface.
 * @return Pointer to the surface.
 */
Surface *Mod::getSurface(const std::string &name) const
{
	return getRule(name, "Surface", _surfaces);
}

/**
 * Returns a specific surface set from the mod.
 * @param name Name of the surface set.
 * @return Pointer to the surface set.
 */
SurfaceSet *Mod::getSurfaceSet(const std::string &name) const
{
	return getRule(name, "Surface Set", _sets);
}

/**
 * Returns a specific music from the mod.
 * @param name Name of the music.
 * @return Pointer to the music.
 */
Music *Mod::getMusic(const std::string &name) const
{
	if (Options::mute)
	{
		return _muteMusic;
	}
	else
	{
		return getRule(name, "Music", _musics);
	}
}

/**
 * Returns a random music from the mod.
 * @param name Name of the music to pick from.
 * @return Pointer to the music.
 */
Music *Mod::getRandomMusic(const std::string &name) const
{
	if (Options::mute)
	{
		return _muteMusic;
	}
	else
	{
		std::vector<Music*> music;
		for (std::map<std::string, Music*>::const_iterator i = _musics.begin(); i != _musics.end(); ++i)
		{
			if (i->first.find(name) != std::string::npos)
			{
				music.push_back(i->second);
			}
		}
		if (music.empty())
		{
			return _muteMusic;
		}
		else
		{
			return music[RNG::seedless(0, music.size() - 1)];
		}
	}
}

/**
 * Plays the specified track if it's not already playing.
 * @param name Name of the music.
 * @param id Id of the music, 0 for random.
 */
void Mod::playMusic(const std::string &name, int id)
{
	if (!Options::mute && _playingMusic != name)
	{
		int loop = -1;
		// hacks
		if (!Options::musicAlwaysLoop && (name == "GMSTORY" || name == "GMWIN" || name == "GMLOSE"))
		{
			loop = 0;
		}

		Music *music = 0;
		if (id == 0)
		{
			music = getRandomMusic(name);
		}
		else
		{
			std::ostringstream ss;
			ss << name << id;
			music = getMusic(ss.str());
		}
		music->play(loop);
		if (music != _muteMusic)
		{
			_playingMusic = name;
		}
	}
}

/**
 * Returns a specific sound set from the mod.
 * @param name Name of the sound set.
 * @return Pointer to the sound set.
 */
SoundSet *Mod::getSoundSet(const std::string &name) const
{
	return getRule(name, "Sound Set", _sounds);
}

/**
 * Returns a specific sound from the mod.
 * @param set Name of the sound set.
 * @param sound ID of the sound.
 * @return Pointer to the sound.
 */
Sound *Mod::getSound(const std::string &set, unsigned int sound) const
{
	if (Options::mute)
	{
		return _muteSound;
	}
	else
	{
		SoundSet *ss = getSoundSet(set);
		if (ss != 0)
		{
			Sound *s = ss->getSound(sound);
			if (s == 0)
			{
				Log(LOG_ERROR) << "Sound " << sound << " in " << set << " not found";				
			}
			return s;
		}
		else
		{
			return 0;
		}
	}
}

/**
 * Returns a specific palette from the mod.
 * @param name Name of the palette.
 * @return Pointer to the palette.
 */
Palette *Mod::getPalette(const std::string &name) const
{
	return getRule(name, "Palette", _palettes);
}

/**
 * Changes the palette of all the graphics in the mod.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void Mod::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	for (std::map<std::string, Font*>::iterator i = _fonts.begin(); i != _fonts.end(); ++i)
	{
		i->second->getSurface()->setPalette(colors, firstcolor, ncolors);
	}
	for (std::map<std::string, Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
	{
		if (i->first.substr(i->first.length() - 3, i->first.length()) != "LBM")
			i->second->setPalette(colors, firstcolor, ncolors);
	}
	for (std::map<std::string, SurfaceSet*>::iterator i = _sets.begin(); i != _sets.end(); ++i)
	{
		i->second->setPalette(colors, firstcolor, ncolors);
	}
}

/**
 * Returns the list of voxeldata in the mod.
 * @return Pointer to the list of voxeldata.
 */
std::vector<Uint16> *Mod::getVoxelData()
{
	return &_voxelData;
}

/**
 * Returns a specific sound from either the land or underwater sound set.
 * @param depth the depth of the battlescape.
 * @param sound ID of the sound.
 * @return Pointer to the sound.
 */
Sound *Mod::getSoundByDepth(unsigned int depth, unsigned int sound) const
{
	if (depth == 0)
		return getSound("BATTLE.CAT", sound);
	else
		return getSound("BATTLE2.CAT", sound);
}

/**
 * Returns the list of color LUTs in the mod.
 * @return Pointer to the list of LUTs.
 */
const std::vector<std::vector<Uint8> > *Mod::getLUTs() const
{
	return &_transparencyLUTs;
}

/**
 * Returns the current mod-based offset for resources.
 * @return Mod offset.
 */
int Mod::getModOffset() const
{
	return _modOffset;
}

/**
 * Returns the appropriate mod-based offset for a sprite.
 * If the ID is bigger than the surfaceset contents, the mod offset is applied.
 * @param id Numeric ID of the sprite.
 * @param resource Name of the surfaceset to lookup.
 */
int Mod::getSpriteOffset(int sprite, const std::string& set) const
{
	std::map<std::string, SurfaceSet*>::const_iterator i = _sets.find(set);
	if (i != _sets.end() && sprite >= (int)i->second->getTotalFrames())
		return sprite + _modOffset;
	else
		return sprite;
}

/**
 * Returns the appropriate mod-based offset for a sound.
 * If the ID is bigger than the soundset contents, the mod offset is applied.
 * @param id Numeric ID of the sound.
 * @param resource Name of the soundset to lookup.
 */
int Mod::getSoundOffset(int sound, const std::string& set) const
{
	std::map<std::string, SoundSet*>::const_iterator i = _sounds.find(set);
	if (i != _sounds.end() && sound >= (int)i->second->getTotalSounds())
		return sound + _modOffset;
	else
		return sound;
}

/**
 * Loads a list of mods specified in the options.
 * @param mods List of <modId, rulesetFiles> pairs.
 */
void Mod::loadAll(const std::vector< std::pair< std::string, std::vector<std::string> > > &mods)
{
	for (size_t i = 0; mods.size() > i; ++i)
	{
		try
		{
			loadMod(mods[i].second, i);
		}
		catch (Exception &e)
		{
			const std::string &modId = mods[i].first;
			Log(LOG_WARNING) << "disabling mod with invalid ruleset: " << modId;
			std::vector<std::pair<std::string, bool> >::iterator it =
				std::find(Options::mods.begin(), Options::mods.end(),
					std::pair<std::string, bool>(modId, true));
			if (it == Options::mods.end())
			{
				Log(LOG_ERROR) << "cannot find broken mod in mods list: " << modId;
				Log(LOG_ERROR) << "clearing mods list";
				Options::mods.clear();
			}
			else
			{
				it->second = false;
			}
			Options::save();

			throw Exception("failed to load '" +
				Options::getModInfos().at(modId).getName() +
				"'; mod disabled for next startup\n" +
				e.what());
		}
	}
	sortLists();
	loadExtraResources();
	modResources();
}

/**
 * Loads a list of rulesets from YAML files for the mod at the specified index. The first
 * mod loaded should be the master at index 0, then 1, and so on.
 * @param rulesetFiles List of rulesets to load.
 * @param modIdx Mod index number.
 */
void Mod::loadMod(const std::vector<std::string> &rulesetFiles, size_t modIdx)
{
	_modOffset = 1000 * modIdx;

	for (std::vector<std::string>::const_iterator i = rulesetFiles.begin(); i != rulesetFiles.end(); ++i)
	{
		Log(LOG_VERBOSE) << "- " << *i;
		try
		{
			loadFile(*i);
		}
		catch (YAML::Exception &e)
		{
			throw Exception((*i) + ": " + std::string(e.what()));
		}
	}

	// these need to be validated, otherwise we're gonna get into some serious trouble down the line.
	// it may seem like a somewhat arbitrary limitation, but there is a good reason behind it.
	// i'd need to know what results are going to be before they are formulated, and there's a heirarchical structure to
	// the order in which variables are determined for a mission, and the order is DIFFERENT for regular missions vs
	// missions that spawn a mission site. where normally we pick a region, then a mission based on the weights for that region.
	// a terror-type mission picks a mission type FIRST, then a region based on the criteria defined by the mission.
	// there is no way i can conceive of to reconcile this difference to allow mixing and matching,
	// short of knowing the results of calls to the RNG before they're determined.
	// the best solution i can come up with is to disallow it, as there are other ways to acheive what this would amount to anyway,
	// and they don't require time travel. - Warboy
	for (std::map<std::string, RuleMissionScript*>::iterator i = _missionScripts.begin(); i != _missionScripts.end(); ++i)
	{
		RuleMissionScript *rule = (*i).second;
		std::set<std::string> missions = rule->getAllMissionTypes();
		if (!missions.empty())
		{
			std::set<std::string>::const_iterator j = missions.begin();
			bool isSiteType = getAlienMission(*j) && getAlienMission(*j)->getObjective() == OBJECTIVE_SITE;
			rule->setSiteType(isSiteType);
			for (;j != missions.end(); ++j)
			{
				if (getAlienMission(*j) && (getAlienMission(*j)->getObjective() == OBJECTIVE_SITE) != isSiteType)
				{
					throw Exception("Error with MissionScript: " + (*i).first + " cannot mix terror/non-terror missions in a single command, so sayeth the wise Alaundo."); 
				}
			}
		}
	}

	// instead of passing a pointer to the region load function and moving the alienMission loading before region loading
	// and sanitizing there, i'll sanitize here, i'm sure this sanitation will grow, and will need to be refactored into
	// its own function at some point, but for now, i'll put it here next to the missionScript sanitation, because it seems
	// the logical place for it, given that this sanitation is required as a result of moving all terror mission handling
	// into missionScripting behaviour. apologies to all the modders that will be getting errors and need to adjust their
	// rulesets, but this will save you weird errors down the line.
	for (std::map<std::string, RuleRegion*>::iterator i = _regions.begin(); i != _regions.end(); ++i)
	{
		// bleh, make copies, const correctness kinda screwed me here.
		WeightedOptions weights = (*i).second->getAvailableMissions();
		std::vector<std::string> names = weights.getNames();
		for (std::vector<std::string>::iterator j = names.begin(); j != names.end(); ++j)
		{
			if (getAlienMission(*j)->getObjective() == OBJECTIVE_SITE)
			{
				throw Exception("Error with MissionWeights: Region: " + (*i).first + " has " + *j + " listed. Terror mission can only be invoked via missionScript, so sayeth the Spider Queen."); 
			}
		}
	}

	if (modIdx == 0)
	{
		loadVanillaResources();
	}
}

/**
 * Loads a ruleset's contents from a YAML file.
 * Rules that match pre-existing rules overwrite them.
 * @param filename YAML filename.
 */
void Mod::loadFile(const std::string &filename)
{
	YAML::Node doc = YAML::LoadFile(filename);

	for (YAML::const_iterator i = doc["countries"].begin(); i != doc["countries"].end(); ++i)
	{
		RuleCountry *rule = loadRule(*i, &_countries, &_countriesIndex);
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
	for (YAML::const_iterator i = doc["regions"].begin(); i != doc["regions"].end(); ++i)
	{
		RuleRegion *rule = loadRule(*i, &_regions, &_regionsIndex);
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
	for (YAML::const_iterator i = doc["facilities"].begin(); i != doc["facilities"].end(); ++i)
	{
		RuleBaseFacility *rule = loadRule(*i, &_facilities, &_facilitiesIndex);
		if (rule != 0)
		{
			_facilityListOrder += 100;
			rule->load(*i, this, _facilityListOrder);
		}
	}
	for (YAML::const_iterator i = doc["crafts"].begin(); i != doc["crafts"].end(); ++i)
	{
		RuleCraft *rule = loadRule(*i, &_crafts, &_craftsIndex);
		if (rule != 0)
		{
			_craftListOrder += 100;
			rule->load(*i, this, _craftListOrder);
		}
	}
	for (YAML::const_iterator i = doc["craftWeapons"].begin(); i != doc["craftWeapons"].end(); ++i)
	{
		RuleCraftWeapon *rule = loadRule(*i, &_craftWeapons, &_craftWeaponsIndex);
		if (rule != 0)
		{
			rule->load(*i, this);
		}
	}
	for (YAML::const_iterator i = doc["items"].begin(); i != doc["items"].end(); ++i)
	{
		RuleItem *rule = loadRule(*i, &_items, &_itemsIndex);
		if (rule != 0)
		{
			_itemListOrder += 100;
			rule->load(*i, this, _itemListOrder);
		}
	}
	for (YAML::const_iterator i = doc["ufos"].begin(); i != doc["ufos"].end(); ++i)
	{
		RuleUfo *rule = loadRule(*i, &_ufos, &_ufosIndex);
		if (rule != 0)
		{
			rule->load(*i, this);
		}
	}
	for (YAML::const_iterator i = doc["invs"].begin(); i != doc["invs"].end(); ++i)
	{
		RuleInventory *rule = loadRule(*i, &_invs, &_invsIndex, "id");
		if (rule != 0)
		{
			_invListOrder += 10;
			rule->load(*i, _invListOrder);
		}
	}
	for (YAML::const_iterator i = doc["terrains"].begin(); i != doc["terrains"].end(); ++i)
	{
		RuleTerrain *rule = loadRule(*i, &_terrains, &_terrainIndex, "name");
		if (rule != 0)
		{
			rule->load(*i, this);
		}
	}
	for (YAML::const_iterator i = doc["armors"].begin(); i != doc["armors"].end(); ++i)
	{
		Armor *rule = loadRule(*i, &_armors, &_armorsIndex);
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
	for (YAML::const_iterator i = doc["soldiers"].begin(); i != doc["soldiers"].end(); ++i)
	{
		RuleSoldier *rule = loadRule(*i, &_soldiers, &_soldiersIndex);
		if (rule != 0)
		{
			rule->load(*i, this);
		}
	}
	for (YAML::const_iterator i = doc["units"].begin(); i != doc["units"].end(); ++i)
	{
		Unit *rule = loadRule(*i, &_units);
		if (rule != 0)
		{
			rule->load(*i, this);
		}
	}
	for (YAML::const_iterator i = doc["alienRaces"].begin(); i != doc["alienRaces"].end(); ++i)
	{
		AlienRace *rule = loadRule(*i, &_alienRaces, &_aliensIndex, "id");
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
	for (YAML::const_iterator i = doc["alienDeployments"].begin(); i != doc["alienDeployments"].end(); ++i)
	{
		AlienDeployment *rule = loadRule(*i, &_alienDeployments, &_deploymentsIndex);
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
	for (YAML::const_iterator i = doc["research"].begin(); i != doc["research"].end(); ++i)
	{
		RuleResearch *rule = loadRule(*i, &_research, &_researchIndex, "name");
		if (rule != 0)
		{
			_researchListOrder += 100;
			rule->load(*i, _researchListOrder);
			if ((*i)["unlockFinalMission"].as<bool>(false))
			{
				_finalResearch = (*i)["name"].as<std::string>(_finalResearch);
			}
		}
	}
	for (YAML::const_iterator i = doc["manufacture"].begin(); i != doc["manufacture"].end(); ++i)
	{
		RuleManufacture *rule = loadRule(*i, &_manufacture, &_manufactureIndex, "name");
		if (rule != 0)
		{
			_manufactureListOrder += 100;
			rule->load(*i, _manufactureListOrder);
		}
	}
	for (YAML::const_iterator i = doc["ufopaedia"].begin(); i != doc["ufopaedia"].end(); ++i)
	{
		if ((*i)["id"])
		{
			std::string id = (*i)["id"].as<std::string>();
			ArticleDefinition *rule;
			if (_ufopaediaArticles.find(id) != _ufopaediaArticles.end())
			{
				rule = _ufopaediaArticles[id];
			}
			else
			{
				UfopaediaTypeId type = (UfopaediaTypeId)(*i)["type_id"].as<int>();
				switch (type)
				{
				case UFOPAEDIA_TYPE_CRAFT: rule = new ArticleDefinitionCraft(); break;
				case UFOPAEDIA_TYPE_CRAFT_WEAPON: rule = new ArticleDefinitionCraftWeapon(); break;
				case UFOPAEDIA_TYPE_VEHICLE: rule = new ArticleDefinitionVehicle(); break;
				case UFOPAEDIA_TYPE_ITEM: rule = new ArticleDefinitionItem(); break;
				case UFOPAEDIA_TYPE_ARMOR: rule = new ArticleDefinitionArmor(); break;
				case UFOPAEDIA_TYPE_BASE_FACILITY: rule = new ArticleDefinitionBaseFacility(); break;
				case UFOPAEDIA_TYPE_TEXTIMAGE: rule = new ArticleDefinitionTextImage(); break;
				case UFOPAEDIA_TYPE_TEXT: rule = new ArticleDefinitionText(); break;
				case UFOPAEDIA_TYPE_UFO: rule = new ArticleDefinitionUfo(); break;
				case UFOPAEDIA_TYPE_TFTD:
				case UFOPAEDIA_TYPE_TFTD_CRAFT:
				case UFOPAEDIA_TYPE_TFTD_CRAFT_WEAPON:
				case UFOPAEDIA_TYPE_TFTD_VEHICLE:
				case UFOPAEDIA_TYPE_TFTD_ITEM:
				case UFOPAEDIA_TYPE_TFTD_ARMOR:
				case UFOPAEDIA_TYPE_TFTD_BASE_FACILITY:
				case UFOPAEDIA_TYPE_TFTD_USO:
					rule = new ArticleDefinitionTFTD();
					break;
				default: rule = 0; break;
				}
				_ufopaediaArticles[id] = rule;
				_ufopaediaIndex.push_back(id);
			}
			_ufopaediaListOrder += 100;
			rule->load(*i, _ufopaediaListOrder);
			if (rule->section != UFOPAEDIA_NOT_AVAILABLE &&
				std::find(_ufopaediaCatIndex.begin(), _ufopaediaCatIndex.end(), rule->section) == _ufopaediaCatIndex.end())
			{
				_ufopaediaCatIndex.push_back(rule->section);
			}
		}
		else if ((*i)["delete"])
		{
			std::string type = (*i)["delete"].as<std::string>();
			std::map<std::string, ArticleDefinition*>::iterator i = _ufopaediaArticles.find(type);
			if (i != _ufopaediaArticles.end())
			{
				_ufopaediaArticles.erase(i);
			}
			std::vector<std::string>::iterator idx = std::find(_ufopaediaIndex.begin(), _ufopaediaIndex.end(), type);
			if (idx != _ufopaediaIndex.end())
			{
				_ufopaediaIndex.erase(idx);
			}
		}
	}
	// Bases can't be copied, so for savegame purposes we store the node instead
	YAML::Node base = doc["startingBase"];
	if (base)
	{
		for (YAML::const_iterator i = base.begin(); i != base.end(); ++i)
		{
			_startingBase[i->first.as<std::string>()] = YAML::Node(i->second);
		}
	}
	if (doc["startingTime"])
	{
		_startingTime.load(doc["startingTime"]);
	}
	_costEngineer = doc["costEngineer"].as<int>(_costEngineer);
	_costScientist = doc["costScientist"].as<int>(_costScientist);
	_timePersonnel = doc["timePersonnel"].as<int>(_timePersonnel);
	_initialFunding = doc["initialFunding"].as<int>(_initialFunding);
	_alienFuel = doc["alienFuel"].as<std::pair<std::string, int> >(_alienFuel);
	_fontName = doc["fontName"].as<std::string>(_fontName);
	_turnAIUseGrenade = doc["turnAIUseGrenade"].as<int>(_turnAIUseGrenade);
	_turnAIUseBlaster = doc["turnAIUseBlaster"].as<int>(_turnAIUseBlaster);
	if (doc["difficultyCoefficient"])
	{
		size_t num = 0;
		for (YAML::const_iterator i = doc["difficultyCoefficient"].begin(); i != doc["difficultyCoefficient"].end() && num < 5; ++i)
		{
			DIFFICULTY_COEFFICIENT[num] = (*i).as<int>(DIFFICULTY_COEFFICIENT[num]);
			_statAdjustment[num].growthMultiplier = DIFFICULTY_COEFFICIENT[num];
			++num;
		}
	}
	for (YAML::const_iterator i = doc["ufoTrajectories"].begin(); i != doc["ufoTrajectories"].end(); ++i)
	{
		UfoTrajectory *rule = loadRule(*i, &_ufoTrajectories, 0, "id");
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
	for (YAML::const_iterator i = doc["alienMissions"].begin(); i != doc["alienMissions"].end(); ++i)
	{
		RuleAlienMission *rule = loadRule(*i, &_alienMissions, &_alienMissionsIndex);
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
	_alienItemLevels = doc["alienItemLevels"].as< std::vector< std::vector<int> > >(_alienItemLevels);
	for (YAML::const_iterator i = doc["MCDPatches"].begin(); i != doc["MCDPatches"].end(); ++i)
	{
		std::string type = (*i)["type"].as<std::string>();
		if (_MCDPatches.find(type) != _MCDPatches.end())
		{
			_MCDPatches[type]->load(*i);
		}
		else
		{
			std::auto_ptr<MCDPatch> patch(new MCDPatch());
			patch->load(*i);
			_MCDPatches[type] = patch.release();
			_MCDPatchesIndex.push_back(type);
		}
	}
	for (YAML::const_iterator i = doc["extraSprites"].begin(); i != doc["extraSprites"].end(); ++i)
	{
		std::string type = (*i)["type"].as<std::string>();
		std::auto_ptr<ExtraSprites> extraSprites(new ExtraSprites());
		// doesn't support modIndex
		if (type != "TEXTURE.DAT")
			extraSprites->load(*i, _modOffset);
		else
			extraSprites->load(*i, 0);
		_extraSprites.push_back(std::make_pair(type, extraSprites.release()));
		_extraSpritesIndex.push_back(type);
	}
	for (YAML::const_iterator i = doc["extraSounds"].begin(); i != doc["extraSounds"].end(); ++i)
	{
		std::string type = (*i)["type"].as<std::string>();
		std::auto_ptr<ExtraSounds> extraSounds(new ExtraSounds());
		extraSounds->load(*i, _modOffset);
		_extraSounds.push_back(std::make_pair(type, extraSounds.release()));
		_extraSoundsIndex.push_back(type);
	}
	for (YAML::const_iterator i = doc["extraStrings"].begin(); i != doc["extraStrings"].end(); ++i)
	{
		std::string type = (*i)["type"].as<std::string>();
		if (_extraStrings.find(type) != _extraStrings.end())
		{
			_extraStrings[type]->load(*i);
		}
		else
		{
			std::auto_ptr<ExtraStrings> extraStrings(new ExtraStrings());
			extraStrings->load(*i);
			_extraStrings[type] = extraStrings.release();
			_extraStringsIndex.push_back(type);
		}
	}

	for (YAML::const_iterator i = doc["statStrings"].begin(); i != doc["statStrings"].end(); ++i)
	{
		StatString *statString = new StatString();
		statString->load(*i);
		_statStrings.push_back(statString);
	}

	for (YAML::const_iterator i = doc["interfaces"].begin(); i != doc["interfaces"].end(); ++i)
	{
		RuleInterface *rule = loadRule(*i, &_interfaces);
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
	for (YAML::const_iterator i = doc["soundDefs"].begin(); i != doc["soundDefs"].end(); ++i)
	{
		SoundDefinition *rule = loadRule(*i, &_soundDefs);
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
	if (doc["globe"])
	{
		_globe->load(doc["globe"]);
	}
	if (doc["converter"])
	{
		_converter->load(doc["converter"]);
	}
	for (YAML::const_iterator i = doc["constants"].begin(); i != doc["constants"].end(); ++i)
	{
		Mod::DOOR_OPEN = (*i)["doorSound"].as<int>(Mod::DOOR_OPEN);
		Mod::SLIDING_DOOR_OPEN = (*i)["slidingDoorSound"].as<int>(Mod::SLIDING_DOOR_OPEN);
		Mod::SLIDING_DOOR_CLOSE = (*i)["slidingDoorClose"].as<int>(Mod::SLIDING_DOOR_CLOSE);
		Mod::SMALL_EXPLOSION = (*i)["smallExplosion"].as<int>(Mod::SMALL_EXPLOSION);
		Mod::LARGE_EXPLOSION = (*i)["largeExplosion"].as<int>(Mod::LARGE_EXPLOSION);
		Mod::EXPLOSION_OFFSET = (*i)["explosionOffset"].as<int>(Mod::EXPLOSION_OFFSET);
		Mod::SMOKE_OFFSET = (*i)["smokeOffset"].as<int>(Mod::SMOKE_OFFSET);
		Mod::UNDERWATER_SMOKE_OFFSET = (*i)["underwaterSmokeOffset"].as<int>(Mod::UNDERWATER_SMOKE_OFFSET);
		Mod::ITEM_DROP = (*i)["itemDrop"].as<int>(Mod::ITEM_DROP);
		Mod::ITEM_THROW = (*i)["itemThrow"].as<int>(Mod::ITEM_THROW);
		Mod::ITEM_RELOAD = (*i)["itemReload"].as<int>(Mod::ITEM_RELOAD);
		Mod::WALK_OFFSET = (*i)["walkOffset"].as<int>(Mod::WALK_OFFSET);
		Mod::FLYING_SOUND = (*i)["flyingSound"].as<int>(Mod::FLYING_SOUND);
		Mod::BUTTON_PRESS = (*i)["buttonPress"].as<int>(Mod::BUTTON_PRESS);
		if ((*i)["windowPopup"])
		{
			int k = 0;
			for (YAML::const_iterator j = (*i)["windowPopup"].begin(); j != (*i)["windowPopup"].end() && k < 3; ++j, ++k)
			{
				Mod::WINDOW_POPUP[k] = (*j).as<int>(Mod::WINDOW_POPUP[k]);
			}
		}
		Mod::UFO_FIRE = (*i)["ufoFire"].as<int>(Mod::UFO_FIRE);
		Mod::UFO_HIT = (*i)["ufoHit"].as<int>(Mod::UFO_HIT);
		Mod::UFO_CRASH = (*i)["ufoCrash"].as<int>(Mod::UFO_CRASH);
		Mod::UFO_EXPLODE = (*i)["ufoExplode"].as<int>(Mod::UFO_EXPLODE);
		Mod::INTERCEPTOR_HIT = (*i)["interceptorHit"].as<int>(Mod::INTERCEPTOR_HIT);
		Mod::INTERCEPTOR_EXPLODE = (*i)["interceptorExplode"].as<int>(Mod::INTERCEPTOR_EXPLODE);
		Mod::GEOSCAPE_CURSOR = (*i)["geoscapeCursor"].as<int>(Mod::GEOSCAPE_CURSOR);
		Mod::BASESCAPE_CURSOR = (*i)["basescapeCursor"].as<int>(Mod::BASESCAPE_CURSOR);
		Mod::BATTLESCAPE_CURSOR = (*i)["battlescapeCursor"].as<int>(Mod::BATTLESCAPE_CURSOR);
		Mod::UFOPAEDIA_CURSOR = (*i)["ufopaediaCursor"].as<int>(Mod::UFOPAEDIA_CURSOR);
		Mod::GRAPHS_CURSOR = (*i)["graphsCursor"].as<int>(Mod::GRAPHS_CURSOR);
		Mod::DAMAGE_RANGE = (*i)["damageRange"].as<int>(Mod::DAMAGE_RANGE);
		Mod::EXPLOSIVE_DAMAGE_RANGE = (*i)["explosiveDamageRange"].as<int>(Mod::EXPLOSIVE_DAMAGE_RANGE);
		size_t num = 0;
		for (YAML::const_iterator j = (*i)["fireDamageRange"].begin(); j != (*i)["fireDamageRange"].end() && num < 2; ++j)
		{
			FIRE_DAMAGE_RANGE[num] = (*j).as<int>(FIRE_DAMAGE_RANGE[num]);
			++num;
		}
		Mod::DEBRIEF_MUSIC_GOOD = (*i)["goodDebriefingMusic"].as<std::string>(Mod::DEBRIEF_MUSIC_GOOD);
		Mod::DEBRIEF_MUSIC_BAD = (*i)["badDebriefingMusic"].as<std::string>(Mod::DEBRIEF_MUSIC_BAD);
	}
	for (YAML::const_iterator i = doc["transparencyLUTs"].begin(); i != doc["transparencyLUTs"].end(); ++i)
	{
		for (YAML::const_iterator j = (*i)["colors"].begin(); j != (*i)["colors"].end(); ++j)
		{
			SDL_Color color;
			color.r = (*j)[0].as<int>(0);
			color.g = (*j)[1].as<int>(0);
			color.b = (*j)[2].as<int>(0);
			color.unused = (*j)[3].as<int>(2);;
			_transparencies.push_back(color);
		}
	}
	for (YAML::const_iterator i = doc["mapScripts"].begin(); i != doc["mapScripts"].end(); ++i)
	{
		std::string type = (*i)["type"].as<std::string>();
		if ((*i)["delete"])
		{
			type = (*i)["delete"].as<std::string>(type);
		}
		if (_mapScripts.find(type) != _mapScripts.end())
		{
			for (std::vector<MapScript*>::iterator j = _mapScripts[type].begin(); j != _mapScripts[type].end();)
			{
				delete *j;
				j = _mapScripts[type].erase(j);
			}
		}
		for (YAML::const_iterator j = (*i)["commands"].begin(); j != (*i)["commands"].end(); ++j)
		{
			std::auto_ptr<MapScript> mapScript(new MapScript());
			mapScript->load(*j);
			_mapScripts[type].push_back(mapScript.release());
		}
	}
	for (YAML::const_iterator i = doc["missionScripts"].begin(); i != doc["missionScripts"].end(); ++i)
	{
		RuleMissionScript *rule = loadRule(*i, &_missionScripts, &_missionScriptIndex, "type");
		if (rule != 0)
		{
			rule->load(*i);
		}
	}

	// refresh _psiRequirements for psiStrengthEval
	for (std::vector<std::string>::const_iterator i = _facilitiesIndex.begin(); i != _facilitiesIndex.end(); ++i)
	{
		RuleBaseFacility *rule = getBaseFacility(*i);
		if (rule->getPsiLaboratories() > 0)
		{
			_psiRequirements = rule->getRequirements();
			break;
		}
	}

	for (YAML::const_iterator i = doc["cutscenes"].begin(); i != doc["cutscenes"].end(); ++i)
	{
		RuleVideo *rule = loadRule(*i, &_videos);
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
	for (YAML::const_iterator i = doc["musics"].begin(); i != doc["musics"].end(); ++i)
	{
		RuleMusic *rule = loadRule(*i, &_musicDefs);
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
	for (YAML::const_iterator i = doc["commendations"].begin(); i != doc["commendations"].end(); ++i)
	{
		std::string type = (*i)["type"].as<std::string>();
		std::auto_ptr<RuleCommendations> commendations(new RuleCommendations());
		commendations->load(*i);
        _commendations[type] = commendations.release();
	}
	size_t count = 0;
	for (YAML::const_iterator i = doc["aimAndArmorMultipliers"].begin(); i != doc["aimAndArmorMultipliers"].end() && count < 5; ++i)
	{
		_statAdjustment[count].aimAndArmorMultiplier = (*i).as<double>(_statAdjustment[count].aimAndArmorMultiplier);
		++count;
	}
	if (doc["statGrowthMultipliers"])
	{
		_statAdjustment[0].statGrowth = doc["statGrowthMultipliers"].as<UnitStats>(_statAdjustment[0].statGrowth);
		for (size_t i = 1; i != 5; ++i)
		{
			_statAdjustment[i].statGrowth = _statAdjustment[0].statGrowth;
		}
	}
}

/**
 * Loads a rule element, adding/removing from vectors as necessary.
 * @param node YAML node.
 * @param map Map associated to the rule type.
 * @param index Index vector for the rule type.
 * @param key Rule key name.
 * @return Pointer to new rule if one was created, or NULL if one was removed.
 */
template <typename T>
T *Mod::loadRule(const YAML::Node &node, std::map<std::string, T*> *map, std::vector<std::string> *index, const std::string &key) const
{
	T *rule = 0;
	if (node[key])
	{
		std::string type = node[key].as<std::string>();
		typename std::map<std::string, T*>::const_iterator i = map->find(type);
		if (i != map->end())
		{
			rule = i->second;
		}
		else
		{
			rule = new T(type);
			(*map)[type] = rule;
			if (index != 0)
			{
				index->push_back(type);
			}
		}
	}
	else if (node["delete"])
	{
		std::string type = node["delete"].as<std::string>();
		typename std::map<std::string, T*>::iterator i = map->find(type);
		if (i != map->end())
		{
			map->erase(i);
		}
		if (index != 0)
		{
			std::vector<std::string>::iterator idx = std::find(index->begin(), index->end(), type);
			if (idx != index->end())
			{
				index->erase(idx);
			}
		}
	}
	return rule;
}

/**
 * Generates a brand new saved game with starting data.
 * @return A new saved game.
 */
SavedGame *Mod::newSave() const
{
	SavedGame *save = new SavedGame();

	// Add countries
	for (std::vector<std::string>::const_iterator i = _countriesIndex.begin(); i != _countriesIndex.end(); ++i)
	{
		RuleCountry *country = getCountry(*i);
		if (!country->getLonMin().empty())
			save->getCountries()->push_back(new Country(country));
	}
	// Adjust funding to total $6M
	int missing = ((_initialFunding - save->getCountryFunding()/1000) / (int)save->getCountries()->size()) * 1000;
	for (std::vector<Country*>::iterator i = save->getCountries()->begin(); i != save->getCountries()->end(); ++i)
	{
		int funding = (*i)->getFunding().back() + missing;
		if (funding < 0)
		{
			funding = (*i)->getFunding().back();
		}
		(*i)->setFunding(funding);
	}
	save->setFunds(save->getCountryFunding());

	// Add regions
	for (std::vector<std::string>::const_iterator i = _regionsIndex.begin(); i != _regionsIndex.end(); ++i)
	{
		RuleRegion *region = getRegion(*i);
		if (!region->getLonMin().empty())
			save->getRegions()->push_back(new Region(region));
	}

	// Set up starting base
	Base *base = new Base(this);
	base->load(_startingBase, save, true);

	// Correct IDs
	for (std::vector<Craft*>::const_iterator i = base->getCrafts()->begin(); i != base->getCrafts()->end(); ++i)
	{
		save->getId((*i)->getRules()->getType());
	}

	// Generate soldiers
	int soldiers = _startingBase["randomSoldiers"].as<int>(0);
	for (int i = 0; i < soldiers; ++i)
	{
		Soldier *soldier = genSoldier(save);
		soldier->setCraft(base->getCrafts()->front());
		base->getSoldiers()->push_back(soldier);
		// Award soldier a special 'original eigth' commendation
		soldier->getDiary()->awardOriginalEightCommendation();
		for (std::vector<SoldierCommendations*>::iterator comm = soldier->getDiary()->getSoldierCommendations()->begin(); comm != soldier->getDiary()->getSoldierCommendations()->end(); ++comm)
		{
			(*comm)->makeOld();
		}
	}

	save->getBases()->push_back(base);
	// Setup alien strategy
	save->getAlienStrategy().init(this);
	save->setTime(_startingTime);

	return save;
}

/**
 * Returns the rules for the specified country.
 * @param id Country type.
 * @return Rules for the country.
 */
RuleCountry *Mod::getCountry(const std::string &id) const
{
	return getRule(id, "Country", _countries);
}

/**
 * Returns the list of all countries
 * provided by the mod.
 * @return List of countries.
 */
const std::vector<std::string> &Mod::getCountriesList() const
{
	return _countriesIndex;
}

/**
 * Returns the rules for the specified region.
 * @param id Region type.
 * @return Rules for the region.
 */
RuleRegion *Mod::getRegion(const std::string &id) const
{
	return getRule(id, "Region", _regions);
}

/**
 * Returns the list of all regions
 * provided by the mod.
 * @return List of regions.
 */
const std::vector<std::string> &Mod::getRegionsList() const
{
	return _regionsIndex;
}

/**
 * Returns the rules for the specified base facility.
 * @param id Facility type.
 * @return Rules for the facility.
 */
RuleBaseFacility *Mod::getBaseFacility(const std::string &id) const
{
	return getRule(id, "Facility", _facilities);
}

/**
 * Returns the list of all base facilities
 * provided by the mod.
 * @return List of base facilities.
 */
const std::vector<std::string> &Mod::getBaseFacilitiesList() const
{
	return _facilitiesIndex;
}

/**
 * Returns the rules for the specified craft.
 * @param id Craft type.
 * @return Rules for the craft.
 */
RuleCraft *Mod::getCraft(const std::string &id) const
{
	return getRule(id, "Craft", _crafts);
}

/**
 * Returns the list of all crafts
 * provided by the mod.
 * @return List of crafts.
 */
const std::vector<std::string> &Mod::getCraftsList() const
{
	return _craftsIndex;
}

/**
 * Returns the rules for the specified craft weapon.
 * @param id Craft weapon type.
 * @return Rules for the craft weapon.
 */
RuleCraftWeapon *Mod::getCraftWeapon(const std::string &id) const
{
	return getRule(id, "Craft Weapon", _craftWeapons);
}

/**
 * Returns the list of all craft weapons
 * provided by the mod.
 * @return List of craft weapons.
 */
const std::vector<std::string> &Mod::getCraftWeaponsList() const
{
	return _craftWeaponsIndex;
}

/**
 * Returns the rules for the specified item.
 * @param id Item type.
 * @return Rules for the item, or 0 when the item is not found.
 */
RuleItem *Mod::getItem(const std::string &id) const
{
	return getRule(id, "Item", _items);
}

/**
 * Returns the list of all items
 * provided by the mod.
 * @return List of items.
 */
const std::vector<std::string> &Mod::getItemsList() const
{
	return _itemsIndex;
}

/**
 * Returns the rules for the specified UFO.
 * @param id UFO type.
 * @return Rules for the UFO.
 */
RuleUfo *Mod::getUfo(const std::string &id) const
{
	return getRule(id, "UFO", _ufos);
}

/**
 * Returns the list of all ufos
 * provided by the mod.
 * @return List of ufos.
 */
const std::vector<std::string> &Mod::getUfosList() const
{
	return _ufosIndex;
}

/**
 * Returns the rules for the specified terrain.
 * @param name Terrain name.
 * @return Rules for the terrain.
 */
RuleTerrain *Mod::getTerrain(const std::string &name) const
{
	return getRule(name, "Terrain", _terrains);
}

/**
 * Returns the list of all terrains
 * provided by the mod.
 * @return List of terrains.
 */
const std::vector<std::string> &Mod::getTerrainList() const
{
	return _terrainIndex;
}

/**
 * Returns the info about a specific map data file.
 * @param name Datafile name.
 * @return Rules for the datafile.
 */
MapDataSet *Mod::getMapDataSet(const std::string &name)
{
	std::map<std::string, MapDataSet*>::iterator map = _mapDataSets.find(name);
	if (map == _mapDataSets.end())
	{
		MapDataSet *set = new MapDataSet(name);
		_mapDataSets[name] = set;
		return set;
	}
	else
	{
		return map->second;
	}
}

/**
 * Returns the info about a specific unit.
 * @param name Unit name.
 * @return Rules for the units.
 */
RuleSoldier *Mod::getSoldier(const std::string &name) const
{
	return getRule(name, "Soldier", _soldiers);
}

/**
 * Returns the list of all soldiers
 * provided by the mod.
 * @return List of soldiers.
 */
const std::vector<std::string> &Mod::getSoldiersList() const
{
	return _soldiersIndex;
}

/**
 * Gets the list of commendations
 * @return The list of commendations.
 */
std::map<std::string, RuleCommendations *> Mod::getCommendation() const
{
	return _commendations;
}

/**
 * Returns the info about a specific unit.
 * @param name Unit name.
 * @return Rules for the units.
 */
Unit *Mod::getUnit(const std::string &name) const
{
	return getRule(name, "Unit", _units);
}

/**
 * Returns the info about a specific alien race.
 * @param name Race name.
 * @return Rules for the race.
 */
AlienRace *Mod::getAlienRace(const std::string &name) const
{
	return getRule(name, "Alien Race", _alienRaces);
}

/**
 * Returns the list of all alien races.
 * provided by the mod.
 * @return List of alien races.
 */
const std::vector<std::string> &Mod::getAlienRacesList() const
{
	return _aliensIndex;
}

/**
 * Returns the info about a specific deployment.
 * @param name Deployment name.
 * @return Rules for the deployment.
 */
AlienDeployment *Mod::getDeployment(const std::string &name) const
{
	return getRule(name, "Alien Deployment", _alienDeployments);
}

/**
 * Returns the list of all alien deployments
 * provided by the mod.
 * @return List of alien deployments.
 */
const std::vector<std::string> &Mod::getDeploymentsList() const
{
	return _deploymentsIndex;
}

/**
 * Returns the info about a specific armor.
 * @param name Armor name.
 * @return Rules for the armor.
 */
Armor *Mod::getArmor(const std::string &name) const
{
	return getRule(name, "Armor", _armors);
}

/**
 * Returns the list of all armors
 * provided by the mod.
 * @return List of armors.
 */
const std::vector<std::string> &Mod::getArmorsList() const
{
	return _armorsIndex;
}

/**
 * Returns the cost of an individual engineer
 * for purchase/maintenance.
 * @return Cost.
 */
int Mod::getEngineerCost() const
{
	return _costEngineer;
}

/**
 * Returns the cost of an individual scientist
 * for purchase/maintenance.
 * @return Cost.
 */
int Mod::getScientistCost() const
{
	return _costScientist;
}

/**
 * Returns the time it takes to transfer personnel
 * between bases.
 * @return Time in hours.
 */
int Mod::getPersonnelTime() const
{
	return _timePersonnel;
}

/**
 * Returns the article definition for a given name.
 * @param name Article name.
 * @return Article definition.
 */
ArticleDefinition *Mod::getUfopaediaArticle(const std::string &name) const
{
	return getRule(name, "Article", _ufopaediaArticles);
}

/**
 * Returns the list of all articles
 * provided by the mod.
 * @return List of articles.
 */
const std::vector<std::string> &Mod::getUfopaediaList() const
{
	return _ufopaediaIndex;
}

/**
* Returns the list of all article categories
* provided by the mod.
* @return List of categories.
*/
const std::vector<std::string> &Mod::getUfopaediaCategoryList() const
{
	return _ufopaediaCatIndex;
}

/**
 * Returns the list of inventories.
 * @return Pointer to inventory list.
 */
std::map<std::string, RuleInventory*> *Mod::getInventories()
{
	return &_invs;
}

/**
 * Returns the rules for a specific inventory.
 * @param id Inventory type.
 * @return Inventory ruleset.
 */
RuleInventory *Mod::getInventory(const std::string &id) const
{
	return getRule(id, "Inventory", _invs);
}

/**
 * Returns the list of inventories.
 * @return The list of inventories.
 */
const std::vector<std::string> &Mod::getInvsList() const
{
	return _invsIndex;
}

/**
 * Returns the rules for the specified research project.
 * @param id Research project type.
 * @return Rules for the research project.
 */
RuleResearch *Mod::getResearch (const std::string &id) const
{
	return getRule(id, "Research", _research);
}

/**
 * Returns the list of research projects.
 * @return The list of research projects.
 */
const std::vector<std::string> &Mod::getResearchList() const
{
	return _researchIndex;
}

/**
 * Returns the rules for the specified manufacture project.
 * @param id Manufacture project type.
 * @return Rules for the manufacture project.
 */
RuleManufacture *Mod::getManufacture (const std::string &id) const
{
	return getRule(id, "Manufacture", _manufacture);
}

/**
 * Returns the list of manufacture projects.
 * @return The list of manufacture projects.
 */
const std::vector<std::string> &Mod::getManufactureList() const
{
	return _manufactureIndex;
}


/**
 * Generates and returns a list of facilities for custom bases.
 * The list contains all the facilities that are listed in the 'startingBase'
 * part of the ruleset.
 * @return The list of facilities for custom bases.
 */
std::vector<OpenXcom::RuleBaseFacility*> Mod::getCustomBaseFacilities() const
{
	std::vector<OpenXcom::RuleBaseFacility*> placeList;

	for (YAML::const_iterator i = _startingBase["facilities"].begin(); i != _startingBase["facilities"].end(); ++i)
	{
		std::string type = (*i)["type"].as<std::string>();
		RuleBaseFacility *facility = getBaseFacility(type);
		if (!facility->isLift())
		{
			placeList.push_back(facility);
		}
	}
	return placeList;
}

/**
 * Returns the data for the specified ufo trajectory.
 * @param id Ufo trajectory id.
 * @return A pointer to the data for the specified ufo trajectory.
 */
const UfoTrajectory *Mod::getUfoTrajectory(const std::string &id) const
{
	return getRule(id, "Trajectory", _ufoTrajectories);
}

/**
 * Returns the rules for the specified alien mission.
 * @param id Alien mission type.
 * @return Rules for the alien mission.
 */
const RuleAlienMission *Mod::getAlienMission(const std::string &id) const
{
	return getRule(id, "Alien Mission", _alienMissions);
}

/**
 * Returns the rules for a random alien mission based on a specific objective.
 * @param objective Alien mission objective.
 * @return Rules for the alien mission.
 */
const RuleAlienMission *Mod::getRandomMission(MissionObjective objective, size_t monthsPassed) const
{
	int totalWeight = 0;
	std::map<int, RuleAlienMission*> possibilities;
	for (std::map<std::string, RuleAlienMission *>::const_iterator i = _alienMissions.begin(); i != _alienMissions.end(); ++i)
	{
		if (i->second->getObjective() == objective && i->second->getWeight(monthsPassed) > 0)
		{
			totalWeight += i->second->getWeight(monthsPassed);
			possibilities[totalWeight] = i->second;
		}
	}
	int pick = RNG::generate(1, totalWeight);
	for (std::map<int, RuleAlienMission*>::const_iterator i = possibilities.begin(); i != possibilities.end(); ++i)
	{
		if (pick <= i->first)
		{
			return i->second;
		}
	}
	return 0;
}

/**
 * Returns the list of alien mission types.
 * @return The list of alien mission types.
 */
const std::vector<std::string> &Mod::getAlienMissionList() const
{
	return _alienMissionsIndex;
}

/**
 * Gets the alien item level table.
 * @return A deep array containing the alien item levels.
 */
const std::vector<std::vector<int> > &Mod::getAlienItemLevels() const
{
	return _alienItemLevels;
}

/**
 * Gets the defined starting base.
 * @return The starting base definition.
 */
const YAML::Node &Mod::getStartingBase() const
{
	return _startingBase;
}

/**
 * Gets the defined starting time.
 * @return The time the game starts in.
 */
const GameTime &Mod::getStartingTime() const
{
	return _startingTime;
}

/**
 * Gets an MCDPatch.
 * @param id The ID of the MCDPatch we want.
 * @return The MCDPatch based on ID, or 0 if none defined.
 */
MCDPatch *Mod::getMCDPatch(const std::string &id) const
{
	std::map<std::string, MCDPatch*>::const_iterator i = _MCDPatches.find(id);
	if (_MCDPatches.end() != i) return i->second; else return 0;
}

/**
 * Gets the list of external sprites.
 * @return The list of external sprites.
 */
std::vector<std::pair<std::string, ExtraSprites *> > Mod::getExtraSprites() const
{
	return _extraSprites;
}

/**
 * Gets the list of external sounds.
 * @return The list of external sounds.
 */
std::vector<std::pair<std::string, ExtraSounds *> > Mod::getExtraSounds() const
{
	return _extraSounds;
}

/**
 * Gets the list of external strings.
 * @return The list of external strings.
 */
std::map<std::string, ExtraStrings *> Mod::getExtraStrings() const
{
	return _extraStrings;
}

/**
 * Gets the list of StatStrings.
 * @return The list of StatStrings.
 */
std::vector<StatString *> Mod::getStatStrings() const
{
	return _statStrings;
}

/**
 * Compares rules based on their list orders.
 */
template <typename T>
struct compareRule : public std::binary_function<const std::string&, const std::string&, bool>
{
	Mod *_mod;
	typedef T*(Mod::*RuleLookup)(const std::string &id);
	RuleLookup _lookup;

	compareRule(Mod *mod, RuleLookup lookup) : _mod(mod), _lookup(lookup)
	{
	}

	bool operator()(const std::string &r1, const std::string &r2) const
	{
		T *rule1 = (_mod->*_lookup)(r1);
		T *rule2 = (_mod->*_lookup)(r2);
		return (rule1->getListOrder() < rule2->getListOrder());
	}
};

/**
 * Craft weapons use the list order of their launcher item.
 */
template <>
struct compareRule<RuleCraftWeapon> : public std::binary_function<const std::string&, const std::string&, bool>
{
	Mod *_mod;

	compareRule(Mod *mod) : _mod(mod)
	{
	}

	bool operator()(const std::string &r1, const std::string &r2) const
	{
		RuleItem *rule1 = _mod->getItem(_mod->getCraftWeapon(r1)->getLauncherItem());
		RuleItem *rule2 = _mod->getItem(_mod->getCraftWeapon(r2)->getLauncherItem());
		return (rule1->getListOrder() < rule2->getListOrder());
	}
};

/**
 * Armor uses the list order of their store item.
 * Itemless armor comes before all else.
 */
template <>
struct compareRule<Armor> : public std::binary_function<const std::string&, const std::string&, bool>
{
	Mod *_mod;

	compareRule(Mod *mod) : _mod(mod)
	{
	}

	bool operator()(const std::string &r1, const std::string &r2) const
	{
		Armor* armor1 = _mod->getArmor(r1);
		Armor* armor2 = _mod->getArmor(r2);
		RuleItem *rule1 = _mod->getItem(armor1->getStoreItem());
		RuleItem *rule2 = _mod->getItem(armor2->getStoreItem());
		if (!rule1 && !rule2)
			return (armor1 < armor2); // tiebreaker, don't care about order, pointers are as good as any
		else if (!rule1)
			return true;
		else if (!rule2)
			return false;
		else
			return (rule1->getListOrder() < rule2->getListOrder());
	}
};

/**
 * Ufopaedia articles use section and list order.
 */
template <>
struct compareRule<ArticleDefinition> : public std::binary_function<const std::string&, const std::string&, bool>
{
	Mod *_mod;
	static std::map<std::string, int> _sections;

	compareRule(Mod *mod) : _mod(mod)
	{
		const std::vector<std::string> &list = mod->getUfopaediaCategoryList();
		int order = 0;
		for (std::vector<std::string>::const_iterator i = list.begin(); i != list.end(); ++i)
		{
			_sections[*i] = order++;
		}
		_sections[UFOPAEDIA_NOT_AVAILABLE] = order++;
	}

	bool operator()(const std::string &r1, const std::string &r2) const
	{
		ArticleDefinition *rule1 = _mod->getUfopaediaArticle(r1);
		ArticleDefinition *rule2 = _mod->getUfopaediaArticle(r2);
		if (_sections[rule1->section] == _sections[rule2->section])
			return (rule1->getListOrder() < rule2->getListOrder());
		else
			return (_sections[rule1->section] < _sections[rule2->section]);
	}
};
std::map<std::string, int> compareRule<ArticleDefinition>::_sections;

/**
 * Sorts all our lists according to their weight.
 */
void Mod::sortLists()
{
	std::sort(_itemsIndex.begin(), _itemsIndex.end(), compareRule<RuleItem>(this, (compareRule<RuleItem>::RuleLookup)&Mod::getItem));
	std::sort(_craftsIndex.begin(), _craftsIndex.end(), compareRule<RuleCraft>(this, (compareRule<RuleCraft>::RuleLookup)&Mod::getCraft));
	std::sort(_facilitiesIndex.begin(), _facilitiesIndex.end(), compareRule<RuleBaseFacility>(this, (compareRule<RuleBaseFacility>::RuleLookup)&Mod::getBaseFacility));
	std::sort(_researchIndex.begin(), _researchIndex.end(), compareRule<RuleResearch>(this, (compareRule<RuleResearch>::RuleLookup)&Mod::getResearch));
	std::sort(_manufactureIndex.begin(), _manufactureIndex.end(), compareRule<RuleManufacture>(this, (compareRule<RuleManufacture>::RuleLookup)&Mod::getManufacture));
	std::sort(_invsIndex.begin(), _invsIndex.end(), compareRule<RuleInventory>(this, (compareRule<RuleInventory>::RuleLookup)&Mod::getInventory));
	// special cases
	std::sort(_craftWeaponsIndex.begin(), _craftWeaponsIndex.end(), compareRule<RuleCraftWeapon>(this));
	std::sort(_armorsIndex.begin(), _armorsIndex.end(), compareRule<Armor>(this));
	std::sort(_ufopaediaIndex.begin(), _ufopaediaIndex.end(), compareRule<ArticleDefinition>(this));
}

/**
 * Gets the research-requirements for Psi-Lab (it's a cache for psiStrengthEval)
 */
std::vector<std::string> Mod::getPsiRequirements() const
{
	return _psiRequirements;
}

/**
 * Creates a new randomly-generated soldier.
 * @param save Saved game the soldier belongs to.
 * @param type The soldier type to generate.
 * @return Newly generated soldier.
 */
Soldier *Mod::genSoldier(SavedGame *save, std::string type) const
{
	Soldier *soldier = 0;
	int newId = save->getId("STR_SOLDIER");
	if (type.empty())
	{
		type = _soldiersIndex.front();
	}

	// Check for duplicates
	// Original X-COM gives up after 10 tries so might as well do the same here
	bool duplicate = true;
	for (int i = 0; i < 10 && duplicate; i++)
	{
		delete soldier;
		soldier = new Soldier(getSoldier(type), getArmor(getSoldier(type)->getArmor()), newId);
		duplicate = false;
		for (std::vector<Base*>::iterator i = save->getBases()->begin(); i != save->getBases()->end() && !duplicate; ++i)
		{
			for (std::vector<Soldier*>::iterator j = (*i)->getSoldiers()->begin(); j != (*i)->getSoldiers()->end() && !duplicate; ++j)
			{
				if ((*j)->getName() == soldier->getName())
				{
					duplicate = true;
				}
			}
			for (std::vector<Transfer*>::iterator k = (*i)->getTransfers()->begin(); k != (*i)->getTransfers()->end() && !duplicate; ++k)
			{
				if ((*k)->getType() == TRANSFER_SOLDIER && (*k)->getSoldier()->getName() == soldier->getName())
				{
					duplicate = true;
				}
			}
		}
	}

	// calculate new statString
	soldier->calcStatString(getStatStrings(), (Options::psiStrengthEval && save->isResearched(getPsiRequirements())));

	return soldier;
}

/**
 * Gets the name of the item to be used as alien fuel.
 * @return the name of the fuel.
 */
std::string Mod::getAlienFuelName() const
{
	return _alienFuel.first;
}

/**
 * Gets the amount of alien fuel to recover.
 * @return the amount to recover.
 */
int Mod::getAlienFuelQuantity() const
{
	return _alienFuel.second;
}

/**
 * Gets name of font collection.
 * @return the name of YAML-file with font data
 */
std::string Mod::getFontName() const
{
	return _fontName;
}

/**
 * Returns the minimum facilitie's radar range.
 * @return The minimum range.
 */
 int Mod::getMinRadarRange() const
 {
	static int minRadarRange = -1;

	if (minRadarRange < 0)
	{
		minRadarRange = 0;
		for (std::vector<std::string>::const_iterator i = _facilitiesIndex.begin(); i != _facilitiesIndex.end(); ++i)
		{
			RuleBaseFacility *f = getBaseFacility(*i);
			if (f == 0) continue;

			int radarRange = f->getRadarRange();
			if (radarRange > 0 && (minRadarRange == 0 || minRadarRange > radarRange))
			{
				minRadarRange = radarRange;
			}
		}
	}

	return minRadarRange;
 }

/**
 * Gets information on an interface.
 * @param id the interface we want info on.
 * @return the interface.
 */
RuleInterface *Mod::getInterface(const std::string &id) const
{
	return getRule(id, "Interface", _interfaces);
}

/**
 * Gets the rules for the Geoscape globe.
 * @return Pointer to globe rules.
 */
RuleGlobe *Mod::getGlobe() const
{
	return _globe;
}

/**
* Gets the rules for the Save Converter.
* @return Pointer to converter rules.
*/
RuleConverter *Mod::getConverter() const
{
	return _converter;
}

const std::map<std::string, SoundDefinition *> *Mod::getSoundDefinitions() const
{
	return &_soundDefs;
}

const std::vector<SDL_Color> *Mod::getTransparencies() const
{
	return &_transparencies;
}

const std::vector<MapScript*> *Mod::getMapScript(std::string id) const
{
	std::map<std::string, std::vector<MapScript*> >::const_iterator i = _mapScripts.find(id);
	if (_mapScripts.end() != i)
	{
		return &i->second;
	}
	else
	{
		Log(LOG_ERROR) << "Map Script " << id << "not found";
		return 0;
	}
}

const std::map<std::string, RuleVideo *> *Mod::getVideos() const
{
	return &_videos;
}

const std::map<std::string, RuleMusic *> *Mod::getMusic() const
{
	return &_musicDefs;
}

const std::vector<std::string> *Mod::getMissionScriptList() const
{
	return &_missionScriptIndex;
}

RuleMissionScript *Mod::getMissionScript(const std::string &name) const
{
	return getRule(name, "Mission Script", _missionScripts);
}
std::string Mod::getFinalResearch() const
{
	return _finalResearch; 
}

namespace
{
	const Uint8 ShadeMax = 15;
	/**
	* Recolor class used in UFO
	*/
	struct HairXCOM1
	{
		static const Uint8 Hair = 9 << 4;
		static const Uint8 Face = 6 << 4;
		static inline void func(Uint8& src, const Uint8& cutoff, int, int, int)
		{
			if (src > cutoff && src <= Face + ShadeMax)
			{
				src = Hair + (src & ShadeMax) - 6; //make hair color like male in xcom_0.pck
			}
		}
	};

	/**
	* Recolor class used in TFTD
	*/
	struct HairXCOM2
	{
		static const Uint8 ManHairColor = 4 << 4;
		static const Uint8 WomanHairColor = 1 << 4;
		static inline void func(Uint8& src, int, int, int, int)
		{
			if (src >= WomanHairColor && src <= WomanHairColor + ShadeMax)
			{
				src = ManHairColor + (src & ShadeMax);
			}
		}
	};

	/**
	* Recolor class used in TFTD
	*/
	struct FaceXCOM2
	{
		static const Uint8 FaceColor = 10 << 4;
		static const Uint8 PinkColor = 14 << 4;
		static inline void func(Uint8& src, int, int, int, int)
		{
			if (src >= FaceColor && src <= FaceColor + ShadeMax)
			{
				src = PinkColor + (src & ShadeMax);
			}
		}
	};

	/**
	* Recolor class used in TFTD
	*/
	struct BodyXCOM2
	{
		static const Uint8 IonArmorColor = 8 << 4;
		static inline void func(Uint8& src, int, int, int, int)
		{
			if (src == 153)
			{
				src = IonArmorColor + 12;
			}
			else if (src == 151)
			{
				src = IonArmorColor + 10;
			}
			else if (src == 148)
			{
				src = IonArmorColor + 4;
			}
			else if (src == 147)
			{
				src = IonArmorColor + 2;
			}
			else if (src >= HairXCOM2::WomanHairColor && src <= HairXCOM2::WomanHairColor + ShadeMax)
			{
				src = IonArmorColor + (src & ShadeMax);
			}
		}
	};
	/**
	* Recolor class used in TFTD
	*/
	struct FallXCOM2
	{
		static const Uint8 RoguePixel = 151;
		static inline void func(Uint8& src, int, int, int, int)
		{
			if (src == RoguePixel)
			{
				src = FaceXCOM2::PinkColor + (src & ShadeMax) + 2;
			}
			else if (src >= BodyXCOM2::IonArmorColor && src <= BodyXCOM2::IonArmorColor + ShadeMax)
			{
				src = FaceXCOM2::PinkColor + (src & ShadeMax);
			}
		}
	};
}

/**
 * Loads the vanilla resources required by the game.
 */
void Mod::loadVanillaResources()
{
	// Load palettes
	const char *pal[] = { "PAL_GEOSCAPE", "PAL_BASESCAPE", "PAL_GRAPHS", "PAL_UFOPAEDIA", "PAL_BATTLEPEDIA" };
	for (size_t i = 0; i < sizeof(pal) / sizeof(pal[0]); ++i)
	{
		std::string s = "GEODATA/PALETTES.DAT";
		_palettes[pal[i]] = new Palette();
		_palettes[pal[i]]->loadDat(FileMap::getFilePath(s), 256, Palette::palOffset(i));
	}
	{
		std::string s1 = "GEODATA/BACKPALS.DAT";
		std::string s2 = "BACKPALS.DAT";
		_palettes[s2] = new Palette();
		_palettes[s2]->loadDat(FileMap::getFilePath(s1), 128);
	}

	// Correct Battlescape palette
	{
		std::string s1 = "GEODATA/PALETTES.DAT";
		std::string s2 = "PAL_BATTLESCAPE";
		_palettes[s2] = new Palette();
		_palettes[s2]->loadDat(FileMap::getFilePath(s1), 256, Palette::palOffset(4));

		// Last 16 colors are a greyish gradient
		SDL_Color gradient[] = { { 140, 152, 148, 255 },
		{ 132, 136, 140, 255 },
		{ 116, 124, 132, 255 },
		{ 108, 116, 124, 255 },
		{ 92, 104, 108, 255 },
		{ 84, 92, 100, 255 },
		{ 76, 80, 92, 255 },
		{ 56, 68, 84, 255 },
		{ 48, 56, 68, 255 },
		{ 40, 48, 56, 255 },
		{ 32, 36, 48, 255 },
		{ 24, 28, 32, 255 },
		{ 16, 20, 24, 255 },
		{ 8, 12, 16, 255 },
		{ 3, 4, 8, 255 },
		{ 3, 3, 6, 255 } };
		for (size_t i = 0; i < sizeof(gradient) / sizeof(gradient[0]); ++i)
		{
			SDL_Color *color = _palettes[s2]->getColors(Palette::backPos + 16 + i);
			*color = gradient[i];
		}
	}

	// Load surfaces
	{
		std::ostringstream s;
		s << "GEODATA/" << "INTERWIN.DAT";
		_surfaces["INTERWIN.DAT"] = new Surface(160, 600);
		_surfaces["INTERWIN.DAT"]->loadScr(FileMap::getFilePath(s.str()));
	}

	const std::set<std::string> &geographFiles(FileMap::getVFolderContents("GEOGRAPH"));
	std::set<std::string> scrs = FileMap::filterFiles(geographFiles, "SCR");
	for (std::set<std::string>::iterator i = scrs.begin(); i != scrs.end(); ++i)
	{
		std::string fname = *i;
		std::transform(i->begin(), i->end(), fname.begin(), toupper);
		_surfaces[fname] = new Surface(320, 200);
		_surfaces[fname]->loadScr(FileMap::getFilePath("GEOGRAPH/" + fname));
	}
	std::set<std::string> bdys = FileMap::filterFiles(geographFiles, "BDY");
	for (std::set<std::string>::iterator i = bdys.begin(); i != bdys.end(); ++i)
	{
		std::string fname = *i;
		std::transform(i->begin(), i->end(), fname.begin(), toupper);
		_surfaces[fname] = new Surface(320, 200);
		_surfaces[fname]->loadBdy(FileMap::getFilePath("GEOGRAPH/" + fname));
	}

	std::set<std::string> spks = FileMap::filterFiles(geographFiles, "SPK");
	for (std::set<std::string>::iterator i = spks.begin(); i != spks.end(); ++i)
	{
		std::string fname = *i;
		std::transform(i->begin(), i->end(), fname.begin(), toupper);
		_surfaces[fname] = new Surface(320, 200);
		_surfaces[fname]->loadSpk(FileMap::getFilePath("GEOGRAPH/" + fname));
	}

	// Load surface sets
	std::string sets[] = { "BASEBITS.PCK",
		"INTICON.PCK",
		"TEXTURE.DAT" };

	for (size_t i = 0; i < sizeof(sets) / sizeof(sets[0]); ++i)
	{
		std::ostringstream s;
		s << "GEOGRAPH/" << sets[i];

		std::string ext = sets[i].substr(sets[i].find_last_of('.') + 1, sets[i].length());
		if (ext == "PCK")
		{
			std::string tab = CrossPlatform::noExt(sets[i]) + ".TAB";
			std::ostringstream s2;
			s2 << "GEOGRAPH/" << tab;
			_sets[sets[i]] = new SurfaceSet(32, 40);
			_sets[sets[i]]->loadPck(FileMap::getFilePath(s.str()), FileMap::getFilePath(s2.str()));
		}
		else
		{
			_sets[sets[i]] = new SurfaceSet(32, 32);
			_sets[sets[i]]->loadDat(FileMap::getFilePath(s.str()));
		}
	}
	_sets["SCANG.DAT"] = new SurfaceSet(4, 4);
	std::ostringstream scang;
	scang << "GEODATA/" << "SCANG.DAT";
	_sets["SCANG.DAT"]->loadDat(FileMap::getFilePath(scang.str()));

	if (!Options::mute)
	{
		// Load sounds
		const std::set<std::string> &soundFiles(FileMap::getVFolderContents("SOUND"));

		if (_soundDefs.empty())
		{
			std::string catsId[] = { "GEO.CAT", "BATTLE.CAT" };
			std::string catsDos[] = { "SOUND2.CAT", "SOUND1.CAT" };
			std::string catsWin[] = { "SAMPLE.CAT", "SAMPLE2.CAT" };

			// Try the preferred format first, otherwise use the default priority
			std::string *cats[] = { 0, catsWin, catsDos };
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
					std::string fname = cats[j][i];
					std::transform(fname.begin(), fname.end(), fname.begin(), tolower);
					std::set<std::string>::iterator file = soundFiles.find(fname);
					if (file != soundFiles.end())
					{
						sound = new SoundSet();
						sound->loadCat(FileMap::getFilePath("SOUND/" + cats[j][i]), wav);
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
			for (std::map<std::string, SoundDefinition*>::const_iterator i = _soundDefs.begin(); i != _soundDefs.end(); ++i)
			{
				std::string fname = i->second->getCATFile();
				std::transform(fname.begin(), fname.end(), fname.begin(), tolower);
				std::set<std::string>::iterator file = soundFiles.find(fname);
				if (file != soundFiles.end())
				{
					if (_sounds.find((*i).first) == _sounds.end())
					{
						_sounds[(*i).first] = new SoundSet();
					}
					for (std::vector<int>::const_iterator j = (*i).second->getSoundList().begin(); j != (*i).second->getSoundList().end(); ++j)
					{
						_sounds[(*i).first]->loadCatbyIndex(FileMap::getFilePath("SOUND/" + fname), *j);
					}
				}
				else
				{
					throw Exception(fname + " not found");
				}
			}
		}

		std::set<std::string>::iterator file = soundFiles.find("intro.cat");
		if (file != soundFiles.end())
		{
			SoundSet *s = _sounds["INTRO.CAT"] = new SoundSet();
			s->loadCat(FileMap::getFilePath("SOUND/INTRO.CAT"), false);
		}

		file = soundFiles.find("sample3.cat");
		if (file != soundFiles.end())
		{
			SoundSet *s = _sounds["SAMPLE3.CAT"] = new SoundSet();
			s->loadCat(FileMap::getFilePath("SOUND/SAMPLE3.CAT"), true);
		}
	}

	TextButton::soundPress = getSound("GEO.CAT", Mod::BUTTON_PRESS);
	Window::soundPopup[0] = getSound("GEO.CAT", Mod::WINDOW_POPUP[0]);
	Window::soundPopup[1] = getSound("GEO.CAT", Mod::WINDOW_POPUP[1]);
	Window::soundPopup[2] = getSound("GEO.CAT", Mod::WINDOW_POPUP[2]);

	loadBattlescapeResources(); // TODO load this at battlescape start, unload at battlescape end?
}

/**
 * Loads the resources required by the Battlescape.
 */
void Mod::loadBattlescapeResources()
{
	// Load Battlescape ICONS
	_sets["SPICONS.DAT"] = new SurfaceSet(32, 24);
	_sets["SPICONS.DAT"]->loadDat(FileMap::getFilePath("UFOGRAPH/SPICONS.DAT"));
	_sets["CURSOR.PCK"] = new SurfaceSet(32, 40);
	_sets["CURSOR.PCK"]->loadPck(FileMap::getFilePath("UFOGRAPH/CURSOR.PCK"), FileMap::getFilePath("UFOGRAPH/CURSOR.TAB"));
	_sets["SMOKE.PCK"] = new SurfaceSet(32, 40);
	_sets["SMOKE.PCK"]->loadPck(FileMap::getFilePath("UFOGRAPH/SMOKE.PCK"), FileMap::getFilePath("UFOGRAPH/SMOKE.TAB"));
	_sets["HIT.PCK"] = new SurfaceSet(32, 40);
	_sets["HIT.PCK"]->loadPck(FileMap::getFilePath("UFOGRAPH/HIT.PCK"), FileMap::getFilePath("UFOGRAPH/HIT.TAB"));
	_sets["X1.PCK"] = new SurfaceSet(128, 64);
	_sets["X1.PCK"]->loadPck(FileMap::getFilePath("UFOGRAPH/X1.PCK"), FileMap::getFilePath("UFOGRAPH/X1.TAB"));
	_sets["MEDIBITS.DAT"] = new SurfaceSet(52, 58);
	_sets["MEDIBITS.DAT"]->loadDat(FileMap::getFilePath("UFOGRAPH/MEDIBITS.DAT"));
	_sets["DETBLOB.DAT"] = new SurfaceSet(16, 16);
	_sets["DETBLOB.DAT"]->loadDat(FileMap::getFilePath("UFOGRAPH/DETBLOB.DAT"));

	// Load Battlescape Terrain (only blanks are loaded, others are loaded just in time)
	_sets["BLANKS.PCK"] = new SurfaceSet(32, 40);
	_sets["BLANKS.PCK"]->loadPck(FileMap::getFilePath("TERRAIN/BLANKS.PCK"), FileMap::getFilePath("TERRAIN/BLANKS.TAB"));

	// Load Battlescape units
	std::set<std::string> unitsContents = FileMap::getVFolderContents("UNITS");
	std::set<std::string> usets = FileMap::filterFiles(unitsContents, "PCK");
	for (std::set<std::string>::iterator i = usets.begin(); i != usets.end(); ++i)
	{
		std::string path = FileMap::getFilePath("UNITS/" + *i);
		std::string tab = FileMap::getFilePath("UNITS/" + CrossPlatform::noExt(*i) + ".TAB");
		std::string fname = *i;
		std::transform(i->begin(), i->end(), fname.begin(), toupper);
		if (fname != "BIGOBS.PCK")
			_sets[fname] = new SurfaceSet(32, 40);
		else
			_sets[fname] = new SurfaceSet(32, 48);
		_sets[fname]->loadPck(path, tab);
	}
	// incomplete chryssalid set: 1.0 data: stop loading.
	if (_sets.find("CHRYS.PCK") != _sets.end() && !_sets["CHRYS.PCK"]->getFrame(225))
	{
		Log(LOG_FATAL) << "Version 1.0 data detected";
		throw Exception("Invalid CHRYS.PCK, please patch your X-COM data to the latest version");
	}
	// TFTD uses the loftemps dat from the terrain folder, but still has enemy unknown's version in the geodata folder, which is short by 2 entries.
	std::set<std::string> terrainContents = FileMap::getVFolderContents("TERRAIN");
	if (terrainContents.find("loftemps.dat") != terrainContents.end())
	{
		MapDataSet::loadLOFTEMPS(FileMap::getFilePath("TERRAIN/LOFTEMPS.DAT"), &_voxelData);
	}
	else
	{
		MapDataSet::loadLOFTEMPS(FileMap::getFilePath("GEODATA/LOFTEMPS.DAT"), &_voxelData);
	}

	std::string scrs[] = { "TAC00.SCR" };

	for (size_t i = 0; i < sizeof(scrs) / sizeof(scrs[0]); ++i)
	{
		_surfaces[scrs[i]] = new Surface(320, 200);
		_surfaces[scrs[i]]->loadScr(FileMap::getFilePath("UFOGRAPH/" + scrs[i]));
	}

	// lower case so we can find them in the contents map
	std::string lbms[] = { "d0.lbm",
		"d1.lbm",
		"d2.lbm",
		"d3.lbm" };
	std::string pals[] = { "PAL_BATTLESCAPE",
		"PAL_BATTLESCAPE_1",
		"PAL_BATTLESCAPE_2",
		"PAL_BATTLESCAPE_3" };

	SDL_Color backPal[] = { { 0, 5, 4, 255 },
	{ 0, 10, 34, 255 },
	{ 2, 9, 24, 255 },
	{ 2, 0, 24, 255 } };

	std::set<std::string> ufographContents = FileMap::getVFolderContents("UFOGRAPH");
	for (size_t i = 0; i < sizeof(lbms) / sizeof(lbms[0]); ++i)
	{
		if (ufographContents.find(lbms[i]) == ufographContents.end())
		{
			continue;
		}

		if (!i)
		{
			delete _palettes["PAL_BATTLESCAPE"];
		}

		Surface *tempSurface = new Surface(1, 1);
		tempSurface->loadImage(FileMap::getFilePath("UFOGRAPH/" + lbms[i]));
		_palettes[pals[i]] = new Palette();
		SDL_Color *colors = tempSurface->getPalette();
		colors[255] = backPal[i];
		_palettes[pals[i]]->setColors(colors, 256);
		createTransparencyLUT(_palettes[pals[i]]);
		delete tempSurface;
	}

	std::string spks[] = { "TAC01.SCR",
		"DETBORD.PCK",
		"DETBORD2.PCK",
		"ICONS.PCK",
		"MEDIBORD.PCK",
		"SCANBORD.PCK",
		"UNIBORD.PCK" };

	for (size_t i = 0; i < sizeof(spks) / sizeof(spks[0]); ++i)
	{
		std::string fname = spks[i];
		std::transform(fname.begin(), fname.end(), fname.begin(), tolower);
		if (ufographContents.find(fname) == ufographContents.end())
		{
			continue;
		}

		_surfaces[spks[i]] = new Surface(320, 200);
		_surfaces[spks[i]]->loadSpk(FileMap::getFilePath("UFOGRAPH/" + spks[i]));
	}


	std::set<std::string> bdys = FileMap::filterFiles(ufographContents, "BDY");
	for (std::set<std::string>::iterator i = bdys.begin(); i != bdys.end(); ++i)
	{
		std::string idxName = *i;
		std::transform(i->begin(), i->end(), idxName.begin(), toupper);
		idxName = idxName.substr(0, idxName.length() - 3);
		if (idxName.substr(0, 3) == "MAN")
		{
			idxName = idxName + "SPK";
		}
		else if (idxName == "TAC01.")
		{
			idxName = idxName + "SCR";
		}
		else
		{
			idxName = idxName + "PCK";
		}
		_surfaces[idxName] = new Surface(320, 200);
		_surfaces[idxName]->loadBdy(FileMap::getFilePath("UFOGRAPH/" + *i));
	}

	// Load Battlescape inventory
	std::set<std::string> invs = FileMap::filterFiles(ufographContents, "SPK");
	for (std::set<std::string>::iterator i = invs.begin(); i != invs.end(); ++i)
	{
		std::string fname = *i;
		std::transform(i->begin(), i->end(), fname.begin(), toupper);
		_surfaces[fname] = new Surface(320, 200);
		_surfaces[fname]->loadSpk(FileMap::getFilePath("UFOGRAPH/" + fname));
	}

	//"fix" of color index in original solders sprites
	if (Options::battleHairBleach)
	{
		std::string name;

		//personal armor
		name = "XCOM_1.PCK";
		if (_sets.find(name) != _sets.end())
		{
			SurfaceSet *xcom_1 = _sets[name];

			for (int i = 0; i < 8; ++i)
			{
				//chest frame
				Surface *surf = xcom_1->getFrame(4 * 8 + i);
				ShaderMove<Uint8> head = ShaderMove<Uint8>(surf);
				GraphSubset dim = head.getBaseDomain();
				surf->lock();
				dim.beg_y = 6;
				dim.end_y = 9;
				head.setDomain(dim);
				ShaderDraw<HairXCOM1>(head, ShaderScalar<Uint8>(HairXCOM1::Face + 5));
				dim.beg_y = 9;
				dim.end_y = 10;
				head.setDomain(dim);
				ShaderDraw<HairXCOM1>(head, ShaderScalar<Uint8>(HairXCOM1::Face + 6));
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
				ShaderDraw<HairXCOM1>(head, ShaderScalar<Uint8>(HairXCOM1::Face + 6));
				surf->unlock();
			}
		}

		//all TFTD armors
		name = "TDXCOM_?.PCK";
		for (int j = 0; j < 3; ++j)
		{
			name[7] = '0' + j;
			if (_sets.find(name) != _sets.end())
			{
				SurfaceSet *xcom_2 = _sets[name];
				for (int i = 0; i < 16; ++i)
				{
					//chest frame without helm
					Surface *surf = xcom_2->getFrame(262 + i);
					surf->lock();
					if (i < 8)
					{
						//female chest frame
						ShaderMove<Uint8> head = ShaderMove<Uint8>(surf);
						GraphSubset dim = head.getBaseDomain();
						dim.beg_y = 6;
						dim.end_y = 18;
						head.setDomain(dim);
						ShaderDraw<HairXCOM2>(head);

						if (j == 2)
						{
							//fix some pixels in ION armor that was overwrite by previous function
							if (i == 0)
							{
								surf->setPixel(18, 14, 16);
							}
							else if (i == 3)
							{
								surf->setPixel(19, 12, 20);
							}
							else if (i == 6)
							{
								surf->setPixel(13, 14, 16);
							}
						}
					}

					//we change face to pink, to prevent mixup with ION armor backpack that have same color group.
					ShaderDraw<FaceXCOM2>(ShaderMove<Uint8>(surf));
					surf->unlock();
				}

				for (int i = 0; i < 2; ++i)
				{
					//fall frame (first and second)
					Surface *surf = xcom_2->getFrame(256 + i);
					surf->lock();

					ShaderMove<Uint8> head = ShaderMove<Uint8>(surf);
					GraphSubset dim = head.getBaseDomain();
					dim.beg_y = 0;
					if (j == 3)
					{
						dim.end_y = 11 + 5 * i;
					}
					else
					{
						dim.end_y = 17;
					}
					head.setDomain(dim);
					ShaderDraw<FallXCOM2>(head);

					//we change face to pink, to prevent mixup with ION armor backpack that have same color group.
					ShaderDraw<FaceXCOM2>(ShaderMove<Uint8>(surf));
					surf->unlock();
				}

				//Palette fix for ION armor
				if (j == 2)
				{
					int size = xcom_2->getTotalFrames();
					for (int i = 0; i < size; ++i)
					{
						Surface *surf = xcom_2->getFrame(i);
						surf->lock();
						ShaderDraw<BodyXCOM2>(ShaderMove<Uint8>(surf));
						surf->unlock();
					}
				}
			}
		}
	}
}

/**
 * Loads the extra resources defined in rulesets.
 */
void Mod::loadExtraResources()
{
	// Load fonts
	YAML::Node doc = YAML::LoadFile(FileMap::getFilePath("Language/" + _fontName));
	Log(LOG_INFO) << "Loading font... " << _fontName;
	Font::setIndex(Language::utf8ToWstr(doc["chars"].as<std::string>()));
	for (YAML::const_iterator i = doc["fonts"].begin(); i != doc["fonts"].end(); ++i)
	{
		std::string id = (*i)["id"].as<std::string>();
		Font *font = new Font();
		font->load(*i);
		_fonts[id] = font;
	}

#ifndef __NO_MUSIC
	// Load musics
	if (!Options::mute)
	{
		const std::set<std::string> &soundFiles(FileMap::getVFolderContents("SOUND"));

		// Check which music version is available
		CatFile *adlibcat = 0, *aintrocat = 0;
		GMCatFile *gmcat = 0;

		for (std::set<std::string>::iterator i = soundFiles.begin(); i != soundFiles.end(); ++i)
		{
			if (0 == i->compare("adlib.cat"))
			{
				adlibcat = new CatFile(FileMap::getFilePath("SOUND/" + *i).c_str());
			}
			else if (0 == i->compare("aintro.cat"))
			{
				aintrocat = new CatFile(FileMap::getFilePath("SOUND/" + *i).c_str());
			}
			else if (0 == i->compare("gm.cat"))
			{
				gmcat = new GMCatFile(FileMap::getFilePath("SOUND/" + *i).c_str());
			}
		}

		// Try the preferred format first, otherwise use the default priority
		MusicFormat priority[] = { Options::preferredMusic, MUSIC_FLAC, MUSIC_OGG, MUSIC_MP3, MUSIC_MOD, MUSIC_WAV, MUSIC_ADLIB, MUSIC_MIDI };
		for (std::map<std::string, RuleMusic *>::const_iterator i = _musicDefs.begin(); i != _musicDefs.end(); ++i)
		{
			Music *music = 0;
			for (size_t j = 0; j < sizeof(priority) / sizeof(priority[0]) && music == 0; ++j)
			{
				music = loadMusic(priority[j], (*i).first, (*i).second->getCatPos(), (*i).second->getNormalization(), adlibcat, aintrocat, gmcat);
			}
			if (music)
			{
				_musics[(*i).first] = music;
			}

		}

		delete gmcat;
		delete adlibcat;
		delete aintrocat;
	}
#endif

	Log(LOG_INFO) << "Loading extra resources from ruleset...";
	for (std::vector< std::pair<std::string, ExtraSprites *> >::const_iterator i = _extraSprites.begin(); i != _extraSprites.end(); ++i)
	{
		std::string sheetName = i->first;
		ExtraSprites *spritePack = i->second;
		bool subdivision = (spritePack->getSubX() != 0 && spritePack->getSubY() != 0);
		if (spritePack->getSingleImage())
		{
			if (_surfaces.find(sheetName) == _surfaces.end())
			{
				Log(LOG_VERBOSE) << "Creating new single image: " << sheetName;
				_surfaces[sheetName] = new Surface(spritePack->getWidth(), spritePack->getHeight());
			}
			else
			{
				Log(LOG_VERBOSE) << "Adding/Replacing single image: " << sheetName;
				delete _surfaces[sheetName];
				_surfaces[sheetName] = new Surface(spritePack->getWidth(), spritePack->getHeight());
			}
			_surfaces[sheetName]->loadImage(FileMap::getFilePath((*spritePack->getSprites())[0]));
		}
		else
		{
			bool adding = false;
			if (_sets.find(sheetName) == _sets.end())
			{
				Log(LOG_VERBOSE) << "Creating new surface set: " << sheetName;
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
				Log(LOG_VERBOSE) << "Adding/Replacing items in surface set: " << sheetName;
			}

			if (subdivision)
			{
				int frames = (spritePack->getWidth() / spritePack->getSubX())*(spritePack->getHeight() / spritePack->getSubY());
				Log(LOG_VERBOSE) << "Subdividing into " << frames << " frames.";
			}

			for (std::map<int, std::string>::iterator j = spritePack->getSprites()->begin(); j != spritePack->getSprites()->end(); ++j)
			{
				int startFrame = j->first;
				std::string fileName = j->second;
				if (fileName.substr(fileName.length() - 1, 1) == "/")
				{
					Log(LOG_VERBOSE) << "Loading surface set from folder: " << fileName << " starting at frame: " << startFrame;
					int offset = startFrame;
					std::set<std::string> contents = FileMap::getVFolderContents(fileName);
					for (std::set<std::string>::iterator k = contents.begin(); k != contents.end(); ++k)
					{
						if (!isImageFile((*k).substr((*k).length() - 4, (*k).length())))
							continue;
						try
						{
							std::string fullPath = FileMap::getFilePath(fileName + *k);
							if (_sets[sheetName]->getFrame(offset))
							{
								Log(LOG_VERBOSE) << "Replacing frame: " << offset;
								_sets[sheetName]->getFrame(offset)->loadImage(fullPath);
							}
							else
							{
								if (adding)
								{
									_sets[sheetName]->addFrame(offset)->loadImage(fullPath);
								}
								else
								{
									Log(LOG_VERBOSE) << "Adding frame: " << offset + spritePack->getModIndex();
									_sets[sheetName]->addFrame(offset + spritePack->getModIndex())->loadImage(fullPath);
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
						std::string fullPath = FileMap::getFilePath(fileName);
						if (_sets[sheetName]->getFrame(startFrame))
						{
							Log(LOG_VERBOSE) << "Replacing frame: " << startFrame;
							_sets[sheetName]->getFrame(startFrame)->loadImage(fullPath);
						}
						else
						{
							Log(LOG_VERBOSE) << "Adding frame: " << startFrame << ", using index: " << startFrame + spritePack->getModIndex();
							_sets[sheetName]->addFrame(startFrame + spritePack->getModIndex())->loadImage(fullPath);
						}
					}
					else
					{
						Surface *temp = new Surface(spritePack->getWidth(), spritePack->getHeight());
						temp->loadImage(FileMap::getFilePath((*spritePack->getSprites())[startFrame]));
						int xDivision = spritePack->getWidth() / spritePack->getSubX();
						int yDivision = spritePack->getHeight() / spritePack->getSubY();
						int offset = startFrame;

						for (int y = 0; y != yDivision; ++y)
						{
							for (int x = 0; x != xDivision; ++x)
							{
								if (_sets[sheetName]->getFrame(offset))
								{
									Log(LOG_VERBOSE) << "Replacing frame: " << offset;
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
										Log(LOG_VERBOSE) << "Adding frame: " << offset + spritePack->getModIndex();
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

	for (std::vector< std::pair<std::string, ExtraSounds *> >::const_iterator i = _extraSounds.begin(); i != _extraSounds.end(); ++i)
	{
		std::string setName = i->first;
		ExtraSounds *soundPack = i->second;
		if (_sounds.find(setName) == _sounds.end())
		{
			Log(LOG_VERBOSE) << "Creating new sound set: " << setName << ", this will likely have no in-game use.";
			_sounds[setName] = new SoundSet();
		}
		else Log(LOG_VERBOSE) << "Adding/Replacing items in sound set: " << setName;
		for (std::map<int, std::string>::iterator j = soundPack->getSounds()->begin(); j != soundPack->getSounds()->end(); ++j)
		{
			int startSound = j->first;
			std::string fileName = j->second;
			if (fileName.substr(fileName.length() - 1, 1) == "/")
			{
				Log(LOG_VERBOSE) << "Loading sound set from folder: " << fileName << " starting at index: " << startSound;
				int offset = startSound;
				std::set<std::string> contents = FileMap::getVFolderContents(fileName);
				for (std::set<std::string>::iterator k = contents.begin(); k != contents.end(); ++k)
				{
					try
					{
						std::string fullPath = FileMap::getFilePath(fileName + *k);
						if (_sounds[setName]->getSound(offset))
						{
							_sounds[setName]->getSound(offset)->load(fullPath);
						}
						else
						{
							_sounds[setName]->addSound(offset + soundPack->getModIndex())->load(fullPath);
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
				std::string fullPath = FileMap::getFilePath(fileName);
				if (_sounds[setName]->getSound(startSound))
				{
					Log(LOG_VERBOSE) << "Replacing index: " << startSound;
					_sounds[setName]->getSound(startSound)->load(fullPath);
				}
				else
				{
					Log(LOG_VERBOSE) << "Adding index: " << startSound;
					_sounds[setName]->addSound(startSound + soundPack->getModIndex())->load(fullPath);
				}
			}
		}
	}
}

/**
 * Applies necessary modifications to vanilla resources.
 */
void Mod::modResources()
{
	// bigger geoscape background
	int newWidth = 320 - 64, newHeight = 200;
	Surface *newGeo = new Surface(newWidth * 3, newHeight * 3);
	Surface *oldGeo = _surfaces["GEOBORD.SCR"];
	for (int x = 0; x < newWidth; ++x)
	{
		for (int y = 0; y < newHeight; ++y)
		{
			newGeo->setPixel(newWidth + x, newHeight + y, oldGeo->getPixel(x, y));
			newGeo->setPixel(newWidth - x - 1, newHeight + y, oldGeo->getPixel(x, y));
			newGeo->setPixel(newWidth * 3 - x - 1, newHeight + y, oldGeo->getPixel(x, y));

			newGeo->setPixel(newWidth + x, newHeight - y - 1, oldGeo->getPixel(x, y));
			newGeo->setPixel(newWidth - x - 1, newHeight - y - 1, oldGeo->getPixel(x, y));
			newGeo->setPixel(newWidth * 3 - x - 1, newHeight - y - 1, oldGeo->getPixel(x, y));

			newGeo->setPixel(newWidth + x, newHeight * 3 - y - 1, oldGeo->getPixel(x, y));
			newGeo->setPixel(newWidth - x - 1, newHeight * 3 - y - 1, oldGeo->getPixel(x, y));
			newGeo->setPixel(newWidth * 3 - x - 1, newHeight * 3 - y - 1, oldGeo->getPixel(x, y));
		}
	}
	_surfaces["ALTGEOBORD.SCR"] = newGeo;

	// here we create an "alternate" background surface for the base info screen.
	_surfaces["ALTBACK07.SCR"] = new Surface(320, 200);
	_surfaces["ALTBACK07.SCR"]->loadScr(FileMap::getFilePath("GEOGRAPH/BACK07.SCR"));
	for (int y = 172; y >= 152; --y)
		for (int x = 5; x <= 314; ++x)
			_surfaces["ALTBACK07.SCR"]->setPixel(x, y + 4, _surfaces["ALTBACK07.SCR"]->getPixel(x, y));
	for (int y = 147; y >= 134; --y)
		for (int x = 5; x <= 314; ++x)
			_surfaces["ALTBACK07.SCR"]->setPixel(x, y + 9, _surfaces["ALTBACK07.SCR"]->getPixel(x, y));
	for (int y = 132; y >= 109; --y)
		for (int x = 5; x <= 314; ++x)
			_surfaces["ALTBACK07.SCR"]->setPixel(x, y + 10, _surfaces["ALTBACK07.SCR"]->getPixel(x, y));

	// we create extra rows on the soldier stat screens by shrinking them all down one pixel.
	// this is done after loading them, but BEFORE loading the extraSprites, in case a modder wants to replace them.

	// first, let's do the base info screen
	// erase the old lines, copying from a +2 offset to account for the dithering
	for (int y = 91; y < 199; y += 12)
		for (int x = 0; x < 149; ++x)
			_surfaces["BACK06.SCR"]->setPixel(x, y, _surfaces["BACK06.SCR"]->getPixel(x, y + 2));
	// drawn new lines, use the bottom row of pixels as a basis
	for (int y = 89; y < 199; y += 11)
		for (int x = 0; x < 149; ++x)
			_surfaces["BACK06.SCR"]->setPixel(x, y, _surfaces["BACK06.SCR"]->getPixel(x, 199));
	// finally, move the top of the graph up by one pixel, offset for the last iteration again due to dithering.
	for (int y = 72; y < 80; ++y)
		for (int x = 0; x < 320; ++x)
		{
			_surfaces["BACK06.SCR"]->setPixel(x, y, _surfaces["BACK06.SCR"]->getPixel(x, y + (y == 79 ? 2 : 1)));
		}

	// now, let's adjust the battlescape info screen.
	// erase the old lines, no need to worry about dithering on this one.
	for (int y = 39; y < 199; y += 10)
		for (int x = 0; x < 169; ++x)
			_surfaces["UNIBORD.PCK"]->setPixel(x, y, _surfaces["UNIBORD.PCK"]->getPixel(x, 30));
	// drawn new lines, use the bottom row of pixels as a basis
	for (int y = 190; y > 37; y -= 9)
		for (int x = 0; x < 169; ++x)
			_surfaces["UNIBORD.PCK"]->setPixel(x, y, _surfaces["UNIBORD.PCK"]->getPixel(x, 199));
	// move the top of the graph down by eight pixels to erase the row we don't need (we actually created ~1.8 extra rows earlier)
	for (int y = 37; y > 29; --y)
		for (int x = 0; x < 320; ++x)
		{
			_surfaces["UNIBORD.PCK"]->setPixel(x, y, _surfaces["UNIBORD.PCK"]->getPixel(x, y - 8));
			_surfaces["UNIBORD.PCK"]->setPixel(x, y - 8, 0);
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
}

/**
 * Determines if an image file is an acceptable format for the game.
 * @param extension Image file extension.
 * @return True/false
 */
bool Mod::isImageFile(std::string extension) const
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
Music *Mod::loadMusic(MusicFormat fmt, const std::string &file, int track, float volume, CatFile *adlibcat, CatFile *aintrocat, GMCatFile *gmcat) const
{
	/* MUSIC_AUTO, MUSIC_FLAC, MUSIC_OGG, MUSIC_MP3, MUSIC_MOD, MUSIC_WAV, MUSIC_ADLIB, MUSIC_MIDI */
	static const std::string exts[] = { "", ".flac", ".ogg", ".mp3", ".mod", ".wav", "", ".mid" };
	Music *music = 0;
	std::set<std::string> soundContents = FileMap::getVFolderContents("SOUND");
	try
	{
		std::string fname = file + exts[fmt];
		std::transform(fname.begin(), fname.end(), fname.begin(), tolower);

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
					if (track < aintrocat->getAmount())
					{
						music->load(aintrocat->load(track, true), aintrocat->getObjectSize(track));
					}
					else
					{
						delete music;
						music = 0;
					}
				}
			}
		}
		// Try MIDI music
		else if (fmt == MUSIC_MIDI)
		{
			// DOS MIDI
			if (gmcat && track < gmcat->getAmount())
			{
				music = gmcat->loadMIDI(track);
			}
			// Windows MIDI
			else
			{
				if (soundContents.find(fname) != soundContents.end())
				{
					music = new Music();
					music->load(FileMap::getFilePath("SOUND/" + fname));
				}
			}
		}
		// Try digital tracks
		else
		{
			if (soundContents.find(fname) != soundContents.end())
			{
				music = new Music();
				music->load(FileMap::getFilePath("SOUND/" + fname));
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

/**
 * Preamble:
 * this is the most horrible function i've ever written, and it makes me sad.
 * this is, however, a necessary evil, in order to save massive amounts of time in the draw function.
 * when used with the default TFTD mod, this function loops 4,194,304 times
 * (4 palettes, 4 tints, 4 levels of opacity, 256 colors, 256 comparisons per)
 * each additional tint in the rulesets will result in over a million iterations more.
 * @param pal the palette to base the lookup table on.
 */
void Mod::createTransparencyLUT(Palette *pal)
{
	SDL_Color desiredColor;
	std::vector<Uint8> lookUpTable;
	// start with the color sets
	for (std::vector<SDL_Color>::const_iterator tint = _transparencies.begin(); tint != _transparencies.end(); ++tint)
	{
		// then the opacity levels, using the alpha channel as the step
		for (int opacity = 1; opacity < 1 + tint->unused * 4; opacity += tint->unused)
		{
			// then the palette itself
			for (int currentColor = 0; currentColor < 256; ++currentColor)
			{
				// add the RGB values from the ruleset to those of the colors contained in the palette
				// in order to determine the desired color
				// yes all this casting and clamping is required, we're dealing with Uint8s here, and there's
				// a lot of potential for values to wrap around, which would be very bad indeed.
				desiredColor.r = std::min(255, (int)(pal->getColors(currentColor)->r) + (tint->r * opacity));
				desiredColor.g = std::min(255, (int)(pal->getColors(currentColor)->g) + (tint->g * opacity));
				desiredColor.b = std::min(255, (int)(pal->getColors(currentColor)->b) + (tint->b * opacity));

				Uint8 closest = 0;
				int lowestDifference = INT_MAX;
				// now compare each color in the palette to find the closest match to our desired one
				for (int comparator = 0; comparator < 256; ++comparator)
				{
					int currentDifference = Sqr(desiredColor.r - pal->getColors(comparator)->r) +
						Sqr(desiredColor.g - pal->getColors(comparator)->g) +
						Sqr(desiredColor.b - pal->getColors(comparator)->b);

					if (currentDifference < lowestDifference)
					{
						closest = comparator;
						lowestDifference = currentDifference;
					}
				}
				lookUpTable.push_back(closest);
			}
		}
	}
	_transparencyLUTs.push_back(lookUpTable);
}

StatAdjustment *Mod::getStatAdjustment(int difficulty)
{
	if (difficulty >= 4)
	{
		return &_statAdjustment[4];
	}
	return &_statAdjustment[difficulty];
}

}
