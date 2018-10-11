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

#include <locale>
#include <codecvt>
#include <cmath>
#include "../Engine/Surface.h"

namespace OpenXcom
{

class Language;
class Action;
class Text;

/*  Exponential Moving Average.

	Used to smooth out various metrics, like framerates.

	Alpha: detemines the speed with which the returned value
		approaches the last submitted one. Defines the 'memory'
		of sorts. Typical value 0.025 for millisecond-order updates.

	Value: metric value

	Seed ceiling: number of initial values to ignore before the returned
		value can be considered meaning anything. Typical: 10.

	Reseed threshold:

	To avoid noticeable stabilization periods like after major
	metric spikes, during which no useful data is provided,
	there's the reseed_threshold parameter, which triggers
	a reseed if abs((sample-value)/value) gets larger than it
	(value being the spike value).

	So when the metric spikes for some reason its value is considerably
	larger than any previous and this triggers a reseed while dropping
	the sample so that it doesn't skew the returned value.

	This thing can be applied to graphics fps, if for some reason
	(large magnitude resize or zoom) it drops or raises abruptly.
	In this case the sample can be used as first seed value, but
	to keep the interface sane it is dropped anyway.

	Don't set it too low, or it'll keep dropping data
	and reseeding all the time.

	Default very large value disables this functionality.

	For the typical use displaying the FPS as a reciprocal of frame time
	in milliseconds, a threshold of about 3 is usually adequate.
*/

class ema_filter_t
{
	float alpha;
	float value;
	float seedsum;
	unsigned seedceil;
	unsigned seedcount;
	float reseed_threshold;
public:
	ema_filter_t(float _alpha = 0.03, float _value = 0.0, unsigned _seedceil = 8, float _reseed_threshold = 3) :
		alpha(_alpha),
		value(_value),
		seedsum(0),
		seedceil(_seedceil),
		seedcount(0),
		reseed_threshold(_reseed_threshold) { }
	float update(const float sample)
	{
		if (fabsf((sample-value)/value) > reseed_threshold)
		{
			reset(sample);
			return sample;
		}
		if (seedcount < seedceil)
		{
			seedsum += sample;
			if (seedcount++ == seedceil)
			{
				value = seedsum / seedcount;
			}
		}
		else
		{
			value = alpha * sample + (1.0 - alpha) * value;
		}
		return value;
	}
	const float get(void) const { return value; }
	void reset(const float value)
	{
		this->value = value;
		seedcount = 0;
		seedsum = 0;
	}
};

/**
 * Counts the amount of frames each second
 * and displays them in a Text surface with a 4x6 font.
 *
 * Typical display is something like
 *
 *		H: 0 L: 0 B: 4 I:12 F:16/16 FPS: 63
 *
 * 	Where
 * 	@H - input handling time ( input event procesing and handle() calls on the states)
 *  @L - logic handling time ( think() calls on the states)
 *  @B - blit time ( blit() calls on the visible states and the screen flip)
 *  @I - idle time ( time left after all of the above if the FPS is limited)
 *  @F - frame duration (sum of the above) / nominal frame duration (1 if FPS is not limited)
 *  @FPS - 1000.0/frame duration
 *
 *  all values in milliseconds.
 *
 */
class FpsCounter : public Surface
{
private:
	Text *_text;
	Font *_font;
	Language *_lang;

	ema_filter_t _input, _logic, _blit, _idle, _total, _frame;
	int _limit;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> _wsconverter;

public:
	/// Creates a new FPS counter linked to a game.
	FpsCounter(int width, int height, int x, int y);
	/// Cleans up all the FPS counter resources.
	~FpsCounter();
	/// Sets the FPS counter's palette.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Sets the FpsCounter's color.
	void setColor(Uint8 color);
	/// Handles keyboard events.
	void handle(Action *action);
	/// Draws the FPS counter.
	void draw();
	void addFrame(const int  inputProcessingTime, const int  logicProcessingTime, const int  blittingTime, const int idleTime, const int limit);
};

}
