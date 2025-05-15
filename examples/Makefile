all: test build print

test:test.cpp
	g++ test.cpp -o test  -I/workspaces/libspatialindex/install/include   -L/workspaces/libspatialindex/install/lib   -lspatialindex

build:build.cpp
	g++ build.cpp -o build  -I/workspaces/libspatialindex/install/include   -L/workspaces/libspatialindex/install/lib   -lspatialindex

print:print.cpp
	g++ print.cpp -o print  -I/workspaces/libspatialindex/install/include   -L/workspaces/libspatialindex/install/lib   -lspatialindex

parse:parse.cpp
	g++ parse.cpp -o parse


clean:
	rm -f test
	rm -f build
	rm -f print	
	rm -f parse