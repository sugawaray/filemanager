#include <cerrno>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <filesystem.h>
#include <tests/fixture/filesystem.h>
#include <tests/fixture/process_dir.h>
#include "copy.h"
#include "fixture/filesystem.h"

using std::logic_error;
using fs::copy;
using fs::Failure;

namespace {

using std::strcmp;
using std::make_pair;
using std::pair;
using fs::get_umask;
using fs::Io;

const char* const src_file = "s";
const char* const dest_file = "d";

class Stub_io {
public:

	Stub_io()
	:	src_open_result(0),
		src_open_errno(0),
		dest_open_result(0),
		dest_open_errno(0),
		read_result(0),
		read_errno(0),
		read_success_count(10),
		read_complete_count(10),
		write_result(0),
		write_errno(0),
		write_success_count(10) {
	}

	void set_open_result(const char* file, int result, int errno_value) {
		if (strcmp(file, src_file) == 0) {
			src_open_result = result;
			src_open_errno = errno_value;
		} else if (strcmp(file, dest_file) == 0) {
			dest_open_result = result;
			dest_open_errno = errno_value;
		} else {
			throw logic_error("test");
		}
	}

	void set_read_complete_count(int count) {
		read_complete_count = count;
	}

	void set_read_result(int success_count, int result, int errno_value) {
		read_success_count = success_count;
		read_result = result;
		read_errno = errno_value;
	}

	void set_write_result(int success_count, int result, int errno_value) {
		write_success_count = success_count;
		write_result = result;
		write_errno = errno_value;
	}

	int open(const char* path, int oflag) {
		return open(path, oflag, 0);
	}

	int open(const char* path, int oflag, mode_t mode) {
		if (strcmp(path, src_file) == 0) {
			errno = src_open_errno;
			return src_open_result;
		} else if (strcmp(path, dest_file) == 0) {
			errno = dest_open_errno;
			return dest_open_result;
		} else {
			throw logic_error("test");
		}
	}

	ssize_t read(int fd, void* buffer, size_t nbytes) {
		if (read_complete_count <= 0)
			return 0;
		--read_complete_count;
		if (read_success_count-- > 0)
			return nbytes;
		errno = read_errno;
		return read_result;
	}

	ssize_t write(int fd, const void* buffer, size_t nbytes) {
		ssize_t r;
		if (write_success_count == 0) {
			errno = write_errno;
			r = write_result;
		} else {
			r = nbytes;
		}
		--write_success_count;
		return r;
	}

	int close(int fd) {
		return 0;
	}

	pair<int, mode_t> get_mode(int fd) {
		return make_pair(0, S_IRUSR);
	}

	mode_t umask() {
		return get_umask();
	}
private:
	int src_open_result;
	int src_open_errno;
	int dest_open_result;
	int dest_open_errno;
	int read_result;
	int read_errno;
	int read_success_count;
	int read_complete_count;
	int write_result;
	int write_errno;
	int write_success_count;
};

class Stub_io_read_failure : public Io {
public:
	Stub_io_read_failure(int success_count, int result, int errno_value)
		:	count(success_count),
			result(result),
			err(errno_value) {
	}

	ssize_t read(int fd, void* buffer, size_t size) {
		if (count == 0) {
			errno = err;
			return result;
		} else {
			--count;
			return Io::read(fd, buffer, size);
		}
	}
private:
	int count;
	int result;
	int err;
};

class Stub_io_umask : public Io {
public:
	explicit Stub_io_umask(mode_t mask)
		:	mask(mask) {
	}

	mode_t umask() {
		return mask;
	}
private:
	mode_t mask;
};

int do_copy(Stub_io& io)
{
	return copy(io, src_file, dest_file);
}

} // unnamed


START_TEST(should_return_failure_when_src_file_open_failed)
{
	Stub_io io;
	io.set_open_result(src_file, -1, EACCES);
	int r(do_copy(io));
	fail_unless(r == Failure , "result value");
}
END_TEST

START_TEST(should_return_failure_when_dest_file_open_failed)
{
	Stub_io io;
	io.set_open_result(dest_file, -1, EACCES);
	int r(do_copy(io));
	fail_unless(r == Failure, "result value");
}
END_TEST

START_TEST(should_return_failure_when_file_write_failed)
{
	Stub_io io;
	io.set_write_result(0, -1, ETIMEDOUT);
	int r(do_copy(io));
	fail_unless(r == Failure, "result value");
}
END_TEST

START_TEST(should_return_failure_when_successive_file_write_failed)
{
	Stub_io io;
	io.set_write_result(1, -1, ETIMEDOUT);
	int r(do_copy(io));
	fail_unless(r == Failure, "result value");
}
END_TEST

namespace fs {
namespace test {

TCase* create_copy_error_tcase()
{
	TCase* tcase(tcase_create("fs::copy error"));
	tcase_add_test(tcase, should_return_failure_when_src_file_open_failed);
	tcase_add_test(tcase,
		should_return_failure_when_dest_file_open_failed);
	tcase_add_test(tcase, should_return_failure_when_file_write_failed);
	tcase_add_test(tcase,
		should_return_failure_when_successive_file_write_failed);
	return tcase;
}

} // test
} // fs

using std::equal;
using std::random_shuffle;
using std::string;
using std::vector;
using test::Process_dir_fixture;
using fs::create_emptyfile;
using fs::Io;
using fs::test::fixture::dummy_content;
using ::test::fixture::read_file;
using ::test::fixture::write_to_file;

START_TEST(should_copy_content_without_destination_file)
{
	Process_dir_fixture f;
	string src(f.get_path() + "/src");
	create_emptyfile(src);
	vector<char> content(dummy_content());
	write_to_file(src, content);

	Io io;
	string dest(f.get_path() + "/dest");
	int r(copy(io, src.c_str(), dest.c_str()));

	fail_unless(r == 0, "result value");
	vector<char> copied;
	read_file(dest, copied);
	fail_unless(content.size() == copied.size(), "size");
	fail_unless(equal(content.begin(), content.end(), copied.begin()),
		"content");
}
END_TEST

START_TEST(should_overwrite_destination_file)
{
	Process_dir_fixture f;
	string src(f.get_path() + "/src");
	create_emptyfile(src);
	vector<char> content(dummy_content());
	write_to_file(src, content);
	vector<char> dest_content(content);
	dest_content.insert(dest_content.end(), content.begin(), content.end());
	random_shuffle(dest_content.begin(), dest_content.end());
	string dest(f.get_path() + "/dest");
	write_to_file(dest, dest_content);

	Io io;
	int r(copy(io, src.c_str(), dest.c_str()));

	fail_unless(r == 0, "result value");
	vector<char> copied;
	read_file(dest, copied);
	fail_unless(content.size() == copied.size(), "size");
	fail_unless(equal(content.begin(), content.end(), copied.begin()),
		"value");
}
END_TEST

START_TEST(should_return_failure_when_the_read_failed)
{
	Process_dir_fixture f;

	string src(f.get_path() + "/src");
	create_emptyfile(src);
	write_to_file(src, dummy_content());

	string dest(f.get_path() + "/dest");

	Stub_io_read_failure io(0, -1, ETIMEDOUT);
	int r(copy(io, src.c_str(), dest.c_str()));

	fail_unless(r == Failure, "result value");
}
END_TEST

START_TEST(should_return_failure_when_the_next_read_failed)
{
	Process_dir_fixture f;

	string src(f.get_path() + "/src");
	create_emptyfile(src);
	write_to_file(src, dummy_content());

	string dest(f.get_path() + "/dest");

	Stub_io_read_failure io(1, -1, ETIMEDOUT);
	int r(copy(io, src.c_str(), dest.c_str()));

	fail_unless(r == Failure, "result value");
}
END_TEST

START_TEST(should_copy_source_mode)
{
	Process_dir_fixture f;

	string src(f.get_path() + "/src");
	create_emptyfile(src);
	write_to_file(src, dummy_content());
	chmod(src.c_str(), S_IRUSR);

	string dest(f.get_path() + "/dest");

	Io io;
	copy(io, src.c_str(), dest.c_str());

	struct stat stat_obj = { 0 };
	stat(dest.c_str(), &stat_obj);
	mode_t all(S_IRWXU | S_IRWXG | S_IRWXO);
	fail_unless((stat_obj.st_mode & all) == S_IRUSR, "value");
}
END_TEST

START_TEST(should_return_failure_when_src_file_stat_failed)
{
	using fs::test::fixture::Stub_io_stat_failure;

	Process_dir_fixture f;
	string src(f.get_path() + "/src");
	create_emptyfile(src);
	write_to_file(src, dummy_content());

	string dest(f.get_path() + "/dest");
	Stub_io_stat_failure io;
	int r(copy(io, src.c_str(), dest.c_str()));

	fail_unless(r == Failure, "code");
}
END_TEST

START_TEST(umask_should_influence_mode)
{
	using fs::get_mode;

	Process_dir_fixture f;
	string src(f.get_path() + "/src");
	create_emptyfile(src);
	write_to_file(src, dummy_content());
	chmod(src.c_str(), S_IRWXU | S_IRWXG);

	string dest(f.get_path() + "/dest");
	Stub_io_umask io(S_IWUSR | S_IXUSR | S_IRWXG);
	copy(io, src.c_str(), dest.c_str());
	int fd(open(dest.c_str(), O_RDONLY));
	fail_unless((get_mode(fd).second & (S_IRWXU | S_IRWXG)) == S_IRUSR,
		"value");
}
END_TEST

namespace fs {
namespace test {

TCase* create_copy_tcase()
{
	TCase* tcase(tcase_create("fs::copy"));
	tcase_add_test(tcase, should_copy_content_without_destination_file);
	tcase_add_test(tcase, should_overwrite_destination_file);
	tcase_add_test(tcase, should_return_failure_when_the_read_failed);
	tcase_add_test(tcase, should_return_failure_when_the_next_read_failed);
	tcase_add_test(tcase, should_copy_source_mode);
	tcase_add_test(tcase, should_return_failure_when_src_file_stat_failed);
	tcase_add_test(tcase, umask_should_influence_mode);
	return tcase;
}

} // test
} // fs

#include <nomagic.h>
#include <absolute_path.h>
#include <tests/fixture/assert.h>

namespace fs {
namespace test {
namespace {

template<class T, class U, class V>
inline void A(T t, U u, V v)
{
	::test::fixture::Assert(t, u, v);
}

typedef Process_dir_fixture F;

void t1()
{
	F f;

	Absolute_path src(f.get_path() + "/src1");
	mkdir(src);

	Absolute_path dest(f.get_path() + "/dest2");
	mkdir(dest);

	A(copy(src, dest) == Directory, __FILE__, __LINE__);
}

} // unnamed

void run_copy_tests()
{
	using nomagic::run;

	run("copy: It is an error if the source is a directory.", t1);
}

} // test
} // fs
