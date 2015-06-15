#ifndef __EXCEPT_H__
#define __EXCEPT_H__

#include <stdexcept>

namespace ml {
namespace db {

class Db_error : public std::exception {
};

class Prepare_error : public Db_error {
};

} // db
} // ml

namespace fm {

class No_memory : public std::runtime_error {
public:
	No_memory()
	:	std::runtime_error("memory not available") {
	}
};

class Not_under_control_error : public std::exception {
};

class Filesystem_error : public std::exception {
};

class Invalid_destination : public std::exception {
};

class Out_of_sync : public std::exception {
};

class File_is_a_directory : public std::exception {
};

class Fmdir_is_not_found : public std::exception {
};

} // fm

namespace fs {

class Invalid_path : public std::runtime_error {
public:
	Invalid_path()
	:	std::runtime_error("invalid path") {
	}
};

} // fs

#endif // __EXCEPT_H__
