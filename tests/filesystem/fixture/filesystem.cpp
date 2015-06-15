#include <cstdlib>
#include <fstream>
#include <ios>
#include <limits>
#include <sys/types.h>
#include <sys/stat.h>
#include <filesystem.h>
#include "filesystem.h"

namespace fs {
namespace test {
namespace fixture {

using std::numeric_limits;
using std::vector;

vector<char> dummy_content()
{
	vector<char> content;
	typedef vector<char>::size_type Size;
	for (Size i = 0; i < 1024 * 1024; ++i)
		content.push_back(i % (numeric_limits<char>::max() + 1));
	return content;
}

} // fixture
} // test
} // fs
