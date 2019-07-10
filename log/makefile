##########makefile#######

MAKEDATE = $(shell date +%Y%m%d%H%M%S)
MACRO = -DMAKEDATE=\"$(MAKEDATE)\"

#define g++ arguments
c = g++ -g -c -fPIC -I ./src/ $(MACRO)
cc = gcc -g -c -fPIC -I ./src/ $(MACRO) 
co = g++ -g -fPIC -I ./src/

INCS := -I./inc
LIBS := -L./lib -levent -ljson -lpthread

#define src obj bin path
vpath %.c $(shell find src/) 
vpath %.cpp $(shell find src/)
bindir = ./sbin
objdir = ./objs
install_dir = /usr/local/record

#define object name and dependence
objname = sp_record

cppobjects = $(objdir)/main.o \
			 $(objdir)/global_conf.o \
			 $(objdir)/pconf.o \
			 $(objdir)/pure_log.o \
			 $(objdir)/p_string.o \
			 $(objdir)/p_signals.o \
			 $(objdir)/refc.o \
			 $(objdir)/main_proc.o \
			 $(objdir)/pure_slab.o \
			 $(objdir)/proc_mod.o \
			 $(objdir)/ppthread.o \
			 $(objdir)/thread_cpu.o \
			 $(objdir)/p_shmem.o \
			 $(objdir)/usage.o

			 
#####usercache
cppobjects += $(objdir)/record_control.o \
	      $(objdir)/process_manager.o

cobjects =   

objects = $(cppobjects) $(cobjects)

all: $(objname)
$(objname) : $(objects)
	$(co) $(objects) -o $(bindir)/$(objname) $(LIBS)

# define a function which creates a target for c proj
define CREATE_TARGET_C
#echo "in C"
$1:$(addsuffix .c,$(basename $(notdir $1)))
	$(cc) $$< -o $1
endef

# define a function which creates a target for cpp proj
define CREATE_TARGET_CPP
#echo "in CPP"
#srcfcpp:=$(srcdir)/$(addsuffix .cpp,$(basename $(notdir $1)))
$1:$(addsuffix .cpp,$(basename $(notdir $1)))
	$(c) $$< -o $1
endef


# create projects C targets
$(foreach obj, $(cobjects), $(eval $(call CREATE_TARGET_C,$(obj))))

# create projects CPP targets
$(foreach obj, $(cppobjects) $(testcppobjects), $(eval $(call CREATE_TARGET_CPP,$(obj))))

clean:
	rm -rf $(objdir)/* $(bindir)/*

install:
	rm -f ../srs/src/app/flv_access.h
	cp src/module/flv_access.h ../srs/src/app/
	rm -f /usr/lib64/libflv_access.so
	cp ./lib/libflv_access.so /usr/lib64/
	mkdir -p $(install_dir)/conf/
	mkdir -p $(install_dir)/log/
	mkdir -p $(install_dir)/sbin/
	cp conf/config.conf $(install_dir)/conf/config.conf
	rm -f $(install_dir)/sbin/$(objname)
	cp $(bindir)/$(objname) $(install_dir)/sbin/$(objname)

.PHONY: install clean

