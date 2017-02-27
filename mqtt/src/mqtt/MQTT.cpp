/*
Copyright (c) 2014, Muzzley
*/

#include <zapata/mqtt/MQTT.h>
#include <ossp/uuid++.hh>

// zpt::MQTTPtr::MQTTPtr() : std::shared_ptr<zpt::MQTT>(new zpt::MQTT()) {
// }

// zpt::MQTTPtr::MQTTPtr(zpt::MQTT* _target) : std::shared_ptr<zpt::MQTT>(_target) {
// }

// zpt::MQTTPtr::~MQTTPtr() {
// }

zpt::MQTT::MQTT() : __self(this) {
	/**
	 * Init mosquitto.
	 * - http://mosquitto.org/api/files/mosquitto-h.html#mosquitto_lib_init
	 * - http://mosquitto.org/api/files/mosquitto-h.html#mosquitto_new
	 */
	 mosquitto_lib_init();
	 this->__mosq = mosquitto_new(nullptr, true, this);

	/**
	 * Register the delegating callbacks.
	 * - http://mosquitto.org/api/files/mosquitto-h.html#mosquitto_connect_callback_set
	 * - http://mosquitto.org/api/files/mosquitto-h.html#mosquitto_disconnect_callback_set
	 * - http://mosquitto.org/api/files/mosquitto-h.html#mosquitto_publish_callback_set
	 * - http://mosquitto.org/api/files/mosquitto-h.html#mosquitto_message_callback_set
	 * - http://mosquitto.org/api/files/mosquitto-h.html#mosquitto_subscribe_callback_set
	 * - http://mosquitto.org/api/files/mosquitto-h.html#mosquitto_unsubscribe_callback_set
	 */
	 mosquitto_connect_callback_set(this->__mosq, MQTT::on_connect);
	 mosquitto_disconnect_callback_set(this->__mosq, MQTT::on_disconnect);
	 mosquitto_publish_callback_set(this->__mosq, MQTT::on_publish);
	 mosquitto_message_callback_set(this->__mosq, MQTT::on_message);
	 mosquitto_subscribe_callback_set(this->__mosq, MQTT::on_subscribe);
	 mosquitto_unsubscribe_callback_set(this->__mosq, MQTT::on_unsubscribe);

}

zpt::MQTT::~MQTT() {
	/**
	 * Destroy and clean up.
	 * - http://mosquitto.org/api/files/mosquitto-h.html#mosquitto_destroy
	 * - http://mosquitto.org/api/files/mosquitto-h.html#mosquitto_lib_cleanup
	 */
	 mosquitto_destroy(this->__mosq);
	 mosquitto_lib_cleanup();
}

auto zpt::MQTT::credentials(std::string _user, std::string _passwd) -> void {
	this->__user = _user;
	this->__passwd = _passwd;
	/**
	 * Sets MQTT server access credentials.
	 * - http://mosquitto.org/api/files/mosquitto-h.html#mosquitto_username_pw_set
	 */
	mosquitto_username_pw_set(this->__mosq, _user.data(), _passwd.data());
}

auto zpt::MQTT::user() -> std::string {
	return this->__user;
}

auto zpt::MQTT::passwd() -> std::string {
	return this->__passwd;
}

auto zpt::MQTT::self() const -> zpt::mqtt::broker {
	return this->__self;
}

auto zpt::MQTT::connect(std::string _host, bool _tls, int _port, int _keep_alive) -> void {
	std::lock_guard< std::mutex > _lock(this->__mtx);

	if (_tls) {
		mosquitto_tls_insecure_set(this->__mosq, false);
		mosquitto_tls_opts_set(this->__mosq, 1, nullptr, nullptr);
		mosquitto_tls_set(this->__mosq, nullptr, "/usr/lib/ssl/certs/", nullptr, nullptr, nullptr);
	}

	/**
	 * Connects to the MQTT server.
	 * - http://mosquitto.org/api/files/mosquitto-h.html#mosquitto_connect
	 */
	mosquitto_connect(this->__mosq, _host.data(), _port, _keep_alive);
}

auto zpt::MQTT::reconnect() -> void {
	std::lock_guard< std::mutex > _lock(this->__mtx);

	/**
	 * Connects to the MQTT server.
	 * - http://mosquitto.org/api/files/mosquitto-h.html#mosquitto_connect
	 */
	if (mosquitto_reconnect(this->__mosq) != MOSQ_ERR_SUCCESS) {
	}
}

auto zpt::MQTT::subscribe(std::string _topic) -> void {
	int _return;
	/**
	 * Subscribes to a given topic. See also http://mosquitto.org/man/mqtt-7.html for topic subscription patterns.
	 * - http://mosquitto.org/api/files/mosquitto-h.html#mosquitto_subscribe
	 */
	{ std::lock_guard< std::mutex > _lock(this->__mtx);
		mosquitto_subscribe(this->__mosq, & _return, _topic.data(), 0); }
}

auto zpt::MQTT::publish(std::string _topic, zpt::json _payload) -> void {
	int _return;
	/**
	 * Publishes a message to a given topic. See also http://mosquitto.org/man/mqtt-7.html for topic subscription patterns.
	 * - http://mosquitto.org/api/files/mosquitto-h.html#mosquitto_publish
	 */
	{ std::lock_guard< std::mutex > _lock(this->__mtx);
		std::string _payload_str = (std::string) _payload;
		mosquitto_publish(this->__mosq, & _return, _topic.data(), _payload_str.length(), (const uint8_t *) _payload_str.data(), 0, false); }
}

auto zpt::MQTT::on(std::string _event, zpt::mqtt::handler _callback) -> void {
	/**
	 * Add to the callback list, the callback *_callback*, attached to the event type *_event*.
	 */
	{ std::lock_guard< std::mutex > _lock(this->__mtx);
		auto _found = this->__callbacks.find(_event);
		if (_found != this->__callbacks.end()) {
			_found->second.push_back(_callback);
		}
		else {
			std::vector< zpt::mqtt::handler > _callbacks;
			_callbacks.push_back(_callback);
			this->__callbacks.insert(std::make_pair(_event, _callbacks));
		} }
}

auto zpt::MQTT::off(std::string _event) -> void {
	/**
	 * Remove from the callback list, the callback *_callback*, attached to the event type *_event*.
	 */
	{ std::lock_guard< std::mutex > _lock(this->__mtx);
		auto _found = this->__callbacks.find(_event);
		if (_found != this->__callbacks.end()) {
			this->__callbacks.erase(_found);
		} }
}

auto zpt::MQTT::trigger(std::string _event, zpt::mqtt::data _data) -> void {
	/**
	 * Searches for and executes registered callbacks under the event type *_event*.
	 */
	std::vector<zpt::mqtt::handler> _callbacks;
	{ std::lock_guard< std::mutex > _lock(this->__mtx);
		auto _found = this->__callbacks.find(_event);
		if (_found != this->__callbacks.end()) {
			_callbacks = _found->second; 
		} }
	for( auto _c : _callbacks) {
		_c(_data, this->self());
	}
}

auto zpt::MQTT::start() -> void {
	/**
	 * Checks if some data is available from MQTT server.
	 * - http://mosquitto.org/api/files/mosquitto-h.html#mosquitto_loop_forever
	 */
	mosquitto_loop_start(this->__mosq);
	//mosquitto_loop_forever(this->__mosq, -1, 1);
}

auto zpt::MQTT::on_connect(struct mosquitto * _mosq, void * _ptr, int _rc) -> void {
	zpt::MQTT* _self = (zpt::MQTT*) _ptr;
	zpt::mqtt::data _data(new MQTTData());
	_data->__rc = _rc;
	_self->trigger("connect", _data);
}

auto zpt::MQTT::on_disconnect(struct mosquitto * _mosq, void * _ptr, int _reason) -> void {
	zpt::MQTT* _self = (zpt::MQTT*) _ptr;
	zpt::mqtt::data _data(new MQTTData());
	_self->trigger("disconnect", _data);
}

auto zpt::MQTT::on_publish(struct mosquitto * _mosq, void * _ptr, int _mid) -> void {
	zpt::MQTT* _self = (zpt::MQTT*) _ptr;
	zpt::mqtt::data _data(new MQTTData());
	_data->__mid = _mid;
	_self->trigger("publish", _data);
}

auto zpt::MQTT::on_message(struct mosquitto * _mosq, void * _ptr, const struct mosquitto_message * _message) -> void {
	zpt::MQTT* _self = (zpt::MQTT*) _ptr;
	zpt::mqtt::data _data(new MQTTData());
	_data->__message = zpt::json(std::string((char*) _message->payload, _message->payloadlen));
	_data->__topic = zpt::json::string(_message->topic);
	_self->trigger("message", _data);
}

auto zpt::MQTT::on_subscribe(struct mosquitto * _mosq, void * _ptr, int _mid, int _qos_count, const int * _granted_qos) -> void {
	zpt::MQTT* _self = (zpt::MQTT*) _ptr;
	zpt::mqtt::data _data(new MQTTData());
	_data->__mid = _mid;
	_data->__qos_count = _qos_count;
	_data->__granted_qos = _granted_qos;
	_self->trigger("subscribe", _data);
}

auto zpt::MQTT::on_unsubscribe(struct mosquitto * _mosq, void * _ptr, int _mid) -> void {
	zpt::MQTT* _self = (zpt::MQTT*) _ptr;
	zpt::mqtt::data _data(new MQTTData());
	_data->__mid = _mid;
	_self->trigger("unsubscribe", _data);
}

auto zpt::MQTT::on_error(struct mosquitto * _mosq, void * _ptr) -> void {
	zpt::MQTT* _self = (zpt::MQTT*) _ptr;
	zpt::mqtt::data _data(new MQTTData());
	_self->trigger("error", _data);
}

extern "C" auto zpt_mqtt() -> int {
	return 1;
}