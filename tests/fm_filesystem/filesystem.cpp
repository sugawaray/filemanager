#include <iostream>
#include <nomagic.h>
#include <absolute_path.h>
#include <fm.h>
#include <fm_filesystem.h>
#include <tests/fixture/fm_filesystem.h>
#include <tests/fixture/assert.h>
#include "filesystem.h"

namespace fm {
namespace fs {
namespace test {

template<class E, class T, class U>
void A(E e, T v, U u)
{
	::test::fixture::Assert(e, v, u);
}

using ::test::Fm_filesystem_fixture;
typedef Fm_filesystem_fixture F;

void t1()
{
	F fi;
	Filesystem f(fi.get_fm_path());
	Absolute_path p(fi.get_fm_parent() + "/file1");
	A(f.is_descendant(p), __FILE__, __LINE__);
}

void t2()
{
	F fi;
	Filesystem f(fi.get_fm_path());
	Absolute_path p(fi.get_fm_parent());
	A(!f.is_descendant(p.parent()), __FILE__, __LINE__);
}

void run_filesystem_tests()
{
	using nomagic::run;
	std::cerr << "fm::fs::Filesystem::is_descendant" << std::endl;

	run(	"It returns true if the file is descendant of fm "
		"file hierachy. ", t1);

	run(	"It returns false if the file is not descendant of fm "
		"file hierachy. ", t2);
}

} // test
} // fs
} // fm
