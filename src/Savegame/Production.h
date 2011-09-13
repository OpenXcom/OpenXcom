/*
 * Copyright 2011 OpenXcom Developers.
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
#ifndef OPENXCOM_PRODUCTION
#define OPENXCOM_PRODUCTION

#include "yaml.h"

namespace OpenXcom
{
class RuleItem;
class Base;
class SavedGame;
enum productionEnd_e { PRODUCTION_END_COMPLETE, PRODUCTION_END_NOT_ENOUGH_MONEY, PRODUCTION_END_NOT_ENOUGH_MATERIALS, PRODUCTION_END_MAX };
class Production
{
public:
	Production (RuleItem * item, int todo);
	int getNumberOfItemTodo () const;
	void setNumberOfItemTodo (int);
	int getTimeSpent () const;
	void setTimeSpent (int);
	int getNumberOfItemDone () const;
	int getAssignedEngineers() const;
	void setAssignedEngineers (int);
	bool step(Base * b, SavedGame * g, productionEnd_e & endType);
	const RuleItem * getRuleItem() const;
	void startItem(Base * b, SavedGame * g);
	void save(YAML::Emitter &out);
	void load(const YAML::Node &node);
private:
	RuleItem * _item;
	int _todo;
	int _timeSpent;
	int _engineers;
};
}
#endif
