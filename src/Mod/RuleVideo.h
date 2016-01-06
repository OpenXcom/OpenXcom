/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#ifndef OPENXCOM_RULEVIDEO_H
#define OPENXCOM_RULEVIDEO_H

#include <yaml-cpp/yaml.h>
#include <vector>
#include <string>
#include <map>
#include "../Interface/Text.h"

namespace OpenXcom
{
struct SlideshowHeader
{
	std::string musicId; // just the extension-less filename, like "GMWIN"
	int transitionSeconds; // number of seconds to show each slide
};

struct SlideshowSlide
{
	std::string imagePath; // relative path to slide image
	std::string caption; // STR id of caption text
	int w, h, x, y, color; // caption rect and color info
	TextHAlign align; // caption alignment
	int transitionSeconds; // number of seconds to show this slide
};

class RuleVideo
{
private:
	std::string _id;
	bool _useUfoAudioSequence;
	std::vector<std::string> _videos;
	SlideshowHeader _slideshowHeader;
	std::vector<SlideshowSlide> _slides;
public:
	RuleVideo(const std::string &type);
	~RuleVideo();
	void load(const YAML::Node &node);
	/// returns whether to use the UFO audio sequence.  in general, this
	/// should only ever be true for the vanilla UFO intro cutscene
	bool useUfoAudioSequence() const;
	const std::vector<std::string> * getVideos() const;
	const SlideshowHeader & getSlideshowHeader() const;
	const std::vector<SlideshowSlide> * getSlides() const;
};

}
#endif
