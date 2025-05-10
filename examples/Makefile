all: test build print

test:
	g++ test.cpp -o test  -I/workspaces/libspatialindex/install/include   -L/workspaces/libspatialindex/install/lib   -lspatialindex

build:
	g++ build.cpp -o build  -I/workspaces/libspatialindex/install/include   -L/workspaces/libspatialindex/install/lib   -lspatialindex

print:
	g++ print.cpp -o print  -I/workspaces/libspatialindex/install/include   -L/workspaces/libspatialindex/install/lib   -lspatialindex
clean:
	rm -f test
	rm -f build
	rm -f print	