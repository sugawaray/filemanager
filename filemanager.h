#ifndef __FM_FILEMANAGER_H__
#define __FM_FILEMANAGER_H__

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include "fm.h"
#include "fm_filesystem.h"
#include "fm_map_impl.h"

namespace fm {

class Filemanager {
public:
	Filemanager();
	Filemanager(const Absolute_path& fm_dir);

	fs::Filesystem& get_filesystem() {
		return *filesystem;
	}

	Fm_map_impl& get_map() {
		return *map;
	}

	void copy(const Absolute_path& source,
		const Absolute_path& destination);
	void copydir(const Absolute_path& source,
		const Absolute_path& destination);
	void move(const Absolute_path& source,
		const Absolute_path& destination);
	void refresh();
	int remove(const Absolute_path& target);
	int rmdir(const Absolute_path& target);
private:
	typedef std::function<void (const std::string&, const std::string&)>
		Move_func;

	void init(const Absolute_path& fm_dir);
	static bool in_sync_dd(int src, int dest);
	static bool in_sync_ddp(int src, int dest);
	static bool in_sync_ff(int src, int dest);
	static bool in_sync_ffp(int src, int dest);
	static bool in_sync_fd(int src, int dest);
	static int get_file_type(const Absolute_path& path);
	int get_map_file_type(const Absolute_path& path);
	static bool is_type_dir_to_dir(int src_type, int dest_type);
	static bool is_type_dir_to_dirpath(int src_type, int dest_type);
	static bool is_type_file_to_file(int src_type, int dest_type);
	static bool is_type_file_to_filepath(int src_type, int dest_type);
	void dir_to_dir(Move_func func, const Absolute_path& source,
		const Absolute_path& destination);
	void dir_to_dirpath(Move_func func, const Absolute_path& source,
		const Absolute_path& destination);
	void file_to_dir(Move_func func, const Absolute_path& source,
		const Absolute_path& destination);
	void file_to_file(Move_func func, const Absolute_path& source,
		const Absolute_path& destination);
	void rmdir_map(const Absolute_path& path);
	Move_func move_func();
	Move_func copy_func();
	static void fs_copy(const Absolute_path& src,
		const Absolute_path& dest);
	void map_move(const std::string& src, const std::string& dest);
	void map_copy(const std::string& src, const std::string& dest);
	static void call_move(Move_func func,
		const std::pair<std::string, std::string>& p);

	std::unique_ptr<fs::Filesystem> filesystem;
	std::unique_ptr<Fm_map_impl> map;
	Absolute_path root;
};

} // fm

#endif // __FM_FILEMANAGER_H__
