
#MAKEDATE = $(shell date +%Y%m%d%H%M%S)
#MACRO = -DMAKEDATE=\"$(MAKEDATE)\"

#****************************************************************************

# DEBUG can be set to YES to include debugging info, or NO otherwise
DEBUG          := YES

# PROFILE can be set to YES to include profiling info, or NO otherwise
PROFILE        := NO

# USE_STL can be used to turn on STL support. NO, then STL
# will not be used. YES will include the STL files.
USE_STL := YES

# WIN32_ENV
# WIN32_ENV := YES
#****************************************************************************

CC     := gcc
CXX    := g++
LD     := g++
AR     := ar rc
RANLIB := ranlib

# ifeq (YES, ${WIN32_ENV})
#   RM     := del
# else
#   RM     := rm -f
# endif

DEBUG_CFLAGS     := -Wall -Wno-format -g -DDEBUG
RELEASE_CFLAGS   := -Wall -Wno-unknown-pragmas -Wno-format -O2

DEBUG_CXXFLAGS   := ${DEBUG_CFLAGS}
RELEASE_CXXFLAGS := ${RELEASE_CFLAGS}

DEBUG_LDFLAGS    := -g
RELEASE_LDFLAGS  := -O3

ifeq (YES, ${DEBUG})
   CFLAGS       := ${DEBUG_CFLAGS}
   CXXFLAGS     := ${DEBUG_CXXFLAGS}
   LDFLAGS      := ${DEBUG_LDFLAGS}
else
   CFLAGS       := ${RELEASE_CFLAGS}
   CXXFLAGS     := ${RELEASE_CXXFLAGS}
   LDFLAGS      := ${RELEASE_LDFLAGS}
endif

ifeq (YES, ${PROFILE})
   CFLAGS   := ${CFLAGS} -pg -O3
   CXXFLAGS := ${CXXFLAGS} -pg -O3
   LDFLAGS  := ${LDFLAGS} -pg
endif

#****************************************************************************
# Preprocessor directives
#****************************************************************************

ifeq (YES, ${USE_STL})
  DEFS := -DUSE_STL
else
  DEFS :=
endif

#****************************************************************************
# Include paths
#****************************************************************************

INCS := -I./inc/ -I./inc/event -I./src/
LIBS := -L./lib -levent -ljson -lmongoc-1.0 -lbson-1.0

#****************************************************************************
# Makefile code common to all platforms
#****************************************************************************

CFLAGS   := ${CFLAGS}   ${DEFS}
CXXFLAGS := ${CXXFLAGS} ${DEFS}

#****************************************************************************
# Targets of the build
#****************************************************************************
OUTPUT_DIR := objs/
TARGET := record

all: ${TARGET}

SRC_DIR = ./src

#****************************************************************************
# Source files
#****************************************************************************

SRCS := $(SRC_DIR)/global_conf.cpp \
$(SRC_DIR)/usage.cpp \
$(SRC_DIR)/main.cpp \
$(SRC_DIR)/conf/pconf.cpp \
$(SRC_DIR)/log/pure_log.cpp \
$(SRC_DIR)/string/p_string.cpp \
$(SRC_DIR)/signal/p_signals.cpp \
$(SRC_DIR)/ref/refc.cpp \
$(SRC_DIR)/process/main_proc.cpp \
$(SRC_DIR)/mem_pool/pure_slab.cpp \
$(SRC_DIR)/process/proc_mod.cpp \
#$(SRC_DIR)/thread/ppthread.cpp \
#$(SRC_DIR)/thread/thread_cpu.cpp \
$(SRC_DIR)/shmem/p_shmem.cpp

# Add on the sources for libraries
SRCS += $(SRC_DIR)/app/record_control.cpp \
$(SRC_DIR)/app/process_manager.cpp \
$(SRC_DIR)/app/mongo_adaptor.cpp

OBJS := $(addsuffix .o,$(basename ${SRCS}))

#****************************************************************************
# Output
#****************************************************************************

${TARGET}: ${OBJS}
	${LD} -o $@ ${LDFLAGS} ${OBJS} ${LIBS}
	mv ./record ./sbin

#****************************************************************************
# common rules
#****************************************************************************

# Rules for compiling source files to object files
%.o : %.cpp
	${CXX} -c ${CXXFLAGS} ${INCS} $< -o $@

%.o : %.c
	${CC} -c ${CFLAGS} ${INCS} $< -o $@

dist:
	bash makedistlinux

clean:
	${RM} core ${OBJS} ${TARGET}
	${RM} sbin/${TARGET}

depend:
	#makedepend ${INCS} ${SRCS}

%.o: %.h
