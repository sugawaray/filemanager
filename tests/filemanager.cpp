#include "filemanager/copy.h"
#include "filemanager/init.h"
#include "filemanager/move.h"
#include "filemanager/refresh.h"
#include "filemanager/remove.h"
#include "filemanager/rmdir.h"
#include "filemanager/move/copy_dir.h"
#include "filemanager/move/overwrite.h"
#include "filemanager/move/to_fmroot.h"
#include "filemanager/move/to_paths_not_managed.h"
#include "filemanager/move/src_equals_dest.h"

namespace fm {
namespace test {

using namespace filemanager::test;

Suite* create_filemanager_test_suite()
{
	Suite* suite(suite_create("fm::Filemanager"));
	suite_add_tcase(suite, create_copy_tcase());
	suite_add_tcase(suite, create_move_tcase());
	suite_add_tcase(suite, create_refresh_core_tcase());
	suite_add_tcase(suite, create_refresh_erase_tcase());
	suite_add_tcase(suite, create_remove_tcase());
	suite_add_tcase(suite, create_rmdir_tcase());
	return suite;
}

void run_filemanager_tests()
{
	run_copy_dir_tests();
	run_to_paths_not_managed_tests();
	run_overwrite_tests();
	run_init_tests();
	run_to_fmroot_tests();
	run_src_equals_dest_tests();
}

} // test
} // fm
