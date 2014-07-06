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
#include <SDL/SDL_stdinc.h>


namespace OpenXcom
{
class Surface;
class ScriptContainerBase;


const int ScriptMaxArg = 4;
const int ScriptMaxRef = 64;
const int ScriptMaxRefCustom = 4;

/**
 * Struct that cache state of script data and is place of script write temporary data
 */
struct ScriptWorker
{
	const Uint8* proc;
	int shade;
	int reg[ScriptMaxRef];
	int regCustom[ScriptMaxRefCustom];

	/// Default constructor
	ScriptWorker() : proc(0), shade(0) //reg & regCustom not initialized
	{

	}

	/// Programmable bliting using script
	void executeBlit(Surface* src, Surface* dest, int x, int y);
};

/**
 * Struct used to store definition of used data by script
 */
struct ScriptContainerData
{
	typedef void (*voidFunc)();

	Uint8 type;
	Uint8 index;

	int value;
	voidFunc envFunc;
};

/**
 * Common base of script execution
 */
class ScriptContainerBase
{
	friend class ScriptParserBase;
	std::vector<Uint8> _proc;
	std::vector<ScriptContainerData> _procRefData;

protected:
	/// Protected destructor
	~ScriptContainerBase() { }
	/// Common typeless part of updating data in script
	void updateBase(ScriptWorker* ref, const void* t, int (*cast)(const void*, ScriptContainerData::voidFunc)) const;
};

/**
 * Strong typed script
 */
template<typename T>
class ScriptContainer : public ScriptContainerBase
{
public:
	typedef int (*typedFunc)(const T*);

private:
	/// Helper function using to hide type
	static int cast(const void* v, ScriptContainerData::voidFunc f)
	{
		return reinterpret_cast<typedFunc>(f)((const T*)v);
	}

public:
	/// Update values in script form unit
	void update(ScriptWorker* ref, const T* t) const
	{
		updateBase(ref, (const void*)t, &cast);
	}
};

/**
 * Struct storing avaliable operation to scripts
 */
struct ScriptParserData
{
	Uint8 procId;
	Uint8 argType[ScriptMaxArg];
	Uint8 argOffset[ScriptMaxArg];
};

/**
 * Common base of script parser
 */
class ScriptParserBase
{
	std::string _name;
	std::map<std::string, ScriptParserData> _procList;
	std::map<std::string, ScriptContainerData> _refList;

protected:
	/// Common typeless part of parsing string
	bool parseBase(ScriptContainerBase* scr, const std::string& code) const;
	/// Common typeless part of adding new function
	void addFunctionBase(const std::string& s, ScriptContainerData::voidFunc f);
	/// Show all builtin script informations
	void logScriptMetadata() const;

public:
	/// Default constructor
	ScriptParserBase(const std::string& name);

	/// Add name for custom parameter
	void addCustom(int i, const std::string& s);
	/// Add const value
	void addConst(const std::string& s, int i);
};

/**
 * Strong typed parser
 */
template<typename T>
class ScriptParser : public ScriptParserBase
{
public:
	/// Default constructor
	ScriptParser(const std::string& name) : ScriptParserBase(name)
	{
		//ScriptParser require static function in T to initialize data!
		T::ScriptRegister(this);
		logScriptMetadata();
	}

	/// Prase string and return new script
	ScriptContainer<T>* parse(const std::string& src_code) const
	{
		ScriptContainer<T>* scr = new ScriptContainer<T>();
		if(parseBase(scr, src_code))
			return scr;
		delete scr;
		return 0;
	}

	/// Add new function that will be avaliable in script
	void addFunction(const std::string& s, typename ScriptContainer<T>::typedFunc f)
	{
		addFunctionBase(s, reinterpret_cast<ScriptContainerData::voidFunc>(f));
	}
};

} //namespace OpenXcom

#endif	/* OPENXCOM_SCRIPT_H */

