#include "init.h"
#include <except.h>
#include <filemanager.h>
#include <fm.h>
#include <nomagic.h>
#include <utils.h>
#include <tests/fixture/assert.h>
#include <tests/fixture/process_dir.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace fm {
namespace filemanager {
namespace test {

namespace {

template<class T, class U, class V>
inline void A(T t, U u, V v)
{
	::test::fixture::Assert(t, u, v);
}

inline void reset_cerr()
{
	fmcerr = &std::cerr;
}

void t1()
{
	auto a(utils::Auto_caller(reset_cerr));
	::test::Process_dir_fixture f;
	std::ostringstream os;
	fmcerr = &os;
	try {
		Filemanager manager(f.get_path() + "/.fm");
		throw std::logic_error("test");
	}
	catch (const Fmdir_is_not_found&) {
	}
	A(os.str() != "", __FILE__, __LINE__);
}

} // unnamed

void run_init_tests()
{
	using nomagic::run;

	std::cerr << "fm::Filemanager initialization tests" << std::endl;

	run(	"It throws an exception if the specified fmdir does not exist "
		"when it is constructed.", t1);
}

} // test
} // filemanager
} // fm
