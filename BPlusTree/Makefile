APP := BPlusTree
CC := g++

SRC := BPlusTree.cpp main.cpp

OBJ := $(SRC:.cpp=.o)

all: depend $(APP)
.PHONY: all

depend: $(SRC)
	$(CC) -MM $(SRC) > .depend

-include .depend

$(APP): $(OBJ)
	$(CC) -o $@ $^

.PHONY: clean
clean:
	rm -f *.o $(APP) *~ .depend
