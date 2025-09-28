#include "jamerror.hpp"

#include <iostream>
#include <system_error>

#include <string.h>

jamerror::jamerror() noexcept : status{status_ok} {}

jamerror::jamerror(const std::string &message, int status) {
	append(message, status);
}

jamerror::operator bool() const noexcept {
	return status != status_ok;
}

jamerror &
jamerror::append(const std::string &message, int status) {
	this->status = status;

	if (message.size() == 0) {
		return *this;
	}

	message_log += message;

	// Add newline if it is not already present.
	if (message_log[message_log.size() - 1] != '\n') {
		message_log += '\n';
	}

	return *this;
}

jamerror &
jamerror::append_errno() noexcept {
	std::error_code error_code(errno, std::system_category());
	
	return append(error_code.message() + ".");
}

int
jamerror::print() const {
	std::cerr << message_log << std::flush;
	return status;
}

const std::string&
jamerror::message_log_for_humans_only() const noexcept {
	return message_log;
}
