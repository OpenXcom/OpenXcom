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
#include "AlienMission.h"
#include "../Battlescape/BattlescapeGenerator.h"
#include "../Battlescape/BriefingState.h"
#include "../Engine/Game.h"
#include "../Engine/Logger.h"
#include "../Engine/RNG.h"
#include "../Geoscape/Globe.h"
#include "../Ruleset/RuleAlienMission.h"
#include "../Ruleset/RuleRegion.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleUfo.h"
#include "../Ruleset/UfoTrajectory.h"
#include "Base.h"
#include "SavedBattleGame.h"
#include "SavedGame.h"
#include "TerrorSite.h"
#include "Ufo.h"
#include "Waypoint.h"
#include <algorithm>
#include <functional>

namespace {
/**
 * Get a random point inside the given region zone.
 * The point will be used to land a UFO, so it HAS to be on land.
 * @note This is only until we fix our zone data.
 */
std::pair<double, double> getLandPoint(const OpenXcom::Globe &globe, const OpenXcom::RuleRegion &region, unsigned zone)
{
	std::pair<double, double> pos;
	do
	{
		pos = region.getRandomPoint(zone);
	}
	while (!globe.insideLand(pos.first, pos.second));
	return pos;

}

}

namespace OpenXcom
{

AlienMission::AlienMission(const RuleAlienMission &rule) : _rule(rule)
{
	// Empty by design.
}

AlienMission::~AlienMission()
{
	// Empty by design.
}

void AlienMission::load(const YAML::Node& node)
{
	node["region"] >> _region;
	node["race"] >> _race;
	node["nextWave"] >> _nextWave;
	node["nextUfoCounter"] >> _nextUfoCounter;
	node["spawnCountdown"] >> _spawnCountdown;
	node["liveUfos"] >> _liveUfos;
}

void AlienMission::save(YAML::Emitter& out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "type" << YAML::Value << _rule.getType();
	out << YAML::Key << "region" << YAML::Value << _region;
	out << YAML::Key << "race" << YAML::Value << _race;
	out << YAML::Key << "nextWave" << YAML::Value << _nextWave;
	out << YAML::Key << "nextUfoCounter" << YAML::Value << _nextUfoCounter;
	out << YAML::Key << "spawnCountdown" << YAML::Value << _spawnCountdown;
	out << YAML::Key << "liveUfos" << YAML::Value << _liveUfos;
	out << YAML::EndMap;
}

/// Saves the mission's ID to YAML.
void AlienMission::saveId(YAML::Emitter& out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "type" << YAML::Value << _rule.getType();
	out << YAML::Key << "region" << YAML::Value << _region;
	out << YAML::EndMap;
}
const std::string &AlienMission::getType() const
{
	return _rule.getType();
}

/**
 * Check if a mission is over and can be safely removed from the game.
 * A mission is over if it will spawn no more UFOs and it has no UFOs still in
 * the game.
 * @return If the mission can be safely removed from game.
 */
bool AlienMission::isOver() const
{
	if (_rule.getType() == "STR_ALIEN_INFILTRATION")
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
	bool operator()(const Base *base) const { return _region.insideRegion(base->getLongitude(), base->getLatitude()); }
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
	RuleUfo &ufoRule = *ruleset.getUfo(wave.ufoType);
	const UfoTrajectory &trajectory = *ruleset.getUfoTrajectory(wave.trajectory);
	Ufo *ufo = spawnUfo(game, ruleset, globe, ufoRule, trajectory);
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
	if (_rule.getType() == "STR_ALIEN_INFILTRATION" && _nextWave == _rule.getWaveCount())
	{
		// Infiltrations loop for ever.
		_nextWave = 0;
	}
	if (_nextWave != _rule.getWaveCount())
	{
		const MissionWave &nwave = _rule.getWave(_nextWave);
		int half = nwave.spawnTimer/2;
		_spawnCountdown = (2 * RNG::generate(0, 1) - 1) * half/2 + half;
	}
}

/**
 * This function will spawn a UFO according the the mission rules.
 * @param game The saved game information.
 * @param ruleset The ruleset.
 * @param globe The globe, for land checks.
 * @param ufoRule The rule for the desired UFO.
 * @param trajectory The rule for the desired trajectory.
 * @return Pointer to the spawned UFO. If the mission does not desire to spawn a UFO, 0 is returned.
 */
Ufo *AlienMission::spawnUfo(const SavedGame &game, const Ruleset &ruleset, const Globe &globe, const RuleUfo &ufoRule, const UfoTrajectory &trajectory)
{
	if (_rule.getType() == "STR_ALIEN_RETALIATION")
	{
		const RuleRegion &regionRules = *ruleset.getRegion(_region);
		std::vector<Base *>::const_iterator found =
		    std::find_if(game.getBases()->begin(), game.getBases()->end(),
				 FindMarkedXCOMBase(regionRules));
		if (found != game.getBases()->end())
		{
			// Spawn a battleship straight for the XCOM base.
			const RuleUfo &battleshipRule = *ruleset.getUfo("STR_BATTLESHIP");
			const UfoTrajectory &assaultTrajectory = *ruleset.getUfoTrajectory("__RETALIATION_ASSAULT_RUN");
			Ufo *ufo = new Ufo(const_cast<RuleUfo*>(&battleshipRule));
			ufo->setMissionInfo(this, &assaultTrajectory);
			std::pair<double, double> pos = regionRules.getRandomPoint(assaultTrajectory.getZone(0));
			ufo->setAltitude(assaultTrajectory.getAltitude(0));
			ufo->setSpeed(assaultTrajectory.getSpeedPercentage(0) * ufoRule.getMaxSpeed());
			ufo->setLongitude(pos.first);
			ufo->setLatitude(pos.second);
			Waypoint *wp = new Waypoint();
			wp->setLongitude((*found)->getLongitude());
			wp->setLatitude((*found)->getLatitude());
			ufo->setDestination(wp);
			return ufo;
		}
	}
	// Spawn according to sequence.
	Ufo *ufo = new Ufo(const_cast<RuleUfo*>(&ufoRule));
	ufo->setMissionInfo(this, &trajectory);
	const RuleRegion &regionRules = *ruleset.getRegion(_region);
	std::pair<double, double> pos = regionRules.getRandomPoint(trajectory.getZone(0));
	ufo->setAltitude(trajectory.getAltitude(0));
	ufo->setSpeed(trajectory.getSpeedPercentage(0) * ufoRule.getMaxSpeed());
	ufo->setLongitude(pos.first);
	ufo->setLatitude(pos.second);
	Waypoint *wp = new Waypoint();
	if (trajectory.getAltitude(1) == "STR_GROUND")
	{
		pos = getLandPoint(globe, regionRules, trajectory.getZone(1));
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

void AlienMission::start(unsigned initialCount)
{
	_nextWave = 0;
	_nextUfoCounter = 0;
	_liveUfos = 0;
	if (initialCount == 0)
	{
		int half = _rule.getWave(0).spawnTimer/2;
		_spawnCountdown = half + (2 * RNG::generate(0, 1) - 1) * half/2;
	}
	else
	{
		_spawnCountdown = initialCount;
	}
}

class LocateXCOMBase: public std::unary_function<const Base *, bool>
{
public:
	LocateXCOMBase(double lon, double lat) : _lon(lon), _lat(lat) { /* Empty by design. */ }
	bool operator()(const Base *base) { return _lon == base->getLongitude() && _lat == base->getLatitude(); }
private:
	double _lon, _lat;
};

void AlienMission::ufoReachedWaypoint(Ufo &ufo, Game &engine, const Globe &globe)
{
	const Ruleset &rules = *engine.getRuleset();
	SavedGame &game = *engine.getSavedGame();
	if (ufo.getTrajectoryPoint() == ufo.getTrajectory().getWaypointCount() - 1)
	{
		//TODO: We should probably score alien points here.
		ufo.setDetected(false);
		ufo.setStatus(Ufo::DESTROYED);
		return;
	}
	ufo.setAltitude(ufo.getTrajectory().getAltitude(ufo.getTrajectoryPoint() + 1));
	if (ufo.getAltitude() != "STR_GROUND")
	{
		ufo.setTrajectoryPoint(ufo.getTrajectoryPoint() + 1);
		// Set next waypoint.
		Waypoint *wp = new Waypoint();
		RuleRegion *region = rules.getRegion(_region);
		ufo.setSpeed((int)(ufo.getRules()->getMaxSpeed() * ufo.getTrajectory().getSpeedPercentage(ufo.getTrajectoryPoint())));
		std::pair<double, double> pos;
		if (ufo.getTrajectory().getAltitude(ufo.getTrajectoryPoint()) == "STR_GROUND")
		{
			pos = getLandPoint(globe, *region, ufo.getTrajectory().getZone(ufo.getTrajectoryPoint()));
		}
		else
		{
			pos = region->getRandomPoint(ufo.getTrajectory().getZone(ufo.getTrajectoryPoint()));
		}
		wp->setLongitude(pos.first);
		wp->setLatitude(pos.second);
		ufo.setDestination(wp);
	}
	else
	{
		// UFO landed.

		if (ufo.getRules()->getType() == "STR_TERROR_SHIP" && _rule.getType() == "STR_ALIEN_TERROR" && ufo.getTrajectory().getZone(ufo.getTrajectoryPoint()) == 0)
		{
			// Specialized: STR_ALIEN_TERROR
			// Remove UFO, replace with TerrorSite.
			ufo.setStatus(Ufo::DESTROYED);
			ufo.setDetected(false);
			TerrorSite *terrorSite = new TerrorSite();
			terrorSite->setLongitude(ufo.getLongitude());
			terrorSite->setLatitude(ufo.getLatitude());
			terrorSite->setId(game.getId("STR_TERROR_SITE"));
			terrorSite->setSecondsRemaining(4 * 3600 + RNG::generate(0, 6) * 3600);
			terrorSite->setAlienRace(_race);
			const City *city = rules.locateCity(ufo.getLongitude(), ufo.getLatitude());
			assert(city);
			game.getTerrorSites()->push_back(terrorSite);
		}
		else if (_rule.getType() == "STR_ALIEN_RETALIATION" && ufo.getTrajectory().getID() == "__RETALIATION_ASSAULT_RUN")
		{
			// Ignore what the trajectory might say, this is a base assault.
			// Remove UFO, replace with Base defense.
			ufo.setStatus(Ufo::DESTROYED);
			ufo.setDetected(false);
			std::vector<Base *>::const_iterator found =
			    std::find_if(game.getBases()->begin(), game.getBases()->end(),
					 LocateXCOMBase(ufo.getLongitude(), ufo.getLatitude()));
			if (found == game.getBases()->end())
			{
				// Only spawn mission if the base is still there.
				return;
			}
			(*found)->setRetaliationTarget(false);
			Log(LOG_INFO) << "TODO: Implement XCOM base defensive fire.";
			SavedBattleGame *bgame = new SavedBattleGame();
			game.setBattleGame(bgame);
			bgame->setMissionType("STR_BASE_DEFENSE");
			BattlescapeGenerator bgen(&engine);
			bgen.setBase(*found);
			bgen.setAlienRace(_race);
			bgen.setAlienItemlevel(0); //TODO: How does this change?
			bgen.run();
			engine.pushState(new BriefingState(&engine, 0, 0));
		}
		else
		{
			// Set timer for UFO on the ground.
			ufo.setSecondsRemaining(ufo.getTrajectory().groundTimer());
		}
	}
}

void AlienMission::ufoShotDown(Ufo &ufo, Game &, const Globe &)
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
			const MissionWave &nwave = _rule.getWave(_nextWave);
			int half = nwave.spawnTimer/2;
			_spawnCountdown += (2 * RNG::generate(0, 1) - 1) * half/2 + half;
		}
		break;
	}
}

void AlienMission::ufoLifting(Ufo &ufo, Game &engine, const Globe &globe)
{
	const Ruleset &rules = *engine.getRuleset();
	switch (ufo.getStatus())
	{
	case Ufo::FLYING:
		assert(0 && "Ufo is already on the air!");
		break;
	case Ufo::LANDED:
		{
			assert(ufo.getTrajectoryPoint() != ufo.getTrajectory().getWaypointCount() - 1);
			ufo.setSpeed((int)(ufo.getRules()->getMaxSpeed() * ufo.getTrajectory().getSpeedPercentage(ufo.getTrajectoryPoint())));
			ufo.setAltitude("STR_VERY_LOW");
			// Set next waypoint.
			Waypoint *wp = new Waypoint();
			RuleRegion *region = rules.getRegion(_region);
			std::pair<double, double> pos;
			if (ufo.getTrajectory().getAltitude(ufo.getTrajectoryPoint() + 1) == "STR_GROUND")
			{
				pos = getLandPoint(globe, *region, ufo.getTrajectory().getZone(ufo.getTrajectoryPoint() + 1));
			}
			else
			{
				pos = region->getRandomPoint(ufo.getTrajectory().getZone(ufo.getTrajectoryPoint() + 1));
			}
			wp->setLongitude(pos.first);
			wp->setLatitude(pos.second);
			ufo.setDestination(wp);
			ufo.setTrajectoryPoint(ufo.getTrajectoryPoint() + 1);
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
void AlienMission::setWaveCountdown(unsigned minutes)
{
	assert(minutes != 0 && minutes % 30 == 0);
	if (isOver())
	{
		return;
	}
	_spawnCountdown = minutes;
}

}
