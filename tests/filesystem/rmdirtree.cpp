#include <cerrno>
#include <stdexcept>
#include <filesystem.h>
#include <tests/fixture/filesystem.h>
#include <tests/fixture/process_dir.h>
#include "rmdirtree.h"
#include <utils_assert.h>

namespace {

template<class T>
inline void A(T expr)
{
	utils::Assert<std::logic_error>(expr);
}

} // unnamed

using fs::Absolute_path;
using fs::create_emptyfile;
using fs::Failure;
using fs::mkdir;
using fs::rmdirtree;
using fs::Success;
using test::fixture::get_stat_result;
using test::Process_dir_fixture;
typedef Process_dir_fixture F;

START_TEST(remove_an_empty_dir)
{
	F f;
	Absolute_path target(f.get_path() + "/dir1");
	mkdir(target);

	int r(rmdirtree(target));
	
	A(r == Success);
	A(get_stat_result(target.to_filepath_string()) == ENOENT);
}
END_TEST

START_TEST(remove_a_dir_with_children)
{
	F f;
	Absolute_path target(f.get_path() + "/dir1");
	mkdir(target);
	create_emptyfile(target.child("file1"));
	create_emptyfile(target.child("file2"));
	mkdir(target.child("dir2"));
	create_emptyfile(target.child("dir2").child("file1"));

	int r(rmdirtree(target));

	A(r == Success);
	A(get_stat_result(target.to_filepath_string()) == ENOENT);
}
END_TEST

START_TEST(return_failure_when_stat_failed)
{
	F f;
	Absolute_path target(f.get_path() + "/dir1/dir2");
	mkdir(target);
	chmod(target.parent().to_filepath_string().c_str(), S_IWUSR);

	int r(rmdirtree(target));

	chmod(target.parent().to_filepath_string().c_str(), S_IRWXU);
	A(r == Failure);
}
END_TEST

START_TEST(return_Not_exist_when_target_does_not_exist)
{
	F f;
	Absolute_path target(f.get_path() + "/dir1");

	int r(rmdirtree(target));

	A(r == fs::Not_exist);
}
END_TEST

START_TEST(return_failure_when_rmdir_failed)
{
	F f;
	Absolute_path target(f.get_path() + "/dir1");
	mkdir(target);
	mkdir(target.child("dir2"));
	chmod(target.child("dir2").to_filepath_string().c_str(), S_IWUSR);

	int r(rmdirtree(target));

	A(r == Failure);
}
END_TEST

namespace fs {
namespace test {

TCase* create_rmdirtree_tcase()
{
	TCase* tcase(tcase_create("rmdirtree"));
	tcase_add_test(tcase, remove_an_empty_dir);
	tcase_add_test(tcase, remove_a_dir_with_children);
	tcase_add_test(tcase, return_failure_when_stat_failed);
	tcase_add_test(tcase, return_Not_exist_when_target_does_not_exist);
	tcase_add_test(tcase, return_failure_when_rmdir_failed);
	return tcase;
}

} // test
} // fs
