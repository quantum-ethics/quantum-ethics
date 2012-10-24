# This is a Unix makefile.  To make a postscript file from 'filename.C' do
#   make F=filename
# notice that there is no '.C' in the above.  This will create the
# executable, run it to generate 'filename.ps' and then remove the
# executable.

CC=g++
CFLAGS=-I/usr/local/include -Wno-deprecated
LINKFLAGS=-lFD -lm

$(F).ps: $(F)
	./$(F) >$(F).ps
	rm $(F)

$(F): $(F).C
	$(CC) $(CFLAGS) -o $(F) $(F).C $(LINKFLAGS)

all:
	make F=Bdecay
	make F=electron_selfenergy
	make F=op_prod_expansion
	make F=penguin
	make F=photon6
	make F=program1
	make F=program2
	make F=program3
	make F=program4
	make F=program5
	make F=program6
	make F=program7
	make F=program8
	make F=program9
	make F=program10

clean:
	rm Bdecay.ps electron_selfenergy.ps op_prod_expansion.ps
	rm penguin.ps photon6.ps
	rm program1.ps program2.ps program3.ps program4.ps
	rm program5.ps program6.ps program7.ps program8.ps
	rm program9.ps program10.ps
