#include <iostream>
#include <string>
#include <utility>
#include <sys/types.h>
#include <sys/stat.h>
#include <absolute_path.h>
#include <filesystem.h>
#include <tests/fixture/process_dir.h>
#include "get_mode.h"

using std::cerr;
using std::endl;
using std::pair;
using std::string;
using fs::Absolute_path;
using fs::create_emptyfile;
using fs::Failure;
using fs::get_mode;
using fs::Success;
using test::Process_dir_fixture;

namespace {

const mode_t all_mode(S_IRWXU | S_IRWXG | S_IRWXO);

} // unnamed

START_TEST(should_return_failure_on_error)
{
	pair<int, mode_t> r(get_mode(-1));
	fail_unless(r.first == Failure, "result code");
}
END_TEST

START_TEST(should_return_mode)
{
	Process_dir_fixture f;
	string path(f.get_path() + "/file");
	create_emptyfile(path);
	chmod(path.c_str(), S_IRUSR);
	int fd(open(path.c_str(), O_RDONLY));

	pair<int, mode_t> r(get_mode(fd));

	fail_unless(r.first == Success, "result code");
	fail_unless((r.second & all_mode) == S_IRUSR, "code");
}
END_TEST

START_TEST(test_get_mode)
{
	Process_dir_fixture f;
	Absolute_path abspath(f.get_path() + "/file");
	create_emptyfile(abspath);
	string path(abspath.to_filepath_string());
	chmod(path.c_str(), S_IRUSR);

	pair<int, mode_t> r(get_mode(path.c_str()));

	fail_unless(r.first == Success, "result code");
	fail_unless((r.second & all_mode) == S_IRUSR, "mode");
}
END_TEST

START_TEST(test_return_error)
{
	Process_dir_fixture f;
	Absolute_path abspath(f.get_path() + "/file");
	string path(abspath.to_filepath_string());

	pair<int, mode_t> r(get_mode(path.c_str()));

	fail_unless(r.first == Failure, "result code");
}
END_TEST

namespace fs {
namespace test {

TCase* create_get_mode_tcase()
{
	TCase* tcase(tcase_create("fs::get_mode"));
	tcase_add_test(tcase, should_return_failure_on_error);
	tcase_add_test(tcase, should_return_mode);
	return tcase;
}

TCase* create_get_mode_path_tcase()
{
	TCase* tcase(tcase_create("fs::get_mode path"));
	tcase_add_test(tcase, test_get_mode);
	tcase_add_test(tcase, test_return_error);
	return tcase;
}

} // test
} // fs
