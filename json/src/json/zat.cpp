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

#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/ipc.h>
#include <zapata/json.h>
#include <zapata/log/log.h>

using namespace std;
#if !defined __APPLE__
using namespace __gnu_cxx;
#endif

int main(int argc, char* argv[]) {
	char _c;
	int _level = 8;

	while ((_c = getopt(argc, argv, "l:")) != -1) {
		switch (_c) {
		case 'l': {
			std::string _l(optarg);
			zpt::fromstr(_l, &_level);
			break;
		}
		}
	}

	std::istringstream _iss;
	std::string _line;
	while (std::getline(std::cin, _line)) {
		zpt::trim(_line);
		if (_line.find("{") != 0) {
			continue;
		}
		try {
			zpt::json _json;
			_iss.str(_line);
			_iss >> _json;

			if ((int)_json["level"] > _level) {
				continue;
			}
			double _intpart;
			double _fracpart = modf((double)_json["timestamp"], &_intpart);
			zpt::timestamp_t _ts = (_intpart * 1000) + (_fracpart * 1000);
			std::string _time = zpt::timestamp(_ts);
			std::string _cmd((std::string)_json["exec"]);
			_cmd.assign(_cmd.substr(_cmd.rfind("/") + 1));
			std::cout << zpt::log_lvl_names[(int)_json["level"]] << "\033[1;37m" << _time << "\033[0m | "
				  << (std::string)_json["short_message"] << " | " << _cmd << ":"
				  << (std::string)_json["pid"] << " " << (std::string)_json["file"] << ":"
				  << (std::string)_json["line"] << endl
				  << flush;
		} catch (...) {
		}
	}
	return 0;
}
