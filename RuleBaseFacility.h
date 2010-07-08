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
#ifndef OPENXCOM__RULEBASEFACILITY_H
#define OPENXCOM__RULEBASEFACILITY_H

#include "LangString.h"

/**
 * Represents a specific type of base facility.
 * Contains constant info about a facility like
 * costs, capacities, size, etc.
 * @sa BaseFacility
 */
class RuleBaseFacility
{
private:
	LangString _type;
	int _spriteShape, _spriteFacility;
	bool _lift;
	int _size, _buildCost, _buildTime, _monthlyCost;
	int _storage, _personnel, _aliens, _crafts, _labs, _workshops, _psiLabs;
	int _radarRange, _radarChance, _defence, _hitRatio;
public:
	/// Creates a blank facility ruleset.
	RuleBaseFacility(LangString type);
	/// Cleans up the facility ruleset.
	~RuleBaseFacility();
	/// Gets the facility's type.
	LangString getType();
	/// Gets the facility's shape sprite.
	int getSpriteShape();
	/// Sets the facility's shape sprite.
	void setSpriteShape(int sprite);
	/// Gets the facility's content sprite.
	int getSpriteFacility();
	/// Sets the facility's content sprite.
	void setSpriteFacility(int sprite);
	/// Gets the facility's size.
	int getSize();
	/// Sets the facility's size.
	void setSize(int size);
	/// Gets if the facility is a lift.
	bool getLift();
	/// Sets if the facility is a lift.
	void setLift(bool lift);
	/// Gets the facility's construction cost.
	int getBuildCost();
	/// Sets the facility's construction cost.
	void setBuildCost(int cost);
	/// Gets the facility's construction time.
	int getBuildTime();
	/// Sets the facility's construction time.
	void setBuildTime(int time);
	/// Gets the facility's monthly cost.
	int getMonthlyCost();
	/// Sets the facility's monthly cost.
	void setMonthlyCost(int cost);
	/// Gets the facility's storage capacity.
	int getStorage();
	/// Sets the facility's storage capacity.
	void setStorage(int storage);
	/// Gets the facility's personnel capacity.
	int getPersonnel();
	/// Sets the facility's personnel capacity.
	void setPersonnel(int personnel);
	/// Gets the facility's alien capacity.
	int getAliens();
	/// Sets the facility's alien capacity.
	void setAliens(int aliens);
	/// Gets the facility's craft capacity.
	int getCrafts();
	/// Sets the facility's craft capacity.
	void setCrafts(int crafts);
	/// Gets the facility's laboratory space.
	int getLaboratories();
	/// Sets the facility's laboratory space.
	void setLaboratories(int laboratories);
	/// Gets the facility's workshop space.
	int getWorkshops();
	/// Sets the facility's workshop space.
	void setWorkshops(int workshops);
	/// Gets the facility's psi-training capacity.
	int getPsiLaboratories();
	/// Sets the facility's psi-training capacity.
	void setPsiLaboratories(int psi);
	/// Gets the facility's radar range.
	int getRadarRange();
	/// Sets the facility's radar range.
	void setRadarRange(int range);
	/// Gets the facility's detection chance.
	int getRadarChance();
	/// Sets the facility's detection chance.
	void setRadarChance(int chance);
	/// Gets the facility's defence value.
	int getDefenceValue();
	/// Sets the facility's defence value.
	void setDefenceValue(int defence);
	/// Gets the facility's weapon hit ratio.
	int getHitRatio();
	/// Sets the facility's weapon hit ratio.
	void setHitRatio(int ratio);
};

#endif
