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
#ifndef OPENXCOM_RULEBASEFACILITY_H
#define OPENXCOM_RULEBASEFACILITY_H

#include <string>
#include "yaml.h"

namespace OpenXcom
{

/**
 * Represents a specific type of base facility.
 * Contains constant info about a facility like
 * costs, capacities, size, etc.
 * @sa BaseFacility
 */
class RuleBaseFacility
{
private:
	std::string _type;
	int _spriteShape, _spriteFacility;
	bool _lift, _hyper, _mind, _grav;
	int _size, _buildCost, _buildTime, _monthlyCost;
	int _storage, _personnel, _aliens, _crafts, _labs, _workshops, _psiLabs;
	int _radarRange, _radarChance, _defence, _hitRatio;
public:
	/// Creates a blank facility ruleset.
	RuleBaseFacility(const std::string &type);
	/// Cleans up the facility ruleset.
	~RuleBaseFacility();
	/// Loads the facility from YAML.
	void load(const YAML::Node& node);
	/// Saves the facility to YAML.
	void save(YAML::Emitter& out) const;
	/// Gets the facility's type.
	std::string getType() const;
	/// Gets the facility's shape sprite.
	int getSpriteShape() const;
	/// Sets the facility's shape sprite.
	void setSpriteShape(int sprite);
	/// Gets the facility's content sprite.
	int getSpriteFacility() const;
	/// Sets the facility's content sprite.
	void setSpriteFacility(int sprite);
	/// Gets the facility's size.
	int getSize() const;
	/// Sets the facility's size.
	void setSize(int size);
	/// Gets if the facility is an access lift.
	bool getLift() const;
	/// Sets if the facility is an access lift.
	void setLift(bool lift);
	/// Gets if the facility has hyperwave detection.
	bool getHyperwave() const;
	/// Sets if the facility has hyperwave detection.
	void setHyperwave(bool hyper);
	/// Gets if the facility is a mind shield.
	bool getMindShield() const;
	/// Sets if the facility is a mind shield.
	void setMindShield(bool mind);
	/// Gets if the facility is a grav shield.
	bool getGravShield() const;
	/// Sets if the facility is a grav shield.
	void setGravShield(bool grav);
	/// Gets the facility's construction cost.
	int getBuildCost() const;
	/// Sets the facility's construction cost.
	void setBuildCost(int cost);
	/// Gets the facility's construction time.
	int getBuildTime() const;
	/// Sets the facility's construction time.
	void setBuildTime(int time);
	/// Gets the facility's monthly cost.
	int getMonthlyCost() const;
	/// Sets the facility's monthly cost.
	void setMonthlyCost(int cost);
	/// Gets the facility's storage capacity.
	int getStorage() const;
	/// Sets the facility's storage capacity.
	void setStorage(int storage);
	/// Gets the facility's personnel capacity.
	int getPersonnel() const;
	/// Sets the facility's personnel capacity.
	void setPersonnel(int personnel);
	/// Gets the facility's alien capacity.
	int getAliens() const;
	/// Sets the facility's alien capacity.
	void setAliens(int aliens);
	/// Gets the facility's craft capacity.
	int getCrafts() const;
	/// Sets the facility's craft capacity.
	void setCrafts(int crafts);
	/// Gets the facility's laboratory space.
	int getLaboratories() const;
	/// Sets the facility's laboratory space.
	void setLaboratories(int laboratories);
	/// Gets the facility's workshop space.
	int getWorkshops() const;
	/// Sets the facility's workshop space.
	void setWorkshops(int workshops);
	/// Gets the facility's psi-training capacity.
	int getPsiLaboratories() const;
	/// Sets the facility's psi-training capacity.
	void setPsiLaboratories(int psi);
	/// Gets the facility's radar range.
	int getRadarRange() const;
	/// Sets the facility's radar range.
	void setRadarRange(int range);
	/// Gets the facility's detection chance.
	int getRadarChance() const;
	/// Sets the facility's detection chance.
	void setRadarChance(int chance);
	/// Gets the facility's defence value.
	int getDefenceValue() const;
	/// Sets the facility's defence value.
	void setDefenceValue(int defence);
	/// Gets the facility's weapon hit ratio.
	int getHitRatio() const;
	/// Sets the facility's weapon hit ratio.
	void setHitRatio(int ratio);
};

}

#endif
