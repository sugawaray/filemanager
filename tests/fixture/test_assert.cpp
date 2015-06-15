#include <sstream>
#include "assert.h"
#include "test_assert.h"

using std::cerr;
using std::endl;
using std::ostringstream;
using test::fixture::Assert;

START_TEST(should_not_output_when_true)
{
	ostringstream os;
	Assert(true, 1, 2, os);
	if (os.str() != "")
		cerr << "should_not_output_when_true" << endl;
}
END_TEST

START_TEST(should_output_when_false_with_2_params)
{
	ostringstream os;
	Assert(false, 1, 2, os);
	ostringstream expected;
	expected << 1 << ":" << 2 << endl;
	if (os.str() != expected.str()) {
		cerr << os.str();
		cerr << "should_output_when_false_with_2_params" << endl;
	}
}
END_TEST

namespace test {
namespace fixture {

TCase* create_assert_tcase()
{
	TCase* tcase(tcase_create("assert"));
	tcase_add_test(tcase, should_not_output_when_true);
	tcase_add_test(tcase, should_output_when_false_with_2_params);
	return tcase;
}

Suite* create_assert_fixture_suite()
{
	Suite* suite(suite_create("test::fixture::Assert"));
	suite_add_tcase(suite, create_assert_tcase());
	return suite;
}

} // fixture
} // test
