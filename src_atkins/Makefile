IDIR =../include
CC=g++  -Wall
#CC=i586-mingw32msvc-c++ -Wall
#CFLAGS=-I$(IDIR)

ODIR=.
#$LDIR =../lib

LIBS=-lm

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ =  main.o SlotMachine.o Reel.o AtkinsMachine.o ran250.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c*  $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

atkins: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
