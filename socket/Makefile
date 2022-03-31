TARGETS=player ringmaster

all: $(TARGETS)
clean:
	rm -f $(TARGETS)

player: player.cpp potato.h
	g++ -g -o $@ $<

server: ringmaster.cpp potato.h
	g++ -g -o $@ $<