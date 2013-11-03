/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#ifndef OPENXCOM_STATEAPPEARANCE_H
#define OPENXCOM_STATEAPPEARANCE_H

#include <string>
#include <yaml-cpp/yaml.h>

enum AppearanceColor{AC_PALETTE = 0, AC_BORDERS, AC_BUTTONS, AC_MAIN, AC_SECOND, AC_COLOR6, AC_COLOR7, AC_COLOR8};

namespace OpenXcom
{

union AppearanceColors
{
	Uint8 c[8];
	Uint64 raw;
	AppearanceColors() : raw(0) {}
	AppearanceColors(Uint64 ac) : raw(ac) {}
	inline AppearanceColors& operator=(const AppearanceColors& ac) { raw = ac.raw; return *this; }
};

/**
 * Represents a appearance properties of screens in the game.
 */
class StateAppearance
{
private:
//	std::string _state;
	std::string _background;
	std::string _palette;
	bool _highContrast;
	AppearanceColors _colors;
public:
	/// Creates a new StateAppearance.
	StateAppearance(const std::string &type);
	/// Cleans up the StateAppearance.
	~StateAppearance();
	/// Loads the StateAppearance from YAML.
	void load(const YAML::Node& node);
	/// Gets name of backgroung picture.
	inline std::string getBackground() const { return _background; }
	/// Gets name of palette.
	inline std::string getPalette() const { return _palette; }
	/// Gets info about contrast.
	inline bool isHighContrast() const { return _highContrast; }
	/// Gets a custom color.
	inline Uint8 getColor(AppearanceColor idx) const { return _colors.c[idx]; }
	/// Gets a specific color.
	inline Uint8 getColorPalette() const { return _colors.c[AC_PALETTE]; }
	inline Uint8 getColorBorders() const { return _colors.c[AC_BORDERS]; }
	inline Uint8 getColorButtons() const { return _colors.c[AC_BUTTONS]; }
	inline Uint8 getColorMain() const { return _colors.c[AC_MAIN]; }
	inline Uint8 getColorSecond() const { return _colors.c[AC_SECOND]; }
};

}

#endif
