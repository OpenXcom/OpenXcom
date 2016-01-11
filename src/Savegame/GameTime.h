#pragma once
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
#include <string>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class Language;

/**
 * Enumerator for time periods.
 */
enum TimeTrigger { TIME_5SEC, TIME_10MIN, TIME_30MIN, TIME_1HOUR, TIME_1DAY, TIME_1MONTH };

/**
 * Stores the current ingame time/date according to GMT.
 * Takes care of managing and representing each component,
 * as well as common time operations.
 */
class GameTime
{
private:
	int _second, _minute, _hour, _weekday, _day, _month, _year;
public:
	/// Creates a new ingame time at a certain point.
	GameTime(int weekday, int day, int month, int year, int hour, int minute, int second);
	/// Cleans up the ingame time.
	~GameTime();
	/// Loads the time from YAML.
	void load(const YAML::Node& node);
	/// Saves the time to YAML.
	YAML::Node save() const;
	/// Advances the time by 5 seconds.
	TimeTrigger advance();
	/// Gets the ingame second.
	int getSecond() const;
	/// Gets the ingame minute.
	int getMinute() const;
	/// Gets the ingame hour.
	int getHour() const;
	/// Gets the ingame weekday.
	int getWeekday() const;
	// Gets a string version of the ingame weekday.
	std::string getWeekdayString() const;
	/// Gets the ingame day.
	int getDay() const;
	// Gets a string version of the ingame day.
	std::wstring getDayString(Language *lang) const;
	/// Gets the ingame month.
	int getMonth() const;
	// Gets a string version of the ingame month.
	std::string getMonthString() const;
	/// Gets the ingame year.
	int getYear() const;
	/// Gets the position of the daylight according to the ingame time.
	double getDaylight() const;
};

}
