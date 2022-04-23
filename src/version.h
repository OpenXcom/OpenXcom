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
#define OPENXCOM_VERSION_ENGINE ""
#define OPENXCOM_VERSION_SHORT "1.0"
#define OPENXCOM_VERSION_LONG "1.0.0.0"
#define OPENXCOM_VERSION_NUMBER 1,0,0,0

#ifdef GIT_BUILD
#include "git_version.h"
#endif

#ifndef OPENXCOM_VERSION_GIT
#define OPENXCOM_VERSION_GIT " Dev"
#endif
