#ifndef __FS_TEST_FIXTURE_FILESYSTEM_H__
#define __FS_TEST_FIXTURE_FILESYSTEM_H__

#include <string>
#include <vector>
#include <sys/types.h>
#include <filesystem.h>

namespace fs {
namespace test {
namespace fixture {

extern std::vector<char> dummy_content();

class Stub_io_stat_failure : public Io {
public:
	std::pair<int, mode_t> get_mode(int fd) {
		return std::make_pair(Failure, 0);
	}

	std::pair<int, mode_t> get_mode(const char* path) {
		return std::make_pair(Failure, 0);
	}
};

} // fixture
} // test
} // fs

#endif // __FS_TEST_FIXTURE_FILESYSTEM_H__
