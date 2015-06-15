#include <cerrno>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>

extern "C" {

#include <check.h>

} // extern "C"

#include <absolute_path.h>
#include <except.h>
#include <filemanager.h>
#include <filesystem.h>
#include <utils_assert.h>
#include <tests/fixture/assert.h>
#include <tests/fixture/filesystem.h>
#include "move_copy_test.h"

using std::string;

namespace fm {
namespace filemanager {
namespace test {

using std::logic_error;

namespace {

inline Fm_map_impl& get_map(Move_copy_test& test)
{
	return test.get_manager().get_map();
}

template<class Expr>
inline void A(Expr expr)
{
	utils::Assert<logic_error>(expr);
}

template<class Expr, class T, class U>
inline void A(Expr expr, T value1, U value2)
{
	::test::fixture::Assert(expr, value1, value2);
}

} // unnamed

using std::back_inserter;
using std::cerr;
using std::endl;
using std::find;
using std::sort;
using std::vector;
using ::test::Fm_filesystem_fixture;
using ::test::fixture::get_stat_result;
namespace gfs = ::fs;
using gfs::mkdir;

Move_copy_test::Move_copy_test()
{
	categories1.push_back("catA");
	categories2.push_back("catB");
}

void Move_copy_test::test_should_move_or_copy_a_file_to_a_file()
{
	add_category_file("dir1/file1", "dir1", categories1);
	add_category_file("dir2/file2", "dir2", categories2);

	string src(fixture.get_fm_parent() + "/dir1/file1");
	string dest(fixture.get_fm_parent() + "/dir2/file2");
	call_method(Absolute_path(src), Absolute_path(dest));

	assert_should_move_or_copy_a_file_to_a_file(src, dest);
}

void Move_copy_test::test_should_move_a_file_to_a_filepath()
{
	add_category_file("dir1/file1", "dir1", categories1);

	string src(fixture.get_fm_parent() + "/dir1/file1");
	string dest(fixture.get_fm_parent() + "/dir2/file2");
	call_method(Absolute_path(src), Absolute_path(dest));

	assert_should_move_a_file_to_a_filepath();
}

void Move_copy_test::test_should_move_a_file_to_a_directory()
{
	add_category_file("dir1/file1", "dir1", categories1);
	add_category_file("dir2/file2", "dir2", categories1);

	string src(fixture.get_fm_parent() + "/dir1/file1");
	string dest(fixture.get_fm_parent() + "/dir2");
	call_method(Absolute_path(src), Absolute_path(dest));

	assert_should_move_a_file_to_a_directory(src, dest);
}

void Move_copy_test::test_should_move_a_directory_to_a_directory()
{
	add_category_file("dir1/file1", "dir1", categories1);
	add_category_file("dir1/file2", "dir1", categories1);

	add_category_file("dir2/file2", "dir2", categories1);

	string src(fixture.get_fm_parent() + "/dir1");
	string dest(fixture.get_fm_parent() + "/dir2");
	call_method(Absolute_path(src), Absolute_path(dest));

	assert_should_move_a_directory_to_a_directory(src, dest);
}

void Move_copy_test::test_should_not_move_similar_files()
{
	add_category_file("dir1/file1", "dir1", categories1);
	add_category_file("dir1/file1.ext", "dir1", categories1);
	string dest(fixture.get_fm_parent() + "/dir2");
	mkdir(Absolute_path(dest));

	string src(fixture.get_fm_parent() + "/dir1/file1");
	call_method(Absolute_path(src), Absolute_path(dest));

	assert_should_not_move_similar_files();
}

void Move_copy_test::test_should_not_move_a_file_when_it_looks_like_a_dir()
{
	auto& map(manager.get_map());
	map.set("dir1/file1/file2", categories1.begin(), categories1.end());
	Absolute_path target(fixture.get_fm_parent() + "/dir1/file1");
	mkdir(target.parent());
	gfs::create_emptyfile(target);

	Absolute_path dest(fixture.get_fm_parent() + "/dir2/file1");
	try {
		call_method(target, dest);
		throw logic_error("test");
	}
	catch (const Out_of_sync&) {
	}

	A(get_stat_result(dest.to_filepath_string()) == ENOENT,
		__FILE__, __LINE__);
}

void Move_copy_test::test_should_not_move_a_dir_when_it_looks_like_a_file()
{
	auto& map(manager.get_map());
	Absolute_path target(fixture.get_fm_parent() + "/dir1/dir2");
	mkdir(target);
	map.set("dir1/dir2", categories1.begin(), categories1.end());

	Absolute_path dest(fixture.get_fm_parent() + "/dir2");
	try {
		call_method(target, dest);
		logic_error("test");
	}
	catch (const Out_of_sync&) {
	}

	A(get_stat_result(dest.to_filepath_string()) == ENOENT,
		__FILE__, __LINE__);
}

void Move_copy_test::test_move_a_file_mapping_without_its_real_file()
{
	auto& map(manager.get_map());
	Absolute_path target(fixture.get_fm_parent() + "/dir1/file1");
	map.set("dir1/file1", categories1.begin(), categories1.end());

	Absolute_path dest(fixture.get_fm_parent() + "/dir2/file1");
	call_method(target, dest);

	assert_move_a_file_mapping_without_its_real_file();
}

void Move_copy_test::test_move_a_dir_mapping_without_its_real_dir()
{
	auto& map(manager.get_map());
	Absolute_path target(fixture.get_fm_parent() + "/dir1/dir2");
	map.set("dir1/dir2/file1", categories1.begin(), categories1.end());
	map.set("dir1/dir2/file2", categories1.begin(), categories1.end());

	Absolute_path dest(fixture.get_fm_parent() + "/dir2");
	add_category_file("dir2/file3", "dir2", categories1);
	call_method(target, dest);

	assert_move_a_dir_mapping_without_its_real_dir();
}

void Move_copy_test::test_should_not_move_a_dir_to_a_dir_which_does_not_exist()
{
	auto& map(manager.get_map());
	add_category_file("dir1/file1", "dir1", categories1);
	map.set("dir2/file1", categories1.begin(), categories1.end());
	Absolute_path target(fixture.get_fm_parent() + "/dir1");
	Absolute_path dest(fixture.get_fm_parent() + "/dir2");
	try {
		call_method(target, dest);
		throw logic_error("test");
	}
	catch (const Out_of_sync&) {
	}
	A(get_stat_result(dest.to_filepath_string()) == ENOENT,
		__FILE__, __LINE__);
	vector<string> r;
	map.get(categories1, back_inserter(r));
	A(r.size() == 2, __FILE__, __LINE__);
	sort(r.begin(), r.end());
	A(r.at(0) == "dir1/file1", __FILE__, __LINE__);
	A(r.at(1) == "dir2/file1", __FILE__, __LINE__);
}

void Move_copy_test::test_move_a_dir_to_a_dirpath_where_there_is_a_dir()
{
	add_category_file("dir1/file1", "dir1", categories1);
	Absolute_path target(fixture.get_fm_parent() + "/dir1");
	Absolute_path dest(fixture.get_fm_parent() + "/dir2");
	mkdir(dest);

	call_method(target, dest);

	assert_move_a_dir_to_a_dirpath_where_there_is_a_dir(target, dest);
}

void Move_copy_test::
	test_should_not_move_a_file_to_a_dest_file_which_does_not_exist()
{
	auto& map(manager.get_map());
	add_category_file("dir1/file1", "dir1", categories1);
	map.set("dir1/file2", categories1.begin(), categories1.end());
	Absolute_path target(fixture.get_fm_parent() + "/dir1/file1");
	Absolute_path dest(fixture.get_fm_parent() + "/dir1/file2");
	try {
		call_method(target, dest);
		throw logic_error("test");
	}
	catch (const Out_of_sync&) {
	}
	A(get_stat_result(target.to_filepath_string()) == 0,
		__FILE__, __LINE__);
	A(get_stat_result(dest.to_filepath_string()) == ENOENT,
		__FILE__, __LINE__);
	vector<string> r;
	map.get(categories1, back_inserter(r));
	A(r.size() == 2, __FILE__, __LINE__);
	sort(r.begin(), r.end());
	A(r.at(0) == "dir1/file1", __FILE__, __LINE__);
	A(r.at(1) == "dir1/file2", __FILE__, __LINE__);
}

void Move_copy_test::test_move_a_file_to_a_path_where_there_is_a_file()
{
	auto& map(manager.get_map());
	add_category_file("dir1/file1", "dir1", categories1);
	add_category_file("dir2/file1", "dir2", categories1);
	map.remove("dir2/file1");
	Absolute_path target(fixture.get_fm_parent() + "/dir1/file1");
	Absolute_path dest(fixture.get_fm_parent() + "/dir2/file1");

	call_method(target, dest);

	assert_move_a_file_to_a_path_where_there_is_a_file(target, dest);
}

void
Move_copy_test::test_should_not_move_a_file_to_a_dir_which_does_not_exist()
{
	add_category_file("dir1/file1", "dir1", categories1);
	auto& map(manager.get_map());
	map.set("dir2/file2", categories1.begin(), categories1.end());
	Absolute_path target(fixture.get_fm_parent() + "/dir1/file1");
	Absolute_path dest(fixture.get_fm_parent() + "/dir2");
	try {
		call_method(target, dest);
		throw logic_error("test");
	}
	catch (const Out_of_sync&) {
	}
	A(get_stat_result(target.to_filepath_string()) == 0,
		__FILE__, __LINE__);
	A(get_stat_result(dest.to_filepath_string()) == ENOENT,
		__FILE__, __LINE__);
	vector<string> r;
	map.get(categories1, back_inserter(r));
	sort(r.begin(), r.end());
	A(r.size() == 2, __FILE__, __LINE__);
	A(r.at(0) == "dir1/file1", __FILE__, __LINE__);
	A(r.at(1) == "dir2/file2", __FILE__, __LINE__);
}

void Move_copy_test::test_should_not_move_a_file_to_a_dir_when_a_dir_exists()
{
	auto& map(manager.get_map());
	map.set("dir1/file1", categories1.begin(), categories1.end());
	map.set("dir2/file1/file2", categories1.begin(), categories1.end());
	gfs::mkdir(fixture.get_fm_parent() + "/dir2/file1");
	Absolute_path target(fixture.get_fm_parent() + "/dir1/file1");
	Absolute_path dest(fixture.get_fm_parent() + "/dir2");

	try {
		call_method(target, dest);
		throw logic_error("test");
	}
	catch (const Invalid_destination&) {
	}
	vector<string> r;
	map.get(categories1, back_inserter(r));
	fail_unless(r.size() == 2, "count");
	fail_unless(find(r.begin(), r.end(), "dir1/file1") != r.end(),
		"value 1");
	fail_unless(find(r.begin(), r.end(), "dir2/file1/file2") != r.end(),
		"value 2");
}

void Move_copy_test::test_should_move_a_directory_to_a_directory_path()
{
	auto& map(manager.get_map());
	map.set("dir1/file1", categories1.begin(), categories1.end());
	map.set("dir1/file2", categories1.begin(), categories1.end());

	call_method(Absolute_path(fixture.get_fm_parent() + "/dir1"),
		Absolute_path(fixture.get_fm_parent() + "/dir2"));

	assert_should_move_a_directory_to_directory_path();
}

void Move_copy_test::test_should_not_move_a_file_to_an_impossible_path()
{
	auto& map(manager.get_map());
	map.set("dir1/file1", categories1.begin(), categories1.end());
	map.set("dir2/file1", categories1.begin(), categories1.end());
	Absolute_path target(fixture.get_fm_parent() + "/dir1/file1");
	Absolute_path dest(fixture.get_fm_parent() + "/dir2/file1/file1");

	try {
		call_method(target, dest);
		throw logic_error("test");
	}
	catch (const Invalid_destination&) {
	}

	vector<string> r;
	map.get(categories1, back_inserter(r));
	fail_unless(r.size() == 2, "count");
	fail_unless(find(r.begin(), r.end(), "dir1/file1") != r.end(),
		"value 1");
	fail_unless(find(r.begin(), r.end(), "dir2/file1") != r.end(),
		"value 2");
}

void Move_copy_test::test_should_not_move_a_directory_to_an_impossible_path()
{
	auto& map(manager.get_map());
	map.set("dir1/file1", categories1.begin(), categories1.end());
	map.set("dir2/file1", categories1.begin(), categories1.end());
	Absolute_path target(fixture.get_fm_parent() + "/dir1");
	Absolute_path dest(fixture.get_fm_parent() + "/dir2/file1/dir1");

	try {
		call_method(target, dest);
		throw logic_error("test");
	}
	catch (const Invalid_destination&) {
	}

	vector<string> r;
	map.get(categories1, back_inserter(r));
	fail_unless(r.size() == 2, "count");
	fail_unless(find(r.begin(), r.end(), "dir1/file1") != r.end(),
		"value 1");
	fail_unless(find(r.begin(), r.end(), "dir2/file1") != r.end(),
		"value 2");
}

void Move_copy_test::test_should_not_move_a_directory_to_its_an_ancestor()
{
	auto& map(manager.get_map());
	map.set("dir1/dir2/dir2/file1", categories1.begin(), categories1.end());
	gfs::mkdir(fixture.get_fm_parent() + "/dir1");
	Absolute_path target(fixture.get_fm_parent() + "/dir1/dir2/dir2");
	Absolute_path dest(fixture.get_fm_parent() + "/dir1");

	try {
		call_method(target, dest);
		throw logic_error("test");
	}
	catch (const Invalid_destination&) {
	}

	vector<string> r;
	map.get(categories1, back_inserter(r));
	fail_unless(r.size() == 1, "count");
	fail_unless(r.at(0) == "dir1/dir2/dir2/file1", "value");
}

void
Move_copy_test::test_move_a_file_to_a_dir_which_does_not_have_any_mappings()
{
	add_category_file("dir1/file1", "dir1", categories1);
	Absolute_path target(fixture.get_fm_parent() + "/dir1/file1");
	Absolute_path dest(fixture.get_fm_parent() + "/dir2");
	mkdir(dest);

	call_method(target, dest);

	assert_move_a_file_to_a_dir_which_does_not_have_any_mappings(
		fixture.get_fm_parent());
}

void Move_copy_test::move_files_to_paths_it_does_not_manage()
{
	Absolute_path fm_path(fixture.get_fm_parent() + "/dir1/dir2/.fm");
	mkdir(fm_path);
	Filemanager m(fm_path);

	Absolute_path src(fm_path.parent().child("file1"));
	gfs::create_emptyfile(src);
	m.get_map().set("file1", categories1.begin(), categories1.end());
	Absolute_path dest(fm_path.parent().parent());
	call_method(m, src, dest);

	assert_move_files_to_paths_it_does_not_manage(m, src, dest);
}

void Move_copy_test::add_category_file(const string& fm_path,
	const string& dir_fm_path, const vector<string>& categories)
{
	manager.get_map().set(fm_path, categories.begin(), categories.end());
	mkdir(fixture.get_fm_parent() + "/" + dir_fm_path);
	gfs::create_emptyfile(fixture.get_fm_parent() + "/" + fm_path);
}

void Move_test::assert_should_move_or_copy_a_file_to_a_file(
	const string& src, const string& dest)
{
	auto& map(get_manager().get_map());
	auto& cat1(get_categories1());
	auto& cat2(get_categories2());

	map.get(cat1, back_inserter(result1));
	fail_unless(result1.size() == 1, "count");
	fail_unless(result1.at(0) == "dir2/file2", "value");

	map.get(cat2, back_inserter(result2));
	fail_unless(result2.empty(), "count");

	struct stat stat_obj = { 0 };
	errno = 0;
	fail_unless(stat(src.c_str(), &stat_obj) == -1, "return value 1");
	fail_unless(errno == ENOENT, "cause");

	fail_unless(stat(dest.c_str(), &stat_obj) == 0, "return value 2");
	fail_unless(S_ISREG(stat_obj.st_mode) != 0, "file type");
}

void Move_test::assert_should_move_a_file_to_a_filepath()
{
	vector<string> r;
	auto& map(get_manager().get_map());
	map.get(get_categories1(), back_inserter(r));

	fail_unless(r.size() == 1, "count");
	fail_unless(r.at(0) == "dir2/file2", "value");
}

void Move_test::assert_should_move_a_file_to_a_directory(
	const string& src, const string& dest)
{
	vector<string> r;
	auto& map(get_manager().get_map());
	map.get(get_categories1(), back_inserter(r));

	fail_unless(r.size() == 2, "count");
	fail_unless(find(r.begin(), r.end(), "dir1/file1") == r.end(),
		"value 1");
	fail_unless(find(r.begin(), r.end(), "dir2/file1") != r.end(),
		"value 2");
	fail_unless(find(r.begin(), r.end(), "dir2/file2") != r.end(),
		"value 3");

	struct stat stat_obj = { 0 };
	errno = 0;
	fail_unless(stat(src.c_str(), &stat_obj) == -1, "stat result 1");
	fail_unless(errno == ENOENT, "stat cause 1");

	fail_unless(stat((dest + "/file1").c_str(), &stat_obj) == 0,
		"stat result 2");
	fail_unless(S_ISREG(stat_obj.st_mode) != 0, "file type 2");
}

void Move_test::assert_should_move_a_directory_to_a_directory(
	const string& src, const string& dest)
{
	vector<string> r;
	auto& map(get_manager().get_map());
	map.get(get_categories1(), back_inserter(r));
	fail_unless(r.size() == 3, "count");

	string expected;
	expected = "dir2/dir1/file1";
	fail_unless(find(r.begin(), r.end(), expected) != r.end(), "value 1");
	expected = "dir2/dir1/file2";
	fail_unless(find(r.begin(), r.end(), expected) != r.end(), "value 2");

	struct stat stat_obj = { 0 };
	errno = 0;
	fail_unless(stat(src.c_str(), &stat_obj) == -1, "result value 1");
	fail_unless(errno == ENOENT, "result cause");

	fail_unless(stat(dest.c_str(), &stat_obj) == 0, "result value 2");
	fail_unless(S_ISDIR(stat_obj.st_mode) != 0, "file type 2");

	fail_unless(stat((dest + "/dir1/file1").c_str(), &stat_obj) == 0,
		"result value 3");
	fail_unless(S_ISREG(stat_obj.st_mode) != 0, "file type 3");

	fail_unless(stat((dest + "/dir1/file2").c_str(), &stat_obj) == 0,
		"result value 4");
	fail_unless(S_ISREG(stat_obj.st_mode) != 0, "file type 4");
}

void Move_test::assert_should_not_move_similar_files()
{
	vector<string> r;
	auto& map(get_map(*this));
	map.get(get_categories1(), back_inserter(r));

	A(find(r.begin(), r.end(), "dir2/file1.ext") == r.end());
}

void Move_test::assert_move_a_dir_mapping_without_its_real_dir()
{
	vector<string> r;
	auto& map(get_map(*this));
	map.get(get_categories1(), back_inserter(r));
	A(r.size() == 3, __FILE__, __LINE__);
	sort(r.begin(), r.end());
	A(r.at(0) == "dir2/dir2/file1", __FILE__, __LINE__);
	A(r.at(1) == "dir2/dir2/file2", __FILE__, __LINE__);
	A(r.at(2) == "dir2/file3", __FILE__, __LINE__);
}

void Move_test::assert_move_a_file_mapping_without_its_real_file()
{
	vector<string> r;
	auto& map(get_map(*this));
	map.get(get_categories1(), back_inserter(r));
	A(r.size() == 1, __FILE__, __LINE__);
	A(r.at(0) == "dir2/file1", __FILE__, __LINE__);
}

void Move_test::assert_should_move_a_directory_to_directory_path()
{
	vector<string> r;
	auto& map(get_map(*this));
	map.get(get_categories1(), back_inserter(r));
	fail_unless(r.size() == 2, "count");
	fail_unless(find(r.begin(), r.end(), "dir2/file1") != r.end(),
		"value 1");
	fail_unless(find(r.begin(), r.end(), "dir2/file2") != r.end(),
		"value 2");
}

void Move_test::assert_move_a_file_to_a_dir_which_does_not_have_any_mappings(
	const string& root)
{
	vector<string> r;
	auto& map(get_map(*this));
	map.get(get_categories1(), back_inserter(r));
	A(r.size() == 1, __FILE__, __LINE__);
	A(r.at(0) == "dir2/file1", __FILE__, __LINE__);
}

void Move_test::assert_move_a_dir_to_a_dirpath_where_there_is_a_dir(
	const Absolute_path& target, const Absolute_path& dest)
{
	vector<string> r;
	auto& map(get_map(*this));
	map.get(get_categories1(), back_inserter(r));
	A(r.size() == 1, __FILE__, __LINE__);
	sort(r.begin(), r.end());
	A(r.at(0) == "dir2/dir1/file1", __FILE__, __LINE__);
	A(get_stat_result(dest.child("dir1").to_filepath_string()) == 0,
		__FILE__, __LINE__);
	A(get_stat_result(dest.child("dir1").child("file1")
		.to_filepath_string()) == 0, __FILE__, __LINE__);
}

void Move_test::assert_move_a_file_to_a_path_where_there_is_a_file(
	const Absolute_path& src, const Absolute_path& dest)
{
	A(get_stat_result(src.to_filepath_string()) == ENOENT, __FILE__,
		__LINE__);
	A(get_stat_result(dest.to_filepath_string()) == 0, __FILE__, __LINE__);
	vector<string> r;
	auto& map(get_map(*this));
	map.get(get_categories1(), back_inserter(r));
	A(r.size() == 1, __FILE__, __LINE__);
	A(r.at(0) == "dir2/file1", __FILE__, __LINE__);
}

void Move_test::assert_move_files_to_paths_it_does_not_manage(
	Filemanager& m, const Absolute_path& src, const Absolute_path& dest)
{
	A(get_stat_result(dest.to_filepath_string()) == 0, __FILE__, __LINE__);
	vector<string> r;
	m.get_map().get(get_categories1(), back_inserter(r));
	A(r.size() == 0, __FILE__, __LINE__);
}

void Copy_test::assert_should_move_or_copy_a_file_to_a_file(
	const string& src, const string& dest)
{
	auto& map(get_manager().get_map());
	auto& cat1(get_categories1());
	auto& cat2(get_categories2());

	map.get(cat1, back_inserter(result1));
	fail_unless(result1.size() == 2, "count");
	fail_unless(find(result1.begin(), result1.end(), "dir1/file1")
		!= result1.end(), "value 1");
	fail_unless(find(result1.begin(), result1.end(), "dir2/file2")
		!= result1.end(), "value 2");

	map.get(cat2, back_inserter(result2));
	fail_unless(result2.empty(), "count");

	struct stat stat_obj = { 0 };
	fail_unless(stat(src.c_str(), &stat_obj) == 0, "return value 1");
	fail_unless(S_ISREG(stat_obj.st_mode) != 0, "file type 1");

	fail_unless(stat(dest.c_str(), &stat_obj) == 0, "return value 2");
	fail_unless(S_ISREG(stat_obj.st_mode) != 0, "file type 2");
}

void Copy_test::assert_should_move_a_file_to_a_filepath()
{
	vector<string> r;
	auto& map(get_manager().get_map());
	map.get(get_categories1(), back_inserter(r));

	fail_unless(r.size() == 2, "count");
	fail_unless(find(r.begin(), r.end(), "dir1/file1") != r.end(),
		"value");
	fail_unless(find(r.begin(), r.end(), "dir2/file2") != r.end(),
		"value");
}

void Copy_test::assert_should_move_a_file_to_a_directory(
	const string& src, const string& dest)
{
	vector<string> r;
	auto& map(get_manager().get_map());
	map.get(get_categories1(), back_inserter(r));

	fail_unless(r.size() == 3, "count");
	fail_unless(find(r.begin(), r.end(), "dir1/file1") != r.end(),
		"value 1");
	fail_unless(find(r.begin(), r.end(), "dir2/file1") != r.end(),
		"value 2");
	fail_unless(find(r.begin(), r.end(), "dir2/file2") != r.end(),
		"value 3");

	struct stat stat_obj = { 0 };
	errno = 0;
	fail_unless(stat(src.c_str(), &stat_obj) == 0, "stat result 1");
	fail_unless(S_ISREG(stat_obj.st_mode) != 0, "file type 1");

	fail_unless(stat((dest + "/file1").c_str(), &stat_obj) == 0,
		"stat result 2");
	fail_unless(S_ISREG(stat_obj.st_mode) != 0, "file type 2");
}

void Copy_test::assert_should_move_a_directory_to_a_directory(
	const string& src, const string& dest)
{
	vector<string> r;
	auto& map(get_manager().get_map());
	map.get(get_categories1(), back_inserter(r));
	fail_unless(r.size() == 5, "count");

	string expected;

	expected = "dir1/file1";
	fail_unless(find(r.begin(), r.end(), expected) != r.end(), "value 1");

	expected = "dir1/file2";
	fail_unless(find(r.begin(), r.end(), expected) != r.end(), "value 2");

	expected = "dir2/dir1/file1";
	fail_unless(find(r.begin(), r.end(), expected) != r.end(), "value 3");

	expected = "dir2/dir1/file2";
	fail_unless(find(r.begin(), r.end(), expected) != r.end(), "value 4");

	struct stat stat_obj = { 0 };
	errno = 0;
	fail_unless(stat(src.c_str(), &stat_obj) == 0, "result value 1");
	fail_unless(stat((src + "/file1").c_str(), &stat_obj) == 0,
		"result value 2");
	fail_unless(stat((src + "/file2").c_str(), &stat_obj) == 0,
		"result value 3");

	fail_unless(stat(dest.c_str(), &stat_obj) == 0, "result value 4");
	fail_unless(S_ISDIR(stat_obj.st_mode) != 0, "file type 2");

	fail_unless(stat((dest + "/dir1/file1").c_str(), &stat_obj) == 0,
		"result value 5");
	fail_unless(S_ISREG(stat_obj.st_mode) != 0, "file type 3");

	fail_unless(stat((dest + "/dir1/file2").c_str(), &stat_obj) == 0,
		"result value 6");
	fail_unless(S_ISREG(stat_obj.st_mode) != 0, "file type 4");
}

void Copy_test::assert_should_not_move_similar_files()
{
	vector<string> r;
	auto& map(get_map(*this));
	map.get(get_categories1(), back_inserter(r));

	A(find(r.begin(), r.end(), "dir2/file1.ext") == r.end());
}

void Copy_test::assert_move_a_dir_mapping_without_its_real_dir()
{
	vector<string> r;
	auto& map(get_map(*this));
	map.get(get_categories1(), back_inserter(r));
	A(r.size() == 5, __FILE__, __LINE__);
	sort(r.begin(), r.end());
	A(r.at(0) == "dir1/dir2/file1", __FILE__, __LINE__);
	A(r.at(1) == "dir1/dir2/file2", __FILE__, __LINE__);
	A(r.at(2) == "dir2/dir2/file1", __FILE__, __LINE__);
	A(r.at(3) == "dir2/dir2/file2", __FILE__, __LINE__);
	A(r.at(4) == "dir2/file3", __FILE__, __LINE__);
}

void Copy_test::assert_should_move_a_directory_to_directory_path()
{
	vector<string> r;
	auto& map(get_map(*this));
	map.get(get_categories1(), back_inserter(r));
	A(r.size() == 4, __FILE__, __LINE__);
	sort(r.begin(), r.end());
	A(r.at(0) == "dir1/file1", __FILE__, __LINE__);
	A(r.at(1) == "dir1/file2", __FILE__, __LINE__);
	A(r.at(2) == "dir2/file1", __FILE__, __LINE__);
	A(r.at(3) == "dir2/file2", __FILE__, __LINE__);
}

void Copy_test::assert_move_a_file_to_a_dir_which_does_not_have_any_mappings(
	const string& root)
{
	vector<string> r;
	auto& map(get_map(*this));
	map.get(get_categories1(), back_inserter(r));
	A(r.size() == 2, __FILE__, __LINE__);
	sort(r.begin(), r.end());
	A(r.at(0) == "dir1/file1", __FILE__, __LINE__);
	A(r.at(1) == "dir2/file1", __FILE__, __LINE__);
	A(get_stat_result(root + "/dir1/file1") == 0, __FILE__, __LINE__);
	A(get_stat_result(root + "/dir2/file1") == 0, __FILE__, __LINE__);
}

void Copy_test::assert_move_a_file_mapping_without_its_real_file()
{
	vector<string> r;
	auto& map(get_map(*this));
	map.get(get_categories1(), back_inserter(r));
	A(r.size() == 2, __FILE__, __LINE__);
	sort(r.begin(), r.end());
	A(r.at(0) == "dir1/file1", __FILE__, __LINE__);
	A(r.at(1) == "dir2/file1", __FILE__, __LINE__);
}

void Copy_test::assert_move_a_dir_to_a_dirpath_where_there_is_a_dir(
	const Absolute_path& target, const Absolute_path& dest)
{
	vector<string> r;
	auto& map(get_map(*this));
	map.get(get_categories1(), back_inserter(r));
	A(r.size() == 2, __FILE__, __LINE__);
	sort(r.begin(), r.end());
	A(r.at(0) == "dir1/file1", __FILE__, __LINE__);
	A(r.at(1) == "dir2/dir1/file1", __FILE__, __LINE__);
	A(get_stat_result(target.child("file1").to_filepath_string()) == 0,
		__FILE__, __LINE__);
	A(get_stat_result(dest.child("dir1").to_filepath_string()) == 0,
		__FILE__, __LINE__);
	A(get_stat_result(dest.child("dir1").child("file1")
		.to_filepath_string()) == 0, __FILE__, __LINE__);
}

void Copy_test::assert_move_a_file_to_a_path_where_there_is_a_file(
	const Absolute_path& src, const Absolute_path& dest)
{
	A(get_stat_result(src.to_filepath_string()) == 0, __FILE__, __LINE__);
	A(get_stat_result(dest.to_filepath_string()) == 0, __FILE__, __LINE__);
	vector<string> r;
	auto& map(get_map(*this));
	map.get(get_categories1(), back_inserter(r));
	A(r.size() == 2, __FILE__, __LINE__);
	sort(r.begin(), r.end());
	A(r.at(0) == "dir1/file1", __FILE__, __LINE__);
	A(r.at(1) == "dir2/file1", __FILE__, __LINE__);
}

void Copy_test::assert_move_files_to_paths_it_does_not_manage(
	Filemanager& m, const Absolute_path& src, const Absolute_path& dest)
{
	A(get_stat_result(dest.to_filepath_string()) == 0, __FILE__, __LINE__);
	vector<string> r;
	m.get_map().get(get_categories1(), back_inserter(r));
	A(r.size() == 1, __FILE__, __LINE__);
}

} // test
} // filemanager
} // fm

