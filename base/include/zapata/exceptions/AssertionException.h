/*
The MIT License (MIT)

Copyright (c) 2017 n@zgul <n@zgul.me>

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

#include <exception>
#include <string>

using namespace std;
#if !defined __APPLE__
using namespace __gnu_cxx;
#endif

namespace zpt {

class AssertionException : public std::exception {
      private:
	string __what;
	int __http_code;
	int __code;
	string __description;
	int __line;
	string __file;
	string __backtrace;

      public:
	AssertionException(string _what, int _http_code, int _code, string _desc, int _line = 0, string _file = "");
	AssertionException(string _in,
			   int _http_code,
			   int _code,
			   string _desc,
			   int _line,
			   string _file,
			   char** _backtrace,
			   size_t _backtrace_size);
	virtual ~AssertionException() throw();

	virtual const char* what();
	virtual const char* description();
	virtual const char* backtrace();
	virtual int code();
	virtual int status();
};

typedef AssertionException assertion;
}
