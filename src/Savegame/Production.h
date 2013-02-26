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
#ifndef OPENXCOM_PRODUCTION_H
#define OPENXCOM_PRODUCTION_H

#include <yaml-cpp/yaml.h>

namespace OpenXcom
{
class RuleManufacture;
class Base;
class SavedGame;
class Ruleset;
enum productionProgress_e { PROGRESS_NOT_COMPLETE, PROGRESS_COMPLETE, PROGRESS_NOT_ENOUGH_MONEY, PROGRESS_NOT_ENOUGH_MATERIALS, PROGRESS_MAX };
class Production
{
public:
	Production (const RuleManufacture * rules, int amount);
	int getAmountTotal () const;
	void setAmountTotal (int);
	int getTimeSpent () const;
	void setTimeSpent (int);
	int getAmountProduced () const;
	int getAssignedEngineers() const;
	void setAssignedEngineers (int);
	productionProgress_e step(Base * b, SavedGame * g, const Ruleset *r);
	const RuleManufacture * getRules() const;
	void startItem(Base * b, SavedGame * g);
	void save(YAML::Emitter &out);
	void load(const YAML::Node &node);
private:
	const RuleManufacture * _rules;
	int _amount;
	int _timeSpent;
	int _engineers;
	bool haveEnoughMoneyForOneMoreUnit(SavedGame * g);
	bool haveEnoughMaterialsForOneMoreUnit(Base * b);
};
}
#endif
