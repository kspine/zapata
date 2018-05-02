#pragma once

#include <zapata/base.h>
#include <zapata/events.h>

namespace zpt {
class RESTOntology : public zpt::Ontology {
	virtual auto
	compose_reply(zpt::ev::performative _method, std::string _resource, zpt::json _payload, zpt::json _headers)
	    -> zpt::json = 0;
	virtual auto
	compose_request(zpt::ev::performative _method, std::string _resource, zpt::json _payload, zpt::json _headers)
	    -> zpt::json = 0;

};

class RESTListener : public zpt::EventListener {
      public:
	RESTListener(std::string _regex);
	virtual ~RESTListener();

	virtual auto get(std::string _resource, zpt::json _envelope, zpt::ev::emitter _emitter) -> void;
	virtual auto put(std::string _resource, zpt::json _envelope, zpt::ev::emitter _emitter) -> void;
	virtual auto post(std::string _resource, zpt::json _envelope, zpt::ev::emitter _emitter) -> void;
	virtual auto del(std::string _resource, zpt::json _envelope, zpt::ev::emitter _emitter) -> void;
	virtual auto head(std::string _resource, zpt::json _envelope, zpt::ev::emitter _emitter) -> void;
	virtual auto options(std::string _resource, zpt::json _envelope, zpt::ev::emitter _emitter) -> void;
	virtual auto patch(std::string _resource, zpt::json _envelope, zpt::ev::emitter _emitter) -> void;
	virtual auto reply(std::string _resource, zpt::json _envelope, zpt::ev::emitter _emitter) -> void;
};

namespace rest {
typedef std::shared_ptr<zpt::RESTListener> listener;
}
}