all: write

write:
	g++ -std=c++98 -stdlib=libstdc++ -c src/ilp_writing.cpp -o lib/ilp_writing.o
	g++ -std=c++98 -stdlib=libstdc++ -pedantic src/write_design_problem.cpp -o bin/write_design_problem lib/ilp_writing.o

test:
	bin/run_tests

.PHONY: all test clean

clean:
	rm -r bin
	rm lib/*
	rm -r test/bin
