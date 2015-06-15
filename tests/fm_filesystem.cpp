#include <sstream>
#include <unistd.h>
#include <absolute_path.h>
#include <filesystem.h>
#include <fm.h>
#include <fm_filesystem.h>
#include "fm_filesystem.h"
#include "fm_filesystem/path.h"
#include <tests/fixture/fm_filesystem.h>

using std::string;
using test::Fm_filesystem_fixture;
using namespace fm;
using namespace fm::fs;
namespace gfs = ::fs;

START_TEST(should_return_fm_dir_path_given_its_parent)
{
	Fm_filesystem_fixture f;
	Absolute_path fm_dirpath;
	int result(find_fm_dir(f.get_fm_parent(), 1, fm_dirpath));

	fail_unless(result == Found, "result value");
	fail_unless(fm_dirpath.to_filepath_string() == f.get_fm_path(),
		"result path");
}
END_TEST

START_TEST(should_return_fm_dir_path_given_its_descendant)
{
	Fm_filesystem_fixture f;
	Absolute_path fm_dirpath;
	string start_path(f.get_fm_parent() + "/dir1/dir2");
	int result(find_fm_dir(start_path, 3, fm_dirpath));

	fail_unless(result == Found, "result value");
	fail_unless(fm_dirpath.to_filepath_string() == f.get_fm_path(),
		"result path");
}
END_TEST

START_TEST(should_accept_file_path)
{
	Fm_filesystem_fixture f;
	Absolute_path fm_dirpath;
	string parent(f.get_fm_parent());
	string filepath(parent + "/dummy_file");
	gfs::create_emptyfile(filepath);
	int result(find_fm_dir(filepath, 2, fm_dirpath));

	fail_unless(result == Found, "result value");
	fail_unless(fm_dirpath.to_filepath_string() == f.get_fm_path(),
		"result path");
}
END_TEST

START_TEST(should_return_not_found_when_the_dir_not_found)
{
	Fm_filesystem_fixture f;
	Absolute_path fm_dirpath;
	int result(find_fm_dir(f.get_branch_path_without_fm(), 1, fm_dirpath));

	fail_unless(result == Not_found, "result value");
}
END_TEST

START_TEST(should_not_find_past_the_limit)
{
	Fm_filesystem_fixture f;
	Absolute_path fm_dirpath;
	string start_path(f.get_fm_parent() + "/dir1/dir2");
	int result(find_fm_dir(start_path, 1, fm_dirpath));

	fail_unless(result == Not_found, "result value");
}
END_TEST

START_TEST(should_accept_zero_based_limit)
{
	Fm_filesystem_fixture f;
	Absolute_path fm_dirpath;
	string start_path(f.get_fm_parent() + "/dir1");
	int result(find_fm_dir(start_path, 1, fm_dirpath));

	fail_unless(result == Found, "result value");
}
END_TEST

namespace fm {
namespace fs {
namespace test {

Suite* create_fm_filesystem_test_suite()
{
	Suite* suite = suite_create("fm_filesystem");
	TCase* tcase = tcase_create("find_fm_dir");
	tcase_add_test(tcase, should_return_fm_dir_path_given_its_parent);
	tcase_add_test(tcase, should_return_fm_dir_path_given_its_descendant);
	tcase_add_test(tcase, should_accept_file_path);
	tcase_add_test(tcase, should_return_not_found_when_the_dir_not_found);
	tcase_add_test(tcase, should_not_find_past_the_limit);
	tcase_add_test(tcase, should_accept_zero_based_limit);
	suite_add_tcase(suite, tcase);
	suite_add_tcase(suite, test::create_tcase_for_path_manipulations());
	return suite;
}

} // test
} // fs
} // fm
