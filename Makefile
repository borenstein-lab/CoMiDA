all: write

CPPFLAGS=-std=c++98
ifeq ($(shell uname), Darwin)
	CPPFLAGS+= -stdlib=libstdc++
endif

write:
	g++ $(CPPFLAGS) -c src/ilp_writing.cpp -o lib/ilp_writing.o
	g++ $(CPPFLAGS) -pedantic src/write_design_problem.cpp -o bin/write_design_problem lib/ilp_writing.o

test:
	bin/run_tests

.PHONY: all test clean

clean:
	rm bin/*
	rm lib/ilp_writing.o
	rm test/bin/*
