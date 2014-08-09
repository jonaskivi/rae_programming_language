opttester:
	g++ -I./cpp/ ./cpp/rae/examples/OptTester.cpp -o opt_tester
	./opt_tester

raetester:
	g++ -I./cpp/ ./cpp/rae/examples/RaeTester.cpp ./cpp/rae/examples/Tester.cpp -o rae_tester
	./rae_tester

all:
	#g++ -I./cpp/ ./cpp/rae/examples/HelloWorld.cpp -o rae_hello
	#g++ -I./cpp/ ./cpp/rae/examples/Simple.cpp -o rae_simple
	g++ -I./cpp/ ./cpp/rae/examples/RaeTester.cpp ./cpp/rae/examples/Tester.cpp -o rae_tester

run:
	#./rae_hello
	#./rae_simple
	./rae_tester

	