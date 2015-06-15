#include <utility>
#include <absolute_path.h>
#include <filesystem.h>
#include <tests/fixture/process_dir.h>
#include "is_dir.h"

using namespace std;
using test::Process_dir_fixture;
using fs::Absolute_path;
using fs::create_emptyfile;
using fs::is_dir;
using fs::Not_exist;
using fs::Success;

START_TEST(should_return_Not_exist_when_it_does_not_exist)
{
	Process_dir_fixture f;
	pair<int, bool> result(is_dir(f.get_path() + "/file1"));
	fail_unless(result.first == Not_exist, "result code");
}
END_TEST

START_TEST(should_return_false_when_it_is_not_a_directory)
{
	Process_dir_fixture f;
	string filepath(f.get_path() + "/file1");
	create_emptyfile(filepath);

	pair<int, bool> result(is_dir(filepath));

	fail_unless(result.first == Success, "result code");
	fail_unless(result.second == false, "result");
}
END_TEST

START_TEST(should_return_true_when_it_is_a_directory)
{
	Process_dir_fixture f;
	string dirpath(f.get_path() + "/dir1");
	fs::mkdir(dirpath);

	pair<int, bool> result(is_dir(dirpath));

	fail_unless(result.first == Success, "result code");
	fail_unless(result.second == true, "result");
}
END_TEST

START_TEST(return_failure_when_unable_to_stat_it)
{
	Process_dir_fixture f;
	Absolute_path target(f.get_path() + "/dir1/dir2");
	fs::mkdir(target);
	chmod(target.parent().to_filepath_string().c_str(), S_IWUSR);

	pair<int, bool> result(is_dir(target));

	chmod(target.parent().to_filepath_string().c_str(), S_IRWXU);
	fail_unless(result.first == fs::Failure, "result code");
}
END_TEST

namespace fs {
namespace test {

TCase* create_tcase_for_is_dir()
{
	TCase* tcase(tcase_create("is_dir"));
	tcase_add_test(tcase, should_return_Not_exist_when_it_does_not_exist);
	tcase_add_test(tcase, should_return_false_when_it_is_not_a_directory);
	tcase_add_test(tcase, should_return_true_when_it_is_a_directory);
	tcase_add_test(tcase, return_failure_when_unable_to_stat_it);
	return tcase;
}

} // test
} // fs
