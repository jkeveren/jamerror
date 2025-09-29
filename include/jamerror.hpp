#pragma once

#include <string>

// Pass jamerror by reference to all functions that can fail in order to remove the nead to try-catch every call.
// Check whether an error occured by implicitly casting to bool.
// The goal is for a deep error caused by a high level mistake to be easy to debug just by seeing the error message. All relavent variable values should be in the error messages.
// However deep errors should not be programmatically accessible; only the highest level error should be. This allows the implementation of the unit to be changed without risk of creating breaking changes.
// This error handling strategy has overhead because of all the string building so don't use this for things that you expect to error a lot.
// If this is used in a performace critical application that is expected to error a lot then this is not ideal.
// If it's performance critical and simple you can re-implement without this; or it's complex and string building is probably not that much of an overhead compared to the complex task.
class jamerror {
	private:
	// message_log is a string of error messages separated by newlines.
	// Each message is appended in chronological order. This results in the lowest level messages being at the top of the stack and are the first ones printed.
	// Each message should be verbose and contain lots of information that could be useful in debugging the issue like labeled variable values.
	// message_log is private because deep errors should not be machine readable.
	// Machine readable errors become part of a units external API.
	// This would cause low level changes to the unit to effect the external API which makes them breaking changes.
	// If a user of a unit needs low level error access then the user should implement it and/or the unit should provide a way of injecting low level resources.
	std::string message_log;

	public:
	// status is a machine readable version of the latest error message appended to message_log.
	// This does not stack like message_log for the same reasons message_log is private, as described earlier.
	// It only stores the highest level status for the error.
	int status;

	// Generic statuses are defined here.
	// When defining your own statuses, make sure to use integers >= 2 to avoid collisions with these generic statuses!
	static constexpr int status_ok = 0; // status_ok means no error occurred.
	static constexpr int status_internal_error = 1; // status_internal_error indicates that an error occurred but we're not exposing it as part of the public API of the unit. It's still available from the print menthod though. This is the default status that means an error has occurred.
	// status_ok and status_internal_error being 0 and 1 makes them also work as sensible exit codes so you can just exit with the status which defaults to status_internal_error which is 1.

	// Default constructor.
	// Constructs a jamerror that does not yet indicate that an error occurred.
	jamerror() noexcept;

	// Constructs a jamerror with message and status. This state indicates that an error has occurred.
	jamerror(const std::string &message, int status = status_internal_error);

	// Returns true if an error has occurred.
	// This may seem unconventional but it allows error checking to be extremely simple "if (err) {/* handle the error */}".
	explicit operator bool() const noexcept;

	// Appends the highest level error message to the message stack.
	// Resets status by default to other so lower level statuses to not get implicitly passed up and exposed as public API.
	// Returns reference to *this for chaining.
	// Calling this indicates that an error has occured.
	jamerror&
	append(const std::string &message, int status = status_internal_error);
	
	// Appends a message derrived from errno.
	jamerror&
	append_errno() noexcept;
	
	// Prints the message_log to stderr and returns status int (handy for exiting program e.g. "int main() {jamerror err; return err.print();}").
	// message_log is only accessible through printing so it cannot be accessed for the reasons stated above in the section about message_log etc.
	int
	print() const;

	// Getter for message_log.
	// Ideally just use print() instead of this. This should only be used if you really want the error to go somewhere other than stderr.
	// This is intended to be used exclusively to pass information to a humam, hance the annoying long name.
	// Information in the message_log is intended to create very useful error messages for humans to read and should be considered not part of the units public API.
	// Don't parse this string and branch based on it. That's what status is for.
	const std::string &
	message_log_for_humans_only() const noexcept;
	
	// Clears the error so it can be re-used.
	// Useful when an error is non-fatal so you don't need to allocate a new jamerror.
	jamerror&
	clear() noexcept;
};
