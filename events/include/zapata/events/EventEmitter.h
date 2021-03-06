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

#include <zapata/base.h>
#include <zapata/json.h>
#include <regex>
#include <string>
#include <map>
#include <memory>
#include <ossp/uuid++.hh>
#include <regex>
#include <poll.h>
#include <zmq.hpp>
#include <zmq.h>

using namespace std;
#if !defined __APPLE__
using namespace __gnu_cxx;
#endif

#define REST_ACCESS_CONTROL_HEADERS                                                                                    \
	"X-Cid,X-Status,X-No-Redirection,X-Redirect-To,Authorization,Accept,Accept-Language,Cache-Control,Connection," \
	"Content-Length,Content-Type,Cookie,Date,Expires,Location,Origin,Server,X-Requested-With,X-Replied-With,"      \
	"Pragma,Cache-Control,E-Tag"

namespace zmq {
typedef std::shared_ptr<zmq::socket_t> socket_ptr;
}

namespace zpt {
extern const char* status_names[];

class EventEmitter;
class EventListener;
class EventGatekeeper;
class EventDirectory;
class EventDirectoryGraph;
class Bridge;
class BridgePtr;
class Poll;
class PollPtr;
class socket_ref;
class Channel;

typedef std::weak_ptr<zpt::EventEmitter> EventEmitterWPtr;
typedef std::shared_ptr<zpt::EventEmitter> EventEmitterPtr;
typedef std::shared_ptr<zpt::EventListener> EventListenerPtr;
typedef std::shared_ptr<zpt::EventGatekeeper> EventGatekeeperPtr;
typedef std::shared_ptr<zpt::EventDirectory> EventDirectoryPtr;
typedef std::shared_ptr<zpt::EventDirectoryGraph> EventDirectoryGraphPtr;
typedef std::shared_ptr<zpt::Channel> socket;

typedef BridgePtr bridge;
typedef PollPtr poll;

namespace ev {
extern std::string* __default_authorization;

typedef zpt::EventEmitterPtr emitter;
typedef zpt::EventListenerPtr listener;
typedef zpt::EventGatekeeperPtr gatekeeper;
typedef zpt::EventDirectoryPtr directory;
typedef zpt::EventDirectoryGraphPtr graph;

typedef std::function<void(zpt::ev::performative, std::string, zpt::json, zpt::ev::emitter)> Handler;
typedef std::function<void(zpt::ev::performative, std::string, zpt::json, zpt::ev::emitter)> handler;
typedef Handler Callback;
typedef handler callback;
typedef std::vector<zpt::ev::handler> handlers;
typedef std::map<std::string, zpt::ev::handler> ReplyHandlerStack;

typedef std::tuple<zpt::json, zpt::ev::handlers, std::regex> node;

typedef std::function<void(zpt::ev::emitter)> initializer;
typedef std::vector<zpt::ev::initializer> OnStartStack;

auto split(std::string _url, zpt::json _orphans) -> zpt::json;
auto join(zpt::json _info, std::size_t _orphans) -> std::string;

auto set_default_authorization(std::string _default_authorization) -> void;
auto get_default_authorization() -> std::string;

auto init_request(std::string _cid = "") -> zpt::json;
auto init_reply(std::string _cid = "", zpt::json _request = zpt::undefined) -> zpt::json;

auto pretty(zpt::json _envelope) -> std::string;

auto not_found(std::string _resource, zpt::json _headers = zpt::undefined) -> zpt::json;
auto bad_request(std::string _resource, zpt::json _headers = zpt::undefined) -> zpt::json;
auto unsupported_media_type(std::string _resource, zpt::json _headers = zpt::undefined) -> zpt::json;
auto accepted(std::string _resource, zpt::json _headers = zpt::undefined) -> zpt::json;
auto no_content(std::string _resource, zpt::json _headers = zpt::undefined) -> zpt::json;
auto temporary_redirect(std::string _resource, std::string _target_resource, zpt::json _headers = zpt::undefined)
    -> zpt::json;
auto see_other(std::string _resource, std::string _target_resource, zpt::json _headers = zpt::undefined) -> zpt::json;
auto options(std::string _resource, std::string _origin, zpt::json _headers = zpt::undefined) -> zpt::json;
auto internal_server_error(std::string _resource, std::exception& _e, zpt::json _headers = zpt::undefined) -> zpt::json;
auto assertion_error(std::string _resource, zpt::assertion& _e, zpt::json _headers = zpt::undefined) -> zpt::json;

namespace uri {
auto get_simplified_topics(std::string _pattern) -> zpt::json;
}

extern emitter __emitter;
}

auto is_sql(std::string _name) -> bool;

class PollPtr : public std::shared_ptr<zpt::Poll> {
      public:
	PollPtr(zpt::Poll* _ptr);
	virtual ~PollPtr();
};

class socket_ref : public std::string {
      public:
	socket_ref();
	socket_ref(std::string _rhs, zpt::poll _poll);
	socket_ref(const zpt::socket_ref& _rhs);

	auto poll(zpt::poll _poll) -> void;
	auto poll() -> zpt::poll;
	auto operator-> () -> zpt::Channel*;
	auto operator*() -> zpt::Channel*;

      private:
	zpt::poll __poll;
};

class Poll {
      public:
	Poll();
	virtual ~Poll();

	virtual auto options() -> zpt::json = 0;
	virtual auto emitter() -> zpt::ev::emitter = 0;

	virtual auto get(std::string _uuid) -> zpt::socket_ref = 0;
	virtual auto relay(std::string _key) -> zpt::Channel* = 0;
	virtual auto add(short _type, std::string _connection, bool _new_connection = false) -> zpt::socket_ref = 0;
	virtual auto add(zpt::Channel* _underlying) -> zpt::socket_ref = 0;
	virtual auto remove(zpt::socket_ref _socket) -> void = 0;
	virtual auto vanished(std::string _connection, zpt::ev::initializer _callback = nullptr) -> void = 0;
	virtual auto vanished(zpt::Channel* _underlying, zpt::ev::initializer _callback = nullptr) -> void = 0;
	virtual auto pretty() -> std::string = 0;

	virtual auto poll(zpt::socket_ref _socket) -> void = 0;
	virtual auto clean_up(zpt::socket_ref _socket, bool _force = false) -> void = 0;

	virtual auto loop() -> void = 0;
};

class Channel {
      public:
	Channel(std::string _connection, zpt::json _options);
	virtual ~Channel();

	virtual auto id() -> std::string;
	virtual auto options() -> zpt::json;
	virtual auto connection() -> std::string;
	virtual auto connection(std::string _connection) -> void;
	virtual auto uri(size_t _idx = 0) -> zpt::json;
	virtual auto uri(std::string _connection) -> void;
	virtual auto detach() -> void;
	virtual auto close() -> void;
	virtual auto available() -> bool;

	virtual auto recv() -> zpt::json;
	virtual auto send(zpt::ev::performative _performative, std::string _resource, zpt::json _payload) -> zpt::json;
	virtual auto send(zpt::json _envelope) -> zpt::json;
	virtual auto loop_iteration() -> void;

	virtual auto socket() -> zmq::socket_ptr = 0;
	virtual auto in() -> zmq::socket_ptr = 0;
	virtual auto out() -> zmq::socket_ptr = 0;
	virtual auto fd() -> int = 0;
	virtual auto in_mtx() -> std::mutex& = 0;
	virtual auto out_mtx() -> std::mutex& = 0;
	virtual auto type() -> short int = 0;
	virtual auto protocol() -> std::string = 0;

      private:
	zpt::json __options;
	std::string __connection;
	std::string __id;
	zpt::json __uri;

      protected:
	std::mutex __mtx;
	zpt::poll __poll;
};

class Connector {
      public:
	Connector();
	virtual ~Connector();

	virtual auto name() -> std::string = 0;
	virtual auto options() -> zpt::json = 0;
	virtual auto events(zpt::ev::emitter _emitter) -> void = 0;
	virtual auto events() -> zpt::ev::emitter = 0;

	virtual auto connect() -> void;
	virtual auto reconnect() -> void;

	virtual auto
	insert(std::string _collection, std::string _id_prefix, zpt::json _record, zpt::json _opts = zpt::undefined)
	    -> std::string;
	virtual auto
	upsert(std::string _collection, std::string _id_prefix, zpt::json _record, zpt::json _opts = zpt::undefined)
	    -> std::string;
	virtual auto save(std::string _collection, std::string _id, zpt::json _record, zpt::json _opts = zpt::undefined)
	    -> int;
	virtual auto set(std::string _collection, std::string _id, zpt::json _record, zpt::json _opts = zpt::undefined)
	    -> int;
	virtual auto set(std::string _collection, zpt::json _query, zpt::json _record, zpt::json _opts = zpt::undefined)
	    -> int;
	virtual auto
	unset(std::string _collection, std::string _id, zpt::json _record, zpt::json _opts = zpt::undefined) -> int;
	virtual auto
	unset(std::string _collection, zpt::json _query, zpt::json _record, zpt::json _opts = zpt::undefined) -> int;
	virtual auto remove(std::string _collection, std::string _id, zpt::json _opts = zpt::undefined) -> int;
	virtual auto remove(std::string _collection, zpt::json _query, zpt::json _opts = zpt::undefined) -> int;
	virtual auto get(std::string _collection, std::string _id, zpt::json _opts = zpt::undefined) -> zpt::json;
	virtual auto query(std::string _collection, std::string _query, zpt::json _opts = zpt::undefined) -> zpt::json;
	virtual auto query(std::string _collection, zpt::json _query, zpt::json _opts = zpt::undefined) -> zpt::json;
	virtual auto all(std::string _collection, zpt::json _opts = zpt::undefined) -> zpt::json;

      protected:
	virtual auto connection() -> zpt::json;
	virtual auto connection(zpt::json _conn_conf) -> void;

      private:
	zpt::json __connection;
};
typedef std::shared_ptr<zpt::Connector> ConnectorPtr;
typedef ConnectorPtr connector;

class BridgePtr : public std::shared_ptr<zpt::Bridge> {
      public:
	BridgePtr(zpt::Bridge* _target);
	BridgePtr();

	template <typename B> static inline auto instance() -> zpt::bridge { return B::instance(); };

	template <typename B> static inline auto is_booted() -> bool { return B::is_booted(); };

	template <typename B> static inline auto boot(zpt::json _options, zpt::ev::emitter _emitter) -> void {
		B::boot(_options);
		zpt::bridge::instance<B>()->events(_emitter);
		zpt::bridge::instance<B>()->initialize();
	};
};

class Bridge {
      public:
	Bridge(zpt::json _options);
	virtual ~Bridge();

	virtual auto options() -> zpt::json;
	virtual auto name() -> std::string = 0;
	virtual auto events(zpt::ev::emitter _emitter) -> void = 0;
	virtual auto events() -> zpt::ev::emitter = 0;
	virtual auto initialize() -> void = 0;
	virtual auto load_module(std::string _module) -> void = 0;
	virtual auto self() const -> zpt::bridge = 0;
	virtual auto unbind() -> void = 0;

	template <typename T> inline auto deflbd(zpt::json _conf, std::function<T(int, T[])> _callback) -> void {
		return T::bridge()->deflbd(_conf, _callback);
	};

	template <typename T> inline auto eval(std::string _expr) -> decltype(T::bridge()->eval(_expr)) {
		return T::bridge()->eval(_expr);
	};

	template <typename T> inline auto from(T _o) -> zpt::json { return _o->tojson(); };

	template <typename T> inline auto to(zpt::json _o) -> T { return T::fromjson(_o); };

	template <typename D> inline auto data() -> D { return D::data(this->self()); };

      private:
	zpt::json __options;
};

class EventEmitter {
      public:
	EventEmitter();
	EventEmitter(zpt::json _options);
	virtual ~EventEmitter();

	virtual auto uuid() -> std::string;
	virtual auto options() -> zpt::json;
	virtual auto self() const -> zpt::ev::emitter;
	virtual auto unbind() -> void;
	virtual auto version() -> std::string = 0;
	virtual auto gatekeeper() -> zpt::ev::gatekeeper;
	virtual auto gatekeeper(zpt::ev::gatekeeper _gatekeeper) -> void;
	virtual auto directory() -> zpt::ev::directory;

	virtual auto authorize(std::string _topic, zpt::json _envelope, zpt::json _roles_needed = zpt::undefined)
	    -> zpt::json;
	virtual auto lookup(std::string _topic, zpt::ev::performative _performative) -> zpt::ev::node;

	virtual auto on(zpt::ev::performative _method,
			std::string _regex,
			zpt::ev::Handler _handler,
			zpt::json _opts = zpt::undefined) -> void = 0;
	virtual auto on(std::string _regex,
			std::map<zpt::ev::performative, zpt::ev::Handler> _handlers,
			zpt::json _opts = zpt::undefined) -> void = 0;
	virtual auto on(zpt::ev::listener _listener, zpt::json _opts = zpt::undefined) -> void = 0;

	virtual auto credentials() -> zpt::json = 0;
	virtual auto credentials(zpt::json _credentials) -> void = 0;

	virtual auto trigger(zpt::ev::performative _method,
			     std::string _resource,
			     zpt::json _payload,
			     zpt::json _opts = zpt::undefined,
			     zpt::ev::handler _callback = nullptr) -> void = 0;
	virtual auto route(zpt::ev::performative _method,
			   std::string _resource,
			   zpt::json _payload,
			   zpt::json _opts = zpt::undefined,
			   zpt::ev::handler _callback = nullptr) -> void = 0;
	virtual auto
	route(zpt::ev::performative _method, std::string _resource, zpt::json _payload, zpt::ev::handler _callback)
	    -> void = 0;
	virtual auto sync_route(zpt::ev::performative _method,
				std::string _url,
				zpt::json _envelope,
				zpt::json _opts = zpt::undefined) -> zpt::json = 0;
	virtual auto reply(zpt::json _request, zpt::json _reply) -> void = 0;
	virtual auto has_pending(zpt::json _envelope) -> bool = 0;

	virtual auto hook(zpt::ev::initializer _callback) -> void = 0;

	virtual auto connector(std::string _name, zpt::connector _connector) -> void final;
	virtual auto connector(std::map<std::string, zpt::connector> _connectors) -> void final;
	virtual auto connector(std::string _name) -> zpt::connector final;

      private:
	zpt::json __options;
	zpt::ev::emitter __self;
	zpt::ev::gatekeeper __keeper;
	zpt::ev::directory __directory;
	std::map<std::string, zpt::connector> __connector;
	std::string __uuid;
};

class EventGatekeeper {
      public:
	EventGatekeeper(zpt::json _options);
	virtual ~EventGatekeeper();

	virtual auto options() -> zpt::json;
	virtual auto unbind() -> void;
	virtual auto self() const -> zpt::ev::gatekeeper;
	virtual auto events() -> zpt::ev::emitter;
	virtual auto events(zpt::ev::emitter _emitter) -> void;
	virtual auto authorize(std::string _topic, zpt::json _envelope, zpt::json _roles_needed = zpt::undefined)
	    -> zpt::json;
	virtual auto get_credentials(zpt::json _client_id,
				     zpt::json _client_secret,
				     zpt::json _address,
				     zpt::json _grant_type,
				     zpt::json _scope) -> zpt::json;

      private:
	zpt::json __options;
	zpt::ev::gatekeeper __self;
	zpt::ev::emitter __emitter;
};

class EventDirectoryGraph {
      public:
	EventDirectoryGraph(std::string _resolver, zpt::ev::node _service);
	virtual ~EventDirectoryGraph();

	virtual auto insert(std::string _topic, zpt::ev::node _node) -> void;
	virtual auto find(std::string _topic, zpt::ev::performative _performative) -> zpt::ev::node;
	virtual auto remove(std::string _uuid) -> void;
	virtual auto list(std::string _uuid = "") -> zpt::json;
	virtual auto pretty(std::string _tabs = "", bool _last = false) -> std::string;

      private:
	std::string __resolver;
	zpt::ev::node __service;
	std::map<std::string, zpt::ev::graph> __children;

	auto merge(zpt::ev::node _service) -> void;
	auto insert(zpt::json _topic, zpt::ev::node _service) -> void;
	auto find(std::string _topic, zpt::json _splited, zpt::ev::performative _performative) -> zpt::ev::node;
};

class EventDirectory {
      public:
	EventDirectory(zpt::json _options);
	virtual ~EventDirectory();

	virtual auto options() -> zpt::json;
	virtual auto unbind() -> void final;
	virtual auto self() const -> zpt::ev::directory;
	virtual auto events() -> zpt::ev::emitter final;
	virtual auto events(zpt::ev::emitter _emitter) -> void final;
	virtual auto lookup(std::string _topic, zpt::ev::performative _performative) -> zpt::ev::node;
	virtual auto notify(std::string _topic, zpt::ev::node _connection) -> void;
	virtual auto make_available(std::string _uuid) -> void;
	virtual auto shutdown(std::string _uuid) -> void;
	virtual auto vanished(std::string _uuid) -> void;
	virtual auto list(std::string _uuid = "") -> zpt::json;
	virtual auto pretty() -> std::string;

      private:
	zpt::json __options;
	zpt::ev::directory __self;
	zpt::ev::emitter __emitter;
	zpt::ev::graph __services;
	std::mutex __mtx;
};

class EventListener {
      public:
	EventListener(std::string _regex);
	virtual ~EventListener();

	virtual std::string regex() final;

	virtual auto get(std::string _resource, zpt::json _envelope, zpt::ev::emitter _emitter) -> void;
	virtual auto put(std::string _resource, zpt::json _envelope, zpt::ev::emitter _emitter) -> void;
	virtual auto post(std::string _resource, zpt::json _envelope, zpt::ev::emitter _emitter) -> void;
	virtual auto del(std::string _resource, zpt::json _envelope, zpt::ev::emitter _emitter) -> void;
	virtual auto head(std::string _resource, zpt::json _envelope, zpt::ev::emitter _emitter) -> void;
	virtual auto options(std::string _resource, zpt::json _envelope, zpt::ev::emitter _emitter) -> void;
	virtual auto patch(std::string _resource, zpt::json _envelope, zpt::ev::emitter _emitter) -> void;
	virtual auto reply(std::string _resource, zpt::json _envelope, zpt::ev::emitter _emitter) -> void;

      private:
	std::string __regex;
};

template <typename T> inline auto emitter() -> decltype(T::instance()) { return T::instance(); }

inline auto emitter() -> zpt::ev::emitter { return zpt::ev::__emitter; }
}
