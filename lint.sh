#!/bin/sh
# Copyright 2018 atlimit8

# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

case "$1" in
	check-file)
		echo Checking "[0;35m$2[m" >&2
		grep --color=always -nHE '[ 	]+$' "$2" >&2 && echo -n .
		;;
	check|"")
		echo Checking for trailing and empty line tabs and spaces...
		file_count="$(find .astylerc .editorconfig .gitattributes .gitignore .travis.yml autogen.sh \
			*.txt *.sh *.bat *.ac *.am *.md  \
			bin cmake docs install m4 res src xcode \
			-type f '(' \
			-name "*.txt" -o -name "*.md" -o -name "*.xml" -o \
			-name .gitignore -o -name .gitattributes -o -name .astylerc -o -name .editorconfig -o \
			-name "*.sh" -o -name "*.bat" -o -name "*.sln" -o -name "*.vcxproj*" -o -name "*.rc" -o -name "Makefile.*" -o \
			-name "*.in" -o -name "*.am" -o -name "*.ac" -o -name "*.cmake" -o -name "*.yml" -o \
			-name "*.nam" -o -name "*.rul" -o -name "*.shader" -o \
			-name "*.cpp" -o -name "*.h" -o -name "*.pch" -o \
			-name "*.m" -o -name "*.json" -o -name "*.plist" -o \
			-name "*.xcworkspacedata" -o -name "*.xcscheme" -o -name "*.pbxproj" -o \
			-name "*.nsi" -o -name "*.nsh" -o -name "*.m4" -o -name "*.rc" -o \
			-name "*.desktop" -o -name "*.spec" -o -name "*.ebuild" -o \
			-path "install/debian/*" -o -path "install/opensuse/*" \
			')' -exec "$0" check-file '{}' \; \
			| wc -c)"

		test -z "$file_count" && echo Unknown error occured. && exit 1
		test "$file_count" -ne 0 && {
			echo Trailing tabs or spaces detected in $file_count files.
			exit 1
		} || {
			echo No trailing tabs or spaces detected.
		}
		;;
esac
