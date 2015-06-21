#include "getcat.h"
#include <filemanager.h>
#include <filesystem.h>
#include <nomagic.h>
#include <tests/fixture/filemanager.h>
#include <tests/fixture/map.h>
#include <tests/fixture/utils.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

namespace fm {
namespace filemanager {
namespace test {

namespace {

using std::back_inserter;
using std::string;
using std::vector;
using ::fs::create_emptyfile;
using ::ml::test::fixture::set_category;
using ::test::fixture::return_param;
using nomagic::Test;
using nomagic::loc;

void t1(const char* ms)
{
	Test t(ms);
	Fm_fixture f;
	auto& m(f.get_manager());

	Absolute_path target(return_param(create_emptyfile,
		f.get_root().child("f1")));
	set_category(m.get_map(), "f1", "catA", "catB");

	vector<string> v;
	int r(m.getcat(target, back_inserter(v)));

	t.a(r == Success, loc(__FILE__, __LINE__));
	t.a(v.size() == 2, loc(__FILE__, __LINE__));
	std::sort(v.begin(), v.end());
	t.a(v.at(0) == "catA", loc(__FILE__, __LINE__));
	t.a(v.at(1) == "catB", loc(__FILE__, __LINE__));
}

void t2(const char* ms)
{
	Test t(ms);
	Fm_fixture f;
	auto& m(f.get_manager());

	Absolute_path child1(return_param(create_emptyfile,
		f.get_root().child("f1")));
	set_category(m.get_map(), "f1", "catA");
	Absolute_path target(return_param(create_emptyfile,
		f.get_another_branch().child("f1")));

	vector<string> v;
	int r(m.getcat(target, back_inserter(v)));

	t.a(r == Not_managed, loc(__FILE__, __LINE__));
	t.a(v.empty(), loc(__FILE__, __LINE__));
}

} // unnamed

void run_getcat_tests()
{
	std::cerr << "fm::Filemanager::getcat" << std::endl;

	using nomagic::run;

	run("It returns file's names when it is called with file paths.",
		t1);

	run(	"It returns Not_managed and it does not return any names "
		"when the target is not managed.", t2);
}

} // test
} // filemanager
} // fm
