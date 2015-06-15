#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include "process_dir.h"
#include <filesystem.h>
#include "filesystem.h"
#include "test_filesystem.h"

using std::remove;
using std::string;
using fs::create_emptyfile;
using test::Process_dir_fixture;
using test::fixture::get_stat_result;

START_TEST(should_return_errno_result)
{
	Process_dir_fixture f;
	string path(f.get_path() + "/emptyfile");
	create_emptyfile(path);
	fail_unless(get_stat_result(path.c_str()) == 0, "result code 1");

	remove(path.c_str());
	fail_unless(get_stat_result(path.c_str()) == ENOENT, "result code 2");
}
END_TEST

namespace test {
namespace fixture {

TCase* create_get_stat_result_tcase()
{
	TCase* tcase(tcase_create("get_stat_result"));
	tcase_add_test(tcase, should_return_errno_result);
	return tcase;
}

Suite* create_filesystem_fixture_suite()
{
	Suite* suite(suite_create("test::fixture::filesystem"));
	suite_add_tcase(suite, create_get_stat_result_tcase());
	return suite;
}

} // fixture
} // test

#include <nomagic.h>
#include "assert.h"

namespace test {
namespace fixture {

namespace {

template<class T, class U, class V>
inline void A(T t, U u, V v)
{
	Assert(t, u, v);
}

using std::vector;
class Fixture : public Process_dir_fixture {
public:
	string get_f1() {
		return get_path() + "/f1";
	}

	string get_f2() {
		return get_path() + "/f2";
	}
};
typedef Fixture F;

void t1()
{
	F f;
	vector<char> v(2, 'a');
	write_to_file(f.get_f1(), v);
	write_to_file(f.get_f2(), v);
	A(equal_files(f.get_f1(), f.get_f2()), __FILE__, __LINE__);
}

void t2()
{
	F f;
	vector<char> v1(2, 'a');
	vector<char> v2(2, 'b');
	write_to_file(f.get_f1(), v1);
	write_to_file(f.get_f2(), v2);
	A(!equal_files(f.get_f1(), f.get_f2()), __FILE__, __LINE__);
}

} // unnamed

void run_filesystem_tests()
{
	using nomagic::run;

	run("equal_files: return true if the files' contents are identical.",
		t1);

	run(	"equal_files: return false if the files' contents are not "
		"identical.", t2);
}

} // fixture
} // test
