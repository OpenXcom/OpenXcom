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
#include <climits>
#include "../Engine/Screen.h"

namespace OpenXcom
{

RuleVideo::RuleVideo(const std::string &id) : _id(id)
{
}

RuleVideo::~RuleVideo()
{
}

static void _loadSlide(SlideshowSlide &slide, const YAML::Node &node)
{
	slide.imagePath = node["imagePath"].as<std::string>("");
	slide.caption = node["caption"].as<std::string>("");

	std::pair<int, int> size = node["captionSize"].as<std::pair<int, int> >(
		std::pair<int, int>(Screen::ORIGINAL_WIDTH, Screen::ORIGINAL_HEIGHT));
	slide.w = size.first;
	slide.h = size.second;

	std::pair<int, int> pos = node["captionPos"].as<std::pair<int, int> >(std::pair<int, int>(0, 0));
	slide.x = pos.first;
	slide.y = pos.second;

	slide.color = node["captionColor"].as<int>(INT_MAX);
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
		_slideshowHeader.musicId = slideshow["musicId"].as<std::string>("");
		_slideshowHeader.transitionSeconds = slideshow["transitionSeconds"].as<int>(30);

		const YAML::Node &slides = slideshow["slides"];
		for (YAML::const_iterator i = slides.begin(); i != slides.end(); ++i)
		{
			SlideshowSlide slide;
			_loadSlide(slide, *i);
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

const SlideshowHeader & RuleVideo::getSlideshowHeader() const
{
	return _slideshowHeader;
}

const std::vector<SlideshowSlide> * RuleVideo::getSlides() const
{
	return &_slides;
}

}
