#include <cstdio>
#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

extern "C" {

#include <check.h>

}

#include "absolute_path.h"
#include "map_impl.h"
#include "map_impl/getall.h"
#include "statement.h"
#include "string_utils.h"
#include "filemanager.h"
#include "filesystem.h"
#include "fm_commands.h"
#include "fm_filesystem.h"
#include "fm_filesystem/filesystem.h"
#include "fm_map_impl.h"
#include "path.h"
#include "tree.h"
#include "fixture/test_assert.h"
#include "fixture/test_filesystem.h"

using fm::test::cp_tests;
using namespace fm::test;
using namespace fm::fs::test;
using namespace fs::test;
using test::fixture::create_assert_fixture_suite;
using test::fixture::create_filesystem_fixture_suite;

namespace {

using std::find_if;
using std::string;

inline bool equalstr(const string& lhs, const string& rhs)
{
	return lhs == rhs;
}

bool is_target(int argc, char* argv[], const string& s)
{
	using std::bind;
	using std::cref;
	using std::placeholders::_1;
	return find_if(argv + 1, argv + argc, bind(equalstr, cref(s), _1))
		!= argv + argc;
}

} // unnamed

int main(int argc, char* argv[])
{
	using std::cerr;
	using std::endl;

	bool doall(argc == 1);

	if (doall || is_target(argc, argv, "fm::cp")) {
		cerr << "fm::cp tests" << endl;
		cp_tests();
	}

	if (doall || is_target(argc, argv, "fm::rm")) {
		cerr << "fm::rm tests" << endl;
		rm_tests();
	}

	if (doall || is_target(argc, argv, "fm::getcat")) {
		cerr << "fm::getcat tests" << endl;
		getcat_tests();
	}

	if (doall || is_target(argc, argv, "fm::set")) {
		cerr << "fm::set tests" << endl;
		set_tests();
	}

	if (doall || is_target(argc, argv, "fm::refresh")) {
		cerr << "fm::refresh tests" << endl;
		refresh_tests();
	}

	if (doall || is_target(argc, argv, "fm::get")) {
		cerr << "fm::get tests" << endl;
		get_tests();
	}

	if (doall || is_target(argc, argv, "fm::mv")) {
		cerr << "fm::mv tests" << endl;
		mv_tests();
	}

	if (doall || is_target(argc, argv, "fm::fs::Filesystem")) {
		cerr << "fm::fs::Filesystem tests" << endl;
		::fm::fs::test::run_filesystem_tests();
	}

	if (doall || is_target(argc, argv, "fm::Filemanager")) {
		cerr << "fm::Filemanager tests" << endl;
		run_filemanager_tests();
	}

	if (doall || is_target(argc, argv, "fs::_filesystem")) {
		cerr << "fm::_filesystem tests" << endl;
		::fs::test::run_filesystem_tests();
	}

	if (doall || is_target(argc, argv, "test::fixture")) {
		cerr << "test::fixture tests" << endl;
		::test::fixture::run_filesystem_tests();
	}

	if (doall || is_target(argc, argv, "ml::Map_impl")) {
		cerr << "ml::Map_impl tests" << endl;
		::ml::map_impl::test::run_getall_tests();
	}

	if (doall) {
		SRunner* runner = srunner_create(create_map_impl_test_suite());
		srunner_add_suite(runner, create_statement_test_suite());
		srunner_add_suite(runner, create_string_utils_test_suite());
		srunner_add_suite(runner, create_filemanager_test_suite());
		srunner_add_suite(runner, create_filesystem_test_suite());
		srunner_add_suite(runner, create_filesystem_fixture_suite());
		srunner_add_suite(runner, create_fm_filesystem_test_suite());
		srunner_add_suite(runner, create_fm_map_impl_test_suite());
		srunner_add_suite(runner, create_absolute_path_test_suite());
		srunner_add_suite(runner, create_assert_fixture_suite());
		srunner_add_suite(runner, create_path_test_suite());
		srunner_add_suite(runner, create_tree_test_suite());
		srunner_run_all(runner, CK_NORMAL);
		srunner_free(runner);
	}
	return 0;
}
