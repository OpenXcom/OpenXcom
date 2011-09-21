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
#ifndef OPENXCOM_RULECOUNTRY_H
#define OPENXCOM_RULECOUNTRY_H

#include <string>

namespace OpenXcom
{

/**
 * Represents a specific funding country.
 * Contains constant info like its location in the
 * world and starting funding range.
 */
class RuleCountry
{
private:
	std::string _type;
	int _fundingMin, _fundingMax;
	double _labelLon, _labelLat;
public:
	/// Creates a blank country ruleset.
	RuleCountry(const std::string &type);
	/// Cleans up the country ruleset.
	~RuleCountry();
	/// Gets the country's type.
	std::string getType() const;
	/// Gets the country's minimum funding.
	int getMinFunding() const;
	/// Sets the country's minimum funding.
	void setMinFunding(int funding);
	/// Gets the country's maximum funding.
	int getMaxFunding() const;
	/// Sets the country's maximum funding.
	void setMaxFunding(int funding);
	/// Gets the country's funding change.
	int getChange() const;
	/// Gets the country's label X position.
	double getLabelLongitude() const;
	/// Sets the country's label X position.
	void setLabelLongitude(double lon);
	/// Gets the country's label Y position.
	double getLabelLatitude() const;
	/// Sets the country's label Y position.
	void setLabelLatitude(double lat);
};

}

#endif
