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
#ifndef OPENXCOM_STATESKIN_H
#define OPENXCOM_STATESKIN_H

#include <string>
#include <yaml-cpp/yaml.h>

enum SkinColor {SC_PALETTE = 0, SC_BORDERS, SC_BUTTONS, SC_MAIN, SC_SECOND, SC_COLOR6, SC_COLOR7, SC_COLOR8};

namespace OpenXcom
{

union SkinColors
{
	Uint8 c[8];
	Uint64 raw;
	SkinColors() : raw(0) {}
	SkinColors(Uint64 sc) : raw(sc) {}
	inline SkinColors& operator=(const SkinColors& sc) { raw = sc.raw; return *this; }
};

/**
 * Represents a appearance properties of screens in the game.
 */
class StateSkin
{
private:
//	std::string _state;
	std::string _background;
	std::string _palette;
	bool _highContrast;
	SkinColors _colors;
public:
	/// Creates a new StateAppearance.
	StateSkin(const std::string &type);
	/// Cleans up the StateAppearance.
	~StateSkin();
	/// Loads the StateAppearance from YAML.
	void load(const YAML::Node& node);
	/// Gets name of backgroung picture.
	inline std::string getBackground() const { return _background; }
	/// Gets name of palette.
	inline std::string getPalette() const { return _palette; }
	/// Gets info about contrast.
	inline bool isHighContrast() const { return _highContrast; }
	/// Gets a custom color.
	inline Uint8 getColor(unsigned idx) const { return _colors.c[idx]; }
	/// Gets a specific color.
	inline Uint8 getColorPalette() const { return _colors.c[SC_PALETTE]; }
	inline Uint8 getColorBorders() const { return _colors.c[SC_BORDERS]; }
	inline Uint8 getColorButtons() const { return _colors.c[SC_BUTTONS]; }
	inline Uint8 getColorMain() const { return _colors.c[SC_MAIN]; }
	inline Uint8 getColorSecond() const { return _colors.c[SC_SECOND]; }
};

}

#endif
