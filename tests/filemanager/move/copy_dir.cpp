#include <cerrno>
#include <iostream>
#include <stdexcept>
#include <nomagic.h>
#include <except.h>
#include <filesystem.h>
#include <tests/fixture/assert.h>
#include <tests/fixture/filesystem.h>
#include <tests/filemanager/move_copy_fixture.h>
#include "copy_dir.h"

namespace fm {
namespace filemanager {
namespace test {

namespace {

template<class T, class U, class V>
inline void A(T t, U u, V v)
{
	::test::fixture::Assert(t, u, v);
}

typedef Move_copy_fixture F;

using ::fs::create_emptyfile;
using ::fs::mkdir;
using ::test::fixture::get_stat_result;

void t1()
{
	F f;
	Absolute_path src(f.get_root().child("dir1"));
	mkdir(src);
	create_emptyfile(src.child("file1"));
	f.add_categories("dir1/file1");

	Absolute_path dest(f.get_root().child("dir2"));
	mkdir(dest);

	try {
		f.get_manager().copy(src, dest);
		throw std::logic_error("test");
	}
	catch (const File_is_a_directory&) {
	}

	A(get_stat_result(dest.child("dir1").to_filepath_string()) == ENOENT,
		__FILE__, __LINE__);
	A(get_stat_result(dest.child("file1").to_filepath_string()) == ENOENT,
		__FILE__, __LINE__);
	std::vector<std::string> r(f.get_categories_values());
	A(r.size() == 1, __FILE__, __LINE__);
	A(r.at(0) == "dir1/file1", __FILE__, __LINE__);
}

void t2()
{
	F f;
	Absolute_path src(f.get_root().child("dir1"));
	f.add_categories("dir1/file1");

	Absolute_path dest(f.get_root().child("dir2"));
	mkdir(dest);
	create_emptyfile(dest.child("file1"));

	try {
		f.get_manager().copy(src, dest);
		throw std::logic_error("test");
	}
	catch (const File_is_a_directory&) {
	}

	std::vector<std::string> r(f.get_categories_values());
	A(r.size() == 1, __FILE__, __LINE__);
	A(r.at(0) == "dir1/file1", __FILE__, __LINE__);
}

} // unnamed

void run_copy_dir_tests()
{
	using ::nomagic::run;

	std::cerr << "fm::Filemanager::copy when source paths are directories"
		<< std::endl;

	run("copy does not copy a directory.", t1);

	run("copy does not copy names in a directory.", t2);
}

} // test
} // filemanager
} // fm
