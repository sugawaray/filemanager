#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>
#include <absolute_path.h>
#include "absolute_path.h"
#include <utils_assert.h>
#include <tests/fixture/except.h>

using std::back_inserter;
using std::invalid_argument;
using std::logic_error;
using std::runtime_error;
using std::string;
using std::vector;
using fs::Absolute_path;
using fs::Invalid_path;
using utils::Assert;
using test::Test_except;

START_TEST(root)
{
	Absolute_path path("/");

	vector<string> components;
	path.get_components(back_inserter(components));

	fail_unless(components.size() == 1, "count");
	fail_unless(components.at(0) == "", "value");
}
END_TEST

START_TEST(invalid_path_empty)
{
	try {
		Absolute_path path("");
		throw logic_error("test");
	}
	catch (const invalid_argument&) {
	}
}
END_TEST

START_TEST(invalid_path_relative)
{
	try {
		Absolute_path path("dir1/file1");
		throw logic_error("test");
	}
	catch (const invalid_argument&) {
	}
}
END_TEST

START_TEST(invalid_path_containing_dot)
{
	try {
		Absolute_path path("/dir1/./file1");
		throw logic_error("test");
	}
	catch (const invalid_argument&) {
	}
}
END_TEST

START_TEST(invalid_path_containing_dot_at_the_first_char)
{
	try {
		Absolute_path path("./dir1/file1");
		throw logic_error("test");
	}
	catch (const invalid_argument&) {
	}
}
END_TEST

START_TEST(invalid_path_containing_two_dot)
{
	try {
		Absolute_path path("/dir1/../file1");
		throw logic_error("test");
	}
	catch (const invalid_argument&) {
	}
}
END_TEST

START_TEST(invalid_path_containing_empty_after_the_first_char)
{
	try {
		Absolute_path path("/dir1//file1");
		throw logic_error("test");
	}
	catch (const invalid_argument&) {
	}
}
END_TEST

START_TEST(to_string_with_only_one_component)
{
	Absolute_path path("/");
	fail_unless(path.to_string() == "/", "result");
}
END_TEST

START_TEST(to_string_with_multiple_components)
{
	Absolute_path path("/dir1/file1");
	fail_unless(path.to_string() == "/dir1/file1/", "result");
}
END_TEST

START_TEST(to_filepath_string_when_it_is_the_root)
{
	Absolute_path path("/");
	fail_unless(path.to_filepath_string() == "/", "result");
}
END_TEST

START_TEST(to_filepath_string_when_it_is_not_the_root)
{
	Absolute_path path("/dir1");
	fail_unless(path.to_filepath_string() == "/dir1", "result");
}
END_TEST

START_TEST(basename)
{
	Absolute_path path("/dir1/dir2");
	fail_unless(path.basename() == "dir2", "result");
}
END_TEST

START_TEST(should_throw_when_path_is_the_root)
{
	Absolute_path path("/");
	try {
		path.parent();
		throw logic_error("test");
	}
	catch (const Invalid_path&) {
	}
}
END_TEST

START_TEST(should_return_parent_path)
{
	Absolute_path path("/dir1/dir2");
	Absolute_path parent(path.parent());
	fail_unless(parent.to_string() == "/dir1/", "result");
}
END_TEST

START_TEST(return_child)
{
	Absolute_path path("/dir1/dir2");
	Absolute_path child(path.child("dir3"));
	Assert<logic_error>(child.to_string() == "/dir1/dir2/dir3/");
}
END_TEST

START_TEST(throw_when_it_is_invalid)
{
	Absolute_path path;
	try {
		path.child("dir1");
		Assert<logic_error>(0);
	}
	catch (const runtime_error&) {
	}
}
END_TEST

START_TEST(throw_given_an_invalid_component)
{
	Absolute_path path("/dir1");
	try {
		path.child("/dir2");
		Assert<Test_except>(0);
	}
	catch (const invalid_argument&) {
	}
	try {
		path.child("");
		Assert<Test_except>(0);
	}
	catch (const invalid_argument&) {
	}
	try {
		path.child(".");
		Assert<Test_except>(0);
	}
	catch (const invalid_argument&) {
	}
	try {
		path.child("..");
		Assert<Test_except>(0);
	}
	catch (const invalid_argument&) {
	}
}
END_TEST

START_TEST(do_not_throw_proper_components)
{
	Absolute_path p("/dir1");
	p.child(".a");
	p.child("..a");
	p.child("...");
	p.child("a.");
	p.child("a..");
	Absolute_path p2("/dir1/dir2/.a");
}
END_TEST

namespace fs {

namespace absolute_path {
namespace test {

TCase* create_core_tcase()
{
	TCase* tcase(tcase_create("core"));
	tcase_add_test(tcase, root);
	tcase_add_test(tcase, invalid_path_empty);
	tcase_add_test(tcase, invalid_path_relative);
	tcase_add_test(tcase, invalid_path_containing_dot);
	tcase_add_test(tcase, invalid_path_containing_dot_at_the_first_char);
	tcase_add_test(tcase, invalid_path_containing_two_dot);
	tcase_add_test(tcase,
		invalid_path_containing_empty_after_the_first_char);
	tcase_add_test(tcase, to_string_with_only_one_component);
	tcase_add_test(tcase, to_string_with_multiple_components);
	tcase_add_test(tcase, to_filepath_string_when_it_is_the_root);
	tcase_add_test(tcase, to_filepath_string_when_it_is_not_the_root);
	tcase_add_test(tcase, basename);
	return tcase;
}

TCase* create_parent_tcase()
{
	TCase* tcase(tcase_create("parent"));
	tcase_add_test(tcase, should_throw_when_path_is_the_root);
	tcase_add_test(tcase, should_return_parent_path);
	return tcase;
}

TCase* create_child_tcase()
{
	TCase* tcase(tcase_create("child"));
	tcase_add_test(tcase, return_child);
	tcase_add_test(tcase, throw_when_it_is_invalid);
	tcase_add_test(tcase, throw_given_an_invalid_component);
	tcase_add_test(tcase, do_not_throw_proper_components);
	return tcase;
}

} // test
} // absolute_path

namespace test {

Suite* create_absolute_path_test_suite()
{
	using namespace absolute_path::test;

	Suite* suite(suite_create("fs::Absolute_path"));
	suite_add_tcase(suite, create_core_tcase());
	suite_add_tcase(suite, create_parent_tcase());
	suite_add_tcase(suite, create_child_tcase());
	return suite;
}

} // test
} // fs
