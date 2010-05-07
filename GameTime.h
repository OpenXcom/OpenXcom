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
#ifndef OPENXCOM__GAMETIME_H
#define OPENXCOM__GAMETIME_H

#include "LangString.h"

enum TimeTrigger { TIME_SEC, TIME_MIN, TIME_HOUR, TIME_DAY, TIME_MONTH };

class GameTime
{
private:
	int _second, _minute, _hour, _weekday, _day, _month, _year;
public:
	GameTime(int weekday, int day, int month, int year, int hour, int minute, int second);
	~GameTime();
	TimeTrigger advance();
	int getSecond();
	int getMinute();
	int getHour();
	int getWeekday();
	LangString getWeekdayString();
	int getDay();
	LangString getDayString();
	int getMonth();
	LangString getMonthString();
	int getYear();
};

#endif