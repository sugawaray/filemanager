#include <iostream>
#include "filesystem.h"
#include "filesystem/copy.h"
#include "filesystem/copy_abs_path.h"
#include "filesystem/copydir.h"
#include "filesystem/create_emptyfile.h"
#include "filesystem/get_abs_path.h"
#include "filesystem/get_mode.h"
#include "filesystem/get_umask.h"
#include "filesystem/is_dir.h"
#include "filesystem/mkdir.h"
#include "filesystem/rmdirtree.h"

using fs::test::create_copy_tcase;
using fs::test::create_copy_abs_path_tcase;
using fs::test::create_copy_error_tcase;
using fs::test::create_copydir_tcase;
using fs::test::create_get_mode_tcase;
using fs::test::create_get_mode_path_tcase;
using fs::test::create_get_umask_tcase;
using fs::test::create_rmdirtree_tcase;

Suite* create_filesystem_test_suite()
{
	Suite* suite(suite_create("filesystem"));

	suite_add_tcase(suite, create_copy_tcase());
	suite_add_tcase(suite, create_copy_abs_path_tcase());
	suite_add_tcase(suite, create_copy_error_tcase());
	suite_add_tcase(suite, create_copydir_tcase());
	suite_add_tcase(suite, create_get_mode_tcase());
	suite_add_tcase(suite, create_get_mode_path_tcase());
	suite_add_tcase(suite, create_get_umask_tcase());
	suite_add_tcase(suite, create_rmdirtree_tcase());
	TCase* tcase_create_emptyfile(
		test::fs::create_tcase_for_create_emptyfile());
	suite_add_tcase(suite, tcase_create_emptyfile);

	TCase* tcase_get_abs_path(test::fs::create_tcase_for_get_abs_path());
	suite_add_tcase(suite, tcase_get_abs_path);

	TCase* tcase_is_dir(fs::test::create_tcase_for_is_dir());
	suite_add_tcase(suite, tcase_is_dir);

	TCase* tcase_mkdir(test::fs::create_tcase_for_mkdir());
	suite_add_tcase(suite, tcase_mkdir);

	return suite;
}

namespace fs {
namespace test {

void run_filesystem_tests()
{
	run_copy_tests();
}

} // test
} // fs

