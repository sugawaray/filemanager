#ifndef __ML_MAP_IMPL_TEST_MOVE_COPY_FIXTURE_H__
#define __ML_MAP_IMPL_TEST_MOVE_COPY_FIXTURE_H__

#include <functional>
#include <string>
#include <vector>
#include <tests/fixture/db.h>
#include <map_impl.h>

namespace ml {
namespace map_impl {
namespace test {

class Move_copy_fixture {
public:
	typedef std::function<void (Map_impl&, const std::string&,
		const std::string&) > Method_delegate;
	Move_copy_fixture(Method_delegate delegate);

	void test_should_not_add_destination_without_entry();
	void test_should_change_destination_without_entry();
	void test_should_change_destination();
	void test_source_removal(bool need_removal);
	void test_should_create_destination();
private:
	::test::Db_fixture f_db;
	Map_impl map;
	std::vector<std::string> categories;
	Method_delegate delegate;
};

} // test
} // map_impl
} // ml

#endif // __ML_MAP_IMPL_TEST_MOVE_COPY_FIXTURE_H__
