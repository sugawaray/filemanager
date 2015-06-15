#include <cerrno>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>
#include <absolute_path.h>
#include <filemanager.h>
#include <filesystem.h>
#include <tests/fixture/fm_filesystem.h>
#include <tests/fixture/filesystem.h>
#include "fixture.h"
#include "remove.h"
#include <utils_assert.h>

using std::string;

namespace {

using std::logic_error;

template<class T>
inline void A(T expr)
{
	Assert<logic_error>(expr);
}

} // unnamed

using std::find;
using std::back_inserter;
using std::vector;

namespace gfs = fs;
using fm::Absolute_path;
using fm::Filemanager;
using fm::Fm_map_impl;
using fm::Not_empty;
using fm::Success;
using fm::filemanager::test::Fixture;
using gfs::create_emptyfile;
using gfs::get_abs_path;
using gfs::mkdir;
using test::fixture::get_stat_result;

START_TEST(remove_a_file_without_the_real_file)
{
	Fixture f;
	auto& cat(f.get_categories());
	f.get_map().set("dir1/file1", cat.begin(), cat.end());

	int rc(f.get_manager().remove(f.get_fm_parent() + "/dir1/file1"));

	A(rc == Success);
	vector<string> r;
	f.get_map().get(cat, back_inserter(r));
	A(r.empty());
}
END_TEST

START_TEST(remove_a_file_with_the_real_file)
{
	Fixture f;
	auto& cat(f.get_categories());
	f.get_map().set("dir1/file1", cat.begin(), cat.end());
	mkdir(f.get_fm_parent() + "/dir1");
	string target(f.get_fm_parent() + "/dir1/file1");
	create_emptyfile(target);

	int rc(f.get_manager().remove(target));

	A(rc == Success);
	vector<string> r;
	f.get_map().get(cat, back_inserter(r));
	A(r.empty());
	A(get_stat_result(target) == ENOENT);
}
END_TEST

START_TEST(remove_an_empty_dir)
{
	Fixture f;
	Absolute_path target(f.get_fm_parent() + "/dir1");
	mkdir(target);

	int r(f.get_manager().remove(target));

	A(r == Success);
	A(get_stat_result(target.to_filepath_string()) == ENOENT);
}
END_TEST

START_TEST(remove_an_empty_dir_with_children_mappings)
{
	Fixture f;
	Absolute_path target(f.get_fm_parent() + "/dir1");
	mkdir(target);
	auto& map(f.get_map());
	auto& cat(f.get_categories());
	map.set("dir1/file1", cat.begin(), cat.end());
	map.set("dir2/file2", cat.begin(), cat.end());

	int r(f.get_manager().remove(target));

	A(r == Success);
	vector<string> v;
	map.get(cat, back_inserter(v));
	A(v.size() == 1);
	A(find(v.begin(), v.end(), "dir2/file2") != v.end());
}
END_TEST

START_TEST(should_not_remove_a_dir_with_children)
{
	Fixture f;
	Absolute_path target(f.get_fm_parent() + "/dir1");
	mkdir(target);
	create_emptyfile(target.child("file1"));
	auto& map(f.get_map());
	auto& cat(f.get_categories());
	map.set("dir1/file1", cat.begin(), cat.end());

	int rc(f.get_manager().remove(target));

	A(rc == Not_empty);
	A(get_stat_result(target.to_filepath_string()) == 0);
	vector<string> r;
	map.get(cat, back_inserter(r));
	A(r.size() == 1);
	A(r.at(0) == "dir1/file1");
}
END_TEST

START_TEST(should_not_remove_similar_files)
{
	Fixture f;
	auto& map(f.get_map());
	auto& cat(f.get_categories());

	Absolute_path target(f.get_fm_parent() + "/dir1/file1");
	mkdir(target.parent());
	create_emptyfile(target);
	map.set("dir1/file1", cat.begin(), cat.end());

	Absolute_path similar_one(f.get_fm_parent() + "/dir1./file1");
	mkdir(similar_one.parent());
	create_emptyfile(similar_one);
	map.set("dir1./file1", cat.begin(), cat.end());

	f.get_manager().remove(target);

	vector<string> r;
	f.get_map().get(f.get_categories(), back_inserter(r));
	A(find(r.begin(), r.end(), "dir1./file1") != r.end());
}
END_TEST

namespace fm {
namespace filemanager {
namespace test {

TCase* create_remove_tcase()
{
	TCase* tcase(tcase_create("remove"));
	tcase_add_test(tcase, remove_a_file_without_the_real_file);
	tcase_add_test(tcase, remove_a_file_with_the_real_file);
	tcase_add_test(tcase, remove_an_empty_dir);
	tcase_add_test(tcase, remove_an_empty_dir_with_children_mappings);
	tcase_add_test(tcase, should_not_remove_a_dir_with_children);
	tcase_add_test(tcase, should_not_remove_similar_files);
	return tcase;
}

} // test
} // filemanager
} // fm
