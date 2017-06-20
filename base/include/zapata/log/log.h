/*
The MIT License (MIT)

Copyright (c) 2014 n@zgul <n@zgul.me>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <zapata/base/config.h>

using namespace std;
#if !defined __APPLE__
using namespace __gnu_cxx;
#endif

#define __HOST__ string(zpt::log_hostname())
#define zlog(x,y) (y <= zpt::log_lvl ? zpt::log(std::string(x), y, __HOST__, __LINE__, __FILE__) : 0)
#define zdbg(x) zlog(std::string(x), zpt::debug)
#define ztrace(x) zlog(std::string(x), zpt::trace)
#define ztrace_v(x) zlog(std::string(x) + std::string(" | ") + std::string(__PRETTY_FUNCTION__), zpt::trace)
#define zverbose(x) zlog(std::string(x), zpt::verbose)

namespace zpt {
	extern short int log_lvl;
	extern ostream* log_fd;
	extern long log_pid;
	extern string* log_pname;
	extern char* log_hname;
	extern short log_format;

	extern const char* log_lvl_names[];

	enum LogLevel {
		emergency = 0,
		alert = 1,
		critical = 2,
		error = 3,
		warning = 4,
		notice = 5,
		info = 6,
		debug = 7,
		trace = 8,
		verbose = 9
	};

	int log(string _text, zpt::LogLevel _level, string _host, int _line, string _file);
	char * log_hostname();
}
