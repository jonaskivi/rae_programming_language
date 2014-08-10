

CXX	:= g++
CXXFLAGS := -g
INCLUDES := -I. -I./rae_compiler/rae_compiler/ -I../boost_uusi/usr_local_include/
LIBS     := -L../boost_uusi/usr_local_lib/ -lboost_chrono -lboost_filesystem -lboost_system

SRC_DIR := ./rae_compiler/rae_compiler/
OBJECTS  := rae_compiler.o

my_program: $(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBS) -o rae_comp

rae_compiler.o: $(SRC_DIR)rae_compiler.cpp $(SRC_DIR)SourceParser.hpp $(SRC_DIR)LangElement.hpp $(SRC_DIR)StringFileWriter.hpp $(SRC_DIR)rae_helpers.hpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $(SRC_DIR)rae_compiler.cpp -o rae_compiler.o

#my_program: $(OBJECTS)
#	$(CXX) $(inputs) -o $(output) $(LIBS)

#%.o: %.cpp
#	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $(SRC_DIR)$(input) -o $(output)

clean:
	rm *.o rae_comp

#opttester:
#	g++ -I./cpp/ ./cpp/rae/examples/OptTester.cpp -o opt_tester
#	./opt_tester

#raetester:
#	g++ -I./cpp/ -I./cpp/rae/examples/RaeTester.cpp ./cpp/rae/examples/Tester.cpp -o rae_tester
#	./rae_tester

raehello:
	g++ -I./cpp/ ./cpp/rae/examples/HelloWorld.cpp -o rae_hello
	./rae_hello

#all:
#	g++ -I./rae_compiler/rae_compiler/ 
#	#g++ -I./cpp/ ./cpp/rae/examples/HelloWorld.cpp -o rae_hello
#	#g++ -I./cpp/ ./cpp/rae/examples/Simple.cpp -o rae_simple
#	#g++ -I./cpp/ ./cpp/rae/examples/RaeTester.cpp ./cpp/rae/examples/Tester.cpp -o rae_tester
#
#run:
#	#./rae_hello
#	#./rae_simple
#	./rae_tester
#


