#ifndef __FM_FM_MAP_IMPL_H__
#define __FM_FM_MAP_IMPL_H__

#include <vector>
#include "fm_map.h"
#include "map_impl.h"

namespace fm {

class Fm_map_impl : public virtual Fm_map, private ml::Map_impl {
public:
	typedef ml::Map_impl Base_impl;

	Fm_map_impl(const std::string& dbfilepath);

	using Base_impl::set;
	using Base_impl::get;
	using Base_impl::getall;
	using Base_impl::get_categories;
	using Base_impl::get_values;
	using Base_impl::copy;
	using Base_impl::move;
	using Base_impl::remove;

	int get_file_type(const std::string& fm_path);
private:
	static bool do_satisfy_dir(const std::string& fm_path,
		const std::vector<std::string>& values);
	bool determine_possibility(const std::string& fm_path);
};

} // fm

#endif // __FM_FM_MAP_IMPL_H__
