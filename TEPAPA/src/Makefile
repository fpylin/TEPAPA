# Makefile
#
#
# Compilation requirements:
# - Must support C++11
# 
# Library pre-requisites:
# - libgsl        - GNU scientific library
# - libgslcblas   - GNU scientific library
# - libpthread    - GNU POSIX threading Library
# - perl -MExtUtils::Embed  for embedding Perl in C/C++ applications
#

TARGETS=tepapa
TARGETDIR=../bin
ALL_CC=$(wildcard *.cc)
OBJS=$(patsubst %.cc,%.o,$(ALL_CC) )
ARCH=-m64
OPTFLAG=-Wall -O3 -g
LDFLAGS=$(OPTFLAG) $(ARCH)
LIBS= -lgslcblas -lgsl -lm -L . -l tepapa -lpthread $(shell perl -MExtUtils::Embed -e ldopts) 
CXXFLAGS=$(ARCH) $(shell perl -MExtUtils::Embed -e ccopts) -std=c++11

##########################################################################################

all: .dep libtepapa.a $(TARGETS)
#	strip $(TARGETS)
	cp -av $(TARGETS) $(TARGETDIR)

include .dep

##########################################################################################

debug: OPTFLAG=-Wall -O3 -rdynamic
debug: LIBS+=-lSegFault
debug: all

##########################################################################################

$(TARGETS): $(OBJS) libtepapa.a 
	@printf "  \e[1;33m[T]\e[0m %-20s" $@
	g++ $(OPTFLAG) $(LDFLAGS) $@.o -o $@ $(LIBS)


libtepapa.a: $(OBJS)
	@printf "  \e[1;34m[A]\e[0m %-20s" $@
	ar rcs $@ $^

eperl.o: eperl.cc  eperl.h  strfriends.h
	@printf "  \e[1;32m[O]\e[0m %-20s" $@
	g++ $(CXXFLAGS) -c -o $@ $<

%.o: %.cc
	@printf "  \e[1;32m[O]\e[0m %-20s" $@
	g++ $(OPTFLAG) $(CXXFLAGS) -c -o $@ $<

.dep: Makefile $(ALL_CC)
	@printf "\e[1;37mMaking dependencies "
	@g++ -MM $(OPTFLAG) $(CXXFLAGS) $^ > $@ 2> /dev/null ; true
	@printf "... \e[1;32mDone\e[0m\n"

clean:
	@printf "\e[1;37mCleaning up...\e[0m \n"
	rm -fv $(TARGETS) *.o *.a

