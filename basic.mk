# This file is the main makefile.
#
# Makefiles in subdirectories use this to use common functionallities and
# settings.
#
# This makefile calls targets of makefiles in subdirectories, specifying this
# file's absolute path. And the called makefiles accomplish their tasks using
# this makefile and specific settings in their own makefiles.
#
# You must provide makefiles in subdirectories with the functionalities and
# settings which they use by specifying a makefile path for them to include
# when you use them independently.
.POSIX :

# Shell commands definitions.
RM = /bin/rm
CP = /bin/cp
ECHO = /bin/echo
FIND = /usr/bin/find
PWD = /bin/pwd
SED = /bin/sed

#SORT = /bin/sort
SORT = /usr/bin/sort

TOUCH = /usr/bin/touch
TR = /usr/bin/tr
UNIQ = /usr/bin/uniq

BASICMK = basic.mk
BASICMK_PATH = $$($(PWD))/$(BASICMK)
MAKEFILE_PATH = $$($(PWD))/makefile
ROOTDIR = $$($(PWD))

# Build parameters.
CC = gcc
LD = gcc
CFLAGS = -g -Wall -std=c++0x -pedantic -I $(ROOTDIR)
LDFLAGS_COMMON = -lstdc++ -lsqlite3

#CC = clang
#LD = clang
#CFLAGS = -g -Wall -std=c++11 -pedantic -I $(ROOTDIR)
#LDFLAGS_COMMON = -lstdc++ -lsqlite3

# libflag for the test framework 'check'
CHECK_LIBFLAG = -lcheck_pic -pthread -lrt -lm 
#CHECK_LIBFLAG = -lcheck

LDFLAGS = $(LDFLAGS_COMMON) $(LDFLAGS_ADDITION)
DEPEND_OPT = -MM

DIRS =

OBJS_COMMON =	absolute_path.o	\
		filesystem.o	\
		filemanager.o	\
		fm_commands.o	\
		fm_filesystem.o	\
		fm_map_impl.o	\
		map_impl.o	\
		path.o		\
		sql_value_spec.o	\
		statement.o	\
		tree.o

TARGET_TEST = test

TARGET_SET = fm-set
OBJS_SET =	$(OBJS_COMMON)	\
		fm-set.o

TARGET_CP = fm-cp
OBJS_CP =	$(OBJS_COMMON)	\
		fm-cp.o

TARGET_GET = fm-get
OBJS_GET =	$(OBJS_COMMON)	\
		fm-get.o

TARGET_GETCAT = fm-getcat
OBJS_GETCAT =	$(OBJS_COMMON)	\
		fm-getcat.o

TARGET_RM = fm-rm
OBJS_RM =	$(OBJS_COMMON)	\
		fm-rm.o	\

TARGET_MV = fm-mv
OBJS_MV =	$(OBJS_COMMON)	\
		fm-mv.o

TARGET_REFRESH = fm-refresh
OBJS_REFRESH =	$(OBJS_COMMON)	\
		fm-refresh.o

TARGETS =	$(TARGET_SET)		\
		$(TARGET_CP)		\
		$(TARGET_GET)		\
		$(TARGET_GETCAT)	\
		$(TARGET_RM)		\
		$(TARGET_REFRESH)	\
		$(TARGET_MV)		\
		$(TARGET_TEST)

TARGET =
# Please don't use this macro at the places other than command lines.
OBJS_ALL =	$$($(ECHO)	\
			$(OBJS_SET)	\
			$(OBJS_CP)	\
			$(OBJS_GET)	\
			$(OBJS_GETCAT)	\
			$(OBJS_REFRESH)	\
			$(OBJS_RM)	\
			$(OBJS_MV) |	\
				$(TR) ' ' '\n'	|	\
				$(SORT)		|	\
				$(UNIQ)		|	\
				$(TR) '\n' ' ')
# Please don't use this macro at the places other than command lines.
SRCS_ALL = $$($(ECHO) $(OBJS_ALL) | $(TR) ' ' '\n' |	\
		$(SED) -e 's/.o$$/.cpp/' | $(TR) '\n' ' ')

all : $(TARGETS)
	./checkodr

$(TARGET_SET) :	$(OBJS_SET)
	@$(MAKE) -f makefile -f depend linkobjs "TARGET=$@" "OBJS=$(OBJS_SET)"

$(TARGET_CP) : $(OBJS_CP)
	@$(MAKE) -f makefile -f depend linkobjs "TARGET=$@" "OBJS=$(OBJS_CP)"

$(TARGET_GET) : $(OBJS_GET)
	@$(MAKE) -f makefile -f depend linkobjs "TARGET=$@" "OBJS=$(OBJS_GET)"

$(TARGET_GETCAT) : $(OBJS_GETCAT)
	@$(MAKE) -f makefile -f depend linkobjs "TARGET=$@"	\
		"OBJS=$(OBJS_GETCAT)"

$(TARGET_REFRESH) : $(OBJS_REFRESH)
	@$(MAKE) -f makefile -f depend linkobjs "TARGET=$@"	\
		"OBJS=$(OBJS_REFRESH)"

$(TARGET_RM) : $(OBJS_RM)
	@$(MAKE) -f makefile -f depend linkobjs "TARGET=$@" "OBJS=$(OBJS_RM)"

$(TARGET_MV) : $(OBJS_MV)
	@$(MAKE) -f makefile -f depend linkobjs "TARGET=$@" "OBJS=$(OBJS_MV)"

test_objs :
	@t=$(MAKEFILE_PATH)	&&	\
	r=$(ROOTDIR)	&&	\
	cd ./tests &&	\
	$(MAKE) -f $$t -f makefile sub-make	\
		"MAKEFILE_PATH=$$t" "ROOTDIR=$$r"

$(TARGET_TEST) : $(OBJS_COMMON) test_objs
	@$(MAKE) -f makefile -f depend linkobjs "TARGET=$@"	\
		"OBJS=$(OBJS_COMMON) $$(				\
			$(FIND) ./tests -type f -name '*.cpp' |		\
				$(SED) -e 's/.cpp$$/.o/' |		\
				$(TR) '\n' ' ')"			\
		"LDFLAGS_ADDITION=$(CHECK_LIBFLAG)"

linkobjs :
	@$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)

FIND_SRCS = $$($(FIND) . -maxdepth 1 -type f -name '*.cpp' | $(TR) '\n' ' ')
FIND_OBJS = $$($(FIND) . -maxdepth 1 -type f -name '*.cpp' |	\
		$(SED) -e 's/.cpp$$/.o/' | $(TR) '\n' ' ')

call-child-genmk :
	@t=$(BASICMK_PATH) &&						\
	r=$(ROOTDIR) &&	\
	cd ./tests &&						\
	$(MAKE) -f $$t -f basic.mk sub-genmk			\
			"BASICMK_PATH=$$t" "ROOTDIR=$$r"

genmk :
	@-$(RM) makefile depend
	@$(MAKE) -f basic.mk call-child-genmk
	@$(CP) basic.mk makefile
	@$(ECHO) "#generated dependencies follow." >> depend
	@-$(CC) $(CFLAGS) $(SRCS_ALL) $(DEPEND_OPT) >> depend
	@$(ECHO) "" >> depend

genmk-fn :
	@-$(RM) makefile depend
	@$(CP) $(BASICMK) makefile
	@$(ECHO) "#generated dependencies follow." >> depend
	@$(CC) $(CFLAGS) $(SRCS) $(DEPEND_OPT) >> depend
	@$(ECHO) "" >> depend

clean :
	@t=$(MAKEFILE_PATH) &&	\
	r=$(ROOTDIR) &&	\
	cd ./tests &&	\
		$(MAKE) -f $$t -f makefile sub-clean	\
			"MAKEFILE_PATH=$$t" "ROOTDIR=$$r"
	@-$(RM) $(TARGETS) $(OBJS_ALL)
	@-$(RM) -rf ./test_dir

do-test : all
	@-$(RM) $(TARGET_TEST)
	@$(MAKE) -f makefile -f depend $(TARGET_TEST)
	@./$(TARGET_TEST)

gen-objs-fn : $(OBJS)


clean-fn :
	@-$(RM) $(OBJS)

.cpp.o :
	@$(CC) -c -o $@ $(CFLAGS) $<


sub-genmk :
	@t=$(BASICMK_PATH) &&	\
	u=$$($(PWD))/$(BASICMK)	&&	\
	r=$(ROOTDIR) &&	\
	for d in $(DIRS);	\
	do	\
		(cd $$d &&	\
		$(MAKE) -f $$t -f $$u -f basic.mk sub-genmk	\
			"BASICMK_PATH=$$t" "ROOTDIR=$$r");	\
	done;
	$(MAKE) -f $(BASICMK_PATH) genmk-fn "SRCS=$(FIND_SRCS)"

sub-clean :
	@t=$(MAKEFILE_PATH) &&	\
	r=$(ROOTDIR)	&&	\
	for d in $(DIRS);	\
	do	\
		(cd $$d &&	\
		$(MAKE) -f $$t -f makefile sub-clean	\
			"MAKEFILE_PATH=$(MAKEFILE_PATH)" \
			"ROOTDIR=$$r");	\
	done;
	@$(MAKE) -f $(MAKEFILE_PATH) -f makefile clean-fn "OBJS=$(FIND_OBJS)"

sub-make :
	@r=$(ROOTDIR) &&	\
	for d in $(DIRS);	\
	do	\
		(cd $$d &&	\
		$(MAKE) -f $(MAKEFILE_PATH)	\
			-f makefile -f depend sub-make	\
			"MAKEFILE_PATH=$(MAKEFILE_PATH)"	\
			"ROOTDIR=$$r");	\
	done;
	@$(MAKE) -f $(MAKEFILE_PATH) -f makefile -f depend gen-objs-fn	\
		"OBJS=$(FIND_OBJS)"



