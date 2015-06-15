#ifndef __FM_COMMANDS_H__
#define __FM_COMMANDS_H__

#include <fm.h>

namespace fm {

extern int cp(int argc, char* argv[]);
extern int rm(int argc, char* argv[]);
extern int getcat(int argc, char* argv[]);
extern int get(int argc, char* argv[]);
extern int mv(int argc, char* argv[]);
extern int refresh(int argc, char* argv[]);
extern int set(int argc, char* argv[]);

} // fm

#endif // __FM_COMMANDS_H__
