#ifndef __FM_TREE_H__
#define __FM_TREE_H__

#include <string>
#include <list>
#include <vector>

namespace fm {

	class Path_node;
	class Path_node_allocator;

class Path_node_handler {
public:
	virtual bool handle(const Path_node& node) = 0;
};

class Path_node {
public:
	explicit Path_node(Path_node_allocator& allocator,
		const std::string& fm_path);
	bool is_leaf() const throw();
	std::string get_value() const throw();
	void insert(const std::string& fm_path);
	void walk(Path_node_handler& handler) const;
private:
	typedef std::vector<Path_node*>::iterator Iter;
	Iter find_child(const std::string& path);

	Path_node_allocator& allocator;
	std::vector<Path_node*> children;
	std::string fm_path;
};

class Path_node_allocator {
public:
	Path_node* generate(const std::string& fm_path);
private:
	std::list<Path_node> resource;
};

class Path_tree {
public:
	Path_tree();
	void insert(const std::string& fm_path);
	void walk(Path_node_handler& handler) const;
private:
	Path_node_allocator allocator;
	Path_node* root;
};

} // fm

#endif // __FM_TREE_H__
