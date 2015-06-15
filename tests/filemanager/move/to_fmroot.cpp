#include "to_fmroot.h"
#include <absolute_path.h>
#include <filemanager.h>
#include <filesystem.h>
#include <nomagic.h>
#include <tests/fixture/assert.h>
#include <tests/fixture/fm_filesystem.h>
#include <tests/fixture/map.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace fm {
namespace filemanager {
namespace test {

namespace {

template<class T, class U, class V>
inline void A(T t, U u, V v)
{
	::test::fixture::Assert(t, u, v);
}

class Fixture : public ::test::Fm_filesystem_fixture {
public:
	Fixture() {
		root = Absolute_path(get_fm_parent());
		d1 = root.child("d1");
		::fs::mkdir(d1);
		chdir(d1.to_filepath_string().c_str());
	}

	Absolute_path getd1() const {
		return d1;
	}

	Absolute_path get_root() const {
		return root;
	}

private:
	Absolute_path d1;
	Absolute_path root;
};

void t1()
{
	Fixture f;
	Absolute_path s(f.getd1().child("srcfile1"));
	::fs::create_emptyfile(s);
	Filemanager m;
	::ml::test::fixture::set_category(m.get_map(), "d1/srcfile1", "catA");
	m.copy(s, f.get_root());

	auto v(::ml::test::fixture::get_values(m.get_map(), "catA"));
	A(v.size() == 2, __FILE__, __LINE__);
	std::sort(v.begin(), v.end());
	A(v.at(0) == "d1/srcfile1", __FILE__, __LINE__);
	A(v.at(1) == "srcfile1", __FILE__, __LINE__);
}

} // unnamed

void run_to_fmroot_tests()
{
	std::cerr << "fm::Filemanager copy or move to the fmroot dir."
		<< std::endl;

	using nomagic::run;

	run(	"It can copy a file with names to the fm root directory "
		"without it's names deleted.", t1);
}

} // test
} // filemanager
} // fm
