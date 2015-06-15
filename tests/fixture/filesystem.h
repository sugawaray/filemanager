#ifndef __TEST_FIXTURE_FILESYSTEM_H__
#define __TEST_FIXTURE_FILESYSTEM_H__

#include <string>
#include <vector>

namespace test {
namespace fixture {

extern int get_stat_result(const std::string& path);

extern void write_to_file(const std::string& path,
	const std::vector<char>& content);

extern void read_file(const std::string& path, std::vector<char>& buffer);

extern bool equal_files(const std::string& op1, const std::string& op2);

} // fixture
} // test

#endif // __TEST_FIXTURE_FILESYSTEM_H__
