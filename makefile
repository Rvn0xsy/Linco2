object=src/Beacon.cpp src/b64.c src/include/b64.h src/include/Beacon.h

beacon: $(object)
	g++ $(object) -o beacon
