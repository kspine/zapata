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

#include <zapata/http.h>
#include <zapata/json.h>
#include <zapata/events.h>
#include <zapata/zmq.h>
#include <zapata/mqtt.h>
#include <zapata/upnp.h>
#include <string>
#include <map>
#include <memory>

using namespace std;
#if !defined __APPLE__
using namespace __gnu_cxx;
#endif

#define no_get nullptr
#define no_put nullptr
#define no_post nullptr
#define no_delete nullptr
#define no_head nullptr
#define no_trace nullptr
#define no_options nullptr
#define no_patch nullptr
#define no_connect nullptr

namespace zpt {

enum RESTfulType {
	RESTfulResource = 0,
	RESTfulDocument = 1,
	RESTfulCollection = 2,
	RESTfulStore = 3,
	RESTfulController = 4
};

class RESTEmitter;
class RESTServer;
class RESTClient;
class RESTServerPtr;
class RESTClientPtr;

namespace rest {
typedef zpt::RESTServerPtr server;
typedef zpt::RESTClientPtr client;
typedef zpt::RESTEmitter emitter;
typedef std::map<std::string, std::vector<std::pair<std::regex, zpt::ev::handlers>>> HandlerStack;

extern zpt::rest::emitter* __emitter;

typedef zpt::ev::handler step;
typedef zpt::ev::initializer end;
}

class RESTServerPtr : public std::shared_ptr<zpt::RESTServer> {
      public:
	RESTServerPtr(std::string _name, zpt::json _options);
	RESTServerPtr(zpt::RESTServer* _ptr);
	virtual ~RESTServerPtr();

	static zpt::rest::server setup(zpt::json _options, std::string _name);
	static int launch(int argc, char* argv[]);
};

class RESTClientPtr : public std::shared_ptr<zpt::RESTClient> {
      public:
	RESTClientPtr(zpt::json _options);
	RESTClientPtr(zpt::RESTClient* _ptr);
	virtual ~RESTClientPtr();

	static zpt::rest::client launch(int argc, char* argv[]);
};

class RESTServer {
      public:
	RESTServer(std::string _name, zpt::json _options);
	virtual ~RESTServer();

	virtual void start();

	virtual auto name() -> std::string;
	virtual auto options() -> zpt::json;
	virtual auto poll() -> zpt::poll;
	virtual auto events() -> zpt::ev::emitter;
	virtual auto credentials() -> zpt::json;
	virtual auto credentials(zpt::json _credentials) -> void;
	virtual auto unbind() -> void;

	virtual auto hook(zpt::ev::initializer _callback) -> void;

	// virtual auto route_mqtt(zpt::mqtt::data _data) -> bool;
	virtual auto route_mqtt(zpt::mqtt::data _data) -> zpt::json;
	virtual auto subscribe(std::string _regex, zpt::json _opts) -> void;
	virtual auto publish(std::string _topic, zpt::json _payload) -> void;
	virtual auto broadcast(zpt::json _envelope) -> void;
	virtual auto notify_peers() -> void;

	virtual auto suicidal() -> bool;

      private:
	std::string __name;
	zpt::ev::emitter __emitter;
	zpt::poll __poll;
	zpt::json __options;
	std::vector<zpt::socket_ref> __pub_sub;
	std::vector<zpt::socket_ref> __router_dealer;
	zpt::rest::server __self;
	zpt::mqtt::broker __mqtt;
	zpt::upnp::broker __upnp;
	zpt::ev::OnStartStack __initializers;
	size_t __max_threads;
	size_t __alloc_threads;
	size_t __n_threads;
	std::mutex __thread_mtx;
	bool __suicidal;

	auto get_subscription_topics(std::string _pattern) -> zpt::json;
};

class RESTEmitter : public zpt::EventEmitter {
      public:
	RESTEmitter(zpt::json _options);
	virtual ~RESTEmitter();

	virtual auto version() -> std::string;
	virtual auto credentials() -> zpt::json;
	virtual auto credentials(zpt::json _credentials) -> void;

	virtual auto on(zpt::ev::performative _method,
			std::string _regex,
			zpt::ev::Handler _handler,
			zpt::json _opts = zpt::undefined) -> void;
	virtual auto on(std::string _regex,
			std::map<zpt::ev::performative, zpt::ev::Handler> _handlers,
			zpt::json _opts = zpt::undefined) -> void;
	virtual auto on(zpt::ev::listener _listener, zpt::json _opts = zpt::undefined) -> void;

	virtual auto trigger(zpt::ev::performative _method,
			     std::string _resource,
			     zpt::json _payload,
			     zpt::json _opts = zpt::undefined,
			     zpt::ev::handler _callback = nullptr) -> void;
	virtual auto route(zpt::ev::performative _method,
			   std::string _resource,
			   zpt::json _payload,
			   zpt::json _opts = zpt::undefined,
			   zpt::ev::handler _callback = nullptr) -> void;
	virtual auto
	route(zpt::ev::performative _method, std::string _resource, zpt::json _payload, zpt::ev::handler _callback)
	    -> void;
	virtual auto sync_route(zpt::ev::performative _method, std::string _url, zpt::json _envelope, zpt::json _opts)
	    -> zpt::json;
	virtual auto reply(zpt::json _request, zpt::json _reply = zpt::undefined) -> void;

	virtual auto hook(zpt::ev::initializer _callback) -> void;
	virtual auto shutdown() -> void;
	virtual auto make_available() -> void;

	virtual auto pending(zpt::json _envelope, zpt::ev::handler _callback) -> void;
	virtual auto has_pending(zpt::json _envelope) -> bool;

	virtual auto poll(zpt::poll _poll) -> void;
	virtual auto poll() -> zpt::poll;
	virtual auto server(zpt::rest::server _server) -> void;
	virtual auto server() -> zpt::rest::server;

	static auto instance() -> zpt::ev::emitter;

      private:
	zpt::ev::Handler __default_options;
	zpt::ev::ReplyHandlerStack __pending;
	zpt::poll __poll;
	zpt::rest::server __server;
	zpt::json __credentials;

	auto resolve(zpt::ev::performative _method,
		     std::string _url,
		     zpt::json _envelope,
		     zpt::json _opts,
		     zpt::ev::handler _callback = nullptr) -> void;
	auto sync_resolve(zpt::ev::performative _method, std::string _url, zpt::json _envelope, zpt::json _opts)
	    -> zpt::json;
};

namespace rest {

auto url_pattern(zpt::json _to_join) -> std::string;
auto collect(zpt::json _args, zpt::json _to_collect_from, zpt::rest::step _step, zpt::rest::end _end) -> void;
auto _collect(zpt::json _args,
	      zpt::json _to_collect_from,
	      size_t _idx,
	      zpt::json _previous,
	      zpt::rest::step _step,
	      zpt::rest::end _end,
	      zpt::ev::emitter _emitter) -> void;
auto iterate(zpt::json _to_iterate_over, zpt::rest::step _step, zpt::rest::end _end) -> void;
auto _iterate(zpt::json _to_iterate_over,
	      size_t _idx,
	      zpt::rest::step _step,
	      zpt::rest::end _end,
	      zpt::ev::emitter _emitter) -> void;
auto _collect_variables(zpt::json _kb, zpt::json _args) -> zpt::json;
}
}
