#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <unistd.h>
#include "absolute_path.h"
#include "except.h"
#include "filesystem.h"
#include "filemanager.h"
#include "fm_commands.h"
#include "fm_filesystem.h"
#include "fm_map_impl.h"
#include "utils_assert.h"

using namespace std;
using fm::Filemanager;
using fm::Invalid_destination;
using fm::Out_of_sync;
using fs::get_abs_path;

namespace fm {

std::ostream* fmcout(&std::cout);
std::ostream* fmcerr(&std::cerr);

namespace {

void usage()
{
	cerr << "Usage: fm-cp source destination" << endl;
}

class Cp_file {
public:
	Cp_file(Filemanager& manager, const string& dest)
		:	manager(manager), dest(dest) {
	}

	void operator()(const string& path) const {
		manager.copy(get_abs_path(path), get_abs_path(dest));
	}

private:
	Filemanager& manager;
	string dest;
};

class Cp_dir {
public:
	Cp_dir(Filemanager& manager, const string& dest)
		:	manager(manager), dest(dest) {
	}

	void operator()(const string& path) const {
		manager.copydir(get_abs_path(path), get_abs_path(dest));
	}
private:
	Filemanager& manager;
	string dest;
};

} // unnamed

using std::unique_ptr;

class Command {
public:
	Command()
		:	valid(false), base_specified(false) {
	}

	static void process_arguments(Command& command, int argc, char* argv[]);

	bool is_valid() const {
		return valid;
	}

	unique_ptr<Filemanager> get_filemanager() const;

protected:
	virtual string get_extra_opts() const {
		return "";
	}

	virtual void procopt(int c, const char* arg) {
	}

	virtual void procremain(int argc, char* argv[], int index) {
		valid = true;
	}

	bool valid;
private:
	Absolute_path get_fmdir() const {
		return get_abs_path(base).child(".fm");
	}

	bool base_specified;
	string base;
};

void Command::process_arguments(Command& command, int argc, char* argv[])
{
	int c;
	opterr = 0;
	string opts(":b:");
	opts += command.get_extra_opts();
	while ((c = getopt(argc, argv, opts.c_str())) != -1) {
		switch (c) {
		case 'b':
			command.base_specified = true;
			command.base.assign(optarg);
			break;
		case ':':
			(*fmcerr) << "option without an option value." << endl;
			return;
		}
		command.procopt(c, optarg);
	}
	command.procremain(argc, argv, optind);
}

unique_ptr<Filemanager> Command::get_filemanager() const
{
	unique_ptr<Filemanager> r;
	try {
		if (base_specified) {
			r.reset(new Filemanager(get_fmdir()));
		} else {
			r.reset(new Filemanager());
		}
		return r;
	}
	catch (const Fmdir_is_not_found&) {
		(*fmcerr) << "invalid arguments" << endl;
		return r;
	}
}

class Recursive_impl {
public:
	Recursive_impl() : recursive(false) {
	}

	bool need_recur() {
		return recursive;
	}

	string get_extra_opts() const {
		return "R";
	}

	bool process(int c, const char* arg) {
		if (c == 'R') {
			recursive = true;
			return true;
		} else
			return false;
	}

private:
	bool recursive;
};

namespace {

inline string convert(const string& o)
{
	return o;
}

} // unnamed

class Cp_command : public Command, private Recursive_impl {
public:
	static Cp_command process_arguments(int argc, char* argv[]) {
		Cp_command r;
		Command::process_arguments(r, argc, argv);
		return r;
	}

	using Recursive_impl::need_recur;

	const string& get_dest() const {
		return dest;
	}

	const vector<string>& get_src_list() const {
		return srclist;
	}

protected:
	string get_extra_opts() const {
		return Recursive_impl::get_extra_opts();
	}

	void procopt(int c, const char* arg) {
		process(c, arg);
	}

	void procremain(int argc, char* argv[], int index) {
		if (argc - index < 2) {
			(*fmcerr) << "invalid arguments" << endl;
			return;
		}
		transform(argv + index, argv + argc - 1,
			back_inserter(srclist), convert);
		dest.assign(argv[argc - 1]);
		valid = true;
	}

private:
	Cp_command() {
	}

	vector<string> srclist;
	string dest;
};

int cp(int argc, char* argv[])
{
	if (argc < 3) {
		usage();
		return 1;
	}
	Cp_command command(Cp_command::process_arguments(argc, argv));
	if (!command.is_valid())
		return 1;
	unique_ptr<Filemanager> manager(command.get_filemanager());
	if (!manager)
		return 1;
	try {
		auto& srclist(command.get_src_list());
		auto& t(command.get_dest());
		if (command.need_recur()) {
			for_each(srclist.begin(), srclist.end(),
				Cp_dir(*manager, t));
		} else {
			for_each(srclist.begin(), srclist.end(),
				Cp_file(*manager, t));
		}
		return 0;
	}
	catch (const Out_of_sync&) {
		cerr << "out of sync" << endl;
		return 1;
	}
	catch (const Invalid_destination&) {
		cerr << "invalid destination" << endl;
		return 1;
	}
}

namespace {

class Rm_file {
public:
	Rm_file(Filemanager& manager)
		:	manager(manager) {
	}

	void operator()(const string& path) {
		using std::endl;
		using std::pair;
		namespace gfs = ::fs;
		using gfs::is_dir;

		pair<int, bool> r(is_dir(get_abs_path(path)));
		if (r.first == gfs::Success && r.second == true) {
			(*fmcerr) << "target file is a directory." << endl;
			return;
		}
		manager.remove(get_abs_path(path));
	}
private:
	Filemanager& manager;
};

class Rm_dir {
public:
	Rm_dir(Filemanager& manager)
		:	manager(manager) {
	}

	void operator()(const string& path) const {
		manager.rmdir(get_abs_path(path));
	}

private:
	Filemanager& manager;
};

} // unnamed

class Rm_command : public Command, private Recursive_impl {
public:
	static Rm_command process_arguments(int argc, char* argv[]) {
		Rm_command r;
		Command::process_arguments(r, argc, argv);
		return r;
	}

	using Recursive_impl::need_recur;

	const vector<string>& get_targets() const {
		return targets;
	}

protected:
	string get_extra_opts() const {
		return Recursive_impl::get_extra_opts();
	}

	void procopt(int c, const char* arg) {
		process(c, arg);
	}

	void procremain(int argc, char* argv[], int index) {
		if (argc - index < 1)
			return;
		transform(argv + index, argv + argc, back_inserter(targets),
			convert);
		valid = true;
	}
private:
	Rm_command() {
	}

	vector<string> targets;
};

int rm(int argc, char* argv[])
{
	Rm_command command(Rm_command::process_arguments(argc, argv));
	if (!command.is_valid())
		return 1;
	unique_ptr<Filemanager> manager(command.get_filemanager());
	if (!manager)
		return 1;
	auto& t(command.get_targets());
	if (!command.need_recur())
		for_each(t.begin(), t.end(), Rm_file(*manager));
	else
		for_each(t.begin(), t.end(), Rm_dir(*manager));
	return 0;
}

class Getcat_command : public Command {
public:
	static Getcat_command process_arguments(int argc, char* argv[]) {
		Getcat_command r;
		Command::process_arguments(r, argc, argv);
		return r;
	}

	bool has_target() const {
		return target;
	}

protected:
	void procremain(int argc, char* argv[], int index) {
		if (argc - index > 1)
			return;
		if (argc - index == 1)
			target = true;
		valid = true;
	}

private:
	Getcat_command()	:	target(false) {
	}

	bool target;
};

int getcat(int argc, char* argv[])
{
	Getcat_command command(Getcat_command::process_arguments(argc, argv));
	if (!command.is_valid())
		return 1;
	unique_ptr<Filemanager> manager(command.get_filemanager());
	if (!manager)
		return 1;
	vector<string> categories;
	auto& map(manager->get_map());
	if (!command.has_target())
		map.get_categories(back_inserter(categories));
	else {
		auto& fsref(manager->get_filesystem());
		if (fsref.is_descendant(get_abs_path(argv[1]))) {
			string target(manager->get_filesystem().get_fm_path(
				get_abs_path(argv[1])));
			map.get_categories(target, back_inserter(categories));
		} else {
			(*fmcerr) << "The file is not managed." << endl;
			return 1;
		}
	}
	copy(categories.begin(), categories.end(),
		ostream_iterator<string>(cout, "\n"));
	return 0;
}

class Mv_command : public Command {
public:
	static Mv_command process_arguments(int argc, char* argv[]) {
		Mv_command r;
		Command::process_arguments(r, argc, argv);
		return r;
	}

	const vector<string>& get_src_list() const {
		return srclist;
	}

	const string& get_dest() const {
		return dest;
	}

protected:
	void procremain(int argc, char* argv[], int index) {
		if (argc - index < 2)
			return;
		transform(argv + index, argv + argc - 1,
			back_inserter(srclist), convert);
		dest.assign(argv[argc - 1]);
		valid = true;
	}

private:
	Mv_command() {
	}

	vector<string> srclist;
	string dest;
};

namespace {

class Mv_func {
public:
	Mv_func(Filemanager& manager, const string& dest)
		:	m(manager), d(dest) {
	}

	void operator()(const string& s) const {
		m.move(get_abs_path(s), get_abs_path(d));
	}
private:
	Filemanager& m;
	const string& d;
};

} // unnamed

int mv(int argc, char* argv[])
{
	Mv_command command(Mv_command::process_arguments(argc, argv));
	if (!command.is_valid())
		return 1;
	unique_ptr<Filemanager> manager(command.get_filemanager());
	if (!manager)
		return 1;
	try {
		auto& srclist(command.get_src_list());
		for_each(srclist.begin(), srclist.end(),
			Mv_func(*manager, command.get_dest()));
		return 0;
	}
	catch (const Invalid_destination&) {
		cerr << "invalid dest path" << endl;
		return 1;
	}
	catch (const Out_of_sync&) {
		cerr << "out of sync" << endl;
		return 2;
	}
}

int refresh(int argc, char* argv[])
{
	Command c;
	Command::process_arguments(c, argc, argv);
	if (!c.is_valid())
		return 1;
	unique_ptr<Filemanager> manager(c.get_filemanager());
	if (!manager)
		return 1;
	manager->refresh();
	return 0;
}

} // fm

namespace fm {

namespace {

class Set_command : public Command {
public:
	static Set_command process_arguments(int argc, char* argv[]);

	const string& get_target() const {
		return target;
	}

	const vector<string>& get_categories() const {
		return cats;
	}

protected:
	void procremain(int argc, char* argv[], int index);

private:
	Set_command() {
	}

	string target;
	vector<string> cats;
};

Set_command Set_command::process_arguments(int argc, char* argv[])
{
	Set_command r;
	Command::process_arguments(r, argc, argv);
	return r;
}

void Set_command::procremain(int argc, char* argv[], int index)
{
	if (argc - index < 2) {
		(*fmcerr) << "invalid arguments" << endl;
		return;
	}
	target.assign(argv[index]);
	transform(argv + index + 1, argv + argc, back_inserter(cats), convert);
	valid = true;
}

} // unnamed

int set(int argc, char* argv[])
{
	Set_command command(Set_command::process_arguments(argc, argv));
	if (!command.is_valid())
		return 1;
	unique_ptr<Filemanager> manager(command.get_filemanager());
	if (!manager)
		return 1;
	string target(manager->get_filesystem().get_fm_path(
		get_abs_path(command.get_target())));
	if (target.length() == 0) {
		cerr << "Unexpected input is specified." << endl;
		return 1;
	}
	auto& cats(command.get_categories());
	manager->get_map().set(target, cats.begin(), cats.end());
	return 0;
}

} // fm

#include <functional>

namespace fm {

namespace {

inline void push_back(vector<string>& c, const char* e)
{
	c.push_back(e);
}

inline string to_filepath(Filemanager& m, const string& in)
{
	return m.get_filesystem().convert_fm_path(in).to_filepath_string();
}

class Get_command : public Command {
public:
	static Get_command process_arguments(int argc, char* argv[]);

	const vector<string>& get_categories() const {
		return cats;
	}

protected:
	void procremain(int argc, char* argv[], int index);

private:
	Get_command() {
	}

	vector<string> cats;
};

Get_command Get_command::process_arguments(int argc, char* argv[])
{
	Get_command r;
	Command::process_arguments(r, argc, argv);
	return r;
}

void Get_command::procremain(int argc, char* argv[], int index)
{
	if (argc - index <= 0)
		return;

	using std::bind;
	using std::placeholders::_1;
	using std::ref;

	for_each(argv + index, argv + argc, bind(push_back, ref(cats), _1));
	valid = true;
}

} // unnamed

int get(int argc, char* argv[])
{
	using std::bind;
	using std::placeholders::_1;
	using std::ref;
	Get_command command(Get_command::process_arguments(argc, argv));
	if (!command.is_valid())
		return 1;
	unique_ptr<Filemanager> manager(command.get_filemanager());
	if (!manager)
		return 1;
	vector<string> results;
	manager->get_map().get(command.get_categories(),
		back_inserter(results));
	transform(results.begin(), results.end(), results.begin(),
		bind(to_filepath, ref(*manager), _1));
	ostream_iterator<string> out_iter(*fmcout, "\n");
	copy(results.begin(), results.end(), out_iter);
	return 0;
}

} // fm
