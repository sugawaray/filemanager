#include "src_equals_dest.h"
#include <filemanager.h>
#include <filesystem.h>
#include <nomagic.h>
#include <tests/fixture/assert.h>
#include <tests/fixture/filesystem.h>
#include <tests/fixture/fm_filesystem.h>
#include <tests/fixture/map.h>
#include <iostream>

namespace fm {
namespace filemanager {
namespace test {

namespace {

template<class T, class U, class V>
inline void A(T t, U u, V v)
{
	::test::fixture::Assert(t, u, v);
}

void t1()
{
	using ::ml::test::fixture::set_category;
	using ::ml::test::fixture::get_values;
	using ::test::fixture::get_stat_result;

	::test::Fm_filesystem_fixture f;
	
	Filemanager m(f.get_fm_path());
	Absolute_path root(f.get_fm_parent());
	Absolute_path src(root.child("file1"));
	::fs::create_emptyfile(src);
	set_category(m.get_map(), "file1", "catA");

	m.move(src, src);

	A(get_stat_result(src.to_filepath_string()) == 0, __FILE__, __LINE__);
	auto v(get_values(m.get_map(), "catA"));
	A(v.size() == 1, __FILE__, __LINE__);
	A(v.at(0) == "file1", __FILE__, __LINE__);
}

} // unnamed

void run_src_equals_dest_tests()
{
	std::cerr << "fm::Filemanager when the source equals the dest."
		<< std::endl;

	using nomagic::run;

	run("move: a file with names", t1);
}

} // test
} // filemanager
} // fm
