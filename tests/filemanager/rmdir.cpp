#include <cerrno>
#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>
#include <filemanager.h>
#include <filesystem.h>
#include <utils.h>
#include <tests/fixture/filesystem.h>
#include <tests/fixture/fm_filesystem.h>
#include "fixture.h"
#include "rmdir.h"
#include <utils_assert.h>

namespace {

template<class T>
inline void A(T expr)
{
	utils::Assert<std::logic_error>(expr);
}

} // unnamed

using std::back_inserter;
using std::find;
using std::string;
using std::vector;
using fm::Absolute_path;
using fm::filemanager::test::Fixture;
using fm::Success;
using test::fixture::get_stat_result;
namespace gfs = fs;
using gfs::create_emptyfile;
using gfs::mkdir;

START_TEST(remove_an_empty_dir)
{
	Fixture f;
	Absolute_path target(f.get_fm_parent() + "/dir1");
	mkdir(target);

	int r(f.get_manager().rmdir(target));

	A(r == Success);
	A(get_stat_result(target.to_filepath_string()) == ENOENT);
}
END_TEST

START_TEST(remove_a_dir_with_children)
{
	Fixture f;
	Absolute_path target(f.get_fm_parent() + "/dir1");
	mkdir(target);
	create_emptyfile(target.child("file1"));
	create_emptyfile(target.child("file2"));
	mkdir(target.child("dir2"));
	auto& map(f.get_map());
	auto& cat(f.get_categories());
	map.set("dir1/file1", cat.begin(), cat.end());
	map.set("dir1/file2", cat.begin(), cat.end());
	map.set("dir1/dir2/file1", cat.begin(), cat.end());
	map.set("dir2/file1", cat.begin(), cat.end());

	int rc(f.get_manager().rmdir(target));

	A(rc == Success);
	A(get_stat_result(target.to_filepath_string()) == ENOENT);
	vector<string> r;
	map.get(cat, back_inserter(r));
	A(r.size() == 1);
	A(find(r.begin(), r.end(), "dir2/file1") != r.end());
}
END_TEST

namespace {

inline void cleanup_filemode(const Absolute_path& target)
{
	chmod(target.to_filepath_string().c_str(), S_IRWXU);
}

} // unnamed

START_TEST(return_failure_when_it_failed_to_remove_a_dir)
{
	Fixture f;
	Absolute_path target(f.get_fm_parent() + "/dir1");
	mkdir(target);
	mkdir(target.child("dir2"));
	chmod(target.to_filepath_string().c_str(), S_IRUSR | S_IWUSR);

	{
		auto auto_caller(utils::Auto_caller(
			std::bind(cleanup_filemode, std::cref(target))));
		int r(f.get_manager().rmdir(target));

		A(r == fm::Failure);
	}
}
END_TEST

START_TEST(remove_a_file)
{
	Fixture f;
	Absolute_path target(f.get_fm_parent() + "/file1");
	create_emptyfile(target);
	auto& map(f.get_map());
	auto& cat(f.get_categories());
	map.set("file1", cat.begin(), cat.end());

	int result(f.get_manager().rmdir(target));

	A(result == fm::Success);
	A(get_stat_result(target.to_filepath_string()) == ENOENT);
	vector<string> r;
	map.get(cat, back_inserter(r));
	A(r.empty());
}
END_TEST

namespace fm {
namespace filemanager {
namespace test {

TCase* create_rmdir_tcase()
{
	TCase* tcase(tcase_create("rmdir"));
	tcase_add_test(tcase, remove_an_empty_dir);
	tcase_add_test(tcase, remove_a_dir_with_children);
	tcase_add_test(tcase, return_failure_when_it_failed_to_remove_a_dir);
	tcase_add_test(tcase, remove_a_file);
	return tcase;
}

} // test
} // filemanager
} // fm
