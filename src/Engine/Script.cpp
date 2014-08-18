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

#include <sstream>
#include <iomanip>

#include "Logger.h"
#include "Options.h"
#include "Script.h"
#include "Surface.h"
#include "ShaderDraw.h"
#include "ShaderMove.h"

/**
 * Script (`ScriptBase`) is build of two parts: proc (`_proc`) and ref (`_procRefData`).
 *
 * proc: vector of bytes describing script operations and its arguments
 *
 * [op0][arg][arg] [op1] [op2][arg] [op3] ...
 *
 * every operation have varied numbers of arguments.
 * every argument is simple index in ref table, but script engine try keep
 * separate every possible typ of arg: label, data, reg and const.
 * all possible operations are stored in macro `MACRO_PROC_DEFINITION` with its definitions.
 *
 * ref: table with definition of all possible arguments in script and functions that extract data form object.
 * from ref is created array that script work on (`ScriptWorkRef`).
 * ref have now fixed (`ScriptMaxRef`) size that can be problematic if people will create more complex scripts.
 * hard limit is 255, after that proc type need be chanced to bigger int.
 *
 * Script have 4 different arguments types.
 *
 * label: position in proc table, read only.
 * data: some data read form object, read only.
 * const: const int value written in source code of script like "0", "42" or "0x13", read only, can be used in place of data arg.
 * reg: register that store values that script work on, write/read, can be used in place of data arg.
 *
 * Internal arguments, not accessible by end user.
 *
 * none: "empty" argument.
 * prog: current position of script execution.
 * test: variable that is used for conditions (alias of one of reg).
 * result: final value of script execution (alias of one of reg).
 */

namespace OpenXcom
{

namespace
{

////////////////////////////////////////////////////////////
//						arg definition
////////////////////////////////////////////////////////////
#define MACRO_ARG_None(i)
#define MACRO_ARG_Prog(i)	int& Prog ,
#define MACRO_ARG_Test(i)	int& Test ,
#define MACRO_ARG_Result(i)	int& Result ,

#define MACRO_ARG_Reg(i)	int& Reg##i ,
#define MACRO_ARG_Data(i)	const int& Data##i ,
#define MACRO_ARG_Const(i)	const int& Const##i ,
#define MACRO_ARG_Label(i)	const int& Label##i ,

#define MACRO_SIZE_None		0
#define MACRO_SIZE_Prog		0
#define MACRO_SIZE_Test		0
#define MACRO_SIZE_Result	0

#define MACRO_SIZE_Reg		1
#define MACRO_SIZE_Data		1
#define MACRO_SIZE_Const	1
#define MACRO_SIZE_Label	1

#define MACRO_ARG_OFFSET(i, A0, A1, A2, A3) ( \
	(i > 0 ? MACRO_SIZE_##A0 : 0) + \
	(i > 1 ? MACRO_SIZE_##A1 : 0) + \
	(i > 2 ? MACRO_SIZE_##A2 : 0) + \
	(i > 3 ? MACRO_SIZE_##A3 : 0) \
)

enum ArgEnum
{
	ArgNone,
	ArgProg,
	ArgTest,
	ArgResult,

	ArgReg,
	ArgData,
	ArgConst,
	ArgLabel,
};

////////////////////////////////////////////////////////////
//						reg definition
////////////////////////////////////////////////////////////
enum RegEnum
{
	RegIn,
	RegCond,
	RegR0,
	RegR1,
	RegR2,
	RegR3,
	RegCustom,
	RegEnumMax = RegCustom + ScriptMaxRefCustom,
};

////////////////////////////////////////////////////////////
//						proc definition
////////////////////////////////////////////////////////////
inline void addShade_h(int& reg, const int& var)
{
	const int newShade = (reg & 0xF) + var;
	if (newShade > 0xF)
	{
		// so dark it would flip over to another color - make it black instead
		reg = 0xF;
		return;
	}
	else if(newShade > 0)
	{
		reg = (reg & 0xF0) | newShade;
		return;
	}
	reg &= 0xF0;
	//prevent overflow to 0 or another color - make it white instead
	if(!reg || newShade < 0)
		reg = 0x01;
}

inline bool mulAddMod_h(int& reg, const int& mul, const int& add, const int& mod)
{
	const int a = reg * mul + add;
	if(mod)
		reg = (a % mod + mod) % mod;
	return !mod;
}

inline bool wavegen_rect_h(int& reg, const int& period, const int& size, const int& max)
{
	if(period <= 0)
		return true;
	reg %= period;
	if(reg < 0)
		reg += reg;
	if(reg > size)
		reg = 0;
	else
		reg = max;
	return false;
}

inline bool wavegen_saw_h(int& reg, const int& period, const int& size, const int& max)
{
	if(period <= 0)
		return true;
	reg %= period;
	if(reg < 0)
		reg += reg;
	if(reg > size)
		reg = 0;
	else if(reg > max)
		reg = max;
	return false;
}

inline bool wavegen_tri_h(int& reg, const int& period, const int& size, const int& max)
{
	if(period <= 0)
		return true;
	reg %= period;
	if(reg < 0)
		reg += reg;
	if(reg > size)
		reg = 0;
	else
	{
		if(reg > size/2)
			reg = size - reg;
		if(reg > max)
			reg = max;
	}
	return false;
}
/**
 * Main macro defining all available operation in script engine.
 * @param IMPL macro function that access data. Take 6 args: Name, type of 4 proc args and definition of operation
 */
#define MACRO_PROC_DEFINITION(IMPL) \
	/*	Name,		Arg0,	Arg1, Arg2, Arg3,	Implementation,							End excecution */ \
	IMPL(exit,		None,	None, None, None,	{										return true; }) \
	\
	IMPL(ret,		Result, Data, None, None,	{				Result = Data1;			return true; }) \
	IMPL(ret_gt,	Result, Data, Test, None,	{ if(Test > 0)	Result = Data1;			return Test > 0; }) \
	IMPL(ret_lt,	Result, Data, Test, None,	{ if(Test < 0)	Result = Data1;			return Test < 0; }) \
	IMPL(ret_eq,	Result, Data, Test, None,	{ if(Test == 0)	Result = Data1;			return Test == 0; }) \
	IMPL(ret_neq,	Result, Data, Test, None,	{ if(Test != 0)	Result = Data1;			return Test != 0; }) \
	\
	IMPL(skip,		Prog, Label, None, None,	{				Prog = Label1;			return false; }) \
	IMPL(skip_gt,	Prog, Label, Test, None,	{ if(Test > 0)	Prog = Label1;			return false; }) \
	IMPL(skip_lt,	Prog, Label, Test, None,	{ if(Test < 0)	Prog = Label1;			return false; }) \
	IMPL(skip_eq,	Prog, Label, Test, None,	{ if(Test == 0)	Prog = Label1;			return false; }) \
	IMPL(skip_neq,	Prog, Label, Test, None,	{ if(Test != 0)	Prog = Label1;			return false; }) \
	\
	IMPL(set,		Reg, Data, None, None,		{				Reg0 = Data1;			return false; }) \
	IMPL(set_gt,	Reg, Data, Test, None,		{ if(Test > 0)	Reg0 = Data1;			return false; }) \
	IMPL(set_lt,	Reg, Data, Test, None,		{ if(Test < 0)	Reg0 = Data1;			return false; }) \
	IMPL(set_eq,	Reg, Data, Test, None,		{ if(Test == 0)	Reg0 = Data1;			return false; }) \
	IMPL(set_neq,	Reg, Data, Test, None,		{ if(Test != 0)	Reg0 = Data1;			return false; }) \
	\
	IMPL(test,		Test, Data, Data, None,		{ Test = Data1 - Data2;					return false; }) \
	\
	IMPL(swap,		Reg, Reg, None, None,		{ std::swap(Reg0, Reg1);				return false; }) \
	IMPL(add,		Reg, Data, None, None,		{ Reg0 += Data1;						return false; }) \
	IMPL(sub,		Reg, Data, None, None,		{ Reg0 -= Data1;						return false; }) \
	IMPL(mul,		Reg, Data, None, None,		{ Reg0 *= Data1;						return false; }) \
	\
	IMPL(muladd,	Reg, Data, Data, None,		{ Reg0 = Reg0 * Data1 + Data2;			return false; }) \
	IMPL(muladdmod,	Reg, Data, Data, Data,		{ return mulAddMod_h(Reg0, Data1, Data2, Data3);		}) \
	\
	IMPL(div,		Reg, Data, None, None,		{ if(Data1) Reg0 /= Data1;				return !Data1; }) \
	IMPL(mod,		Reg, Data, None, None,		{ if(Data1) Reg0 %= Data1;				return !Data1; }) \
	\
	IMPL(shl,		Reg, Data, None, None,		{ Reg0 <<= Data1;						return false; }) \
	IMPL(shr,		Reg, Data, None, None,		{ Reg0 >>= Data1;						return false; }) \
	\
	IMPL(abs,		Reg, None, None, None,		{ Reg0 = std::abs(Reg0);				return false; }) \
	IMPL(min,		Reg, Data, None, None,		{ Reg0 = std::min(Reg0, Data1);			return false; }) \
	IMPL(max,		Reg, Data, None, None,		{ Reg0 = std::max(Reg0, Data1);			return false; }) \
	\
	IMPL(wavegen_rect,	Reg, Data, Data, Data,	{ return wavegen_rect_h(Reg0, Data1, Data2, Data3);		}) \
	IMPL(wavegen_saw,	Reg, Data, Data, Data,	{ return wavegen_saw_h(Reg0, Data1, Data2, Data3);		}) \
	IMPL(wavegen_tri,	Reg, Data, Data, Data,	{ return wavegen_tri_h(Reg0, Data1, Data2, Data3);		}) \
	\
	IMPL(get_color,	Reg, Data, None, None,		{ Reg0 = Data1 >> 4;					return false; }) \
	IMPL(set_color,	Reg, Data, None, None,		{ Reg0 = (Reg0 & 0xF) | (Data1 << 4);	return false; }) \
	IMPL(get_shade,	Reg, Data, None, None,		{ Reg0 = Data1 & 0xF;					return false; }) \
	IMPL(set_shade,	Reg, Data, None, None,		{ Reg0 = (Reg0 & 0xF0) | (Data1 & 0xF); return false; }) \
	IMPL(add_shade,	Reg, Data, None, None,		{ addShade_h(Reg0, Data1);				return false; }) \
	/* BEWARE OF COMMA IN "Implementation"! IT WILL BREAK CODE IF USED WITHOUT PARENTHESES */

////////////////////////////////////////////////////////////
//					Proc_Enum definition
////////////////////////////////////////////////////////////

/**
 * Macro returning enum form ProcEnum
 */
#define MACRO_PROC_ID(id) Proc##id

/**
 * Macro used for creating ProcEnum from MACRO_PROC_DEFINITION
 */
#define MACRO_CREATE_PROC_ENUM(NAME, A0, A1, A2, A3, Impl) \
	MACRO_PROC_ID(NAME),

/**
 * Enum storing id of all avaliable operations in script engine
 */
enum ProcEnum
{
	MACRO_PROC_DEFINITION(MACRO_CREATE_PROC_ENUM)
	ProcEnumMax,
};

#undef MACRO_CREATE_PROC_ENUM

////////////////////////////////////////////////////////////
//					function definition
////////////////////////////////////////////////////////////

/**
 * Macro returning name of function
 */
#define MACRO_FUNC_ID(id) Func##id

/**
 * Macro used for creating functions from MACRO_PROC_DEFINITION
 */
#define MACRO_CREATE_FUNC(NAME, A0, A1, A2, A3, Impl) \
	inline bool MACRO_FUNC_ID(NAME)(MACRO_ARG_##A0(0) MACRO_ARG_##A1(1) MACRO_ARG_##A2(2) MACRO_ARG_##A3(3) int Dummy) Impl

MACRO_PROC_DEFINITION(MACRO_CREATE_FUNC)

#undef MACRO_CREATE_FUNC

////////////////////////////////////////////////////////////
//					core loop function
////////////////////////////////////////////////////////////

/**
 * Core function in script engine used to executing scripts
 * @param in arg that is visible in script under name "in"
 * @param custom0 optional argument with is send to script under user defined name
 * @param custom1 optional argument with is send to script under user defined name
 * @param reg array of int storing data modified and read by script
 * @param proc array storing operation of script
 * @return Result of executing script
 */
inline Uint8 scriptExe(int in, ScriptWorker& data)
{
	memset(data.reg, 0, RegCustom*sizeof(int));
	memcpy(data.reg + RegCustom, data.regCustom, ScriptMaxRefCustom*sizeof(int));
	data.reg[RegIn] = in;
	int curr = 0;
	//--------------------------------------------------
	//			helper macros for this function
	//--------------------------------------------------
	#define MACRO_REG_CURR(i)	data.proc[curr + (i)]

	#define MACRO_OP_None(i)
	#define MACRO_OP_Prog(i)	curr ,
	#define MACRO_OP_Test(i)	data.reg[RegCond] ,
	#define MACRO_OP_Result(i)	data.reg[RegIn] ,

	#define MACRO_OP_Reg(i)		data.reg[MACRO_REG_CURR(i)] ,
	#define MACRO_OP_Data(i)	data.reg[MACRO_REG_CURR(i)] ,
	#define MACRO_OP_Const(i)	data.reg[MACRO_REG_CURR(i)] ,
	#define MACRO_OP_Label(i)	data.reg[MACRO_REG_CURR(i)] ,

	#define MACRO_OP_OFFSET(i, A0, A1, A2, A3) (MACRO_ARG_OFFSET(i, A0, A1, A2, A3) - MACRO_ARG_OFFSET(4, A0, A1, A2, A3))

	/**
	 * Macro creating switch case from MACRO_PROC_DEFINITION
	 */
	#define MACRO_OP(NAME, A0, A1, A2, A3, Impl) \
		case MACRO_PROC_ID(NAME): \
			curr += 1 + MACRO_ARG_OFFSET(4, A0, A1, A2, A3) ; \
			if(MACRO_FUNC_ID(NAME)( \
					MACRO_OP_##A0(MACRO_OP_OFFSET(0, A0, A1, A2, A3)) \
					MACRO_OP_##A1(MACRO_OP_OFFSET(1, A0, A1, A2, A3)) \
					MACRO_OP_##A2(MACRO_OP_OFFSET(2, A0, A1, A2, A3)) \
					MACRO_OP_##A3(MACRO_OP_OFFSET(3, A0, A1, A2, A3)) \
					0)) \
				return data.reg[RegIn]; \
			continue;
	//--------------------------------------------------

	while(true)
	{
		switch(MACRO_REG_CURR(0))
		{
		MACRO_PROC_DEFINITION(MACRO_OP)
		}
	}

	//--------------------------------------------------
	//			removing helper macros
	//--------------------------------------------------
	#undef MACRO_OP_None
	#undef MACRO_OP_Prog
	#undef MACRO_OP_Test
	#undef MACRO_OP_Result

	#undef MACRO_OP_Reg
	#undef MACRO_OP_Data
	#undef MACRO_OP_Const
	#undef MACRO_OP_Label

	#undef MACRO_REG_CURR
	#undef MACRO_OP_OFFSET
	#undef MACRO_OP
	//--------------------------------------------------
}

////////////////////////////////////////////////////////////
//						helper class
////////////////////////////////////////////////////////////
typedef std::string::const_iterator ite;
typedef typename std::vector<ScriptContainerData>::const_iterator vec_ite;
typedef typename std::map<std::string, ScriptParserData>::const_iterator cop_ite;
typedef typename std::map<std::string, ScriptContainerData>::const_iterator cref_ite;
typedef typename std::map<std::string, ScriptContainerData>::iterator ref_ite;

/**
 * Token type
 */
enum TokenEnum
{
	TokenNone,
	TokenInvaild,
	TokenColon,
	TokenSemicolon,
	TokenSymbol,
	TokenNumber,
};

/**
 * Struct represents position of token in input string
 */
struct SelectedToken
{
	///type of this token
	TokenEnum type;
	//iterator pointing place of first character
	ite begin;
	//iterator pointing place past of last character
	ite end;
};

/**
 * Helper structure used by ScriptParser::parse
 */
struct ParserHelper
{
	///member pointer accessing script operations
	std::vector<Uint8>& proc;
	///member pointer accessing script data
	std::vector<ScriptContainerData>& procRefData;
	///all available operations for script
	const std::map<std::string, ScriptParserData>& procList;
	///all available data for script
	const std::map<std::string, ScriptContainerData>& refList;
	///temporary script data
	std::map<std::string, ScriptContainerData> refListCurr;

	///index of used script data
	Uint8 refIndexUsed;

	/**
	 * Constructor
     * @param scr parsed script
     * @param proc member pointer accessing script operations
     * @param procRefData member pointer accessing script data
     * @param procList all available operations for script
     * @param ref all available data for script
     */
	ParserHelper(
			std::vector<Uint8>& proc,
			std::vector<ScriptContainerData>& procRefData,
			const std::map<std::string, ScriptParserData>& procList,
			const std::map<std::string, ScriptContainerData>& ref):
		proc(proc), procRefData(procRefData),
		procList(procList), refList(ref),
		refListCurr(),
		refIndexUsed(RegEnumMax)
	{

	}

	/**
	 * Function finalizing parsing of script
     */
	void relese()
	{
		procRefData.reserve(RegEnumMax + refListCurr.size());
		procRefData.resize(RegEnumMax);
		for(ref_ite i = refListCurr.begin(); i != refListCurr.end(); ++i)
			procRefData.push_back(i->second);

		proc.push_back(MACRO_PROC_ID(exit));
	}

	/**
	 * Function returning index of label
     * @param s name of label
     * @param index place where store found index of label
     * @return true if label exists and is valid
     */
	bool getLabel(const std::string& s, int& index)
	{
		ref_ite pos = refListCurr.find(s);
		if(pos == refListCurr.end())
		{
			ScriptContainerData data = { ArgLabel, refIndexUsed, -1 };
			refListCurr.insert(std::make_pair(s, data));
			++refIndexUsed;
			index = data.index;
			return true;
		}
		if(pos->second.type != ArgLabel || pos->second.value != -1)
			return false;
		index = pos->second.index;
		return true;
	}

	/**
	 * Function setting offset of label
     * @param s name of label
     * @param offset set value where label is pointing
     * @return true if operation success
     */
	bool setLabel(const std::string& s, int offset)
	{
		ref_ite pos = refListCurr.find(s);
		if(pos == refListCurr.end())
			return true;
		if(pos->second.value != -1)
			return false;
		pos->second.value = offset;
		return true;
	}

	/**
	 * Function returning index of data (can be reg and const too)
     * @param s name of data
     * @param index place where store found index of data
     * @return true if data exists and is valid
     */
	bool getData(const std::string& s, int& index)
	{
		if(getReg(s, index))
			return true;

		if(getConst(s, index))
			return true;

		cref_ite pos = refListCurr.find(s);
		if(pos == refListCurr.end())
		{
			pos = refList.find(s);
			if(pos == refList.end())
			{
				return false;
			}

			//change named constant to unnamed one
			if(pos->second.type == ArgConst)
			{
				std::stringstream ss;
				ss << pos->second.value;
				return getConst(ss.str(), index);
			}

			ScriptContainerData data = pos->second;
			if(data.type != ArgReg)
			{
				data.index = refIndexUsed;
				++refIndexUsed;
			}
			pos = refListCurr.insert(std::make_pair(s, data)).first;
		}
		if(pos->second.type == ArgLabel)
			return false;
		index = pos->second.index;
		return true;
	}

	/**
	 * Function returning index of const
     * @param s name of const
     * @param index place where store found index of const
     * @return true if const exists and is valid
     */
	bool getConst(const std::string& s, int& index)
	{
		cref_ite pos = refListCurr.find(s);
		if(pos == refListCurr.end())
		{
			int value = 0;
			size_t offset = 0;
			std::stringstream ss(s);
			if(s[0] == '-' || s[0] == '+')
				offset = 1;
			if(s.size() > 2 + offset && s[offset] == '0' && (s[offset + 1] == 'x' || s[offset + 1] == 'X'))
				ss >> std::hex;
			if(!(ss >> value))
				return false;

			//normalize string for constant value
			ss.str("");
			ss.clear();
			ss << std::dec;
			ss << value;
			std::string newS = ss.str();
			pos = refListCurr.find(newS);
			if(pos == refListCurr.end())
			{
				ScriptContainerData data = { ArgConst, refIndexUsed, value };
				refListCurr.insert(std::make_pair(newS, data));
				++refIndexUsed;
				index = data.index;
				return true;
			}
		}
		if(pos->second.type != ArgConst)
			return false;
		index = pos->second.index;
		return true;
	}

	/**
	 * Function returning index of reg
     * @param s name of reg
     * @param index place where store found index of reg
     * @return true if reg exists and is valid
     */
	bool getReg(const std::string& s, int& index)
	{
		if(s.compare("in") == 0)
		{
			index = RegIn;
			return true;
		}
		else if(s.compare("r0") == 0)
		{
			index = RegR0;
			return true;
		}
		else if(s.compare("r1") == 0)
		{
			index = RegR1;
			return true;
		}
		else if(s.compare("r2") == 0)
		{
			index = RegR2;
			return true;
		}
		else if(s.compare("r3") == 0)
		{
			index = RegR3;
			return true;
		}
		return false;
	}

	/**
	 * Function finding next token in string
     * @param pos current position in string
     * @param end end of string
     * @return true if pos pointing on next token
     */
	bool findToken(ite& pos, const ite& end)
	{
		bool coment = false;
		for(; pos != end; ++pos)
		{
			const char c = *pos;
			if(coment)
			{
				if(c == '\n')
				{
					coment = false;
				}
				continue;
			}
			else if(isspace(c))
			{
				continue;
			}
			else if(c == '#')
			{
				coment = true;
				continue;
			}
			return true;
		}
		return false;
	}

	/**
	 * Function extracting token form string
     * @param pos current position in string
     * @param end end of string
     * @param excepted what token type we expecting now
     * @return extracted token
     */
	SelectedToken getToken(ite& pos, const ite& end, TokenEnum excepted = TokenNone)
	{
		//groups of different types of ASCII characters
		const Uint8 none = 1;
		const Uint8 spec = 2;
		const Uint8 digit = 3;
		const Uint8 charHex = 4;
		const Uint8 charRest = 5;
		const Uint8 digitSign = 6;

		//array storing data about every ASCII character
		static Uint8 charDecoder[256] = { 0 };
		static bool init = true;
		if(init)
		{
			init = false;
			for(int i = 0; i < 256; ++i)
			{
				if(i == '#' || isspace(i))	charDecoder[i] = none;
				if(i == ':' || i == ';')	charDecoder[i] = spec;

				if(i == '+' || i == '-')	charDecoder[i] = digitSign;
				if(i >= '0' && i <= '9')	charDecoder[i] = digit;
				if(i >= 'A' && i <= 'F')	charDecoder[i] = charHex;
				if(i >= 'a' && i <= 'f')	charDecoder[i] = charHex;

				if(i >= 'G' && i <= 'Z')	charDecoder[i] = charRest;
				if(i >= 'g' && i <= 'z')	charDecoder[i] = charRest;
				if(i == '_' || i == '.')	charDecoder[i] = charRest;
			}
		}

		SelectedToken token = { TokenNone, end, end };
		if(!findToken(pos, end))
			return token;

		token.begin = pos;
		bool hex = false;
		int off = 0;

		for(; pos != end; ++pos, ++off)
		{
			const Uint8 c = *pos;
			const Uint8 decode = charDecoder[c];

			//end of string
			if(decode == none)
			{
				break;
			}
			else if(decode == spec)
			{
				if(c == ':')
				{
					//colon start new token, skip if we are in another one
					if(token.type != TokenNone)
						break;

					++pos;
					token.type = excepted == TokenColon ? TokenColon : TokenInvaild;
					break;
				}
				else if(c == ';')
				{
					//semicolon start new token, skip if we are in another one
					if(token.type != TokenNone)
						break;
					//semicolon wait for his turn, returning empty token
					if(excepted != TokenSemicolon)
						break;

					++pos;
					token.type = TokenSemicolon;
					break;
				}
				else
				{
					token.type = TokenInvaild;
					break;
				}
			}

			switch(token.type)
			{
			//begin of string
			case TokenNone:
				switch(decode)
				{
				//start of number
				case digitSign:
					--off; //skipping +- sign
				case digit:
					hex = c == '0'; //expecting hex number
					token.type = TokenNumber;
					continue;

				//start of symbol
				case charHex:
				case charRest:
					token.type = TokenSymbol;
					continue;
				}
				break;

			//middle of number
			case TokenNumber:
				switch(decode)
				{
				case charRest:
					if(off != 1) break;
					if(c != 'x' && c != 'X') break; //X in "0x1"
				case charHex:
					if(!hex) break;
				case digit:
					if(off == 0) hex = c == '0'; //expecting hex number
					continue;
				}
				break;

			//middle of symbol
			case TokenSymbol:
				switch(decode)
				{
				case charRest:
				case charHex:
				case digit:
					continue;
				}
				break;
			default:
				break;
			}
			//when decode == 0 or we find unexpected char we should end there
			token.type = TokenInvaild;
			break;
		}
		token.end = pos;
		return token;
	}
}; //struct ParserHelper

} //namespace


////////////////////////////////////////////////////////////
//						Script class
////////////////////////////////////////////////////////////
void ScriptContainerBase::updateBase(ScriptWorker* ref, const void* t, int (*cast)(const void*, ScriptContainerData::voidFunc)) const
{
	ref->proc = &(_proc[0]);
	for(vec_ite i = _procRefData.begin(); i != _procRefData.end(); ++i)
	{
		int& r = ref->reg[i->index];
		switch(i->type)
		{
		case ArgReg:
			r = 0;
			break;
		case ArgConst:
			r = i->value;
			break;
		case ArgLabel:
			r = i->value;
			break;
		case ArgData:
			r =  cast(t, i->envFunc);
			break;
		}
	}
}

namespace
{

/**
 * struct used to bliting script
 */
struct ScriptReplace
{
	static inline void func(Uint8& dest, const Uint8& src, ScriptWorker& ref, int, int)
	{
		if(src)
		{
			const int s = scriptExe(src, ref);
			if (s) dest = s;
		}
	}
};

} //namespace



/**
 * Bliting one surface to another using script
 * @param src source surface
 * @param dest destination surface
 * @param x x offset of source surface
 * @param y y offset of source surface
 */
void ScriptWorker::executeBlit(Surface* src, Surface* dest, int x, int y)
{
	if(proc)
		ShaderDraw<ScriptReplace>(ShaderSurface(dest, 0, 0), ShaderSurface(src, x, y), ShaderScalar(*this));
	else
		ShaderDraw<StandartShade>(ShaderSurface(dest, 0, 0), ShaderSurface(src, x, y), ShaderScalar(shade));
}
////////////////////////////////////////////////////////////
//					ScriptParser class
////////////////////////////////////////////////////////////

/**
 * Default constructor
 */
ScriptParserBase::ScriptParserBase(const std::string& name) : _name(name), _procList(), _refList()
{
	//--------------------------------------------------
	//					op_data init
	//--------------------------------------------------
	#define MACRO_ALL_INIT(NAME, A0, A1, A2, A3, Impl) \
	{ \
		ScriptParserData temp_var_##NAME = \
		{ \
			MACRO_PROC_ID(NAME), \
			{ \
				/* types */\
				Arg##A0, \
				Arg##A1, \
				Arg##A2, \
				Arg##A3, \
			}, \
			{ \
			/* args offsets */\
				MACRO_ARG_OFFSET(0, A0, A1, A2, A3), \
				MACRO_ARG_OFFSET(1, A0, A1, A2, A3), \
				MACRO_ARG_OFFSET(2, A0, A1, A2, A3), \
				MACRO_ARG_OFFSET(3, A0, A1, A2, A3), \
			}, \
		}; \
		_procList[#NAME] = temp_var_##NAME; \
	}

	MACRO_PROC_DEFINITION(MACRO_ALL_INIT)

	#undef MACRO_ALL_INIT
}

/**
 * Add new function extracting data for script
 * @param s name for new data
 * @param f void function pointer to function extrating data
 */
void ScriptParserBase::addFunctionBase(const std::string& s, ScriptContainerData::voidFunc f)
{
	ref_ite pos = _refList.find(s);
	if(pos == _refList.end())
	{
		ScriptContainerData data = { ArgData, 0, 0, f };
		_refList.insert(std::make_pair(s, data));
	}
	else
	{
		pos->second.envFunc = f;
	}
}

/**
 * Set name for custom script param
 * @param i what custom param
 * @param s name for first custom parameter
 */
void ScriptParserBase::addCustom(int i, const std::string& s)
{
	if(i >= 0 && i < ScriptMaxRefCustom)
	{
		ScriptContainerData data = { ArgReg, (Uint8)(RegCustom + i) };
		_refList.insert(std::make_pair(s, data));
	}
}

/**
 * Add const value to script
 * @param s name for const
 * @param i value
 */
void ScriptParserBase::addConst(const std::string& s, int i)
{
	ScriptContainerData data = { ArgConst, 0, i };
	_refList.insert(std::make_pair(s, data));
}

/**
 * Parse string and write script to ScriptBase
 * @param src struct where final script is write to
 * @param src_code string with script
 * @return true if string have valid script
 */
bool ScriptParserBase::parseBase(ScriptContainerBase* destScript, const std::string& srcCode) const
{
	ParserHelper help(
		destScript->_proc, destScript->_procRefData,
		_procList, _refList
	);
	ite curr = srcCode.begin();
	ite end = srcCode.end();
	if(curr == end)
		return false;

	while(true)
	{
		if(!help.findToken(curr, end))
		{
			if(help.refIndexUsed > ScriptMaxRef)
			{
				Log(LOG_ERROR) << "script used to many references\n";
				return false;
			}
			for(ref_ite i = help.refListCurr.begin(); i != help.refListCurr.end(); ++i)
			{
				if(i->second.type == ArgLabel && i->second.value == -1)
				{
					Log(LOG_ERROR) << "invalid use of label: '" << i->first << "' without declaration\n";
					return false;
				}
			}
			help.relese();
			return true;
		}

		ite line_begin = curr;
		ite line_end;
		SelectedToken label = { TokenNone };

		SelectedToken op = help.getToken(curr, end);
		SelectedToken args[ScriptMaxArg];
		args[0] = help.getToken(curr, end, TokenColon);
		if(args[0].type == TokenColon)
		{
			std::swap(op, label);
			op = help.getToken(curr, end);
			args[0] = help.getToken(curr, end);
		}
		for(int i = 1; i < ScriptMaxArg; ++i)
			args[i] = help.getToken(curr, end);
		SelectedToken f = help.getToken(curr, end, TokenSemicolon);

		line_end = curr;
		//validation
		bool valid = true;
		valid &= label.type == TokenSymbol || label.type == TokenNone;
		valid &= op.type == TokenSymbol;
		for(int i = 0; i < ScriptMaxArg; ++i)
			valid &= args[i].type == TokenSymbol || args[i].type == TokenNumber || args[i].type == TokenNone;
		valid &= f.type == TokenSemicolon;

		if(!valid)
		{
			if(f.type != TokenSemicolon)
			{
				//fixing `line_end` position
				while(curr != end && *curr != ';')
					++curr;
				if(curr != end)
					++curr;
				line_end = curr;
			}
			Log(LOG_ERROR) << "invalid line: '" << std::string(line_begin, line_end) << "'\n";
			return false;
		}
		else
		{
			int temp = 0;
			std::string op_str(op.begin, op.end);
			std::string label_str(label.begin, label.end);
			cop_ite op_curr = _procList.find(op_str);
			if(op_curr == _procList.end())
			{
				Log(LOG_ERROR) << "invalid operation '"<< op_str <<"' in line: '" << std::string(line_begin, line_end) << "'\n";
				return false;
			}

			if(!label_str.empty() && !help.setLabel(label_str, destScript->_proc.size()))
			{
				Log(LOG_ERROR) << "invalid label '"<< label_str <<"' in line: '" << std::string(line_begin, line_end) << "'\n";
				return false;
			}

			//matching args form operation definition with args avaliable in string
			int i = 0, j = 0;
			destScript->_proc.push_back(op_curr->second.procId);
			while(i < ScriptMaxArg && j < ScriptMaxArg)
			{
				std::string arg_val(args[j].begin, args[j].end);
				switch(op_curr->second.argType[i])
				{
				//this is special args that arent visible in source, skip
				case ArgNone:
				case ArgProg:
				case ArgTest:
				case ArgResult:
					++i;
					continue;

				//arg that is label
				case ArgLabel:
					if(!help.getLabel(arg_val, temp))
					{
						Log(LOG_ERROR) << "invalid label argument '"<< arg_val <<"' in line: '" << std::string(line_begin, line_end) << "'\n";
						return false;
					}
					destScript->_proc.push_back(temp);
					break;

				//arg that is data like "solder_gender", "hp" etc. can be const or reg too.
				case ArgData:
					if(!help.getData(arg_val, temp))
					{
						Log(LOG_ERROR) << "invalid data argument '"<< arg_val <<"' in line: '" << std::string(line_begin, line_end) << "'\n";
						return false;
					}
					destScript->_proc.push_back(temp);
					break;

				//arg that is const value e.g. "1", "-0x13"
				case ArgConst:
					if(!help.getConst(arg_val, temp))
					{
						Log(LOG_ERROR) << "invalid const argument '"<< arg_val <<"' in line: '" << std::string(line_begin, line_end) << "'\n";
						return false;
					}
					destScript->_proc.push_back(temp);
					break;

				//arg that is reg like "in", "r0"
				case ArgReg:
					if(!help.getReg(arg_val, temp))
					{
						Log(LOG_ERROR) << "invalid reg argument '"<< arg_val <<"' in line: '" << std::string(line_begin, line_end) << "'\n";
						return false;
					}
					destScript->_proc.push_back(temp);
					break;
				}
				++i;
				++j;
			}
			if(!(j == ScriptMaxArg || args[j].type == TokenNone))
			{
				Log(LOG_ERROR) << "wrong arguments in line: '" << std::string(line_begin, line_end) << "'\n";
				return false;
			}
		}
	}
}

/**
 * Print all metadata
 */
void ScriptParserBase::logScriptMetadata() const
{
	if (Options::debug)
	{
		const int tabSize = 8;
		static bool printOp = true;
		if(printOp)
		{
			printOp = false;
			Logger opLog;
			#define MACRO_ALL_LOG(NAME, A0, A1, A2, A3, Impl) \
				opLog.get() \
					<< "Op:    " << std::setw(tabSize*2) << #NAME \
					<< "Args:  " << std::setw(tabSize) << #A0 "," << std::setw(tabSize) << #A1 "," << std::setw(tabSize) << #A2 "," << std::setw(tabSize + 5) << #A3 \
					<< "Impl:  " #Impl "\n";

			opLog.get() << "Available script operations:\n" << std::left;
			MACRO_PROC_DEFINITION(MACRO_ALL_LOG)
			#undef MACRO_ALL_LOG
		}

		Logger refLog;
		refLog.get() << "Script data for: " << _name << "\n" << std::left;
		for(cref_ite ite = _refList.begin(); ite != _refList.end(); ++ite)
		{
			if(ite->second.type == ArgConst)
				refLog.get() << "Ref: " << std::setw(30) << ite->first << "Value: " << ite->second.value << "\n";
			else
				refLog.get() << "Ref: " << std::setw(30) << ite->first << "\n";
		}
	}
}

} //namespace OpenXcom
