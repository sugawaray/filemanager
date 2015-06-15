#include <algorithm>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <absolute_path.h>
#include <filesystem.h>
#include <tests/fixture/filesystem.h>
#include <tests/fixture/process_dir.h>
#include "copy_abs_path.h"
#include "fixture/filesystem.h"

using std::string;
using std::vector;
using fs::Absolute_path;
using fs::create_emptyfile;
using fs::Failure;
using fs::get_abs_path;
using fs::mkdir;
using fs::test::fixture::dummy_content;
using ::test::fixture::read_file;
using ::test::fixture::write_to_file;

namespace {

vector<char> create_file(const Absolute_path& path)
{
	create_emptyfile(path.to_filepath_string());
	vector<char> content(dummy_content());
	write_to_file(path.to_filepath_string(), content);
	return content;
}

bool equal_content(const vector<char>& lhs, const vector<char>& rhs)
{
	if (lhs.size() != rhs.size())
		return false;
	else
		return equal(lhs.begin(), lhs.end(), rhs.begin());
}

} // unnamed

using std::equal;
using fs::copy;
using test::Process_dir_fixture;

START_TEST(should_copy_a_file)
{
	Process_dir_fixture f;
	Absolute_path src(get_abs_path(f.get_path() + "/src"));
	vector<char> src_content(create_file(src));
	Absolute_path dest(get_abs_path(f.get_path() + "/dest"));

	int r(copy(src, dest));

	vector<char> dest_content;
	read_file(dest.to_filepath_string(), dest_content);

	fail_unless(r == 0, "result code");
	fail_unless(equal_content(src_content, dest_content), "content");
}
END_TEST

START_TEST(should_make_intermediate_directories)
{
	Process_dir_fixture f;
	Absolute_path src(get_abs_path(f.get_path() + "/src"));
	vector<char> src_content(create_file(src));
	Absolute_path dest(get_abs_path(f.get_path() + "/dir1/dir2/dest"));

	int r(copy(src, dest));

	fail_unless(r == 0, "result code");

	vector<char> dest_content;
	read_file(dest.to_filepath_string(), dest_content);
	fail_unless(equal_content(src_content, dest_content), "content");
}
END_TEST

START_TEST(should_fail_when_it_failed_to_make_intermediate_directories)
{
	Process_dir_fixture f;
	Absolute_path src(get_abs_path(f.get_path() + "/src"));
	create_file(src);
	Absolute_path dest(get_abs_path(f.get_path() + "/dir1/dir2/dest"));
	mkdir(get_abs_path(f.get_path() + "/dir1"));
	chmod((f.get_path() + "/dir1").c_str(), S_IRUSR | S_IXUSR);

	int r(copy(src, dest));
	fail_unless(r == Failure, "result code");
}
END_TEST

namespace fs {
namespace test {

TCase* create_copy_abs_path_tcase()
{
	TCase* tcase(tcase_create("fs::copy abs path"));
	tcase_add_test(tcase, should_copy_a_file);
	tcase_add_test(tcase, should_make_intermediate_directories);
	tcase_add_test(tcase,
		should_fail_when_it_failed_to_make_intermediate_directories);
	return tcase;
}

} // test
} // fs
