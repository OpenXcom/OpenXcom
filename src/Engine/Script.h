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

/**
 * struct used to store definiton of used data by script
 */
struct ScriptData
{
	typedef void (*voidFunc)();

	Uint8 type;
	Uint8 index;

	int value;
	voidFunc envFunc;
};

/**
 * struct that cache state of script data and is place of script wrtie temporaly data
 */
struct ScriptWorkRef
{
	int ref[ScriptMaxRef];
};

/**
 * common base of script execution
 */
class ScriptBase
{
	std::vector<Uint8> _proc;
	std::vector<ScriptData> _procRefData;

protected:
	~ScriptBase() { }
	///common typeless part of updating data in script
	void updateBase(ScriptWorkRef& ref, const void* t, int (*cast)(const void*, ScriptData::voidFunc)) const;

public:
	///Blit src to dest using current script
	void executeBlit(ScriptWorkRef& ref, Surface* src, Surface* dest, int custom0, int custom1) const;

	friend class ScriptParserBase;
};

/**
 * strong typed script
 */
template<typename T>
class Script : public ScriptBase
{
public:
	typedef int (*typedFunc)(const T*);

private:
	///helper function using to hide type
	static int cast(const void* v, ScriptData::voidFunc f)
	{
		return reinterpret_cast<typedFunc>(f)((const T*)v);
	}

public:
	///Update values in strcip form unit
	void update(ScriptWorkRef& ref, const T* t) const
	{
		updateBase(ref, (const void*)t, &cast);
	}
};

/**
 * struct storing avaiable operation to scripts
 */
struct ScriptParserData
{
	Uint8 procId;
	Uint8 argType[ScriptMaxArg];
	Uint8 argOffset[ScriptMaxArg];
};

/**
 * common base of script parser
 */
class ScriptParserBase
{
	std::map<std::string, ScriptParserData> _procList;
	std::map<std::string, ScriptData> _refList;

protected:
	///common typeless part of parsing string
	bool parseBase(ScriptBase* scr, const std::string& code) const;
	///common typeless part of adding new function
	void addFunctionBase(const std::string& s, ScriptData::voidFunc f);

public:
	///Default constructor
	ScriptParserBase();

	///Add name for first custom parametr
	void addCustom0(const std::string& s);
	///Add name for second custom parametr
	void addCustom1(const std::string& s);
	///Add const value
	void addConst(const std::string& s, int i);
};

/**
 * strong typed parser
 */
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
	void addFunction(const std::string& s, typename Script<T>::typedFunc f)
	{
		addFunctionBase(s, reinterpret_cast<ScriptData::voidFunc>(f));
	}
};

}

#endif	/* OPENXCOM_SCRIPT_H */

