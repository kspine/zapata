/*
The MIT License (MIT)

Copyright (c) 2014 Pedro (n@zgul) Figueiredo <pedro.figueiredo@gmail.com>

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

#include <json/JSONObj.h>

#include <iostream>
#include <text/convert.h>
#include <exceptions/CastException.h>
#include <exceptions/NoAttributeNameException.h>

zapata::JSONIntRef::JSONIntRef(long long _in) : __value(_in) {
}

zapata::JSONIntRef::~JSONIntRef() {
}

zapata::JSONType zapata::JSONIntRef::type() {
	return zapata::JSInteger;
}

void zapata::JSONIntRef::put(int _in) {
	this->__value = _in;
}

void zapata::JSONIntRef::put(long _in) {
	this->__value = _in;
}

void zapata::JSONIntRef::put(long long _in) {
	this->__value = _in;
}

#ifdef __LP64__
void zapata::JSONIntRef::put(unsigned int _in) {
	this->__value = _in;
}
#endif

void zapata::JSONIntRef::put(size_t _in) {
	this->__value = _in;
}

void zapata::JSONIntRef::put(double _in) {
	this->__value = _in;
}

void zapata::JSONIntRef::put(bool _in) {
	this->__value = _in;
}

void zapata::JSONIntRef::put(string _in) {
	zapata::fromstr(_in, &this->__value);
}

void zapata::JSONIntRef::put(JSONObj& _in) {
}

void zapata::JSONIntRef::put(JSONArr& _in) {
}

void zapata::JSONIntRef::put(JSONBool& _in) {
	this->put((bool) *_in.get());
}

void zapata::JSONIntRef::put(JSONInt& _in) {
	this->put((long long) *_in.get());
}

void zapata::JSONIntRef::put(JSONDbl& _in) {
	this->put((double) *_in.get());
}

void zapata::JSONIntRef::put(JSONStr& _in) {
	this->put((string) *_in.get());
}

void zapata::JSONIntRef::put(JSONNil& _in) {
	this->__value = 0;
}

bool zapata::JSONIntRef::compare(JSONElement& _in) {
	return this->type() == _in.type() && this->__value == ((JSONIntRef) _in).__value;
}

zapata::JSONElement& zapata::JSONIntRef::get(size_t _idx) {
	return *this;
}

zapata::JSONElement& zapata::JSONIntRef::get(const char* _idx) {
	return *this;
}

int zapata::JSONIntRef::getInt() {
	return (int) this->__value;
}

long zapata::JSONIntRef::getLong() {
	return (long) this->__value;
}

long zapata::JSONIntRef::getLongLong() {
	return (long long) this->__value;
}

unsigned int zapata::JSONIntRef::getUnsignedInt() {
	return (unsigned int) this->__value;
}

double zapata::JSONIntRef::getDouble() {
	return (double) this->__value;
}

bool zapata::JSONIntRef::getBool() {
	return (bool) this->__value;
}

string zapata::JSONIntRef::getString() {
	string _ret;
	zapata::tostr(_ret, this->__value);
	return _ret;
}

zapata::JSONObjRef& zapata::JSONIntRef::getJSONObj() {
	throw CastException("can not convert from basic type to JSONObj");
}

zapata::JSONArrRef& zapata::JSONIntRef::getJSONArr() {
	throw CastException("can not convert from basic type to JSONArr");
}

void zapata::JSONIntRef::stringify(ostream& _out, short _flags, string _tabs) {
	_out << this->__value << flush;
}

void zapata::JSONIntRef::stringify(string& _out, short _flags, string _tabs) {
	zapata::tostr(_out, this->__value);
}

