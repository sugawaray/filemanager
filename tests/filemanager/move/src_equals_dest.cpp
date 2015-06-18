#include "src_equals_dest.h"
#include <filemanager.h>
#include <filesystem.h>
#include <nomagic.h>
#include <utils.h>
#include <tests/fixture/assert.h>
#include <tests/fixture/filesystem.h>
#include <tests/fixture/fm_filesystem.h>
#include <tests/fixture/map.h>
#include <iostream>
#include <sstream>

namespace fm {
namespace filemanager {
namespace test {

namespace {

template<class T, class U, class V>
inline void A(T t, U u, V v)
{
	::test::fixture::Assert(t, u, v);
}

using std::cerr;

inline void reset_cerr()
{
	fmcerr = &cerr;
}

class Test : public ::test::Fm_filesystem_fixture {
public:
	Test()
		:	m(get_fm_path()),
			root(get_fm_parent()),
			src(root.child("file1")),
			file_exists(true) {
	}

	void test() {
		auto a(utils::Auto_caller(reset_cerr));

		if (file_exists)
			::fs::create_emptyfile(src);

		using ::ml::test::fixture::set_category;
		set_category(m.get_map(), "file1", "catA");

		using ::ml::test::fixture::get_values;
		using ::test::fixture::get_stat_result;

		os.clear();
		os.str("");
		fmcerr = &os;

		call_method(src, src);

		verify();
	}

	void set_file_existence(bool value) {
		file_exists = value;
	}
protected:
	virtual void call_method(const Absolute_path& src,
		const Absolute_path& dest) = 0;
	virtual void verify() = 0;

	Filemanager m;
	Absolute_path root;
	Absolute_path src;
	std::ostringstream os;
	bool file_exists;
};

using ::test::fixture::get_stat_result;
using ::ml::test::fixture::get_values;

class Move_test : public Test {
protected:
	void call_method(const Absolute_path& src, const Absolute_path& dest) {
		m.move(src, dest);
	}

	void verify() {
		if (file_exists) {
			A(get_stat_result(src.to_filepath_string()) == 0,
				__FILE__, __LINE__);
		}
		auto v(get_values(m.get_map(), "catA"));
		A(v.size() == 1, __FILE__, __LINE__);
		A(v.at(0) == "file1", __FILE__, __LINE__);
	}
};

class Copy_test : public Test {
public:
	void call_method(const Absolute_path& src, const Absolute_path& dest) {
		m.copy(src, dest);
	}

	void verify() {
		A(os.str() != "", __FILE__, __LINE__);
		if (file_exists) {
			A(get_stat_result(src.to_filepath_string()) == 0,
				__FILE__, __LINE__);
		}
		auto v(get_values(m.get_map(), "catA"));
		A(v.size() == 1, __FILE__, __LINE__);
		A(v.at(0) == "file1", __FILE__, __LINE__);
	}
};

void t1()
{
	Move_test t;
	t.test();
}

void t2()
{
	Copy_test t;
	t.test();
}

void t3()
{
	Copy_test t;
	t.set_file_existence(false);
	t.test();
}

} // unnamed

void run_src_equals_dest_tests()
{
	cerr << "fm::Filemanager when the source equals the dest."
		<< std::endl;

	using nomagic::run;

	run("move: a file with names", t1);

	run("copy: a file with names", t2);

	run("copy: names without a file", t3);
}

} // test
} // filemanager
} // fm
