TARGETS=player ringmaster potato.cpp

all: $(TARGETS)
clean:
	rm -f $(TARGETS)

ringmaster: ringmaster.cpp potato.cpp potato.h
	g++ -g -o ringmaster ringmaster.cpp potato.cpp 
player: player.cpp potato.cpp potato.h
	g++ -g -o player player.cpp potato.cpp 