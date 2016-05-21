/*
The MIT License (MIT)

Copyright (c) 2014 n@zgul <naazgull@dfz.pt>

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

#include <zapata/zmq/ZMQPolling.h>

zpt::ZMQ::ZMQ(std::string _connection, zpt::JSONPtr _options, zpt::EventEmitterPtr _emitter) : __options( _options ), __context(1), __connection(_connection.data()), __self(this), __emitter(_emitter) {
	this->__pt_mtx = new pthread_mutex_t();
	this->__pt_attr = new pthread_mutexattr_t();
	pthread_mutexattr_init(this->__pt_attr);
	pthread_mutex_init(this->__pt_mtx, this->__pt_attr);
}

zpt::ZMQ::~ZMQ() {
	pthread_mutexattr_destroy(this->__pt_attr);
	pthread_mutex_destroy(this->__pt_mtx);
	delete this->__pt_mtx;
	delete this->__pt_attr;
}

zpt::JSONPtr zpt::ZMQ::options() {
	return this->__options;
}

zmq::context_t& zpt::ZMQ::context() {
	return this->__context;
}

std::string& zpt::ZMQ::connection() {
	return this->__connection;
}

zpt::ZMQPtr zpt::ZMQ::self() {
	return this->__self;
}

zpt::EventEmitterPtr zpt::ZMQ::emitter() {
	return this->__emitter;
}

void zpt::ZMQ::lock() {
	pthread_mutex_lock(this->__pt_mtx);
}

void zpt::ZMQ::unlock() {
	pthread_mutex_unlock(this->__pt_mtx);	
}

zpt::JSONPtr zpt::ZMQ::recv() {
	std::vector<std::string> _parts;
	int _more = 0;
	size_t _more_size = sizeof(_more);

	this->lock();
	{
		do {
			zmq::message_t _request;
			try {
				this->in().recv(& _request);
			}
			catch(zmq::error_t& e) {
				throw;
			}
			_parts.push_back(string(static_cast<char*>(_request.data()), _request.size()));

			_more = 0;
			this->in().getsockopt(ZMQ_RCVMORE, & _more, & _more_size);
		}
		while(_more);
	}
	this->unlock();

	string _channel(_parts[0]);
	string _method(_parts[1]);
	string _resource(_parts[2]);
	string _body(_parts[_parts.size() - 1]);

	zpt::JSONPtr _headers = zpt::mkobj();
	if (_parts.size() > 4) {
		for (size_t _idx = 3; _idx != _parts.size() - 1; _idx++) {
			zpt::JSONPtr _pair = zpt::split(_parts[_idx], ":");
			std::string _key(_parts[_idx].substr(0, _parts[_idx].find(":")));
			std::string _value(_parts[_idx].substr(_parts[_idx].find(":") + 1));
			zpt::trim(_key);
			zpt::trim(_value);
			_headers << _key << _value;
		}
	}

	zpt::JSONPtr _payload;
	try {
		_payload = zpt::json(_body);
	}
	catch(zpt::SyntaxErrorException& _e) {
		_payload = zpt::undefined;
	}

	zpt::ev::Performative _performative = zpt::ev::from_str(_method);
	zlog(string("<- | \033[33;40m") + _method + string("\033[0m ") + (_performative == zpt::ev::Reply ? string("\033[") + (((int) _headers["X-Status"]) > 299 ? "31" : "32") + string(";40m") + ((string) _headers["X-Status"]) + string("\033[0m ") : "") + _resource, zpt::info);
	return zpt::mkptr(JSON(
		"channel" << _channel <<
		"performative" << _performative <<
		"resource" << _resource <<
		"headers" << _headers <<
		"payload" << _payload
	));
}

zpt::JSONPtr zpt::ZMQ::send(zpt::ev::Performative _performative, std::string _resource, zpt::JSONPtr _payload) {
	return this->send(zpt::mkptr(JSON(
		"channel" << _resource <<
		"performative" << _performative <<
		"resource" << _resource <<
		"payload" << _payload
	)));
}

zpt::JSONPtr zpt::ZMQ::send(zpt::JSONPtr _envelope) {
	assertz(_envelope["channel"]->ok(), "'channel' attribute is required", 412, 0);
	assertz(_envelope["performative"]->ok() && _envelope["resource"]->ok(), "'performative' and 'resource' attributes are required", 412, 0);
	assertz(!_envelope["headers"]->ok() || _envelope["headers"]->type() == zpt::JSObject, "'headers' must be of type JSON object", 412, 0);

	if ((zpt::ev::Performative) ((int) _envelope["performative"]) != zpt::ev::Reply) {
		_envelope << "headers" << (zpt::ev::init_request() + _envelope["headers"]);
	}
	else {
		assertz(_envelope["status"]->ok(), "'status' attribute is required", 412, 0);
		_envelope << "headers" << (zpt::ev::init_reply() + _envelope["headers"]);
		_envelope["headers"] << "X-Status" << _envelope["status"];
	}		
	if (!_envelope["payload"]->ok()) {
		_envelope << "payload" << zpt::mkobj();
	}

	zlog(string("-> | \033[33;40m") + zpt::ev::to_str((zpt::ev::Performative) ((int) _envelope["performative"])) + string("\033[0m ") + (((int) _envelope["performative"]) == zpt::ev::Reply ? string("\033[") + (((int) _envelope["headers"]["X-Status"]) > 299 ? "31" : "32") + string(";40m") + ((string) _envelope["headers"]["X-Status"]) + string("\033[0m ") : "") + ((string) _envelope["resource"]), zpt::info);
	std::vector<std::string> _parts;
	_parts.push_back((string) _envelope["channel"]);
	_parts.push_back(zpt::ev::to_str((zpt::ev::Performative) ((int) _envelope["performative"])));
	_parts.push_back((string) _envelope["resource"]);
	if (_envelope["headers"]->ok()) {
		for (auto _header : _envelope["headers"]->obj()) {
			_parts.push_back(_header.first + string(": ") + ((string) _header.second));
		}
	}

	this->lock(); 
	{
		for (auto _buffer : _parts) {
			zmq::message_t _request(_buffer.size());
			memcpy ((void *) _request.data(), _buffer.data(), _buffer.size());
			this->out().send(_request, ZMQ_SNDMORE);
		}

		std::string _payload = (string) _envelope["payload"];
		zmq::message_t _request(_payload.size());
		memcpy ((void *) _request.data(), _payload.data(), _payload.size());
		this->out().send(_request);
	} 
	this->unlock();

	return zpt::undefined;
}

zpt::ZMQReq::ZMQReq(std::string _connection, zpt::JSONPtr _options, zpt::EventEmitterPtr _emitter) : zpt::ZMQ( _connection, _options, _emitter ) {
	this->__socket = new zmq::socket_t(this->context(), ZMQ_REQ);
	this->__socket->connect(_connection.data());
	zlog(string("connecting REQ socket on ") + _connection, zpt::notice);
}

zpt::ZMQReq::~ZMQReq() {
	this->__socket->close();
	delete this->__socket;
}

zmq::socket_t& zpt::ZMQReq::socket() {
	return * this->__socket;
}

zmq::socket_t& zpt::ZMQReq::in() {
	return * this->__socket;
}

zmq::socket_t& zpt::ZMQReq::out() {
	return * this->__socket;
}

short int zpt::ZMQReq::type() {
	return ZMQ_REQ;
}

zpt::JSONPtr zpt::ZMQReq::send(zpt::JSONPtr _envelope) {
	zpt::ZMQ::send(_envelope);
	return this->recv();
}

void zpt::ZMQReq::listen(zpt::ZMQPollPtr _poll) {
}

zpt::ZMQRep::ZMQRep(std::string _connection, zpt::JSONPtr _options, zpt::EventEmitterPtr _emitter) : zpt::ZMQ( _connection, _options, _emitter ) {
	this->__socket = new zmq::socket_t(this->context(), ZMQ_REP);
	this->__socket->bind(_connection.data());
	zlog(string("binding REP socket on ") + _connection, zpt::notice);
}

zpt::ZMQRep::~ZMQRep() {
	this->__socket->close();
	delete this->__socket;
}

zmq::socket_t& zpt::ZMQRep::socket() {
	return * this->__socket;
}

zmq::socket_t& zpt::ZMQRep::in() {
	return * this->__socket;
}

zmq::socket_t& zpt::ZMQRep::out() {
	return * this->__socket;
}

short int zpt::ZMQRep::type() {
	return ZMQ_REP;
}

void zpt::ZMQRep::listen(zpt::ZMQPollPtr _poll) {
	_poll->poll(this->self());
}

zpt::ZMQXPubXSub::ZMQXPubXSub(std::string _connection, zpt::JSONPtr _options, zpt::EventEmitterPtr _emitter) : zpt::ZMQ( _connection, _options, _emitter ) {
	std::string _connection1(_connection.substr(0, _connection.find(",")));
	std::string _connection2(_connection.substr(_connection.find(",") + 1));
	this->__socket_sub = new zmq::socket_t(this->context(), ZMQ_XSUB);
	this->__socket_sub->bind(_connection1.data());
	this->__socket_pub = new zmq::socket_t(this->context(), ZMQ_XPUB);
	this->__socket_pub->bind(_connection2.data());
	zlog(string("binding XPUB/XSUB socket on ") + _connection, zpt::notice);
}

zpt::ZMQXPubXSub::~ZMQXPubXSub() {
	this->__socket_sub->close();
	delete this->__socket_sub;
	this->__socket_pub->close();
	delete this->__socket_pub;
}

zmq::socket_t& zpt::ZMQXPubXSub::socket() {
	return * this->__socket_sub;
}

zmq::socket_t& zpt::ZMQXPubXSub::in() {
	return * this->__socket_sub;
}

zmq::socket_t& zpt::ZMQXPubXSub::out() {
	return * this->__socket_pub;
}

void zpt::ZMQXPubXSub::loop() {
	try {
		zlog("going to proxy PUB/SUB...", zpt::notice);
		zmq::proxy(static_cast<void *>(* this->__socket_pub), static_cast<void *>(* this->__socket_sub), nullptr);
	}
	catch(zmq::error_t& _e) {
		zlog(_e.what(), zpt::emergency);
	}
}

short int zpt::ZMQXPubXSub::type() {
	return ZMQ_XPUB;
}

void zpt::ZMQXPubXSub::listen(zpt::ZMQPollPtr _poll) {
}

zpt::ZMQPubSub::ZMQPubSub(std::string _connection, zpt::JSONPtr _options, zpt::EventEmitterPtr _emitter) : zpt::ZMQ( _connection, _options, _emitter ) {
	std::string _connection1(_connection.substr(0, _connection.find(",")));
	std::string _connection2(_connection.substr(_connection.find(",") + 1));
	this->__socket_sub = new zmq::socket_t(this->context(), ZMQ_SUB);
	this->__socket_sub->connect(_connection2.data());
	this->__socket_pub = new zmq::socket_t(this->context(), ZMQ_PUB);
	this->__socket_pub->connect(_connection1.data());
	zlog(string("connecting PUB/SUB socket on ") + _connection, zpt::notice);
}

zpt::ZMQPubSub::~ZMQPubSub() {
	this->__socket_sub->close();
	delete this->__socket_sub;
	this->__socket_pub->close();
	delete this->__socket_pub;
}

zmq::socket_t& zpt::ZMQPubSub::socket() {
	return * this->__socket_sub;
}

zmq::socket_t& zpt::ZMQPubSub::in() {
	return * this->__socket_sub;
}

zmq::socket_t& zpt::ZMQPubSub::out() {
	return * this->__socket_pub;
}

short int zpt::ZMQPubSub::type() {
	return ZMQ_PUB;
}

zpt::JSONPtr zpt::ZMQPubSub::send(zpt::JSONPtr _envelope) {
	if (((int) _envelope["performative"]) != zpt::ev::Reply) {
		uuid _uuid;
		_uuid.make(UUID_MAKE_V1);
		std::string _cid(_uuid.string());
		if (_envelope["headers"]->ok()) {
			_envelope["headers"] << "X-Cid" << _cid;
		}
		else {
			_envelope << "headers" << JSON( "X-Cid" << _cid );
		}
		this->lock();
		{
			this->in().setsockopt(ZMQ_SUBSCRIBE, _cid.data(), _cid.length());
		}
		this->unlock();
	}
	zpt::ZMQ::send(_envelope);
	return zpt::undefined;
}

zpt::JSONPtr zpt::ZMQPubSub::recv() {
	zpt::JSONPtr _envelope = zpt::ZMQ::recv();
	if (((int) _envelope["performative"]) == zpt::ev::Reply) {
		std::string _cid(_envelope["channel"]->str());
		this->lock();
		{
			this->in().setsockopt(ZMQ_UNSUBSCRIBE, _cid.data(), _cid.length());
		}
		this->unlock();
	}
	return _envelope;
}

void zpt::ZMQPubSub::listen(zpt::ZMQPollPtr _poll) {
	_poll->poll(this->self());
}

short zpt::str2type(std::string _type) {
	std::transform(_type.begin(), _type.end(), _type.begin(), ::toupper);
	if (_type == "REQ/REP"){
		return ZMQ_REQ;
	}
	if (_type == "PUB/SUB"){
		return ZMQ_PUB;
	}
	if (_type == "PUSH/PULL"){
		return ZMQ_PUSH;
	}
	return ZMQ_REQ;
}
