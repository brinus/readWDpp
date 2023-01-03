readWD: readWD.cc readWD.hh
	g++ -std=c++17 -shared -fpic -Wall -o libreadWD.so readWD.cc

main0: example/main0.cc readWD
	g++ -std=c++17 -Wall -L. -l readWD example/main0.cc -o main0

main1: example/main1.cc readWD
	g++ -std=c++17 -Wall -L. -l readWD example/main1.cc -o main1	

main3: example/main3.cc readWD
	g++ -std=c++17 -Wall `root-config --cflags --libs` -L. -l readWD example/main3.cc -o main3	