#include <iterator>
#include <string>
#include <vector>
#include <absolute_path.h>
#include <filesystem.h>
#include <filemanager.h>
#include <fm_filesystem.h>
#include <fm_map_impl.h>
#include <tests/fixture/fm_filesystem.h>
#include "refresh.h"

using namespace std;
using test::Fm_filesystem_fixture;
using namespace fm;
using fm::fs::Filesystem;
namespace gfs = ::fs;

START_TEST(should_not_erase_files_when_they_exist)
{
	Fm_filesystem_fixture f;
	Filemanager manager(f.get_fm_path());
	gfs::mkdir(f.get_fm_parent() + "/dir1");
	gfs::create_emptyfile(f.get_fm_parent() + "/dir1/file1");
	vector<string> categories;
	categories.push_back("catA");
	auto& map(manager.get_map());
	map.set("dir1/file1", categories.begin(), categories.end());
	manager.refresh();

	vector<string> values;
	map.get(categories, back_inserter(values));
	fail_unless(values.size() == 1, "count");
}
END_TEST

START_TEST(should_erase_files_when_they_do_not_exist)
{
	Fm_filesystem_fixture f;
	gfs::mkdir(f.get_fm_parent() + "/dir1");
	Filemanager manager(f.get_fm_path());
	vector<string> categories;
	categories.push_back("catA");
	auto& map(manager.get_map());
	map.set("dir1/file1", categories.begin(), categories.end());
	manager.refresh();

	vector<string> values;
	map.get(categories, back_inserter(values));
	fail_unless(values.empty(), "should be empty");
}
END_TEST

START_TEST(should_erase_files_when_they_are_directories)
{
	Fm_filesystem_fixture f;
	gfs::mkdir(f.get_fm_parent() + "/dir1/file1");
	vector<string> categories;
	categories.push_back("catA");
	Filemanager manager(f.get_fm_path());
	auto& map(manager.get_map());
	map.set("dir1/file1", categories.begin(), categories.end());

	manager.refresh();

	vector<string> values;
	map.get(categories, back_inserter(values));
	fail_unless(values.empty(), "should be empty");
}
END_TEST

START_TEST(when_there_are_similar_pathes_in_different_branches)
{
	Fm_filesystem_fixture f;
	gfs::mkdir(f.get_fm_parent() + "/dir2/dir1");
	gfs::create_emptyfile(f.get_fm_parent() + "/dir2/dir1/file1");
	vector<string> categories;
	categories.push_back("catA");
	Filemanager manager(f.get_fm_path());
	auto& map(manager.get_map());
	map.set("dir1/file1", categories.begin(), categories.end());
	map.set("dir2/dir1/file1", categories.begin(), categories.end());

	manager.refresh();

	vector<string> values;
	map.get(categories, back_inserter(values));
	fail_unless(values.size() == 1, "count");
}
END_TEST

START_TEST(when_a_directory_is_really_a_file)
{
	Fm_filesystem_fixture f;
	gfs::mkdir(f.get_fm_parent() + "/dir2/file1");
	gfs::create_emptyfile(f.get_fm_parent() + "/file1");
	gfs::create_emptyfile(f.get_fm_parent() + "/dir2/file1/file2");
	vector<string> categories;
	categories.push_back("catA");
	Filemanager manager(f.get_fm_path());
	auto& map(manager.get_map());
	map.set("file1/file2", categories.begin(), categories.end());
	map.set("dir2/file1/file2", categories.begin(), categories.end());

	manager.refresh();

	vector<string> values;
	map.get(categories, back_inserter(values));
	fail_unless(values.size() == 1, "count");
}
END_TEST

namespace fm {
namespace test {

TCase* create_refresh_core_tcase()
{
	TCase* tcase(tcase_create("core"));
	tcase_add_test(tcase, should_not_erase_files_when_they_exist);
	tcase_add_test(tcase, should_erase_files_when_they_do_not_exist);
	tcase_add_test(tcase, should_erase_files_when_they_are_directories);
	return tcase;
}

/* Here are test cases to check that it does not erase wrong files whose pathes
   are similar to those of the files which should be erased. */
TCase* create_refresh_erase_tcase()
{
	TCase* tcase(tcase_create("erase functionality"));
	tcase_add_test(tcase,
		when_there_are_similar_pathes_in_different_branches);
	tcase_add_test(tcase, when_a_directory_is_really_a_file);
	return tcase;
}

} // test
} // fm
