#include <string>
#include <utility>
#include <sys/types.h>
#include <sys/stat.h>
#include <absolute_path.h>
#include <filesystem.h>
#include <tests/fixture/filesystem.h>
#include <tests/fixture/process_dir.h>
#include <tests/fixture/except.h>
#include "copydir.h"
#include "fixture/filesystem.h"
#include <utils_assert.h>

using std::string;
using fs::Absolute_path;
using fs::mkdir;

namespace {

void prepare_dir(const Absolute_path& target, mode_t mode)
{
	mkdir(target);
	string path(target.to_filepath_string());
	chmod(path.c_str(), mode);
}

} // unnamed

using std::pair;
using fs::copydir;
using fs::create_emptyfile;
using fs::Failure;
using fs::get_mode;
using fs::Success;
using test::fixture::get_stat_result;
using test::Process_dir_fixture;
using test::Test_except;

START_TEST(should_return_error_when_it_can_not_open_dir)
{
	Process_dir_fixture f;
	Absolute_path src(f.get_path() + "/dir1");
	mkdir(src);
	chmod(src.to_filepath_string().c_str(), S_IWUSR);

	Absolute_path dest(f.get_path() + "/dir2");
	int r(copydir(src, dest));

	chmod(src.to_filepath_string().c_str(), S_IRWXU);
	fail_unless(r == Failure, "result code");
	fail_unless(get_stat_result(dest.to_filepath_string()) == ENOENT,
		"file stat");
}
END_TEST

START_TEST(copy_a_dir)
{
	Process_dir_fixture f;
	Absolute_path src(f.get_path() + "/dir1");
	prepare_dir(src, S_IRUSR);
	Absolute_path dest(f.get_path() + "/dir2");

	int r(copydir(src, dest));

	fail_unless(r == 0, "result code");

	string srcpath(src.to_filepath_string());
	fail_unless(get_stat_result(srcpath) == 0, "src stat");
	pair<int, mode_t> srcmode(get_mode(srcpath.c_str()));
	
	string destpath(dest.to_filepath_string());
	fail_unless(get_stat_result(destpath) == 0, "dest stat");
	pair<int, mode_t> destmode(get_mode(destpath.c_str()));

	fail_unless(srcmode.second == destmode.second, "mode");
}
END_TEST

START_TEST(fail_when_unable_to_stat)
{
	using fs::test::fixture::Stub_io_stat_failure;

	Process_dir_fixture f;
	Absolute_path src(f.get_path() + "/dir1");
	mkdir(src);
	Absolute_path dest(f.get_path() + "/dir2");

	Stub_io_stat_failure io;
	int r(copydir(src, dest, io));

	fail_unless(r == Failure, "result code");
}
END_TEST

START_TEST(copy_files_in_the_dir)
{
	Process_dir_fixture f;
	Absolute_path src(f.get_path() + "/dir1");
	mkdir(src);
	string srcfilepath1(src.to_string() + "file1");
	create_emptyfile(srcfilepath1);
	string srcfilepath2(src.to_string() + "file2");
	create_emptyfile(srcfilepath2);

	Absolute_path dest(f.get_path() + "/dir2");
	string destfilepath1(dest.to_string() + "file1");
	string destfilepath2(dest.to_string() + "file2");

	int r(copydir(src, dest));

	fail_unless(r == 0, "result code");
	fail_unless(get_mode(srcfilepath1.c_str()).first == Success, "stat 1");
	fail_unless(get_mode(destfilepath1.c_str()).first == Success, "stat 2");
	fail_unless(get_mode(srcfilepath2.c_str()).first == Success, "stat 3");
	fail_unless(get_mode(destfilepath2.c_str()).first == Success, "stat 4");
}
END_TEST

START_TEST(copy_dirs_in_the_dir)
{
	Process_dir_fixture f;
	Absolute_path src(f.get_path() + "/dir1");
	mkdir(src.child("dir2"));
	Absolute_path dest(f.get_path() + "/dir3");
	mkdir(dest.child("dir4"));

	int r(copydir(src, dest));

	Assert<Test_except>(r == 0);
	pair<int, mode_t> srcmode;
	pair<int, mode_t> destmode;
	srcmode = get_mode(src.to_filepath_string().c_str());
	destmode = get_mode(dest.to_filepath_string().c_str());
	Assert<Test_except>(srcmode.first == Success);
	Assert<Test_except>(destmode.first == Success);
	Assert<Test_except>(srcmode.second == destmode.second);
	srcmode = get_mode(src.child("dir2").to_filepath_string().c_str());
	destmode = get_mode(dest.child("dir4").to_filepath_string().c_str());
	Assert<Test_except>(srcmode.first == Success);
	Assert<Test_except>(destmode.first == Success);
	Assert<Test_except>(srcmode.second == destmode.second);
}
END_TEST

START_TEST(copy_recursively)
{
	Process_dir_fixture f;

	Absolute_path src(f.get_path() + "/dir1");
	mkdir(src);
	mkdir(src.child("dir11"));
	create_emptyfile(src.child("dir11").to_string() + "file1");
	create_emptyfile(src.child("dir11").child("dir111"));

	Absolute_path dest(f.get_path() + "/dir2");
	int r(copydir(src, dest));

	Assert<Test_except>(r == 0);
	pair<int, mode_t> srcmode;
	pair<int, mode_t> destmode;

	srcmode = get_mode(src.to_filepath_string().c_str());
	Assert<Test_except>(srcmode.first == Success);
	destmode = get_mode(dest.to_filepath_string().c_str());
	Assert<Test_except>(destmode.first == Success);
	Assert<Test_except>(srcmode.second == destmode.second);

	srcmode = get_mode(src.child("dir11").to_filepath_string().c_str());
	Assert<Test_except>(srcmode.first == Success);
	destmode = get_mode(dest.child("dir11").to_filepath_string().c_str());
	Assert<Test_except>(destmode.first == Success);
	Assert<Test_except>(srcmode.second == destmode.second);

	string tmp;
	tmp = src.child("dir11").child("file1").to_filepath_string();
	srcmode = get_mode(tmp.c_str());
	Assert<Test_except>(srcmode.first == Success);
	tmp = dest.child("dir11").child("file1").to_filepath_string();
	destmode = get_mode(tmp.c_str());
	Assert<Test_except>(destmode.first == Success);
	Assert<Test_except>(srcmode.second == destmode.second);

	tmp = src.child("dir11").child("dir111").to_filepath_string();
	srcmode = get_mode(tmp.c_str());
	Assert<Test_except>(srcmode.first == Success);
	tmp = dest.child("dir11").child("dir111").to_filepath_string();
	Assert<Test_except>(destmode.first == Success);
	Assert<Test_except>(srcmode.second == destmode.second);
}
END_TEST

namespace fs {
namespace test {

TCase* create_copydir_tcase()
{
	TCase* tcase(tcase_create("fs::copydir"));
	tcase_add_test(tcase, should_return_error_when_it_can_not_open_dir);
	tcase_add_test(tcase, copy_a_dir);
	tcase_add_test(tcase, fail_when_unable_to_stat);
	tcase_add_test(tcase, copy_files_in_the_dir);
	tcase_add_test(tcase, copy_dirs_in_the_dir);
	tcase_add_test(tcase, copy_recursively);
	return tcase;
}

} // test
} // fs
