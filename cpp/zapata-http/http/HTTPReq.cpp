#include <http/HTTPObj.h>

#include <iostream>
#include <exceptions/CastException.h>
#include <exceptions/NoHeaderNameException.h>

zapata::HTTPReqRef::HTTPReqRef() : __name(NULL) {
}

zapata::HTTPReqRef::~HTTPReqRef() {
}

zapata::HTTPMethod zapata::HTTPReqRef::method() {
	return this->__method;
}

void zapata::HTTPReqRef::method(zapata::HTTPMethod _method) {
	this->__method = _method;
}

string& zapata::HTTPReqRef::url() {
	return this->__url;
}

void zapata::HTTPReqRef::url(string _url) {
	this->__url.assign(_url.data());
}

string& zapata::HTTPReqRef::body() {
	return this->__body;
}

void zapata::HTTPReqRef::body(string _body) {
	this->__body.assign(_body.data());
}

string& zapata::HTTPReqRef::query() {
	return this->__query;
}

void zapata::HTTPReqRef::query(string _query) {
	this->__query.assign(_query.data());
}

void zapata::HTTPReqRef::unset(string _in) {
	HTTPReqRef::iterator _i;
	if ((this->__flags & zapata::params) == zapata::params) {
		if ((_i = this->__params.find(_in)) != this->__params.end()) {
			return this->__params.erase(_i);
		}
	}
	else {
		if ((_i = this->find(_in)) != this->end()) {
			return this->erase(_i);
		}
	}
}

void zapata::HTTPReqRef::unset(long long _in) {
}

void zapata::HTTPReqRef::put(int _in) {
	if (this->__name == NULL) {
		this->__name = new string();
		zapata::tostr(*this->__name, _in);
		this->__name->insert(0, "_");
	}
	else {
		string* _s = new string();
		zapata::tostr(*_s, _in);
		if ((this->__flags & zapata::params) == zapata::params) {
			this->__params.insert(string(this->__name->data()), _s);
		}
		else {
			this->insert(string(this->__name->data()), _s);
		}
		delete this->__name;
		this->__name = NULL;
	}
}

void zapata::HTTPReqRef::put(long _in) {
	if (this->__name == NULL) {
		this->__name = new string();
		zapata::tostr(*this->__name, _in);
		this->__name->insert(0, "_");
	}
	else {
		string* _s = new string();
		zapata::tostr(*_s, _in);
		if ((this->__flags & zapata::params) == zapata::params) {
			this->__params.insert(string(this->__name->data()), _s);
		}
		else {
			this->insert(string(this->__name->data()), _s);
		}
		delete this->__name;
		this->__name = NULL;
	}
}

void zapata::HTTPReqRef::put(long long _in) {
	if (this->__name == NULL) {
		this->__name = new string();
		zapata::tostr(*this->__name, _in);
		this->__name->insert(0, "_");
	}
	else {
		string* _s = new string();
		zapata::tostr(*_s, _in);
		if ((this->__flags & zapata::params) == zapata::params) {
			this->__params.insert(string(this->__name->data()), _s);
		}
		else {
			this->insert(string(this->__name->data()), _s);
		}
		delete this->__name;
		this->__name = NULL;
	}
}

void zapata::HTTPReqRef::put(double _in) {
	if (this->__name == NULL) {
		this->__name = new string();
		zapata::tostr(*this->__name, _in);
		size_t dot = this->__name->find(".");
		if (dot != string::npos) {
			this->__name->erase(dot, 1);
			this->__name->insert(dot, "_");
		}
		this->__name->insert(0, "_");
	}
	else {
		string* _s = new string();
		zapata::tostr(*_s, _in);
		if ((this->__flags & zapata::params) == zapata::params) {
			this->__params.insert(string(this->__name->data()), _s);
		}
		else {
			this->insert(string(this->__name->data()), _s);
		}
		delete this->__name;
		this->__name = NULL;
	}
}

void zapata::HTTPReqRef::put(bool _in) {
	if (this->__name == NULL) {
		this->__name = new string();
		zapata::tostr(*this->__name, _in);
		this->__name->insert(0, "_");
	}
	else {
		string* _s = new string();
		zapata::tostr(*_s, _in);
		if ((this->__flags & zapata::params) == zapata::params) {
			this->__params.insert(string(this->__name->data()), _s);
		}
		else {
			this->insert(string(this->__name->data()), _s);
		}
		delete this->__name;
		this->__name = NULL;
	}
}

void zapata::HTTPReqRef::put(string _in) {
	if (this->__name == NULL) {
		this->__name = new string(_in);
	}
	else {
		string* _s = new string(_in);
		if ((this->__flags & zapata::params) == zapata::params) {
			this->__params.insert(string(this->__name->data()), _s);
		}
		else {
			this->insert(string(this->__name->data()), _s);
		}
		delete this->__name;
		this->__name = NULL;
	}
}

string& zapata::HTTPReqRef::get(size_t _idx) {
	if ((this->__flags & zapata::params) == zapata::params) {
		if(_idx < this->__params.size()) {
			return *this->__params.at(_idx);
		}
	}
	else {
		if(_idx < this->size()) {
			return *this->at(_idx);
		}
	}
	return zapata::nil_header;
}

string& zapata::HTTPReqRef::get(const char* _idx) {
	HTTPReqRef::iterator i;
	if ((this->__flags & zapata::params) == zapata::params) {
		if ((i = this->__params.find(_idx)) != this->__params.end()) {
			return *((*i)->second);
		}
	}
	else {
		if ((i = this->find(_idx)) != this->end()) {
			return *((*i)->second);
		}
	}
	return zapata::nil_header;
}

string& zapata::HTTPReqRef::header(const char* _idx) {
	return this->get(_idx);
}

string& zapata::HTTPReqRef::param(const char* _idx) {
	HTTPReqRef::iterator i;
	if ((i = this->__params.find(_idx)) != this->__params.end()) {
		return *((*i)->second);
	}
	return zapata::nil_header;
}

void zapata::HTTPReqRef::stringify(ostream& _out, short _flags, string _tabs) {
	if (_flags & zapata::pretty) {
	}
	_out << zapata::method_names[this->__method] << " " << this->__url;
	if (this->__params.size() != 0) {
		_out << "?";
		for (HTTPReqRef::iterator i = this->__params.begin(); i != this->__params.end(); i++) {
			if (i != this->__params.begin()) {
				_out << "&";
			}
			_out << (*i)->first << "=" << *(*i)->second;
		}
	}
	_out  << " HTTP/1.1" << CRLF;;
	for (HTTPReqRef::iterator i = this->begin(); i != this->end(); i++) {
		_out << (*i)->first << ": " << *(*i)->second << CRLF;
	}
	_out << CRLF;
	_out << this->__body;
	if (_flags & zapata::pretty) {
	}
}

void zapata::HTTPReqRef::stringify(string& _out, short _flags, string _tabs) {
	ostringstream _ret;
	this->stringify(_ret, _flags, _tabs);
	_ret << flush;
	_out.insert(_out.length(), _ret.str());
	_ret.clear();
}
