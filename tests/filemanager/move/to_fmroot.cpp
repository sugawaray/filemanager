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

using ::fs::mkdir;

class Fixture : public ::test::Fm_filesystem_fixture {
public:
	Fixture() {
		root = Absolute_path(get_fm_parent());
		d1 = root.child("d1");
		mkdir(d1);
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

class File_test : private Fixture {
public:
	void test();
protected:
	virtual void call_method(const Absolute_path& src,
		const Absolute_path& dest) = 0;
	virtual void verify() = 0;

	Filemanager m;
};

using ::ml::test::fixture::set_category;
using ::fs::create_emptyfile;

void File_test::test()
{
	Absolute_path s(getd1().child("srcfile1"));
	create_emptyfile(s);
	set_category(m.get_map(), "d1/srcfile1", "catA");

	call_method(s, get_root());

	verify();
}

class Copy_file_test : public File_test {
public:
	void call_method(const Absolute_path& src,
		const Absolute_path& dest) {
		m.copy(src, dest);
	}

	void verify();
};

using ::ml::test::fixture::get_values;
using std::sort;

void Copy_file_test::verify()
{
	auto v(get_values(m.get_map(), "catA"));
	A(v.size() == 2, __FILE__, __LINE__);
	sort(v.begin(), v.end());
	A(v.at(0) == "d1/srcfile1", __FILE__, __LINE__);
	A(v.at(1) == "srcfile1", __FILE__, __LINE__);
}

class Move_file_test : public File_test {
public:
	void call_method(const Absolute_path& src,
		const Absolute_path& dest) {
		m.move(src, dest);
	}

	void verify();
};

void Move_file_test::verify()
{
	auto v(get_values(m.get_map(), "catA"));
	A(v.size() == 1, __FILE__, __LINE__);
	A(v.at(0) == "srcfile1", __FILE__, __LINE__);
}

class Copy_dir_test : private Fixture {
public:
	void test();
};

void Copy_dir_test::test()
{
	Absolute_path s(getd1().child("srcdir1"));
	mkdir(s);
	create_emptyfile(s.child("srcfile1"));
	Filemanager m;
	set_category(m.get_map(), "d1/srcdir1/srcfile1", "catA");

	m.copydir(s, get_root());

	auto v(get_values(m.get_map(), "catA"));
	A(v.size() == 2, __FILE__, __LINE__);
	sort(v.begin(), v.end());
	std::cerr << v.at(0) << std::endl;
	std::cerr << v.at(1) << std::endl;
	A(v.at(0) == "d1/srcdir1/srcfile1", __FILE__, __LINE__);
	A(v.at(1) == "srcdir1/srcfile1", __FILE__, __LINE__);
}

void t1()
{
	Copy_file_test t;
	t.test();
}

void t2()
{
	Move_file_test t;
	t.test();
}

void t3()
{
	Copy_dir_test t;
	t.test();
}

} // unnamed

void run_to_fmroot_tests()
{
	std::cerr << "fm::Filemanager copy or move to the fmroot dir."
		<< std::endl;

	using nomagic::run;

	run(	"It can copy a file with names to the fm root directory "
		"without it's names deleted.", t1);

	run(	"It can move a file with names to the fm root directory "
		"without it's names deleted.", t2);

	run(	"It can copy a directory with child names to the fm root "
		"directory without it's names deleted.", t3);
}

} // test
} // filemanager
} // fm
