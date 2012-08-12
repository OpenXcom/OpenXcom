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
enum productionProgress_e { PRODUCTION_PROGRESS_NOT_COMPLETE, PRODUCTION_PROGRESS_COMPLETE, PRODUCTION_PROGRESS_NOT_ENOUGH_MONEY, PRODUCTION_PROGRESS_NOT_ENOUGH_MATERIALS, PRODUCTION_PROGRESS_MAX };
class Production
{
public:
	Production (RuleManufacture * item, int todo);
	int getNumberOfItemTodo () const;
	void setNumberOfItemTodo (int);
	int getTimeSpent () const;
	void setTimeSpent (int);
	int getNumberOfItemDone () const;
	int getAssignedEngineers() const;
	void setAssignedEngineers (int);
	productionProgress_e step(Base * b, SavedGame * g);
	const RuleManufacture * getRuleManufacture() const;
	void startItem(Base * b, SavedGame * g);
	void save(YAML::Emitter &out);
	void load(const YAML::Node &node);
private:
	RuleManufacture * _item;
	int _todo;
	int _timeSpent;
	int _engineers;
};
}
#endif
