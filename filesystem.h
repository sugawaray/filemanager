#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include <cerrno>
#include <string>
#include <utility>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "absolute_path.h"
#include "helper.h"

namespace fs {

enum Result {
	Success,	Not_exist,	Failure,	Directory
};

/* return an absolute path for the specified path. The current working
   directory is determined by the specified function. */
extern Absolute_path get_abs_path(const std::string& path,
	char* (* getcwdfn)(char*, std::size_t) = getcwd);

/* return a pair whose first item is result.
   The second item of it is the current working directory path when the result
   is true. */
extern std::pair<bool, Absolute_path> getcwd(
	char* (*fn)(char*, std::size_t) = getcwd);

/* generate an empty file at the give absolute path.
   return 0 at success or return -1 at failure.*/
extern int create_emptyfile(const Absolute_path& path);

/* create a directory at the given absolute path. 
   create intermediate directories if necessary.
   return a result value as if calling posix mkdir with the path. */
extern int mkdir(const Absolute_path& path);

/* removes an entire specified directory tree.

   returns Success when it remove the directory.
   returns Failure when it failed to remove the directory.
   returns Not_exist when the specified directory does not exist. */
extern int rmdirtree(const Absolute_path& path);

/* checks whether the file is a directory given it's absolute path.
   returns Success as the first element if it could determine it's file
   type. returns true as the second element when the file is a directory or
   false when it isn't a directory. returns Not_exist as the first element
   when it couldn't determine the file type because the file doesn't exist. */
extern std::pair<int, bool> is_dir(const Absolute_path& path);

/* moves the source file to the destination path. This function works almost
   the same as the posix rename(). The difference between this function and
   the posix rename() is that this function creates intermediate directories
   when needed. This function may not remove the intermediate directories it 
   has created when moving the file has failed by the other causes after
   it has created the intermediate directories. */
extern int rename(const Absolute_path& source,
	const Absolute_path& destination);

extern std::pair<int, mode_t> get_mode(int fd);
extern std::pair<int, mode_t> get_mode(const char* path);
extern mode_t get_umask();

class Io {
public:
	int open(const char* path, int oflag) {
		return ::open(path, oflag);
	}

	int open(const char* path, int oflag, mode_t mode) {
		return ::open(path, oflag, mode);
	}

	int close(int fd) {
		return ::close(fd);
	}

	ssize_t read(int fd, void* buf, size_t nbytes) {
		return ::read(fd, buf, nbytes);
	}

	ssize_t write(int fd, const void *buf, size_t nbytes) {
		return ::write(fd, buf, nbytes);
	}

	std::pair<int, mode_t> get_mode(int fd) {
		return fs::get_mode(fd);
	}

	std::pair<int, mode_t> get_mode(const char* path) {
		return fs::get_mode(path);
	}

	mode_t umask() {
		return fs::get_umask();
	}
};

template<class T>
int write_to_fd(T& io, int fd, const char* buf, ssize_t size)
{
	const char* p(buf);
	ssize_t sw = io.write(fd, p, size);
	if (sw == -1)
		return Failure;
	for (p += sw; p < buf + size; p += sw) {
		sw = io.write(fd, p, size);
		if (sw == -1)
			break;
	}
	if (sw == -1)
		return Failure;
	else
		return 0;
}

template<class T>
int copyfd(T& io, int src, int dest)
{
	char buf[8192];
	int r(0);
	ssize_t sr = io.read(src, buf, sizeof array_size(buf));
	for ( ; sr > 0; ) {
		if (write_to_fd(io, dest, buf, sr) != 0)
			return Failure;
		sr = io.read(src, buf, sizeof array_size(buf));
	}
	if (sr == 0)
		r = 0;
	else
		r = Failure;
	return r;
}

template<class T>
int copy(T& io, const char* src, const char* dest)
{
	int sfd(io.open(src, O_RDONLY));
	if (sfd == -1)
		return Failure;
	int r(Failure);
	std::pair<int, mode_t> mode(io.get_mode(sfd));
	if (mode.first == Success && S_ISDIR(mode.second))
		r = Directory;
	else if (mode.first == Success) {
		int dfd(io.open(dest, O_WRONLY | O_CREAT | O_TRUNC,
			mode.second & ~io.umask()));
		if (dfd != -1) {
			r = copyfd(io, sfd, dfd);
			io.close(dfd);
		}
	}
	if (sfd >= 0)
		io.close(sfd);
	return r;
}

extern int copy(const Absolute_path& src, const Absolute_path& dest);

template<class T> int copydir(const Absolute_path&, const Absolute_path&, T&);

inline int copydir(const Absolute_path& src, const Absolute_path& dest)
{
	Io io;
	return copydir(src, dest, io);
}

/* Fix me.
   I do not know how to test that this function closes a DIR object. */
template<class T>
int copydir(const Absolute_path& src, const Absolute_path& dest, T& io)
{
	using std::pair;
	using std::string;

	string srcpath(src.to_filepath_string());
	DIR *dir(opendir(srcpath.c_str()));
	if (dir == 0)
		return Failure;
	pair<int, mode_t> mode(io.get_mode(srcpath.c_str()));
	if (mode.first != Success) {
		closedir(dir);
		return Failure;
	}
	mkdir(dest);
	string destpath(dest.to_filepath_string());
	chmod(destpath.c_str(), mode.second);
	for (struct dirent* entry = readdir(dir); entry != 0;
		entry = readdir(dir)) {
		if (string(".") == entry->d_name)
			continue;
		if (string("..") == entry->d_name)
			continue;
		Absolute_path src_filepath(src.to_string() + entry->d_name);
		Absolute_path dest_filepath(dest.to_string() + entry->d_name);
		pair<int, bool> tmp(is_dir(src_filepath));
		if (tmp.second)
			copydir(src_filepath, dest_filepath);
		else
			copy(src_filepath, dest_filepath);
	}
	closedir(dir);
	return 0;
}

} // fs

#endif // __FILESYSTEM_H__
