#include "getall.h"
#include <tests/fixture/db.h>
#include <map_impl.h>
#include <nomagic.h>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#define L	(::nomagic::loc(__FILE__, __LINE__))

namespace ml {
namespace map_impl {
namespace test {
namespace {

void t1(const char* ms)
{
	using std::string;
	using std::vector;

	nomagic::Test t(ms);
	::test::Db_fixture f;
	Map_impl map(f.get_dbfilepath());
	vector<string> c(1, "some");
	vector<string> r;
	map.getall(c, std::back_inserter(r));
	t.a(r.empty(), L);
}

} // unnamed

void run_getall_tests()
{
	std::cerr << "ml::Map_impl::getall tests" << std::endl;

	nomagic::run(	"It does not return any values if there are not.", t1); 
}

} // test
} // map_impl
} // ml
