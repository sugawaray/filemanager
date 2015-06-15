#include <cerrno>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <nomagic.h>
#include <filemanager.h>
#include <filesystem.h>
#include <tests/fixture/assert.h>
#include <tests/fixture/filesystem.h>
#include <tests/fixture/fm_filesystem.h>
#include "overwrite.h"

namespace fm {
namespace filemanager {
namespace test {

namespace {

template<class T, class U, class V>
inline void A(T t, U u, V v)
{
	::test::fixture::Assert(t, u, v);
}

class Overwrite_mappings_test : public ::test::Fm_filesystem_fixture {
public:
	Overwrite_mappings_test()
		:	manager(get_fm_path()),
			cat(1, "catA") {
	}

	void test() {
		src = Absolute_path(get_fm_parent() + "/srcfile1");
		create_emptyfile(src);

		dest = Absolute_path(get_fm_parent() + "/destfile1");
		create_emptyfile(dest);

		manager.get_map().set("destfile1", cat.begin(), cat.end());

		call_method(src, dest);

		verify();
	}
protected:
	virtual void call_method(const Absolute_path& src,
		const Absolute_path& dest) = 0;
	virtual void verify() = 0;

	Filemanager manager;
	Absolute_path src;
	Absolute_path dest;
	std::vector<std::string> cat;
};

class Copy_overwrite_mappings_test : public Overwrite_mappings_test {
protected:
	void call_method(const Absolute_path& src,
		const Absolute_path& dest) {
		manager.copy(src, dest);
	}

	void verify() {
		std::vector<std::string> r;
		manager.get_map().get(cat, std::back_inserter(r));
		A(r.empty(), __FILE__, __LINE__);
	}
};

class Move_overwrite_mappings_test : public Overwrite_mappings_test {
protected:
	void call_method(const Absolute_path& src,
		const Absolute_path& dest) {
		manager.move(src, dest);
	}

	void verify() {
		using ::test::fixture::get_stat_result;

		std::vector<std::string> r;
		manager.get_map().get(cat, std::back_inserter(r));
		A(get_stat_result(src.to_filepath_string()) == ENOENT,
			__FILE__, __LINE__);
		A(get_stat_result(dest.to_filepath_string()) == 0,
			__FILE__, __LINE__);
		A(r.empty(), __FILE__, __LINE__);
	}
};

void t1()
{
	Copy_overwrite_mappings_test t;
	t.test();
}

void t2()
{
	Move_overwrite_mappings_test t;
	t.test();
}

} // unnamed

void run_overwrite_tests()
{
	std::cerr << "fm::Filemanager::_overwrite tests" << std::endl;

	using nomagic::run;

	run(	"copy: overwrites destination mappings", t1);

	run(	"move: overwrites destination mappings", t2);
}

} // test
} // filemanager
} // fm
