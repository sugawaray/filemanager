#include <cerrno>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unistd.h>
#include <absolute_path.h>
#include <tests/filesystem.h>
#include "get_abs_path.h"
#include <except.h>
#include <filesystem.h>
#include <helper.h>

using namespace fs;
using namespace std;

namespace {

const size_t path_buffer_size(1024);

char working_dir_path[path_buffer_size];

void setUp()
{
	fill_n(working_dir_path, sizeof array_size(working_dir_path), 0);
	getcwd(working_dir_path, sizeof array_size(working_dir_path));
	ostringstream oss;
	oss << "./test_dir/" << getpid() << "/dir1/dir2/dir3";
	::fs::mkdir(get_abs_path(oss.str()));
}

void tearDown()
{
}

char* getcwd_longstub(char* buffer, size_t n)
{
	if (n >= 1024) {
		errno = 0;
		char* s = working_dir_path;
		s += char_traits<char>::length(working_dir_path) + 1;
		copy(working_dir_path, s, buffer);
		return buffer;
	} else {
		errno = ERANGE;
		return 0;
	}
}

} // unnamed

START_TEST(should_return_abs_path_from_abs_path)
{
	Absolute_path result(get_abs_path(working_dir_path));
	fail_unless(result.to_filepath_string() == string(working_dir_path),
		"should be the same");
}
END_TEST

START_TEST(should_return_abs_path_from_relative_path)
{
	Absolute_path result(get_abs_path("test_filesystem.cpp"));
	string expected(string(working_dir_path) +
		"/test_filesystem.cpp");
	fail_unless(result.to_filepath_string() == expected,
		"should be the same");
}
END_TEST

START_TEST(should_return_for_relative_path_with_dot)
{
	Absolute_path result(get_abs_path("./test_filesystem.cpp"));
	string expected(string(working_dir_path) +
		"/test_filesystem.cpp");
	fail_unless(result.to_filepath_string() == expected,
		"should be the same");
}
END_TEST

START_TEST(should_return_for_relative_path_with_2dot)
{
	Absolute_path result(get_abs_path("../test_filesystem.cpp"));
	string working_dir(working_dir_path);
	working_dir = working_dir.substr(0, working_dir.find_last_of('/'));
	string expected(working_dir + "/test_filesystem.cpp");
	fail_unless(result.to_filepath_string() == expected,
		"should be the same");
}
END_TEST

START_TEST(should_be_ok_with_zero_length_path)
{
	try {
		get_abs_path("");
		throw logic_error("should not reach here.");
	}
	catch (const invalid_argument&) {
	}
}
END_TEST

START_TEST(should_be_ok_with_long_path)
{
	Absolute_path result(get_abs_path("test_filesystem.cpp",
		getcwd_longstub));
	string working_dir(working_dir_path);
	string expected(working_dir + "/test_filesystem.cpp");
	fail_unless(result.to_filepath_string() == expected,
		"should be the same");
}
END_TEST

namespace test {
namespace fs {

TCase* create_tcase_for_get_abs_path()
{
	TCase* tcase(tcase_create("get_abs_path"));
	tcase_add_checked_fixture(tcase, setUp, tearDown);
	tcase_add_test(tcase, should_return_abs_path_from_abs_path);
	tcase_add_test(tcase, should_return_abs_path_from_relative_path);
	tcase_add_test(tcase, should_return_for_relative_path_with_dot);
	tcase_add_test(tcase, should_return_for_relative_path_with_2dot);
	tcase_add_test(tcase, should_be_ok_with_zero_length_path);
	tcase_add_test(tcase, should_be_ok_with_long_path);
	return tcase;
}

} // fs
} // test
