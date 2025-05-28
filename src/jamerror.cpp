#include "jamerror.hpp"

#include <iostream>

#include <string.h>

jamerror::jamerror() : reason{reason_none} {}

jamerror::jamerror(const std::string &message, int reason) {
	append(message, reason);
}

jamerror::operator bool() const {
	return reason != reason_none;
}

jamerror &
jamerror::append(const std::string &message, int reason) {
	this->reason = reason;
	if (message.size() == 0) {
		return *this;
	}
	
	message_stack += message;
	
	// Add newline if it is not already present.
	if (message_stack[message_stack.size() - 1] != '\n') {
		message_stack += '\n';
	}
			
	return *this;
}

int
jamerror::print() const {
	std::cerr << message_stack << std::flush;
	// fputs(message_stack.c_str(), stderr);
	// std::cerr << message_stack << std::flush; // Message stack already ends witha newline, no need to add one.
	return reason;
}

jamerror
jamerror::strerror() {
	// This looks weird because the GNU version of strerror_r is weird and this is compatible.
	char *message = static_cast<char *>(alloca(100));
	message = strerror_r(errno, message, sizeof(message));
	return append(std::string(message) + ".");
}
