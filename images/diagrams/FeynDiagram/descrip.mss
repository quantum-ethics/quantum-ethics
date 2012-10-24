!
!  This is the VMS/MMS description file for FeynDiagram.
!
FD=adk_root:[utilities.feyndiagram
INCLUDEDIR=$(FD)]
LIBDIR=$(FD)]
EXAMPLES=$(FD).examples]
PROLOGDIR=$(FD).prolog]
LIBNAME=$(LIBDIR)FD.OLB
CXXFLAGS=
CLIBFLAGS=/DEFINE=("PROLOGDIR=""$(PROLOGDIR)""",NOOSFCN)$(CXXFLAGS)

.SUFFIXES
.SUFFIXES .PS .EXE .OBJ .CXX

.EXE.PS
	@ DEFINE/USER_MODE SYS$OUTPUT $(MMS$TARGET)
	RUN $(MMS$SOURCE)

.OBJ.EXE
	LINK/EXECUTABLE=$(MMS$TARGET) $(MMS$SOURCE),$(LIBNAME)/LIBRARY

.CXX.OBJ
	@ DEFINE/USER_MODE feyndiagram $(INCLUDEDIR)
	$(CXX)$(CXXFLAGS)/OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

examples : $(EXAMPLES)program1.ps, -
	   $(EXAMPLES)program2.ps, -
	   $(EXAMPLES)program3.ps, -
	   $(EXAMPLES)program4.ps, -
	   $(EXAMPLES)program5.ps, -
	   $(EXAMPLES)program6.ps, -
	   $(EXAMPLES)program7.ps, -
	   $(EXAMPLES)program8.ps, -
	   $(EXAMPLES)program9.ps, -
	   $(EXAMPLES)program10.ps, -
	   $(EXAMPLES)penguin.ps, -
	   $(EXAMPLES)bdecay.ps, -
	   $(EXAMPLES)electron_selfenergy.ps, -
	   $(EXAMPLES)op_prod_expansion.ps
	@ WRITE SYS$OUTPUT "$(MMS$TARGET) built"

$(EXAMPLES)program1.ps : $(EXAMPLES)program1.exe
$(EXAMPLES)program2.ps : $(EXAMPLES)program2.exe
$(EXAMPLES)program3.ps : $(EXAMPLES)program3.exe
$(EXAMPLES)program4.ps : $(EXAMPLES)program4.exe
$(EXAMPLES)program5.ps : $(EXAMPLES)program5.exe
$(EXAMPLES)program6.ps : $(EXAMPLES)program6.exe
$(EXAMPLES)program7.ps : $(EXAMPLES)program7.exe
$(EXAMPLES)program8.ps : $(EXAMPLES)program8.exe
$(EXAMPLES)program9.ps : $(EXAMPLES)program9.exe
$(EXAMPLES)program10.ps : $(EXAMPLES)program10.exe
$(EXAMPLES)penguin.ps : $(EXAMPLES)penguin.exe
$(EXAMPLES)bdecay.ps : $(EXAMPLES)bdecay.exe
$(EXAMPLES)electron_selfenergy.ps : $(EXAMPLES)electron_selfenergy.exe
$(EXAMPLES)op_prod_expansion.ps : $(EXAMPLES)op_prod_expansion.exe

$(EXAMPLES)program1.exe : $(EXAMPLES)program1.obj, $(LIBNAME)
$(EXAMPLES)program2.exe : $(EXAMPLES)program2.obj, $(LIBNAME)
$(EXAMPLES)program3.exe : $(EXAMPLES)program3.obj, $(LIBNAME)
$(EXAMPLES)program4.exe : $(EXAMPLES)program4.obj, $(LIBNAME)
$(EXAMPLES)program5.exe : $(EXAMPLES)program5.obj, $(LIBNAME)
$(EXAMPLES)program6.exe : $(EXAMPLES)program6.obj, $(LIBNAME)
$(EXAMPLES)program7.exe : $(EXAMPLES)program7.obj, $(LIBNAME)
$(EXAMPLES)program8.exe : $(EXAMPLES)program8.obj, $(LIBNAME)
$(EXAMPLES)program9.exe : $(EXAMPLES)program9.obj, $(LIBNAME)
$(EXAMPLES)program10.exe : $(EXAMPLES)program10.obj, $(LIBNAME)
$(EXAMPLES)penguin.exe : $(EXAMPLES)penguin.obj, $(LIBNAME)
$(EXAMPLES)bdecay.exe : $(EXAMPLES)bdecay.obj, $(LIBNAME)
$(EXAMPLES)electron_selfenergy.exe : $(EXAMPLES)electron_selfenergy.obj, -
					$(LIBNAME)
$(EXAMPLES)op_prod_expansion.exe : $(EXAMPLES)op_prod_expansion.obj, $(LIBNAME)

$(EXAMPLES)program1.obj : $(EXAMPLES)program1.cxx, $(INCLUDEDIR)fd.h
$(EXAMPLES)program2.obj : $(EXAMPLES)program2.cxx, $(INCLUDEDIR)fd.h
$(EXAMPLES)program3.obj : $(EXAMPLES)program3.cxx, $(INCLUDEDIR)fd.h
$(EXAMPLES)program4.obj : $(EXAMPLES)program4.cxx, $(INCLUDEDIR)fd.h
$(EXAMPLES)program5.obj : $(EXAMPLES)program5.cxx, $(INCLUDEDIR)fd.h
$(EXAMPLES)program6.obj : $(EXAMPLES)program6.cxx, $(INCLUDEDIR)fd.h
$(EXAMPLES)program7.obj : $(EXAMPLES)program7.cxx, $(INCLUDEDIR)fd.h
$(EXAMPLES)program8.obj : $(EXAMPLES)program8.cxx, $(INCLUDEDIR)fd.h
$(EXAMPLES)program9.obj : $(EXAMPLES)program9.cxx, $(INCLUDEDIR)fd.h
$(EXAMPLES)program10.obj : $(EXAMPLES)program10.cxx, $(INCLUDEDIR)fd.h
$(EXAMPLES)penguin.obj : $(EXAMPLES)penguin.cxx, $(INCLUDEDIR)fd.h
$(EXAMPLES)bdecay.obj : $(EXAMPLES)bdecay.cxx, $(INCLUDEDIR)fd.h
$(EXAMPLES)electron_selfenergy.obj : $(EXAMPLES)electron_selfenergy.cxx, -
					$(INCLUDEDIR)fd.h
$(EXAMPLES)op_prod_expansion.obj : $(EXAMPLES)op_prod_expansion.cxx, -
					$(INCLUDEDIR)fd.h

$(LIBNAME) :: default.obj
	IF F$SEARCH("$(MMS$TARGET)") .EQS. "" THEN LIBRARY/CREATE $(MMS$TARGET)
	LIBRARY/LOG $(MMS$TARGET) $(MMS$SOURCE)

$(LIBNAME) :: fd.obj
	IF F$SEARCH("$(MMS$TARGET)") .EQS. "" THEN LIBRARY/CREATE $(MMS$TARGET)
	LIBRARY/LOG $(MMS$TARGET) $(MMS$SOURCE)

$(LIBNAME) :: text2PS.obj
	IF F$SEARCH("$(MMS$TARGET)") .EQS. "" THEN LIBRARY/CREATE $(MMS$TARGET)
	LIBRARY/LOG $(MMS$TARGET) $(MMS$SOURCE)

default.obj : default.cxx default.h
	@ DEFINE/USER_MODE feyndiagram $(INCLUDEDIR)
	$(CXX)$(CLIBFLAGS) default

fd.obj : fd.cxx fd.h text2PS.h
	@ DEFINE/USER_MODE feyndiagram $(INCLUDEDIR)
	$(CXX)$(CLIBFLAGS) fd

text2PS.obj : text2PS.cxx text2PS.h
	@ DEFINE/USER_MODE feyndiagram $(INCLUDEDIR)
	$(CXX)$(CLIBFLAGS) text2PS


