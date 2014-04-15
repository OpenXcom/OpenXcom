/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#ifndef OPENXCOM_SCRIPT_H
#define	OPENXCOM_SCRIPT_H

#include <map>
#include <vector>
#include <string>
#include <string.h>
#include <SDL/SDL_stdinc.h>


namespace OpenXcom
{
class Surface;


const int ScriptMaxArg = 4;
const int ScriptMaxRef = 32;

struct ScriptData
{
	typedef void (*void_func)();

	Uint8 type;
	Uint8 index;

	int value;
	void_func env_func;
};

struct ScriptWorkRef
{
	int ref[ScriptMaxRef];
};

class ScriptBase
{
	std::vector<Uint8> _proc;
	std::vector<ScriptData> _proc_ref_data;

protected:
	~ScriptBase() { }
	///common typeless part of updating data in script
	void updateBase(ScriptWorkRef& ref, const void* t, int (*cast)(const void*, ScriptData::void_func)) const;

public:
	///Blit src to dest using current script
	void executeBlit(ScriptWorkRef& ref, Surface* src, Surface* dest) const;

	friend class ScriptParserBase;
};

template<typename T>
class Script : public ScriptBase
{
public:
	typedef int (*typed_func)(const T*);

private:
	///helper function using to hide type
	static int cast(const void* v, ScriptData::void_func f)
	{
		return reinterpret_cast<typed_func>(f)((const T*)v);
	}

public:
	///Update values in strcip form unit
	void update(ScriptWorkRef& ref, const T* t) const
	{
		updateBase(ref, (const void*)t, &cast);
	}
};

struct ScriptParserData
{
	Uint8 proc_id;
	Uint8 arg_type[ScriptMaxArg];
	Uint8 arg_offset[ScriptMaxArg];
};

class ScriptParserBase
{
	std::map<std::string, ScriptParserData> _op_list;
	std::map<std::string, ScriptData> _ref_list;

protected:
	///common typeless part of parsing string
	bool parseBase(ScriptBase* scr, const std::string& src_code) const;
	///common typeless part of adding new function
	void addFunctionBase(const std::string& s, ScriptData::void_func f);

public:
	ScriptParserBase();
};

template<typename T>
class ScriptParser : public ScriptParserBase
{
public:
	///Default constructor
	ScriptParser()
	{
		//ScriptParser requare static function in T to initialize data!
		T::registScript(this);
	}

	///prase string and return new script
	Script<T>* parse(const std::string& src_code) const
	{
		Script<T>* scr = new Script<T>();
		if(parseBase(scr, src_code))
			return scr;
		delete scr;
		return 0;
	}

	///add new funcion that will be avaiable in script
	void addFunction(const std::string& s, typename Script<T>::typed_func f)
	{
		addFunctionBase(s, reinterpret_cast<ScriptData::void_func>(f));
	}
};

}

#endif	/* OPENXCOM_SCRIPT_H */

