EXECUTABLE := ./rae_compiler/raec
EXECUTABLE_NAME := raec
CXX	:= g++
CXXFLAGS := -g -std=c++0x
INCLUDES := -I. -I./rae_compiler/src/

ifeq ($(OS),Windows_NT)
    #CCFLAGS += -D WIN32
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
OBJECTS  := rae_compiler.o LangElement.o ReportError.o rae_helpers.o SourceParser.o

install:
	ifeq ($(OS),Windows_NT)
		# Windows install is untested:
		cp $(EXECUTABLE) ~/bin/$(EXECUTABLE_NAME)
	else
		# you need to use: sudo make install
		cp $(EXECUTABLE) /usr/local/bin/$(EXECUTABLE_NAME)
	endif

all: compile raehello

compile: $(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBS) -o $(EXECUTABLE)

rae_compiler.o: $(SRC_DIR)rae_compiler.cpp $(SRC_DIR)RaeStdLib.hpp $(SRC_DIR)ReportError.hpp $(SRC_DIR)SourceParser.hpp $(SRC_DIR)SourceValidate.hpp $(SRC_DIR)SourceWriter.hpp $(SRC_DIR)LangElement.hpp $(SRC_DIR)StringFileWriter.hpp $(SRC_DIR)rae_helpers.hpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $(SRC_DIR)rae_compiler.cpp -o rae_compiler.o

ReportError.o: $(SRC_DIR)ReportError.hpp $(SRC_DIR)ReportError.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $(SRC_DIR)ReportError.cpp -o ReportError.o

LangElement.o: $(SRC_DIR)LangElement.hpp $(SRC_DIR)LangElement.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $(SRC_DIR)LangElement.cpp -o LangElement.o

SourceParser.o: $(SRC_DIR)SourceParser.cpp $(SRC_DIR)RaeStdLib.hpp $(SRC_DIR)ReportError.hpp $(SRC_DIR)SourceParser.hpp $(SRC_DIR)SourceValidate.hpp $(SRC_DIR)SourceWriter.hpp $(SRC_DIR)LangElement.hpp $(SRC_DIR)StringFileWriter.hpp $(SRC_DIR)rae_helpers.hpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $(SRC_DIR)SourceParser.cpp -o SourceParser.o

rae_helpers.o: $(SRC_DIR)rae_helpers.hpp $(SRC_DIR)rae_helpers.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $(SRC_DIR)rae_helpers.cpp -o rae_helpers.o

clean:
	rm *.o $(EXECUTABLE)

raehello:
	./$(EXECUTABLE) ./tests/HelloWorld.rae
	$(CXX) $(CXXFLAGS) -I./cpp/ ./cpp/rae/examples/HelloWorld.cpp -o rae_hello
	./rae_hello

raehellocpp:
	$(CXX) $(CXXFLAGS) -I./cpp/ ./cpp/rae/examples/HelloWorld.cpp -o rae_hello
	./rae_hello

debugraehello:
	gdb --args ./$(EXECUTABLE) ./tests/HelloWorld.rae
	$(CXX) $(CXXFLAGS) -I./cpp/ ./cpp/rae/examples/HelloWorld.cpp -o rae_hello
	./rae_hello

small: ./tests/small.rae ./cpp/rae/examples/small.hpp ./cpp/rae/examples/small.cpp
	./$(EXECUTABLE) ./tests/small.rae
	$(CXX) $(CXXFLAGS) -I./cpp/ ./cpp/rae/examples/small.cpp -o small
	./small

opttester:
	./$(EXECUTABLE) ./tests/OptTester.rae
	$(CXX) $(CXXFLAGS) -I./cpp/ ./cpp/rae/examples/OptTester.cpp -o opttester
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


