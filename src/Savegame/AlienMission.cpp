/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#define _USE_MATH_DEFINES
#include "AlienMission.h"
#include "AlienBase.h"
#include "Base.h"
#include "../fmath.h"
#include "../Engine/Exception.h"
#include "../Engine/Game.h"
#include "../Engine/Logger.h"
#include "../Engine/RNG.h"
#include "../Geoscape/Globe.h"
#include "../Ruleset/RuleAlienMission.h"
#include "../Ruleset/RuleRegion.h"
#include "../Ruleset/RuleCountry.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleUfo.h"
#include "../Ruleset/UfoTrajectory.h"
#include "../Ruleset/RuleGlobe.h"
#include "../Ruleset/Texture.h"
#include "SavedGame.h"
#include "MissionSite.h"
#include "Ufo.h"
#include "Craft.h"
#include "Region.h"
#include "Country.h"
#include "Waypoint.h"
#include <assert.h>
#include <algorithm>
#include <functional>
#include <math.h>
#include "../Ruleset/AlienDeployment.h"

namespace OpenXcom
{

AlienMission::AlienMission(const RuleAlienMission &rule) : _rule(rule), _nextWave(0), _nextUfoCounter(0), _spawnCountdown(0), _liveUfos(0), _uniqueID(0), _base(0)
{
	// Empty by design.
}

AlienMission::~AlienMission()
{
	// Empty by design.
}

class matchById: public std::unary_function<const AlienBase *, bool>
{
public:
	/// Remember ID.
	matchById(int id) : _id(id) { /* Empty by design. */ }
	/// Match with stored ID.
	bool operator()(const AlienBase *ab) const { return ab->getId() == _id; }
private:
	int _id;
};

/**
 * Loads the alien mission from a YAML file.
 * @param node The YAML node containing the data.
 * @param game The game data, required to locate the alien base.
 */
void AlienMission::load(const YAML::Node& node, SavedGame &game)
{
	_region = node["region"].as<std::string>(_region);
	_race = node["race"].as<std::string>(_race);
	_nextWave = node["nextWave"].as<size_t>(_nextWave);
	_nextUfoCounter = node["nextUfoCounter"].as<size_t>(_nextUfoCounter);
	_spawnCountdown = node["spawnCountdown"].as<size_t>(_spawnCountdown);
	_liveUfos = node["liveUfos"].as<size_t>(_liveUfos);
	_uniqueID = node["uniqueID"].as<int>(_uniqueID);
	if (const YAML::Node &base = node["alienBase"])
	{
		int id = base.as<int>();
		std::vector<AlienBase*>::const_iterator found = std::find_if (game.getAlienBases()->begin(), game.getAlienBases()->end(), matchById(id));
		if (found == game.getAlienBases()->end())
		{
			throw Exception("Corrupted save: Invalid base for mission.");
		}
		_base = *found;
	}

}

/**
 * Saves the alien mission to a YAML file.
 * @return YAML node.
 */
YAML::Node AlienMission::save() const
{
	YAML::Node node;
	node["type"] = _rule.getType();
	node["region"] = _region;
	node["race"] = _race;
	node["nextWave"] = _nextWave;
	node["nextUfoCounter"] = _nextUfoCounter;
	node["spawnCountdown"] = _spawnCountdown;
	node["liveUfos"] = _liveUfos;
	node["uniqueID"] = _uniqueID;
	if (_base)
	{
		node["alienBase"] = _base->getId();
	}
	return node;
}

/**
 * Check if a mission is over and can be safely removed from the game.
 * A mission is over if it will spawn no more UFOs and it has no UFOs still in
 * the game.
 * @return If the mission can be safely removed from game.
 */
bool AlienMission::isOver() const
{
	if (_rule.getObjective() == OBJECTIVE_INFILTRATION)
	{
		//Infiltrations continue for ever.
		return false;
	}
	if (_nextWave == _rule.getWaveCount() && !_liveUfos)
	{
		return true;
	}
	return false;
}

/**
 * Find an XCOM base in this region that is marked for retaliation.
 */
class FindMarkedXCOMBase: public std::unary_function<const Base *, bool>
{
public:
	FindMarkedXCOMBase(const RuleRegion &region) : _region(region) { /* Empty by design. */ }
	bool operator()(const Base *base) const { return (_region.insideRegion(base->getLongitude(), base->getLatitude()) && base->getRetaliationTarget()); }
private:
	const RuleRegion &_region;
};

void AlienMission::think(Game &engine, const Globe &globe)
{
	const Ruleset &ruleset = *engine.getRuleset();
	SavedGame &game = *engine.getSavedGame();
	if (_nextWave >= _rule.getWaveCount())
		return;
	if (_spawnCountdown > 30)
	{
		_spawnCountdown -= 30;
		return;
	}
	const MissionWave &wave = _rule.getWave(_nextWave);
	const UfoTrajectory &trajectory = *ruleset.getUfoTrajectory(wave.trajectory);
	Ufo *ufo = spawnUfo(game, ruleset, globe, wave, trajectory);
	if (ufo)
	{
		//Some missions may not spawn a UFO!
		game.getUfos()->push_back(ufo);
	}
	++_nextUfoCounter;
	if (_nextUfoCounter == wave.ufoCount)
	{
		_nextUfoCounter = 0;
		++_nextWave;
	}
	if (_rule.getObjective() == OBJECTIVE_INFILTRATION && _nextWave == _rule.getWaveCount())
	{
		for (std::vector<Country*>::iterator c = game.getCountries()->begin(); c != game.getCountries()->end(); ++c)
		{
			if (!(*c)->getPact() && !(*c)->getNewPact() && ruleset.getRegion(_region)->insideRegion((*c)->getRules()->getLabelLongitude(), (*c)->getRules()->getLabelLatitude()))
			{
				(*c)->setNewPact();
				spawnAlienBase(globe, engine, _rule.getSpawnZone());
				break;
			}
		}


		// Infiltrations loop for ever.
		_nextWave = 0;
	}
	if (_rule.getObjective() == OBJECTIVE_BASE && _nextWave == _rule.getWaveCount())
	{
		spawnAlienBase(globe, engine, _rule.getSpawnZone());
	}
	if (_nextWave != _rule.getWaveCount())
	{
		size_t spawnTimer = _rule.getWave(_nextWave).spawnTimer / 30;
		_spawnCountdown = (spawnTimer/2 + RNG::generate(0, spawnTimer)) * 30;
	}
}

/**
 * This function will spawn a UFO according the the mission rules.
 * Some code is duplicated between cases, that's ok for now. It's on different
 * code paths and the function is MUCH easier to read written this way.
 * @param game The saved game information.
 * @param ruleset The ruleset.
 * @param globe The globe, for land checks.
 * @param wave The wave for the desired UFO.
 * @param trajectory The rule for the desired trajectory.
 * @return Pointer to the spawned UFO. If the mission does not desire to spawn a UFO, 0 is returned.
 */
Ufo *AlienMission::spawnUfo(const SavedGame &game, const Ruleset &ruleset, const Globe &globe, const MissionWave &wave, const UfoTrajectory &trajectory)
{
	RuleUfo &ufoRule = *ruleset.getUfo(wave.ufoType);
	if (_rule.getObjective() == OBJECTIVE_RETALIATION)
	{
		const RuleRegion &regionRules = *ruleset.getRegion(_region);
		std::vector<Base *>::const_iterator found =
		    std::find_if (game.getBases()->begin(), game.getBases()->end(),
				 FindMarkedXCOMBase(regionRules));
		if (found != game.getBases()->end())
		{
			// Spawn a battleship straight for the XCOM base.
			const RuleUfo &battleshipRule = *ruleset.getUfo(_rule.getSpawnUfo());
			const UfoTrajectory &assaultTrajectory = *ruleset.getUfoTrajectory("__RETALIATION_ASSAULT_RUN");
			Ufo *ufo = new Ufo(&battleshipRule);
			ufo->setMissionInfo(this, &assaultTrajectory);
			std::pair<double, double> pos;
			if (trajectory.getAltitude(0) == "STR_GROUND")
			{
				pos = getLandPoint(globe, regionRules, trajectory.getZone(0));
			}
			else
			{
				pos = regionRules.getRandomPoint(trajectory.getZone(0));
			}
			ufo->setAltitude(assaultTrajectory.getAltitude(0));
			ufo->setSpeed(assaultTrajectory.getSpeedPercentage(0) * battleshipRule.getMaxSpeed());
			ufo->setLongitude(pos.first);
			ufo->setLatitude(pos.second);
			Waypoint *wp = new Waypoint();
			wp->setLongitude((*found)->getLongitude());
			wp->setLatitude((*found)->getLatitude());
			ufo->setDestination(wp);
			return ufo;
		}
	}
	else if (_rule.getObjective() == OBJECTIVE_SUPPLY)
	{
		if (wave.objective && !_base)
		{
			// No base to supply!
			return 0;
		}
		// Our destination is always an alien base.
		Ufo *ufo = new Ufo(&ufoRule);
		ufo->setMissionInfo(this, &trajectory);
		const RuleRegion &regionRules = *ruleset.getRegion(_region);
		std::pair<double, double> pos;
		if (trajectory.getAltitude(0) == "STR_GROUND")
		{
			pos = getLandPoint(globe, regionRules, trajectory.getZone(0));
		}
		else
		{
			pos = regionRules.getRandomPoint(trajectory.getZone(0));
		}
		ufo->setAltitude(trajectory.getAltitude(0));
		ufo->setSpeed(trajectory.getSpeedPercentage(0) * ufoRule.getMaxSpeed());
		ufo->setLongitude(pos.first);
		ufo->setLatitude(pos.second);
		Waypoint *wp = new Waypoint();
		if (trajectory.getAltitude(1) == "STR_GROUND")
		{
			if (wave.objective)
			{
				// Supply ships on supply missions land on bases, ignore trajectory zone.
				pos.first = _base->getLongitude();
				pos.second = _base->getLatitude();
			}
			else
			{
				// Other ships can land where they want.
				pos = getLandPoint(globe, regionRules, trajectory.getZone(1));
			}
		}
		else
		{
			pos = regionRules.getRandomPoint(trajectory.getZone(1));
		}
		wp->setLongitude(pos.first);
		wp->setLatitude(pos.second);
		ufo->setDestination(wp);
		return ufo;
	}
	// Spawn according to sequence.
	Ufo *ufo = new Ufo(&ufoRule);
	ufo->setMissionInfo(this, &trajectory);
	const RuleRegion &regionRules = *ruleset.getRegion(_region);
	std::pair<double, double> pos = getWaypoint(trajectory, 0, globe, regionRules);
	ufo->setAltitude(trajectory.getAltitude(0));
	if (trajectory.getAltitude(0) == "STR_GROUND")
	{
		ufo->setSecondsRemaining(trajectory.groundTimer()*5);
	}
	ufo->setSpeed(trajectory.getSpeedPercentage(0) * ufoRule.getMaxSpeed());
	ufo->setLongitude(pos.first);
	ufo->setLatitude(pos.second);
	Waypoint *wp = new Waypoint();
	pos = getWaypoint(trajectory, 1, globe, regionRules);
	wp->setLongitude(pos.first);
	wp->setLatitude(pos.second);
	ufo->setDestination(wp);
	return ufo;
}

void AlienMission::start(size_t initialCount)
{
	_nextWave = 0;
	_nextUfoCounter = 0;
	_liveUfos = 0;
	if (initialCount == 0)
	{
		size_t spawnTimer = _rule.getWave(0).spawnTimer / 30;
		_spawnCountdown = (spawnTimer / 2 + RNG::generate(0, spawnTimer)) * 30;
	}
	else
	{
		_spawnCountdown = initialCount;
	}
}

/** @brief Match a base from it's coordinates.
 * This function object uses coordinates to match a base.
 */
class MatchBaseCoordinates: public std::unary_function<const Base *, bool>
{
public:
	/// Remember the query coordinates.
	MatchBaseCoordinates(double lon, double lat) : _lon(lon), _lat(lat) { /* Empty by design. */ }
	/// Match with base's coordinates.
	bool operator()(const Base *base) const { return AreSame(base->getLongitude(), _lon) && AreSame(base->getLatitude(), _lat); }
private:
	double _lon, _lat;
};

/**
 * This function is called when one of the mission's UFOs arrives at it's current destination.
 * It takes care of sending the UFO to the next waypoint, landing UFOs and
 * marking them for removal as required. It must set the game data in a way that the rest of the code
 * understands what to do.
 * @param ufo The UFO that reached it's waypoint.
 * @param engine The game engine, required to get access to game data and game rules.
 * @param globe The earth globe, required to get access to land checks.
 */
void AlienMission::ufoReachedWaypoint(Ufo &ufo, Game &engine, const Globe &globe)
{
	const Ruleset &rules = *engine.getRuleset();
	SavedGame &game = *engine.getSavedGame();
	const size_t curWaypoint = ufo.getTrajectoryPoint();
	const size_t nextWaypoint = curWaypoint + 1;
	const UfoTrajectory &trajectory = ufo.getTrajectory();
	const MissionWave &wave = _rule.getWave(_nextWave - 1);
	if (nextWaypoint >= trajectory.getWaypointCount())
	{
		ufo.setDetected(false);
		ufo.setStatus(Ufo::DESTROYED);
		return;
	}
	ufo.setAltitude(trajectory.getAltitude(nextWaypoint));
	ufo.setTrajectoryPoint(nextWaypoint);
	const RuleRegion &regionRules = *rules.getRegion(_region);
	std::pair<double, double> pos = getWaypoint(trajectory, nextWaypoint, globe, regionRules);

	Waypoint *wp = new Waypoint();
	wp->setLongitude(pos.first);
	wp->setLatitude(pos.second);
	ufo.setDestination(wp);
	if (ufo.getAltitude() != "STR_GROUND")
	{
		if (ufo.getLandId() != 0)
		{
			ufo.setLandId(0);
		}
		// Set next waypoint.
		ufo.setSpeed((int)(ufo.getRules()->getMaxSpeed() * trajectory.getSpeedPercentage(nextWaypoint)));
	}
	else
	{
		// UFO landed.
		if (wave.objective && trajectory.getZone(curWaypoint) == _rule.getSpawnZone())
		{
			// Remove UFO, replace with MissionSite.
			addScore(ufo.getLongitude(), ufo.getLatitude(), game);
			ufo.setStatus(Ufo::DESTROYED);

			MissionArea area = regionRules.getMissionPoint(trajectory.getZone(curWaypoint), &ufo);
			Texture *texture = rules.getGlobe()->getTexture(area.texture);
			AlienDeployment *deployment = rules.getDeployment(texture->getDeployment());
			
			MissionSite *missionSite = new MissionSite(&_rule, deployment);
			missionSite->setLongitude(ufo.getLongitude());
			missionSite->setLatitude(ufo.getLatitude());
			missionSite->setId(game.getId(deployment->getMarkerName()));
			missionSite->setSecondsRemaining(RNG::generate(deployment->getDurationMin(), deployment->getDurationMax()) * 3600);
			missionSite->setAlienRace(_race);
			missionSite->setTexture(area.texture);
			missionSite->setCity(area.name);
			game.getMissionSites()->push_back(missionSite);
			for (std::vector<Target*>::iterator t = ufo.getFollowers()->begin(); t != ufo.getFollowers()->end();)
			{
				Craft* c = dynamic_cast<Craft*>(*t);
				if (c && c->getNumSoldiers() != 0)
				{
					c->setDestination(missionSite);
					t = ufo.getFollowers()->begin();
				}
				else
				{
					++t;
				}
			}
		}
		else if (trajectory.getID() == "__RETALIATION_ASSAULT_RUN")
		{
			// Ignore what the trajectory might say, this is a base assault.
			// Remove UFO, replace with Base defense.
			ufo.setDetected(false);
			std::vector<Base *>::const_iterator found =
			    std::find_if (game.getBases()->begin(), game.getBases()->end(),
					 MatchBaseCoordinates(ufo.getLongitude(), ufo.getLatitude()));
			if (found == game.getBases()->end())
			{
				ufo.setStatus(Ufo::DESTROYED);
				// Only spawn mission if the base is still there.
				return;
			}
			ufo.setDestination(*found);
		}
		else
		{
			// Set timer for UFO on the ground.
			ufo.setSecondsRemaining(trajectory.groundTimer()*5);
			if (ufo.getDetected() && ufo.getLandId() == 0)
			{
				ufo.setLandId(engine.getSavedGame()->getId("STR_LANDING_SITE"));
			}
		}
	}
}

/**
 * This function is called when one of the mission's UFOs is shot down (crashed or destroyed).
 * Currently the only thing that happens is delaying the next UFO in the mission sequence.
 * @param ufo The UFO that was shot down.
 */
void AlienMission::ufoShotDown(Ufo &ufo)
{
	switch (ufo.getStatus())
	{
	case Ufo::FLYING:
	case Ufo::LANDED:
		assert(0 && "Ufo seems ok!");
		break;
	case Ufo::CRASHED:
	case Ufo::DESTROYED:
		if (_nextWave != _rule.getWaveCount())
		{
			// Delay next wave
			_spawnCountdown += 30 * (RNG::generate(0, 48) + 400);
		}
		break;
	}
}

/**
 * This function is called when one of the mission's UFOs has finished it's time on the ground.
 * It takes care of sending the UFO to the next waypoint and marking them for removal as required.
 * It must set the game data in a way that the rest of the code understands what to do.
 * @param ufo The UFO that reached it's waypoint.
 * @param game The saved game information.
 * @param globe The earth globe, required to get access to land checks.
 */
void AlienMission::ufoLifting(Ufo &ufo, SavedGame &game, const Globe &globe)
{
	switch (ufo.getStatus())
	{
	case Ufo::FLYING:
		assert(0 && "Ufo is already on the air!");
		break;
	case Ufo::LANDED:
		{
			// base missions only get points when they are completed.
			if (_rule.getPoints() > 0 && _rule.getObjective() != OBJECTIVE_BASE)
			{
				addScore(ufo.getLongitude(), ufo.getLatitude(), game);
			}
			ufo.setAltitude("STR_VERY_LOW");
			ufo.setSpeed((int)(ufo.getRules()->getMaxSpeed() * ufo.getTrajectory().getSpeedPercentage(ufo.getTrajectoryPoint())));
		}
		break;
	case Ufo::CRASHED:
		// Mission expired
		ufo.setDetected(false);
		ufo.setStatus(Ufo::DESTROYED);
		break;
	case Ufo::DESTROYED:
		assert(0 && "UFO can't fly!");
		break;
	}
}

/**
 * The new time must be a multiple of 30 minutes, and more than 0.
 * Calling this on a finished mission has no effect.
 * @param minutes The minutes until the next UFO wave will spawn.
 */
void AlienMission::setWaveCountdown(size_t minutes)
{
	assert(minutes != 0 && minutes % 30 == 0);
	if (isOver())
	{
		return;
	}
	_spawnCountdown = minutes;
}

/**
 * Assigns a unique ID to this mission.
 * It is an error to assign two IDs to the same mission.
 * @param id The UD to assign.
 */
void AlienMission::setId(int id)
{
	assert(_uniqueID == 0 && "Reassigning ID!");
	_uniqueID = id;
}

/**
 * @return The unique ID assigned to this mission.
 */
int AlienMission::getId() const
{
	assert(_uniqueID != 0 && "Uninitalized mission!");
	return _uniqueID;
}

/**
 * Sets the alien base associated with this mission.
 * Only the alien supply missions care about this.
 * @param base A pointer to an alien base.
 */
void AlienMission::setAlienBase(const AlienBase *base)
{
	_base = base;
}

/**
 * Only alien supply missions ever have a valid pointer.
 * @return A pointer (possibly 0) of the AlienBase for this mission.
 */
const AlienBase *AlienMission::getAlienBase() const
{
	return _base;
}

/**
 * Add alien points to the country and region at the coordinates given.
 * @param lon Longitudinal coordinates to check.
 * @param lat Latitudinal coordinates to check.
 * @param game The saved game information.
 */
void AlienMission::addScore(const double lon, const double lat, SavedGame &game)
{
	for (std::vector<Region *>::iterator region = game.getRegions()->begin(); region != game.getRegions()->end(); ++region)
	{
		if ((*region)->getRules()->insideRegion(lon, lat))
		{
			(*region)->addActivityAlien(_rule.getPoints());
			break;
		}
	}
	for (std::vector<Country *>::iterator country = game.getCountries()->begin(); country != game.getCountries()->end(); ++country)
	{
		if ((*country)->getRules()->insideCountry(lon, lat))
		{
			(*country)->addActivityAlien(_rule.getPoints());
			break;
		}
	}
}

/**
 * Spawn an alien base.
 * @param globe The earth globe, required to get access to land checks.
 * @param engine The game engine, required to get access to game data and game rules.
 * @param zone The mission zone, required for determining the base coordinates.
 */
void AlienMission::spawnAlienBase(const Globe &globe, Game &engine, int zone)
{
	SavedGame &game = *engine.getSavedGame();
	const Ruleset &ruleset = *engine.getRuleset();
	// Once the last UFO is spawned, the aliens build their base.
	const RuleRegion &regionRules = *ruleset.getRegion(_region);
	std::pair<double, double> pos = getLandPoint(globe, regionRules, zone);
	AlienBase *ab = new AlienBase();
	ab->setAlienRace(_race);
	ab->setId(game.getId("STR_ALIEN_BASE"));
	ab->setLongitude(pos.first);
	ab->setLatitude(pos.second);
	game.getAlienBases()->push_back(ab);
	addScore(pos.first, pos.second, game);
}

/*
 * Sets the mission's region. if the region is incompatible with
 * actually carrying out an attack, use the "fallback" region as
 * defined in the ruleset.
 * (this is a slight difference from the original, which just
 * defaulted them to zone[0], North America)
 * @param region the region we want to try to set the mission to.
 * @param rules the ruleset, in case we need to swap out the region.
 */
void AlienMission::setRegion(const std::string &region, const Ruleset &rules)
{
	if (!rules.getRegion(region)->getMissionRegion().empty())
	{
		_region = rules.getRegion(region)->getMissionRegion();
	}
	else
	{
		_region = region;
	}
}

/**
 * Select a destination based on the criteria of our trajectory and desired waypoint.
 * @param trajectory the trajectory in question.
 * @param nextWaypoint the next logical waypoint in sequence (0 for newly spawned UFOs)
 * @param globe The earth globe, required to get access to land checks.
 * @param region the ruleset for the region of our mission.
 * @return a set of lon and lat coordinates based on the criteria of the trajectory.
 */
std::pair<double, double> AlienMission::getWaypoint(const UfoTrajectory &trajectory, const size_t nextWaypoint, const Globe &globe, const RuleRegion &region)
{
	/* LOOK MA! NO HANDS!
	if (trajectory.getAltitude(nextWaypoint) == "STR_GROUND")
	{
		return getLandPoint(globe, region, trajectory.getZone(nextWaypoint));
	}
	else
	*/
		return region.getRandomPoint(trajectory.getZone(nextWaypoint));
}

/**
 * Get a random point inside the given region zone.
 * The point will be used to land a UFO, so it HAS to be on land.
 */
std::pair<double, double> AlienMission::getLandPoint(const Globe &globe, const RuleRegion &region, size_t zone)
{
	int tries = 0;
	std::pair<double, double> pos;
	do
	{
		pos = region.getRandomPoint(zone);
		++tries;
	}
	while (!(globe.insideLand(pos.first, pos.second)
		&& region.insideRegion(pos.first, pos.second))
		&& tries < 100);
	if (tries == 100)
	{
		Log(LOG_DEBUG) << "Region: " << region.getType() << " Longitude: " << pos.first << " Lattitude: " << pos.second << " invalid zone: " << zone << " ufo forced to land on water!";
	}
	return pos;

}

}
