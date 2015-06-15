#include <iostream>
#include <nomagic.h>
#include "move_copy_test.h"
#include "move.h"

using fm::filemanager::test::Move_test;

START_TEST(should_move_a_file_to_a_file)
{
	Move_test test;
	test.test_should_move_or_copy_a_file_to_a_file();
}
END_TEST

START_TEST(should_move_a_file_to_a_filepath)
{
	Move_test test;
	test.test_should_move_a_file_to_a_filepath();
}
END_TEST

START_TEST(should_move_a_file_to_a_directory)
{
	Move_test test;
	test.test_should_move_a_file_to_a_directory();
}
END_TEST

START_TEST(should_not_move_a_file_to_a_dir_when_a_dir_exists)
{
	Move_test test;
	test.test_should_not_move_a_file_to_a_dir_when_a_dir_exists();
}
END_TEST

START_TEST(should_not_move_a_file_to_an_impossible_path)
{
	Move_test test;
	test.test_should_not_move_a_file_to_an_impossible_path();
}
END_TEST

START_TEST(should_move_a_directory_to_a_directory)
{
	Move_test test;
	test.test_should_move_a_directory_to_a_directory();
}
END_TEST

START_TEST(should_move_a_directory_to_a_directory_path)
{
	Move_test test;
	test.test_should_move_a_directory_to_a_directory_path();
}
END_TEST

START_TEST(should_not_move_a_directory_to_an_impossible_path)
{
	Move_test test;
	test.test_should_not_move_a_directory_to_an_impossible_path();
}
END_TEST

START_TEST(should_not_move_a_directory_to_its_an_ancestor)
{
	Move_test test;
	test.test_should_not_move_a_directory_to_its_an_ancestor();
}
END_TEST

START_TEST(should_not_move_similar_files)
{
	Move_test test;
	test.test_should_not_move_similar_files();
}
END_TEST

START_TEST(should_not_move_a_file_when_it_looks_like_a_dir)
{
	Move_test test;
	test.test_should_not_move_a_file_when_it_looks_like_a_dir();
}
END_TEST

START_TEST(should_not_move_a_dir_when_it_looks_like_a_file)
{
	Move_test test;
	test.test_should_not_move_a_dir_when_it_looks_like_a_file();
}
END_TEST

START_TEST(move_a_file_mapping_without_its_real_file)
{
	Move_test test;
	test.test_move_a_file_mapping_without_its_real_file();
}
END_TEST

START_TEST(move_a_dir_mapping_without_its_real_dir)
{
	Move_test test;
	test.test_move_a_dir_mapping_without_its_real_dir();
}
END_TEST

START_TEST(should_not_move_a_dir_to_a_dir_which_does_not_exist)
{
	Move_test test;
	test.test_should_not_move_a_dir_to_a_dir_which_does_not_exist();
}
END_TEST

START_TEST(move_a_dir_to_a_dirpath_where_there_is_a_dir)
{
	Move_test test;
	test.test_move_a_dir_to_a_dirpath_where_there_is_a_dir();
}
END_TEST

START_TEST(should_not_move_a_file_to_a_dest_file_which_does_not_exist)
{
	Move_test test;
	test.test_should_not_move_a_file_to_a_dest_file_which_does_not_exist();
}
END_TEST

START_TEST(move_a_file_to_a_path_where_there_is_a_file)
{
	Move_test test;
	test.test_move_a_file_to_a_path_where_there_is_a_file();
}
END_TEST

START_TEST(should_not_move_a_file_to_a_dir_which_does_not_exist)
{
	Move_test test;
	test.test_should_not_move_a_file_to_a_dir_which_does_not_exist();
}
END_TEST

START_TEST(move_a_file_to_a_dir_which_does_not_have_any_mappings)
{
	Move_test test;
	test.test_move_a_file_to_a_dir_which_does_not_have_any_mappings();
}
END_TEST

namespace fm {
namespace filemanager {
namespace test {

TCase* create_move_tcase()
{
	TCase* tcase(tcase_create("move"));
	tcase_add_test(tcase, should_move_a_file_to_a_file);
	tcase_add_test(tcase, should_move_a_file_to_a_filepath);
	tcase_add_test(tcase, should_move_a_file_to_a_directory);
	tcase_add_test(tcase,
		should_not_move_a_file_to_a_dir_when_a_dir_exists);
	tcase_add_test(tcase,
		should_not_move_a_file_to_an_impossible_path);
	tcase_add_test(tcase, should_move_a_directory_to_a_directory);
	tcase_add_test(tcase, should_move_a_directory_to_a_directory_path);
	tcase_add_test(tcase,
		should_not_move_a_directory_to_an_impossible_path);
	tcase_add_test(tcase, should_not_move_a_directory_to_its_an_ancestor);
	tcase_add_test(tcase, should_not_move_similar_files);
	tcase_add_test(tcase, should_not_move_a_file_when_it_looks_like_a_dir);
	tcase_add_test(tcase, should_not_move_a_dir_when_it_looks_like_a_file);
	tcase_add_test(tcase, move_a_file_mapping_without_its_real_file);
	tcase_add_test(tcase, move_a_dir_mapping_without_its_real_dir);
	tcase_add_test(tcase,
		should_not_move_a_dir_to_a_dir_which_does_not_exist);
	tcase_add_test(tcase, move_a_dir_to_a_dirpath_where_there_is_a_dir);
	tcase_add_test(tcase,
		should_not_move_a_file_to_a_dest_file_which_does_not_exist);
	tcase_add_test(tcase, move_a_file_to_a_path_where_there_is_a_file);
	tcase_add_test(tcase,
		should_not_move_a_file_to_a_dir_which_does_not_exist);
	tcase_add_test(tcase,
		move_a_file_to_a_dir_which_does_not_have_any_mappings);
	return tcase;
}

} // test
} // filemanager
} // fm
