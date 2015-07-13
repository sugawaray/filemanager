#include "getall.h"
#include <tests/fixture/db.h>
#include <tests/fixture/map.h>
#include <map_impl.h>
#include <nomagic.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#define L	(::nomagic::loc(__FILE__, __LINE__))

namespace ml {
namespace map_impl {
namespace test {
namespace {

using nomagic::Test;
using std::string;
using std::vector;

class Fixture : public Test, private ::test::Db_fixture {
public:
	Fixture(const char* ms)
		:	Test(ms), map_obj(get_dbfilepath()) {
	}

	Map_impl& map() {
		return map_obj;
	}

	vector<string>& v1() {
		return v1_obj;
	}

	vector<string>& v2() {
		return v2_obj;
	}
private:
	Map_impl map_obj;
	vector<string> v1_obj;
	vector<string> v2_obj;
};

using std::back_inserter;

void t1(const char* ms)
{
	Fixture fi(ms);
	fi.map().getall(vector<string>(1, "some"), back_inserter(fi.v1()));
	fi.a(fi.v1().empty(), L);
}

vector<string> create_vector(const string& s1, const string& s2)
{
	vector<string> v;
	v.push_back(s1);
	v.push_back(s2);
	return v;
}

using ml::test::fixture::set_category;

void t2(const char* ms)
{
	Fixture fi(ms);
	set_category(fi.map(), "dir1/file1", "some");
	set_category(fi.map(), "dir1/file2", "some");
	auto& v(fi.v1());
	fi.map().getall(vector<string>(1, "some"), back_inserter(v));
	fi.a(v.size() == 2, L);
	std::sort(v.begin(), v.end());
	fi.a(v.at(0) == "dir1/file1", L);
	fi.a(v.at(1) == "dir1/file2", L);
}

void t3(const char* ms)
{
	Fixture fi(ms);
	auto& m(fi.map());
	set_category(m, "dir1/file1", "some1");
	set_category(m, "dir1/file2", "some2");
	auto& v(fi.v1());
	m.getall(create_vector("some1", "some2"), back_inserter(v));
	fi.a(v.size() == 2, L);
	std::sort(v.begin(), v.end());
	fi.a(v.at(0) == "dir1/file1", L);
	fi.a(v.at(1) == "dir1/file2", L);
}

void t4(const char* ms)
{
	Fixture fi(ms);
	auto& m(fi.map());
	set_category(m, "dir1/file1", "some");
	auto& v(fi.v1());
	m.getall(vector<string>(), back_inserter(v));
	fi.a(v.empty(), L);
}

} // unnamed

void run_getall_tests()
{
	std::cerr << "ml::Map_impl::getall tests" << std::endl;

	using nomagic::run;

	run("It does not return any values if there are not.", t1); 

	run("It returns values if there are.", t2);

	run("It returns all values that matches some of categories.", t3);

	run("It does not return any values if the categories are empty.", t4);
}

} // test
} // map_impl
} // ml
