SYSTEMC_HOME    = /usr/local/systemc-2.3.1
TARGET_ARCH     = linux64

SYSTEMC_INC_DIR = $(SYSTEMC_HOME)/include
SYSTEMC_LIB_DIR = $(SYSTEMC_HOME)/lib-$(TARGET_ARCH)

FLAGS           = -g -Wall -pedantic -Wno-long-long \
                 -DSC_INCLUDE_DYNAMIC_PROCESSES -fpermissive \
                 -I$(SYSTEMC_INC_DIR)
LDFLAGS         = -L$(SYSTEMC_LIB_DIR) -lsystemc -lm -Wl,-rpath=$(SYSTEMC_LIB_DIR)

#SRCS = src/main.cpp
SRCS = src/*.cpp
OBJS = $(SRCS:.cpp=.o)

EXE = model

main:
	g++ -o $(EXE) $(LDFLAGS) $(FLAGS) $(SRCS)

run: main
	./$(EXE)
