EXECUTABLE := rae_comp
CXX	:= g++
CXXFLAGS := -g -std=c++0x
INCLUDES := -I. -I./rae_compiler/rae_compiler/
#LIBS     := -L../boost_uusi/usr_local_lib/ -lboost_chrono -lboost_filesystem -lboost_system

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
        #INCLUDES += -I../boost_uusi/usr_local_include/
		#LIBS     := -L../boost_linux32/usr_local_lib/ -L/usr/lib/ -lboost_chrono -lboost_filesystem -lboost_system
		LIBS     := -L/usr/lib/ -lboost_chrono -lboost_filesystem -lboost_system
    endif
    ifeq ($(UNAME_S),Darwin)
        #CCFLAGS += -D OSX
        INCLUDES += -I../boost_uusi/usr_local_include/
		LIBS     := -L../boost_uusi/usr_local_lib/ -lboost_chrono -lboost_filesystem -lboost_system
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

SRC_DIR := ./rae_compiler/rae_compiler/
OBJECTS  := rae_compiler.o LangElement.o ReportError.o

all: compile raehello

compile: $(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBS) -o $(EXECUTABLE)

rae_compiler.o: $(SRC_DIR)rae_compiler.cpp $(SRC_DIR)RaeStdLib.hpp $(SRC_DIR)ReportError.hpp $(SRC_DIR)SourceParser.hpp $(SRC_DIR)SourceValidate.hpp $(SRC_DIR)SourceWriter.hpp $(SRC_DIR)LangElement.hpp $(SRC_DIR)StringFileWriter.hpp $(SRC_DIR)rae_helpers.hpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $(SRC_DIR)rae_compiler.cpp -o rae_compiler.o

ReportError.o: $(SRC_DIR)ReportError.hpp $(SRC_DIR)ReportError.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $(SRC_DIR)ReportError.cpp -o ReportError.o

LangElement.o: $(SRC_DIR)LangElement.hpp $(SRC_DIR)LangElement.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $(SRC_DIR)LangElement.cpp -o LangElement.o

#my_program: $(OBJECTS)
#	$(CXX) $(inputs) -o $(output) $(LIBS)

#%.o: %.cpp
#	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $(SRC_DIR)$(input) -o $(output)

clean:
	rm *.o $(EXECUTABLE)

#opttester:
#	g++ -I./cpp/ ./cpp/rae/examples/OptTester.cpp -o opt_tester
#	./opt_tester

#raetester:
#	g++ -I./cpp/ -I./cpp/rae/examples/RaeTester.cpp ./cpp/rae/examples/Tester.cpp -o rae_tester
#	./rae_tester

raehello:
	./$(EXECUTABLE) ./rae/examples/HelloWorld.rae
	$(CXX) $(CXXFLAGS) -I./cpp/ ./cpp/rae/examples/HelloWorld.cpp -o rae_hello
	./rae_hello

raehellocpp:
	$(CXX) $(CXXFLAGS) -I./cpp/ ./cpp/rae/examples/HelloWorld.cpp -o rae_hello
	./rae_hello

debugraehello:
	gdb --args ./$(EXECUTABLE) ./rae/examples/HelloWorld.rae
	$(CXX) $(CXXFLAGS) -I./cpp/ ./cpp/rae/examples/HelloWorld.cpp -o rae_hello
	./rae_hello

opttester:
	./$(EXECUTABLE) ./rae/examples/OptTester.rae
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


