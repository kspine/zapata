/*
Copyright (c) 2017, Muzzley
*/

#include <zapata/smtp/SMTP.h>
#include <ossp/uuid++.hh>

#ifndef CRLF
#define CRLF "\r\n"
#endif

zpt::SMTPPtr::SMTPPtr() : std::shared_ptr<zpt::SMTP>(new zpt::SMTP()) {}

zpt::SMTPPtr::~SMTPPtr() {}

zpt::SMTP::SMTP() : __port(0) {}

zpt::SMTP::~SMTP() {}

auto zpt::SMTP::credentials(std::string _user, std::string _passwd) -> void {
	this->__user.assign(_user);
	this->__passwd.assign(_passwd);
}

auto zpt::SMTP::user() -> std::string { return this->__user; }

auto zpt::SMTP::passwd() -> std::string { return this->__passwd; }

auto zpt::SMTP::connect(std::string _connection) -> void {
	std::lock_guard<std::mutex> _lock(this->__mtx);
	this->__connection.assign(_connection);
	this->__uri = zpt::uri::parse(_connection);

	this->__host.assign(this->__uri["domain"]->ok() ? std::string(this->__uri["domain"]) : "localhost");
	this->__port = this->__uri["port"]->ok() ? (unsigned int)this->__uri["port"] : 25;
	this->__type = zpt::split(std::string(this->__uri["scheme"]), "+");

	if (this->__uri["user"]->ok()) {
		this->__user.assign(std::string(this->__uri["user"]));
	}
	if (this->__uri["password"]->ok()) {
		this->__passwd.assign(std::string(this->__uri["password"]));
	}
}

auto zpt::SMTP::compose(zpt::json _e_mail) -> std::string {
	std::ostringstream _oss;
	_oss.str("");

	bool first = true;
	std::string _recipients;
	zpt::json _parsed_to = zpt::json::array();
	for (auto _r : _e_mail["To"]->arr()) {
		zpt::json _parsed = zpt::email::parse(std::string(_r));
		_parsed_to << _parsed;
		if (!first) {
			_recipients += std::string(", ");
		}
		first = false;
		_recipients +=
		    (_parsed["name"]->ok()
			 ? zpt::quoted_printable::r_encode(std::string(_parsed["name"]), "utf-8") + std::string(" <")
			 : std::string("")) +
		    std::string(_parsed["address"]) + (_parsed["name"]->ok() ? std::string(">") : std::string(""));
	}

	zpt::json _from = zpt::email::parse(std::string(_e_mail["From"]));
	_oss << "Date: " << zpt::tostr(time(nullptr), "%a, %d %b %Y %X %Z") << CRLF << flush;
	_oss << "From: "
	     << (_from["name"]->ok()
		     ? zpt::quoted_printable::r_encode(std::string(_from["name"]), "utf-8") + std::string(" <")
		     : std::string(""))
	     << std::string(_from["address"]) << (_from["name"]->ok() ? std::string(">") : std::string("")) << CRLF
	     << flush;
	if (_e_mail["Sender"]->ok()) {
		zpt::json _sender = zpt::email::parse(std::string(_e_mail["Sender"]));
		_oss << "Sender: " << std::string(_sender["address"]) << CRLF << flush;
	} else {
		_oss << "Sender: " << std::string(_from["address"]) << CRLF << flush;
	}
	_oss << "Subject: " << zpt::quoted_printable::r_encode(std::string(_e_mail["Subject"]), "utf-8") << CRLF
	     << flush;
	if (_e_mail["Reply-To"]->ok()) {
		zpt::json _reply_to = zpt::email::parse(std::string(_e_mail["Reply-To"]));
		_oss << "Reply-To: "
		     << (_reply_to["name"]->ok()
			     ? zpt::quoted_printable::r_encode(std::string(_reply_to["name"]), "utf-8") +
				   std::string(" <")
			     : std::string(""))
		     << std::string(_reply_to["address"])
		     << (_reply_to["name"]->ok() ? std::string(">") : std::string("")) << CRLF << flush;
	}
	_oss << "To: " << _recipients << CRLF << flush;

	std::string _boundary = std::string("============") + zpt::generate::r_uuid() + std::string("==");
	_oss << "Content-Type: multipart/alternative; boundary=\"" << _boundary << "\"" << CRLF << flush;

	for (auto _part : _e_mail["Body"]->obj()) {
		_oss << CRLF << flush;
		_oss << "--" << _boundary << CRLF << flush;
		_oss << "Content-Type: " << _part.first << "; charset=\"utf-8\"" << CRLF << flush;
		_oss << "MIME-Version: 1.0" << CRLF << flush;
		_oss << "Content-Transfer-Encoding: 8bit" << CRLF << CRLF << flush;
		_oss << std::string(_part.second) << CRLF << flush;
	}
	_oss << "--" << _boundary << "--" << CRLF << flush;
	_oss << "." << CRLF << flush;

	_e_mail << "To" << _parsed_to;
	_e_mail << "From" << _from;

	return _oss.str();
}

auto zpt::SMTP::open() -> mailsmtp* {
	mailsmtp* _smtp = nullptr;

	assertz((_smtp = mailsmtp_new(0, NULL)) != NULL, "could not create mailsmtp instance", 500, 1500);

	try {
		if (this->__type[1] == zpt::json::string("ssl")) {
			assertz(mailsmtp_ssl_connect(_smtp, this->__host.data(), this->__port) == MAILSMTP_NO_ERROR,
				std::string("could not connect to ") + this->__connection,
				503,
				1501);
		} else if (this->__type[1] == zpt::json::string("tls")) {
			assertz(mailsmtp_socket_connect(_smtp, this->__host.data(), this->__port) == MAILSMTP_NO_ERROR,
				std::string("could not connect to ") + this->__connection,
				503,
				1501);
		} else {
			assertz(mailsmtp_socket_connect(_smtp, this->__host.data(), this->__port) == MAILSMTP_NO_ERROR,
				std::string("could not connect to ") + this->__connection,
				503,
				1501);
		}

		if (this->__type[0] == zpt::json::string("smtp")) {
			assertz(mailsmtp_helo(_smtp) == MAILSMTP_NO_ERROR,
				std::string("could not introduce my self to ") + this->__connection +
				    std::string(" using SMTP"),
				400,
				1502);
		} else if (this->__type[0] == zpt::json::string("esmtp")) {
			assertz(mailesmtp_ehlo(_smtp) == MAILSMTP_NO_ERROR,
				std::string("could not introduce my self to ") + this->__connection +
				    std::string(" using ESMTP"),
				400,
				1502);
		}

		if (this->__type[0] == zpt::json::string("esmtp") && this->__type[1] == zpt::json::string("tls")) {
			assertz(mailsmtp_socket_starttls(_smtp) == MAILSMTP_NO_ERROR,
				std::string("could not STARTTLS with ") + this->__connection,
				403,
				1503);
			assertz(mailesmtp_ehlo(_smtp) == MAILSMTP_NO_ERROR,
				std::string("could not introduce my self to ") + this->__connection +
				    std::string(" using ESMTP"),
				400,
				1502);
		}

		if (this->__user.length() != 0 && this->__type[0] == zpt::json::string("esmtp")) {
			assertz(mailsmtp_auth(_smtp, this->__user.data(), this->__passwd.data()) == MAILSMTP_NO_ERROR,
				std::string("could not authenticate propertly with ") + this->__connection,
				401,
				1504);
		}
	} catch (zpt::assertion& _e) {
		this->close(_smtp);
		throw;
	}
	return _smtp;
}

auto zpt::SMTP::close(mailsmtp* _smtp) -> void {
	if (_smtp != nullptr) {
		mailsmtp_free(_smtp);
	}
}

auto zpt::SMTP::send(zpt::json _e_mail) -> void {
	mailsmtp* _smtp = this->open();

	try {
		if (this->__type[0] == zpt::json::string("esmtp")) {
			assertz(mailesmtp_mail(_smtp,
					       std::string(_e_mail["From"]["address"]).data(),
					       1,
					       "etPanSMTPZapataWrapper") == MAILSMTP_NO_ERROR,
				std::string("could not send email through ") + this->__connection,
				502,
				1505);
		} else {
			assertz(mailsmtp_mail(_smtp, std::string(_e_mail["From"]["address"]).data()) ==
				    MAILSMTP_NO_ERROR,
				std::string("could not send email through ") + this->__connection,
				502,
				1505);
		}

		assertz(_e_mail["To"]->is_array(), "'To' attribute must be a JSON array", 412, 1506)
		    std::string _mimed_mail = this->compose(_e_mail);
		zdbg(_mimed_mail);

		for (auto _r : _e_mail["To"]->arr()) {
			if (this->__type[0] == zpt::json::string("esmtp")) {
				assertz(mailesmtp_rcpt(_smtp,
						       std::string(_r["address"]).data(),
						       MAILSMTP_DSN_NOTIFY_FAILURE | MAILSMTP_DSN_NOTIFY_DELAY,
						       nullptr) == MAILSMTP_NO_ERROR,
					std::string("could not add recipient ") + std::string(_r["address"]),
					400,
					1507);
			} else {
				assertz(mailsmtp_rcpt(_smtp, std::string(_r["address"]).data()) == MAILSMTP_NO_ERROR,
					std::string("could not add recipient ") + std::string(_r["address"]),
					400,
					1507);
			}
		}

		assertz(mailsmtp_data(_smtp) == MAILSMTP_NO_ERROR,
			std::string("could not send DATA command to ") + this->__connection,
			502,
			1508);
		assertz(mailsmtp_data_message(_smtp, _mimed_mail.data(), _mimed_mail.length()) == MAILSMTP_NO_ERROR,
			std::string("could not send email through ") + this->__connection,
			502,
			1509);

		this->close(_smtp);
	} catch (zpt::assertion& _e) {
		this->close(_smtp);
		throw;
	}
}

extern "C" auto zpt_smtp() -> int { return 1; }
