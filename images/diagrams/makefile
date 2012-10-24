CC=g++
CFLAGS=-I/usr/local/include -Wno-deprecated
LINKFLAGS=-lFD -lm

$(F).ps: $(F)
	./$(F) >$(F).ps
	rm $(F)

$(F): $(F).C
	$(CC) $(CFLAGS) -o $(F) $(F).C $(LINKFLAGS)

all:
	make F=Coulomb_Scattering
	make F=Vacuum_Energy_2
	make F=Charged_Fermion_Energy_2a
	make F=Charged_Fermion_Energy_2b
	make F=Photon_Energy_2a
	make F=Photon_Energy_2b

clean:
	rm Coulomb_Scattering.ps
	rm Vacuum_Energy_2.ps
	rm Charged_Fermion_Energy_2a.ps
	rm Charged_Fermion_Energy_2b.ps
	rm Photon_Energy_2a.ps
	rm Photon_Energy_2b.ps

