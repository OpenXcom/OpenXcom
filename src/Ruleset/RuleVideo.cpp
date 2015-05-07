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
#include "RuleVideo.h"

namespace OpenXcom
{

RuleVideo::RuleVideo(const std::string &id) : _id(id)
{
}

RuleVideo::~RuleVideo()
{
}

static void _loadSlide(SlideshowSlide &slide, const YAML::Node &node, const SlideshowSlide &defaults)
{
	slide.caption = node["caption"].as<std::string>(defaults.caption);
	slide.captionCategory = node["captionCategory"].as<std::string>(defaults.captionCategory);
	slide.captionId = node["captionId"].as<std::string>(defaults.captionId);
	slide.durationSeconds = node["durationSeconds"].as<int>(defaults.durationSeconds);
	slide.imagePath = node["imagePath"].as<std::string>(defaults.imagePath);
	slide.musicId = node["musicId"].as<std::string>(defaults.musicId);
}

void RuleVideo::load(const YAML::Node &node)
{
	_useUfoAudioSequence = node["useUfoAudioSequence"].as<bool>(false);

	if (const YAML::Node &videos = node["videos"])
	{
		for (YAML::const_iterator i = videos.begin(); i != videos.end(); ++i)
			_videos.push_back((*i).as<std::string>());
	}

	if (const YAML::Node &slideshow = node["slideshow"])
	{
		SlideshowSlide def;
		def.durationSeconds = 30;

		if (slideshow["defaults"].IsDefined())
		{
			_loadSlide(def, slideshow["defaults"], def);
		}

		const YAML::Node &slides = slideshow["slides"];
		for (YAML::const_iterator i = slides.begin(); i != slides.end(); ++i)
		{
			SlideshowSlide slide;
			_loadSlide(slide, *i, def);
			_slides.push_back(slide);
		}
	}
}

bool RuleVideo::useUfoAudioSequence() const
{
	return _useUfoAudioSequence;
}

const std::vector<std::string> * RuleVideo::getVideos() const
{
	return &_videos;
}

const std::vector<SlideshowSlide> * RuleVideo::getSlides() const
{
	return &_slides;
}

}
