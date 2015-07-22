all: write

write:
	g++ -c src/ilp_writing.cpp -o lib/ilp_writing.o
	g++ src/write_design_problem.cpp -o bin/write_design_problem.o lib/ilp_writing.o

test:
	bin/run_tests

.PHONY: all test clean

clean:
	rm -r bin
	rm lib/*
	rm -r test/bin
