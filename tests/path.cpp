#include <stdexcept>
#include <except.h>
#include <path.h>
#include "path.h"

using fs::Absolute_path;
using fs::Invalid_path;
using fs::Path;
using std::invalid_argument;
using std::logic_error;

START_TEST(root)
{
	Path path("/");
	fail_unless(path.stringrep() == "/", "value");
	fail_unless(path.is_absolute() == true, "absolute");
}
END_TEST

START_TEST(absolute_path)
{
	Path path("/dir1/dir2/dir3");
	fail_unless(path.stringrep() == "/dir1/dir2/dir3/", "value");
	fail_unless(path.is_absolute() == true, "absolute");
}
END_TEST

START_TEST(relative_path)
{
	Path path("dir1/dir2/dir3");
	fail_unless(path.stringrep() == "dir1/dir2/dir3/", "value");
	fail_unless(path.is_absolute() == false, "absolute");
}
END_TEST

START_TEST(relative_path_from_current_directory)
{
	Path path("./dir1/dir2");
	fail_unless(path.stringrep() == "./dir1/dir2/", "value");
	fail_unless(path.is_absolute() == false, "absolute");
}
END_TEST

START_TEST(relative_path_from_parent_directory)
{
	Path path("../dir1/dir2");
	fail_unless(path.stringrep() == "../dir1/dir2/", "value");
	fail_unless(path.is_absolute() == false, "absolute");
}
END_TEST

START_TEST(slash_at_tail)
{
	Path path1("/");
	Path path2("/dir1/dir2/");
	fail_unless(path1.stringrep() == "/", "value 1");
	fail_unless(path2.stringrep() == "/dir1/dir2/", "value 2");
}
END_TEST

START_TEST(should_throw_if_path_is_empty)
{
	try {
		Path path("");
		throw logic_error("test");
	}
	catch (const invalid_argument&) {
	}
}
END_TEST

START_TEST(when_path_is_absolute)
{
	Path path("/dir1/file1");
	Absolute_path abs_path(path.to_absolute_path(Absolute_path("/dir2")));
	fail_unless(abs_path.to_string() == "/dir1/file1/", "result");
}
END_TEST

START_TEST(when_path_contains_dot)
{
	Path path("/dir1/./file1");
	Absolute_path abs_path(path.to_absolute_path(Absolute_path("/dir2")));
	fail_unless(abs_path.to_string() == "/dir1/file1/", "result");
}
END_TEST

START_TEST(when_path_contains_two_dots)
{
	Path path("/dir1/../file1");
	Absolute_path abs_path(path.to_absolute_path(Absolute_path("/dir2")));
	fail_unless(abs_path.to_string() == "/file1/", "result");
}
END_TEST

START_TEST(when_an_invalid_path_is_generated)
{
	try {
		Path path("/dir1/../../file1");
		Absolute_path current_dir(Absolute_path("/dir2"));
		Absolute_path abs_path(path.to_absolute_path(current_dir));
		throw logic_error("test");
	}
	catch (const Invalid_path&) {
	}
}
END_TEST

START_TEST(when_path_is_relative)
{
	Path path("./dir1/file1");
	Absolute_path abs_path(path.to_absolute_path(Absolute_path("/dir2")));
	fail_unless(abs_path.to_string() == "/dir2/dir1/file1/", "result");
}
END_TEST

START_TEST(when_path_is_root)
{
	Path path("/");
	Absolute_path abs_path(path.to_absolute_path(Absolute_path("/dir2")));
	fail_unless(abs_path.to_string() == "/", "result");
}
END_TEST

namespace fs {

namespace path {
namespace test {

TCase* create_core_tcase()
{
	TCase* tcase(tcase_create("core"));
	tcase_add_test(tcase, root);
	tcase_add_test(tcase, absolute_path);
	tcase_add_test(tcase, relative_path);
	tcase_add_test(tcase, relative_path_from_current_directory);
	tcase_add_test(tcase, relative_path_from_parent_directory);
	tcase_add_test(tcase, slash_at_tail);
	tcase_add_test(tcase, should_throw_if_path_is_empty);
	return tcase;
}

TCase* create_to_absolute_path_tcase()
{
	TCase* tcase(tcase_create("to_absolute_path"));
	tcase_add_test(tcase, when_path_is_absolute);
	tcase_add_test(tcase, when_path_contains_dot);
	tcase_add_test(tcase, when_path_contains_two_dots);
	tcase_add_test(tcase, when_an_invalid_path_is_generated);
	tcase_add_test(tcase, when_path_is_relative);
	tcase_add_test(tcase, when_path_is_root);
	return tcase;
}

} // test
} // path

namespace test {

using namespace path::test;

Suite* create_path_test_suite()
{
	Suite* suite(suite_create("fs::Path"));
	suite_add_tcase(suite, create_core_tcase());
	suite_add_tcase(suite, create_to_absolute_path_tcase());
	return suite;
}

} // test
} // fs
