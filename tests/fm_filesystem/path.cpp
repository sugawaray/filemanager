#include <stdexcept>
#include <string>
#include "path.h"
#include <absolute_path.h>
#include <fm_filesystem.h>

using namespace std;
using namespace fm::fs;

START_TEST(should_return_fm_path_given_an_absolute_path)
{
	string fm_dirpath("/dir1/dir2/.fm");
	string path("/dir1/dir2/dir3/file1");
	string result(get_fm_path(fm_dirpath, path));

	fail_unless(result == "dir3/file1", "result");
}
END_TEST

START_TEST(should_throw_exception_given_invalid_fm_path)
{
	string fm_dirpath("/dir1/dir2/dir3");
	string path("/dir1/dir2/dir3/file1");
	try {
		get_fm_path(fm_dirpath, path);
		throw logic_error("should not reach here.");
	}
	catch (const invalid_argument&) {
	}
}
END_TEST

START_TEST(should_throw_given_path_is_not_in_fm_filesystem)
{
	string fm_dirpath("/dir1/dir2/.fm");
	string path("/dir3/dir4/dir2/file1");
	try {
		get_fm_path(fm_dirpath, path);
		throw logic_error("should not reach here.");
	}
	catch (const invalid_argument&) {
	}
}
END_TEST

START_TEST(should_convert_fm_path)
{
	string fm_dirpath("/dir1/dir2/.fm");
	string fm_path("dir3/dir4/file1");
	string result(
		convert_fm_path(fm_dirpath, fm_path).to_filepath_string());
	fail_unless(result == "/dir1/dir2/dir3/dir4/file1", "result");
}
END_TEST

START_TEST(convert_fm_path_should_throw_exception_given_invalid_fm_path)
{
	string fm_dirpath("/dir1/dir2");
	string fm_path("dir3/dir4/file1");
	try {
		convert_fm_path(fm_dirpath, fm_path);
		throw logic_error("should not reach here.");
	}
	catch (const invalid_argument&) {
	}
}
END_TEST

START_TEST(should_return_dbfilepath)
{
	Filesystem fs("/dir1/dir2/.fm");
	string result(fs.get_dbfilepath());
	fail_unless(result == "/dir1/dir2/.fm/map", "result");
}
END_TEST

namespace fm {
namespace fs {
namespace test {

TCase* create_tcase_for_path_manipulations()
{
	TCase* tcase(tcase_create("path manipulations"));
	tcase_add_test(tcase, should_return_fm_path_given_an_absolute_path);
	tcase_add_test(tcase, should_throw_exception_given_invalid_fm_path);
	tcase_add_test(tcase, should_throw_given_path_is_not_in_fm_filesystem);
	tcase_add_test(tcase, should_convert_fm_path);
	tcase_add_test(tcase,
		convert_fm_path_should_throw_exception_given_invalid_fm_path);
	tcase_add_test(tcase, should_return_dbfilepath);
	return tcase;
}

} // test
} // fs
} // fm
