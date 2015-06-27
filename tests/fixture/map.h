#ifndef __ML_TEST_FIXTURE_MISC_H__
#define __ML_TEST_FIXTURE_MISC_H__

#include <string>
#include <vector>

namespace ml {
	class Map;
namespace test {
namespace fixture {

extern std::vector<std::string> get_values(Map& map,
	const std::string& category);

extern void set_category(Map& map, const std::string& target,
	const std::string& category);

extern void set_category(Map& map, const std::string& target,
	const std::string& category1, const std::string& category2);

extern void set_category(Map& map, const std::string& target,
	const std::string& category1, const std::string& category2,
	const std::string& category3);

} // fixture
} // test
} // ml

#endif // __ML_TEST_FIXTURE_MISC_H__
