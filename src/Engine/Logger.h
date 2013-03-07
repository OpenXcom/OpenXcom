/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#ifndef OPENXCOM_LOGGER_H
#define OPENXCOM_LOGGER_H

#include <sstream>
#include <string>
#include <stdio.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// the following macros interfere with std::max and std::min as used throughout...
#undef min 
#undef max 
#ifndef LOCALE_INVARIANT
#define LOCALE_INVARIANT 0x007f
#endif
#else
#include <time.h>
#endif

namespace OpenXcom
{

inline std::string now();

/**
 * Defines the various severity levels of
 * information logged by the game.
 */
enum SeverityLevel
{
	LOG_FATAL,		/**< Something horrible has happened and the game is going to die! */
	LOG_ERROR,		/**< Something bad happened but we can still move on. */
	LOG_WARNING,	/**< Something weird happened, nothing special but it's good to know. */
	LOG_INFO,		/**< Useful information for users/developers to help debug and figure stuff out. */
	LOG_DEBUG		/**< Purely test stuff to help developers implement, not really relevant to users. */
};

/**
 * A basic logging and debugging class, prints output to stdout/files
 * and can capture stack traces of fatal errors too.
 * @note Wasn't really satisfied with any of the libraries around
 * so I rolled my own. Based on http://www.drdobbs.com/cpp/logging-in-c/201804215
 */
class Logger
{
public:
    Logger();
    virtual ~Logger();
    std::ostringstream& get(SeverityLevel level = LOG_INFO);
	
    static SeverityLevel& reportingLevel();
	static std::string& logFile();
    static std::string toString(SeverityLevel level);
protected:
    std::ostringstream os;
private:
    Logger(const Logger&);
    Logger& operator =(const Logger&);
};

inline Logger::Logger()
{
}

inline std::ostringstream& Logger::get(SeverityLevel level)
{
	os << "[" << toString(level) << "]" << "\t";
    return os;
}

inline Logger::~Logger()
{
    os << std::endl;
	if (reportingLevel() == LOG_DEBUG)
	{
		fprintf(stderr, "%s", os.str().c_str());
		fflush(stderr);
	}
	std::stringstream ss;
	ss << "[" << now() << "]" << "\t" << os.str();
	FILE *file = fopen(logFile().c_str(), "a");
	fprintf(file, "%s", ss.str().c_str());
    fflush(file);
	fclose(file);
}

inline SeverityLevel& Logger::reportingLevel()
{
    static SeverityLevel reportingLevel = LOG_DEBUG;
    return reportingLevel;
}

inline std::string& Logger::logFile()
{
    static std::string logFile = "openxcom.log";
    return logFile;
}

inline std::string Logger::toString(SeverityLevel level)
{
	static const char* const buffer[] = {"FATAL", "ERROR", "WARN", "INFO", "DEBUG"};
    return buffer[level];
}

#define Log(level) \
    if (level > Logger::reportingLevel()) ; \
    else Logger().get(level)

inline std::string now()
{
    const int MAX_LEN = 25, MAX_RESULT = 80;
#ifdef _WIN32
    char date[MAX_LEN], time[MAX_LEN];
	if (GetDateFormatA(LOCALE_INVARIANT, 0, 0, 
            "dd'-'MM'-'yyyy", date, MAX_LEN) == 0)
        return "Error in Now()";
    if (GetTimeFormatA(LOCALE_INVARIANT, TIME_FORCE24HOURFORMAT, 0, 
            "HH':'mm':'ss", time, MAX_LEN) == 0)
        return "Error in Now()";

    char result[MAX_RESULT] = {0};
    sprintf(result, "%s %s", date, time);
#else
	char buffer[MAX_LEN];
    time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
    strftime(buffer, MAX_LEN, "%d-%m-%Y %H:%M:%S", timeinfo);
    char result[MAX_RESULT] = {0};
    sprintf(result, "%s", buffer); 
#endif
    return result;
}

}

#endif
