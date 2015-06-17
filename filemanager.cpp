#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>
#include <unistd.h>
#include "absolute_path.h"
#include "except.h"
#include "filesystem.h"
#include "filemanager.h"
#include "receiver_impls.h"
#include "sql_value_spec.h"
#include "tree.h"
#include "utils_assert.h"

namespace fm {

using std::back_inserter;
using std::cerr;
using std::endl;
using std::exit;
using std::for_each;
using std::invalid_argument;
using std::pair;
using std::string;
using std::transform;
using std::vector;

namespace gfs = ::fs;
using fs::find_fm_dir;
using fs::Filesystem;
using gfs::mkdir;
using gfs::rename;
using ml::Receiver_fun;
using ml::Sql_left_value_spec;
using ml::Sql_middle_value_spec;

namespace {

class Path_processor : public Path_node_handler {
public:
	Path_processor(Filesystem& filesystem, Fm_map& map_impl)
		:	fs(filesystem),
			map(map_impl) {
	}
	bool handle(const Path_node& node);
private:
	Filesystem& fs;
	Fm_map& map;
};

bool Path_processor::handle(const Path_node& node)
{
	Absolute_path path(fs.convert_fm_path(node.get_value()));
	pair<int, bool> check_result(gfs::is_dir(path));
	if (check_result.first == gfs::Not_exist) {
		if (node.is_leaf())
			map.remove(node.get_value());
		else
			map.remove(Sql_left_value_spec(node.get_value()));
	} else if (check_result.first == gfs::Success) {
		if (node.is_leaf() && check_result.second == true)
			map.remove(node.get_value());
		else if (!node.is_leaf() && check_result.second == false)
			map.remove(Sql_left_value_spec(node.get_value()));
	}
	return true;
}

} // unnamed

inline void Filemanager::map_move(const string& src, const string& dest)
{
	map->move(src, dest);
}

inline void Filemanager::map_copy(const string& src, const string& dest)
{
	map->copy(src, dest);
}

inline
void Filemanager::call_move(Move_func func, const pair<string, string>& p)
{
	func(p.first, p.second);
}

using std::bind;
using std::placeholders::_1;
using std::placeholders::_2;

inline Filemanager::Move_func Filemanager::move_func()
{
	return bind(&Filemanager::map_move, this, _1, _2);
}

inline Filemanager::Move_func Filemanager::copy_func()
{
	return bind(&Filemanager::map_copy, this, _1, _2);
}

namespace {

const char* const msg_fmdir_not_found("The .fm directory is not found.");

} // unnamed

Filemanager::Filemanager()
{
	Absolute_path fmdir;
	if (find_fm_dir(gfs::getcwd().second, 20, fmdir) == Not_found) {
		cerr << msg_fmdir_not_found << endl;
		exit(0);
	}
	init(fmdir);
}

Filemanager::Filemanager(const Absolute_path& fm_dir)
{
	pair<int, bool> r(gfs::is_dir(fm_dir));
	if (r.first != gfs::Success || r.second == false) {
		(*fmcerr) << msg_fmdir_not_found << endl;
		throw Fmdir_is_not_found();
	} else
		init(fm_dir);
}

void Filemanager::copy(const Absolute_path& src, const Absolute_path& dest)
{
	int fssrc_type(get_file_type(src));
	int fsdest_type(get_file_type(dest));
	int src_type(map->get_file_type(filesystem->get_fm_path(src)));
	int dest_type(get_map_file_type(dest));

	if (dest_type == Impossible)
		throw Invalid_destination();
	if (fssrc_type == Type_dir)
		throw File_is_a_directory();
	else if (fssrc_type == Type_file && fsdest_type == Type_file) {
		if (dest_type == Not_managed)
			;
		else if (in_sync_ff(src_type, dest_type))
			file_to_file(copy_func(), src, dest);
		else
			throw Out_of_sync();
		gfs::copy(src, dest);
	} else if (fssrc_type == Type_file && fsdest_type == Not_exist) {
		if (dest_type == Not_managed)
			;
		else if (in_sync_ffp(src_type, dest_type))
			file_to_file(copy_func(), src, dest);
		else
			throw Out_of_sync();
		gfs::copy(src, dest);
	} else if (fssrc_type == Type_file && fsdest_type == Type_dir) {
		if (dest_type == Not_managed)
			;
		else if (in_sync_fd(src_type, dest_type))
			file_to_dir(copy_func(), src, dest);
		else
			throw Out_of_sync();
		gfs::copy(src, dest.to_string() + src.basename());
	} else if (fssrc_type == Not_exist && fsdest_type == Not_exist) {
		if (in_sync_ddp(src_type, dest_type))
			dir_to_dirpath(copy_func(), src, dest);
		else if (in_sync_dd(src_type, dest_type))
			dir_to_dir(copy_func(), src, dest);
		else if (in_sync_ff(src_type, dest_type))
			file_to_file(copy_func(), src, dest);
	} else if (fssrc_type == Not_exist && fsdest_type == Type_dir) {
		if (src_type == Type_dir)
			throw File_is_a_directory();
		else if (in_sync_dd(src_type, dest_type))
			dir_to_dir(copy_func(), src, dest);
		else if (dest_type != Not_managed &&
			in_sync_fd(src_type, dest_type))
			file_to_dir(copy_func(), src, dest);
		else if (dest_type == Not_managed)
			;
		else
			throw Out_of_sync();
	} else
		throw Invalid_destination();
}

void Filemanager::copydir(const Absolute_path& src, const Absolute_path& dest)
{
	int fssrc_type(get_file_type(src));
	int fsdest_type(get_file_type(dest));
	int src_type(map->get_file_type(filesystem->get_fm_path(src)));
	int dest_type(get_map_file_type(dest));

	if (dest_type == Impossible)
		throw Invalid_destination();
	if (fssrc_type == Type_dir && fsdest_type == Type_dir) {
		if (dest_type == Not_managed)
			;
		else if (in_sync_dd(src_type, dest_type))
			dir_to_dir(copy_func(), src, dest);
		else
			throw Out_of_sync();
		gfs::copydir(src, dest.to_string() + src.basename());
	} else if (fssrc_type == Type_dir && fsdest_type == Not_exist) {
		if (dest_type == Not_managed)
			;
		else if (in_sync_ddp(src_type, dest_type))
			dir_to_dirpath(copy_func(), src, dest);
		else
			throw Out_of_sync();
		gfs::copydir(src, dest);
	} else if (fssrc_type == Type_file && fsdest_type == Type_file) {
		if (dest_type == Not_managed)
			;
		else if (in_sync_ff(src_type, dest_type))
			file_to_file(copy_func(), src, dest);
		else
			throw Out_of_sync();
		gfs::copy(src, dest);
	} else if (fssrc_type == Type_file && fsdest_type == Not_exist) {
		if (dest_type == Not_managed)
			;
		else if (in_sync_ffp(src_type, dest_type))
			file_to_file(copy_func(), src, dest);
		else
			throw Out_of_sync();
		gfs::copy(src, dest);
	} else if (fssrc_type == Type_file && fsdest_type == Type_dir) {
		if (dest_type == Not_managed)
			;
		else if (in_sync_fd(src_type, dest_type))
			file_to_dir(copy_func(), src, dest);
		else
			throw Out_of_sync();
		gfs::copy(src, dest.to_string() + src.basename());
	} else if (fssrc_type == Not_exist && fsdest_type == Not_exist) {
		if (in_sync_ddp(src_type, dest_type))
			dir_to_dirpath(copy_func(), src, dest);
		else if (in_sync_dd(src_type, dest_type))
			dir_to_dir(copy_func(), src, dest);
		else if (in_sync_ff(src_type, dest_type))
			file_to_file(copy_func(), src, dest);
	} else if (fssrc_type == Not_exist && fsdest_type == Type_dir) {
		if (dest_type == Not_managed)
			;
		else if (in_sync_dd(src_type, dest_type))
			dir_to_dir(copy_func(), src, dest);
		else if (in_sync_fd(src_type, dest_type))
			file_to_dir(copy_func(), src, dest);
		else
			throw Out_of_sync();
	} else
		throw Invalid_destination();
}

void Filemanager::move(const Absolute_path& src, const Absolute_path& dest)
{
	int fssrc_type(get_file_type(src));
	int fsdest_type(get_file_type(dest));
	int src_type(map->get_file_type(filesystem->get_fm_path(src)));
	int dest_type(get_map_file_type(dest));

	if (dest_type == Impossible)
		throw Invalid_destination();
	if (fssrc_type == Type_dir && fsdest_type == Type_dir) {
		if (dest_type == Not_managed)
			rmdir_map(src);
		else if (in_sync_dd(src_type, dest_type))
			dir_to_dir(move_func(), src, dest);
		else
			throw Out_of_sync();
		rename(src, dest.to_string() + src.basename());
	} else if (fssrc_type == Type_dir && fsdest_type == Not_exist) {
		if (dest_type == Not_managed && src_type == Type_dir)
			rmdir_map(src);
		else if (dest_type == Not_managed && src_type == Not_exist)
			;
		else if (in_sync_ddp(src_type, dest_type))
			dir_to_dirpath(move_func(), src, dest);
		else
			throw Out_of_sync();
		rename(src, dest);
	} else if (fssrc_type == Type_file && fsdest_type == Type_file) {
		if (dest_type == Not_managed && src_type == Type_file)
			map->remove(filesystem->get_fm_path(src));
		else if (dest_type == Not_managed && src_type == Not_exist)
			;
		else if (in_sync_ff(src_type, dest_type))
			file_to_file(move_func(), src, dest);
		else
			throw Out_of_sync();
		rename(src, dest);
	} else if (fssrc_type == Type_file && fsdest_type == Not_exist) {
		if (dest_type == Not_managed)
			map->remove(filesystem->get_fm_path(src));
		else if (in_sync_ffp(src_type, dest_type))
			file_to_file(move_func(), src, dest);
		else
			throw Out_of_sync();
		rename(src, dest);
	} else if (fssrc_type == Type_file && fsdest_type == Type_dir) {
		if (dest_type == Not_managed)
			map->remove(filesystem->get_fm_path(src));
		else if (in_sync_fd(src_type, dest_type))
			file_to_dir(move_func(), src, dest);
		else
			throw Out_of_sync();
		rename(src, dest.to_string() + src.basename());
	} else if (fssrc_type == Not_exist && fsdest_type == Not_exist) {
		if (in_sync_ddp(src_type, dest_type))
			dir_to_dirpath(move_func(), src, dest);
		else if (in_sync_dd(src_type, dest_type))
			dir_to_dir(move_func(), src, dest);
		else if (in_sync_ff(src_type, dest_type))
			file_to_file(move_func(), src, dest);
	} else if (fssrc_type == Not_exist && fsdest_type == Type_dir) {
		if (dest_type == Not_managed && src_type == Type_file)
			map->remove(filesystem->get_fm_path(src));
		else if (dest_type == Not_managed && src_type == Type_dir)
			rmdir_map(src);
		else if (in_sync_dd(src_type, dest_type))
			dir_to_dir(move_func(), src, dest);
		else if (in_sync_fd(src_type, dest_type))
			file_to_dir(move_func(), src, dest);
		else
			throw Out_of_sync();
	} else
		throw Invalid_destination();
}

void Filemanager::refresh()
{
	vector<string> values;
	auto inserter(back_inserter(values));
	auto receiver(Receiver_fun(inserter));
	map->get_values(Sql_middle_value_spec(""), receiver);
	Path_tree tree;
	auto handler(Path_processor(*filesystem, *map));
	for_each(values.begin(), values.end(),
		bind(&Path_tree::insert, &tree, _1));
	tree.walk(handler);
}

int Filemanager::remove(const Absolute_path& target)
{
	int type(map->get_file_type(filesystem->get_fm_path(target)));
	if (type == Type_file) {
		map->remove(filesystem->get_fm_path(target));
	}
	int r(std::remove(target.to_filepath_string().c_str()));
	if (r == 0 || errno == ENOENT) {
		if (type == Type_dir) {
			Sql_left_value_spec spec(
				filesystem->get_fm_path(target) + "/");
			map->remove(spec);
		}
	}
	if (r == 0) {
		return Success;
	} else if (errno == ENOENT) {
		return Success;
	} else {
		return Not_empty;
	}
}

int Filemanager::rmdir(const Absolute_path& target)
{
	rmdir_map(target);
	if (gfs::rmdirtree(target) == gfs::Success)
		return Success;
	else
		return Failure;
}

void Filemanager::init(const Absolute_path& fm_dir)
{
	root = fm_dir.parent();
	filesystem.reset(new Filesystem(fm_dir));
	map.reset(new Fm_map_impl(filesystem->get_dbfilepath()));
}

bool Filemanager::in_sync_dd(int src, int dest)
{
	if (src == Not_exist && dest == Not_exist)
		return true;
	else if (src == Not_exist && dest == Type_dir)
		return true;
	else if (src == Type_dir && dest == Not_exist)
		return true;
	else if (src == Type_dir && dest == Type_dir)
		return true;
	else
		return false;
}

bool Filemanager::in_sync_ddp(int src, int dest)
{
	if (src == Not_exist && dest == Not_exist)
		return true;
	else if (src == Type_dir && dest == Not_exist)
		return true;
	else
		return false;
}

bool Filemanager::in_sync_ff(int src, int dest)
{
	if (src == Not_exist && dest == Not_exist)
		return true;
	else if (src == Not_exist && dest == Type_file)
		return true;
	else if (src == Type_file && dest == Not_exist)
		return true;
	else if (src == Type_file && dest == Type_file)
		return true;
	else
		return false;
}

bool Filemanager::in_sync_ffp(int src, int dest)
{
	if (src == Not_exist && dest == Not_exist)
		return true;
	else if (src == Type_file && dest == Not_exist)
		return true;
	else
		return false;
}

bool Filemanager::in_sync_fd(int src, int dest)
{
	if (src == Not_exist && dest == Not_exist)
		return true;
	else if (src == Not_exist && dest == Type_dir)
		return true;
	else if (src == Type_file && dest == Not_exist)
		return true;
	else if (src == Type_file && dest == Type_dir)
		return true;
	else
		return false;
}

int Filemanager::get_file_type(const Absolute_path& path)
{
	using gfs::Success;
	pair<int, bool> cr(gfs::is_dir(path));
	if (cr.first != Success)
		return Not_exist;
	else if (cr.second == true)
		return Type_dir;
	else
		return Type_file;
}

int Filemanager::get_map_file_type(const Absolute_path& path)
{
	if (root.to_string() == path.to_string())
		return map->get_file_type(filesystem->get_fm_path(path));
	else if (filesystem->is_descendant(path))
		return map->get_file_type(filesystem->get_fm_path(path));
	else
		return Not_managed;
}

bool Filemanager::is_type_dir_to_dir(int src_type, int dest_type)
{
	return src_type == Type_dir && dest_type == Type_dir;
}

bool Filemanager::is_type_dir_to_dirpath(int src_type, int dest_type)
{
	return src_type == Type_dir && dest_type == Not_exist;
}

bool Filemanager::is_type_file_to_file(int src_type, int dest_type)
{
	return src_type == Type_file && dest_type == Type_file;
}

bool Filemanager::is_type_file_to_filepath(int src_type, int dest_type)
{
	return src_type == Type_file && dest_type == Not_exist;
}

namespace {

class Make_move_pair {
public:
	Make_move_pair(const string& src_dir, const string& dest_dir)
		:	src_dir(src_dir), dest_dir(dest_dir) {
	}
	pair<string, string> operator()(const string& in) const {
		string tmp(in.substr(src_dir.length()));
		if (dest_dir.empty())
			return make_pair(in, tmp);
		else
			return make_pair(in, dest_dir + "/" + tmp);
	}
private:
	const string& src_dir;
	const string& dest_dir;
};

} // unnamed

void Filemanager::dir_to_dir(Move_func f, const Absolute_path& src,
	const Absolute_path& dest)
{
	string src_fm_path(filesystem->get_fm_path(src));
	vector<string> values;
	auto inserter(back_inserter(values));
	auto receiver(Receiver_fun(inserter));
	map->get_values(Sql_left_value_spec(src_fm_path + "/"), receiver);

	vector<pair<string, string> > src_dest;
	string src_root(src_fm_path.substr(0, src_fm_path.find_last_of('/')));
	if (src_root == src_fm_path)
		src_root = "";
	else
		src_root += "/";
	string src_base(src_fm_path.substr(src_root.length()));
	string dest_base(filesystem->get_fm_path(dest));
	string dest_path;
	if (dest_base.empty())
		dest_path = src_base;
	else
		dest_path = dest_base + "/" + src_base;
	if (src_fm_path.find(dest_path) == 0)
		throw Invalid_destination();
	transform(values.begin(), values.end(), back_inserter(src_dest),
		Make_move_pair(src_root, dest_base));
	for_each(src_dest.begin(), src_dest.end(), bind(call_move, f, _1));
}

void Filemanager::dir_to_dirpath(Move_func f, const Absolute_path& src,
	const Absolute_path& dest)
{
	string src_fm_path(filesystem->get_fm_path(src));
	vector<string> values;
	auto inserter(back_inserter(values));
	auto receiver(Receiver_fun(inserter));
	map->get_values(Sql_left_value_spec(src_fm_path + "/"), receiver);

	vector<pair<string, string> > src_dest;
	string src_root(src_fm_path);
	if (src_root != "")
		src_root += "/";
	string dest_path(filesystem->get_fm_path(dest));
	if (src_fm_path.find(dest_path) == 0)
		throw Invalid_destination();
	transform(values.begin(), values.end(), back_inserter(src_dest),
		Make_move_pair(src_root, dest_path));
	for_each(src_dest.begin(), src_dest.end(), bind(call_move, f, _1));
}

void Filemanager::file_to_dir(Move_func f, const Absolute_path& src,
	const Absolute_path& dest)
{
	int dest_type(map->get_file_type(filesystem->get_fm_path(
		dest.to_string() + src.basename())));
	if (dest_type == Type_file || dest_type == Not_exist)
		file_to_file(f, src, dest.to_string() + src.basename());
	else
		throw Invalid_destination();
}

void Filemanager::file_to_file(Move_func f, const Absolute_path& src,
	const Absolute_path& dest)
{
	f(filesystem->get_fm_path(src), filesystem->get_fm_path(dest));
}

void Filemanager::rmdir_map(const Absolute_path& path)
{
	int type(map->get_file_type(filesystem->get_fm_path(path)));
	if (type == Type_file) {
		map->remove(filesystem->get_fm_path(path));
	} else {
		Sql_left_value_spec spec(filesystem->get_fm_path(path) + "/");
		map->remove(spec);
	}
}

} // fm
