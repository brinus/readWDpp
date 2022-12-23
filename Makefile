readWD: readWD.cc readWD.hh
	g++ -std=c++17 -shared -fpic -Wall -o libreadWD.so readWD.cc

main0: example/main0.cc readWD
	g++ -std=c++17 -Wall -L. -l readWD example/main0.cc -o main0

main1: example/main1.cc readWD
	g++ -std=c++17 -Wall -L. -l readWD example/main1.cc -o main1	