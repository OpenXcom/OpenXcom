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
namespace OpenXcom
{
/**
 * Fast line clip.
 */
class FastLineClip
{
private:
	double FC_xn, FC_yn, FC_xk, FC_yk;
	void Clip0_Top(void);
	void Clip0_Bottom(void);
	void Clip0_Right(void);
	void Clip0_Left(void);
	void Clip1_Top(void);
	void Clip1_Bottom(void);
	void Clip1_Right(void);
	void Clip1_Left(void);
public:
	double Wxlef, Wxrig, Wytop, Wybot;
	/// Creates a fastlineclip.
	FastLineClip(double Wxl, double Wxr, double Wyt, double Wyb) : FC_xn(0), FC_yn(0), FC_xk(0), FC_yk(0)
	{
		Wxlef = Wxl;
		Wxrig = Wxr;
		Wytop = Wyt;
		Wybot = Wyb;
	};
	/// Cleans up the fastlineclip.
	~FastLineClip() {};
	/// LineClip itself.
	int LineClip(double *x0, double *y0, double *x1, double *y1);
};

}
