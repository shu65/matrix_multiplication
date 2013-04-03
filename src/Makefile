#!/bin/make
CC = icc
CXX = icpc
INCLUDE=
CFLAGS = -Wall
CXXFLAGS = -Wall
LDFLAGS = -lm
ifeq ($(Debug), Y) 
	CFLAGS += -O0 -g3
	CXXFLAGS += -O0 -g3
else
	CFLAGS += -O2
	CXXFLAGS += -O2
endif

ifeq ($(Profile), Y)
	CFLAGS += -profile-functions -profile-loops=all -profile-loops=2
	CXXFLAGS += -profile-functions -profile-loops=all -profile-loops=2
endif

ifeq ($(Mic), Y)
	CFLAGS += -DMIC -openmp
	CXXFLAGS += -DMIC -openmp
endif

CPP_SRC=main.cpp \
	matrix_multiplication.cpp \

OBJS =
OBJS += $(C_SRC:%.c=%.o)
OBJS += $(CPP_SRC:%.cpp=%.o)

.PHONY: all clean
.SUFFIXES: .o

all:matrix_multiplication

matrix_multiplication: $(OBJS)	
	$(CXX) -o $@ $(OBJS) $(LDFLAGS) $(LDLIBS)

clean:
	rm -rf $(OBJS)

#.c.o:
#    $(CC) $(CFLAGS) -o -o $@ -c $<
#.cpp.o
#    $(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ -c $<
#.c.o:
#    $(CC) $(CFLAGS) -o $@ $^ -c
#.cpp.o:
#    $(CXX) $(CXXFLAGS) -o $@ $^ -c

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $< -o $@  $(INCLUDES)
