EXECUTABLE := ./bin/raec
EXECUTABLE_NAME := raec
CXX	:= g++
CXXFLAGS := -g -std=c++0x
INCLUDES := -I. -I./rae_compiler/src/

INSTALL_PATH := /usr/local/bin/

ifeq ($(OS),Windows_NT)
    #CCFLAGS += -D WIN32
    INSTALL_PATH := ~/bin/
    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
        #CCFLAGS += -D AMD64
    endif
    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
        #CCFLAGS += -D IA32
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        #CCFLAGS += -D LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        #CCFLAGS += -D OSX
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        #CCFLAGS += -D AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        #CCFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        #CCFLAGS += -D ARM
    endif
endif

SRC_DIR := ./rae_compiler/src/
OBJECTS  := rae_compiler.o Compiler.o Element.o ReportError.o rae_helpers.o SourceParser.o

all: compile raehello

# you need to use: sudo make install, windows install is untested
install:
	cp $(EXECUTABLE) $(INSTALL_PATH)$(EXECUTABLE_NAME)

compile: $(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBS) -o $(EXECUTABLE)

rae_compiler.o: $(SRC_DIR)rae_compiler.cpp $(SRC_DIR)RaeStdLib.hpp $(SRC_DIR)ReportError.hpp $(SRC_DIR)SourceParser.hpp $(SRC_DIR)SourceValidate.hpp $(SRC_DIR)SourceWriter.hpp $(SRC_DIR)Element.hpp $(SRC_DIR)StringFileWriter.hpp $(SRC_DIR)rae_helpers.hpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $(SRC_DIR)rae_compiler.cpp -o rae_compiler.o

ReportError.o: $(SRC_DIR)ReportError.hpp $(SRC_DIR)ReportError.cpp $(SRC_DIR)rae_helpers.hpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $(SRC_DIR)ReportError.cpp -o ReportError.o

Element.o: $(SRC_DIR)Element.hpp $(SRC_DIR)Element.cpp $(SRC_DIR)rae_helpers.hpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $(SRC_DIR)Element.cpp -o Element.o

Compiler.o: $(SRC_DIR)Compiler.hpp $(SRC_DIR)Compiler.cpp $(SRC_DIR)Element.hpp $(SRC_DIR)Element.cpp $(SRC_DIR)rae_helpers.hpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $(SRC_DIR)Compiler.cpp -o Compiler.o

SourceParser.o: $(SRC_DIR)SourceParser.cpp $(SRC_DIR)SourceParser.hpp $(SRC_DIR)SourceValidate.hpp $(SRC_DIR)SourceWriter.hpp $(SRC_DIR)RaeStdLib.hpp $(SRC_DIR)ReportError.hpp $(SRC_DIR)Element.hpp $(SRC_DIR)StringFileWriter.hpp $(SRC_DIR)rae_helpers.hpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $(SRC_DIR)SourceParser.cpp -o SourceParser.o

rae_helpers.o: $(SRC_DIR)rae_helpers.hpp $(SRC_DIR)rae_helpers.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $(SRC_DIR)rae_helpers.cpp -o rae_helpers.o

clean:
	rm *.o $(EXECUTABLE)

# to compile and run hello.rae: make raehello
raehello:
	./$(EXECUTABLE) ./examples/hello.rae
	$(CXX) $(CXXFLAGS) -I./cpp/ ./cpp/examples/hello.cpp -o rae_hello
	./rae_hello

raetest:
	./$(EXECUTABLE) ./tests/test.rae
	$(CXX) $(CXXFLAGS) -I./cpp/ ./cpp/tests/test.cpp -o rae_test
	./rae_test

raetestcpp:
	$(CXX) $(CXXFLAGS) -I./cpp/ ./cpp/tests/test.cpp -o rae_test
	./rae_test

debugraetest:
	gdb --args ./$(EXECUTABLE) ./tests/test.rae
	$(CXX) $(CXXFLAGS) -I./cpp/ ./cpp/tests/test.cpp -o rae_test
	./rae_test

small: ./tests/small.rae ./cpp/rae/tests/small.hpp ./cpp/rae/tests/small.cpp
	./$(EXECUTABLE) ./tests/small.rae
	$(CXX) $(CXXFLAGS) -I./cpp/ ./cpp/tests/small.cpp -o small
	./small

opttester:
	./$(EXECUTABLE) ./tests/OptTester.rae
	$(CXX) $(CXXFLAGS) -I./cpp/ ./cpp/tests/OptTester.cpp -o opttester
	./opttester

#all:
#	g++ -I./rae_compiler/rae_compiler/ 
#	#g++ -I./cpp/ ./cpp/rae/examples/HelloWorld.cpp -o rae_hello
#	#g++ -I./cpp/ ./cpp/rae/examples/Simple.cpp -o rae_simple
#	#g++ -I./cpp/ ./cpp/rae/examples/RaeTester.cpp ./cpp/rae/examples/Tester.cpp -o rae_tester
#

#run:
#	./$(EXECUTABLE) ./rae/examples/HelloWorld.rae

#	#./rae_hello
#	#./rae_simple
#	./rae_tester
#


