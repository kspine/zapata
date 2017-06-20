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

#include <zapata/http/config.h>

#include <string>
#include <zapata/http/HTTPObj.h>
#include <zapata/http/HTTPParser.h>

using namespace std;
#if !defined __APPLE__
using namespace __gnu_cxx;
#endif

namespace zpt {

	zpt::HTTPReq& fromhttpfile(ifstream& _in, zpt::HTTPReq& _out) ;
	zpt::HTTPRep& fromhttpfile(ifstream& _in, zpt::HTTPRep& _out) ;
	zpt::HTTPReq& fromhttpstream(istream& _in, zpt::HTTPReq& _out) ;
	zpt::HTTPRep& fromhttpstream(istream& _in, zpt::HTTPRep& _out) ;
	zpt::HTTPReq& fromhttpstr(string& _in, zpt::HTTPReq& _out) ;
	zpt::HTTPRep& fromhttpstr(string& _in, zpt::HTTPRep& _out) ;

	void tostr(string& _out, HTTPReq& _in) ;
	void tostr(string& _out, HTTPRep& _in) ;
	void tostr(ostream& _out, HTTPReq& _in) ;
	void tostr(ostream& _out, HTTPRep& _in) ;
}

