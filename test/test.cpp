#include "jamerror.hpp"

#include <iostream>
#include <sstream>

#include <errno.h>

class tester {
	long test_count = 0;
	long passing_test_count = 0;
	
	public:
	
	template <typename t> void
	test(std::string description, bool pass, t wanted, t got) {
		++test_count;
		
		std::stringstream result_string;
		if (pass) {
			++passing_test_count;
			result_string << "Pass";
		} else {
			result_string << "FAIL (expected: \"" << wanted << "\", actual: \"" << got << "\").";
		}
		
		std::cout << description << ": " << result_string.str() << std::endl;
	}
	
	int
	summary() {
		std::cout << passing_test_count << "/" << test_count << " tests passed." << std::endl;
		
		if (passing_test_count == test_count) {
			std::cout << "All tests passed!" << std::endl;
			return 0;
		} else {
			std::cout << "FAIL!" << std::endl;
			return 1;
		}
	}
};

int main() {
	tester t;
	
	{
		std::string desc = "Not errored by default, ";
		jamerror err;
		
		t.test<int>(desc + "by state", err.status == jamerror::status_ok, jamerror::status_ok, err.status);
		
		bool error_occurred = static_cast<bool>(err);
		bool pass = !error_occurred;
		t.test<bool>(desc + "by bool cast", pass, false, error_occurred);
	}
	
	{
		std::string desc = "Errored after append(), ";
		jamerror err;
		err.append("Example error.");
		
		t.test<int>(desc + "by status", err.status == jamerror::status_internal_error, jamerror::status_internal_error, err.status);
		
		bool err_bool = static_cast<bool>(err);
		t.test<bool>(desc + "by bool cast", err_bool, true, err_bool);
	}
	
	{
		std::string desc = "append_errno() appends message from errno";
		
		jamerror err;
		
		errno = EACCES;
		err.append_errno();
		
		std::string expected = "Permission denied.\n";
		std::string actual = err.message_log_for_humans_only();
		t.test(desc, actual == expected, expected, actual);
	}
	
	{
		std::string desc = "message_log has error message, ";
		jamerror err;
		std::string test_message = "test_message";
		err.append(test_message);
		std::string expected = test_message + "\n";
		
		{
			std::ostringstream output;
			std::streambuf* original_buffer = std::cerr.rdbuf(output.rdbuf());
			err.print();
			std::cerr.rdbuf(original_buffer);
			std::string actual = output.str();
			t.test<std::string>(desc + "from print()", actual == expected, expected, actual);
		}
		
		{
			std::string actual = err.message_log_for_humans_only();
			t.test<std::string>(desc + "from getter", actual == expected, expected, actual);
		}
	}
	
	return t.summary();
}
