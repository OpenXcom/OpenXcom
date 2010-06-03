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
	LangString getType();
	int getSpriteShape();
	void setSpriteShape(int sprite);
	int getSpriteFacility();
	void setSpriteFacility(int sprite);
	int getSize();
	void setSize(int size);
	void setLift(bool lift);
	void setBuildCost(int cost);
	void setBuildTime(int time);
	void setMonthlyCost(int cost);
	void setStorage(int storage);
	void setPersonnel(int personnel);
	void setAliens(int aliens);
	void setCrafts(int crafts);
	void setLaboratories(int laboratories);
	void setWorkshops(int workshops);
	void setPsiLaboratories(int psi);
	void setRadarRange(int range);
	void setRadarChance(int chance);
	void setDefenceValue(int defence);
	void setHitRatio(int ratio);
};

#endif