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

#include <zapata/json.h>
#include <zapata/events.h>
#include <zapata/zmq.h>
#include <zapata/http.h>
#include <zapata/couchdb/convert_couchdb.h>
#include <ossp/uuid++.hh>
#include <mutex>

using namespace std;
#if !defined __APPLE__
using namespace __gnu_cxx;
#endif

namespace zpt {

namespace couchdb {

class Client : public zpt::Connector {
      public:
	Client(zpt::json _options, std::string _conf_path);
	virtual ~Client();

	virtual auto name() -> std::string;
	virtual auto options() -> zpt::json;
	virtual auto events(zpt::ev::emitter _emitter) -> void;
	virtual auto events() -> zpt::ev::emitter;

	virtual auto connect() -> void;
	virtual auto reconnect() -> void;

	virtual auto send(zpt::http::req _req) -> zpt::http::rep;
	virtual auto send_assync(zpt::json _envelope, zpt::ev::handler _callback) -> void;

	virtual auto init_request(zpt::http::req _req) -> void;
	virtual auto init_request(zpt::json _envelope) -> void;
	virtual auto create_database(std::string _db_name) -> void;
	virtual auto create_index(std::string _collection, zpt::json _fields) -> void;

	virtual auto
	insert(std::string _collection, std::string _href_prefix, zpt::json _record, zpt::json _opts = zpt::undefined)
	    -> std::string;
	virtual auto
	upsert(std::string _collection, std::string _href_prefix, zpt::json _record, zpt::json _opts = zpt::undefined)
	    -> std::string;
	virtual auto
	save(std::string _collection, std::string _href, zpt::json _record, zpt::json _opts = zpt::undefined) -> int;
	virtual auto
	set(std::string _collection, std::string _href, zpt::json _record, zpt::json _opts = zpt::undefined) -> int;
	virtual auto set(std::string _collection, zpt::json _query, zpt::json _record, zpt::json _opts = zpt::undefined)
	    -> int;
	virtual auto
	unset(std::string _collection, std::string _href, zpt::json _record, zpt::json _opts = zpt::undefined) -> int;
	virtual auto
	unset(std::string _collection, zpt::json _query, zpt::json _record, zpt::json _opts = zpt::undefined) -> int;
	virtual auto remove(std::string _collection, std::string _href, zpt::json _opts = zpt::undefined) -> int;
	virtual auto remove(std::string _collection, zpt::json _query, zpt::json _opts = zpt::undefined) -> int;
	virtual auto get(std::string _collection, std::string _href, zpt::json _opts = zpt::undefined) -> zpt::json;
	virtual auto query(std::string _collection, std::string _query, zpt::json _opts = zpt::undefined) -> zpt::json;
	virtual auto query(std::string _collection, zpt::json _query, zpt::json _opts = zpt::undefined) -> zpt::json;
	virtual auto all(std::string _collection, zpt::json _opts = zpt::undefined) -> zpt::json;

      private:
	zpt::json __options;
	zpt::ev::emitter __events;
	std::vector<zpt::socketstream_ptr> __sockets;
	std::vector<std::mutex*> __mtxs;
	std::mutex __global;
	unsigned short __pool_size;
	unsigned short __round_robin;
};

class ClientPtr : public std::shared_ptr<zpt::couchdb::Client> {
      public:
	/**
	 * @brief Creates an std::shared_ptr to an Self instance.
	 *
	 * @param _options the configuration object retrieved from the configuration JSON file
	 */
	ClientPtr(zpt::couchdb::Client* _target);
	ClientPtr(zpt::json _options, std::string _conf_path);

	/**
	 * @brief Destroys the current Self instance, freeing all allocated memory.
	 */
	virtual ~ClientPtr();
};

typedef zpt::couchdb::ClientPtr client;
}
}
