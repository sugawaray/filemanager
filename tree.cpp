#include <algorithm>
#include <functional>
#include "tree.h"

namespace fm {

using std::find_if;
using std::for_each;
using std::string;

Path_node::Path_node(Path_node_allocator& allocator, const string& fm_path)
:	allocator(allocator),
	fm_path(fm_path)
{
}

bool Path_node::is_leaf() const throw()
{
	return children.empty();
}

string Path_node::get_value() const throw()
{
	return fm_path;
}

void Path_node::insert(const string& fm_path_arg)
{
	auto mycount(std::count(fm_path.begin(), fm_path.end(), '/'));
	auto count(std::count(fm_path_arg.begin(), fm_path_arg.end(), '/'));
	if (mycount < count || (count == mycount && fm_path.empty())) {
		string childpath(fm_path_arg.substr(0,
			fm_path_arg.find_first_of('/', fm_path.length() + 1)));
		auto child(find_child(childpath));
		if (child == children.end()) {
			children.push_back(allocator.generate(childpath));
			child = children.end() - 1;
		}
		(*child)->insert(fm_path_arg);
	}
}

using std::bind;
using std::cref;
using std::placeholders::_1;
using std::ref;

void Path_node::walk(Path_node_handler& handler) const
{
	handler.handle(*this);
	for_each(children.begin(), children.end(),
		bind(&Path_node::walk, _1, ref(handler)));
}

namespace {

inline bool equal_path(const Path_node* in, const string& path)
{
	return in->get_value() == path;
}

} // unnamed

Path_node::Iter Path_node::find_child(const string& path)
{
	return find_if(children.begin(), children.end(),
		bind(equal_path, _1, cref(path)));
}

Path_node* Path_node_allocator::generate(const string& fm_path)
{
	resource.push_back(Path_node(*this, fm_path));
	return &resource.back();
}

Path_tree::Path_tree()
:	root(0)
{
}

void Path_tree::insert(const string& fm_path)
{
	if (root == 0) {
		root = allocator.generate("");
		if (!fm_path.empty())
			root->insert(fm_path);
	} else
		root->insert(fm_path);
}

void Path_tree::walk(Path_node_handler& handler) const
{
	if (root != 0)
		root->walk(handler);
}

} // fm
