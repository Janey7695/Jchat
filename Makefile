all:
	cd cJSON;mkdir build;cd build;cmake ..;cmake --build .;cd ../..
	cd libhv;mkdir build;cd build;cmake ..;cmake --build .;cd ../..
	mkdir build;cd build;cmake ..;cmake --build .
clean:
	rm -fr build;

