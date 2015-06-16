#include <string>
#include <vector>
#include <fm.h>
#include <fm_map_impl.h>
#include "fixture/db.h"
#include "fm_map_impl.h"

using test::Db_fixture;
using namespace fm;
using std::string;
using std::vector;

namespace {

class Fixture : public Db_fixture {
public:
	Fixture() {
		categories.push_back("catA");
	}

	vector<string>& get_categories() {
		return categories;
	}

private:
	vector<string> categories;
};

} // unnamed

START_TEST(should_return_file_type_given_file_value)
{
	Fixture f;
	Fm_map_impl map(f.get_dbfilepath());
	map.set("dir1/file1", f.get_categories().begin(),
		f.get_categories().end());
	fail_unless(map.get_file_type("dir1/file1") == Type_file, "result");
}
END_TEST

START_TEST(should_return_dir_type_given_dir_value)
{
	Fixture f;
	Fm_map_impl map(f.get_dbfilepath());
	auto& categories(f.get_categories());
	map.set("dir1/dir2/file1", categories.begin(), categories.end());
	fail_unless(map.get_file_type("dir1/dir2") == Type_dir, "result");
}
END_TEST

START_TEST(should_return_Not_exist_given_new_value)
{
	Fixture f;
	Fm_map_impl map(f.get_dbfilepath());
	fail_unless(map.get_file_type("dir1/file1") == Not_exist, "result");
}
END_TEST

START_TEST(should_return_impossible_when_the_value_conflicts)
{
	Fixture f;
	Fm_map_impl map(f.get_dbfilepath());
	auto& categories(f.get_categories());
	map.set("dir1/file1", categories.begin(), categories.end());
	fail_unless(map.get_file_type("dir1/file1/file2") == Impossible,
		"result");
}
END_TEST

START_TEST(return_file_type_given_similar_name_files)
{
	Fixture f;
	Fm_map_impl map(f.get_dbfilepath());
	auto& cat(f.get_categories());
	map.set("dir1/file1", cat.begin(), cat.end());
	map.set("dir1/file1.ext", cat.begin(), cat.end());
	fail_unless(map.get_file_type("dir1/file1") == Type_file, "result");
}
END_TEST

START_TEST(return_dir_type_given_root_value)
{
	Fixture f;
	Fm_map_impl map(f.get_dbfilepath());
	fail_unless(map.get_file_type("") == Type_dir, "result");
}
END_TEST

namespace fm {
namespace test {

namespace fm_map_impl {

TCase* create_get_file_type_tcase()
{
	TCase* tcase(tcase_create("get_file_type"));
	tcase_add_test(tcase, should_return_file_type_given_file_value);
	tcase_add_test(tcase, should_return_dir_type_given_dir_value);
	tcase_add_test(tcase, should_return_Not_exist_given_new_value);
	tcase_add_test(tcase,
		should_return_impossible_when_the_value_conflicts);
	tcase_add_test(tcase, return_file_type_given_similar_name_files);
	tcase_add_test(tcase, return_dir_type_given_root_value);
	return tcase;
}

} // fm_map_impl

Suite* create_fm_map_impl_test_suite()
{
	using namespace fm_map_impl;
	Suite* suite(suite_create("fm_map_impl"));
	suite_add_tcase(suite, create_get_file_type_tcase());
	return suite;
}

} // test
} // fm
