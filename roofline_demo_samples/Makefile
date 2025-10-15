SOURCEDIR=src/
OUTDIR=release/
CXX=icpc
INCLUDES=
CXXFLAGS=-c -O2 -std=c++0x -march=alderlake -g -qopenmp -qopt-report5 -diag-disable=10441 $(INCLUDES)

PROGS=$(OUTDIR)roofline_demo

.PHONY: default
default: $(OUTDIR)roofline.o
	$(CXX) -diag-disable=10441 -qopenmp $(OUTDIR)roofline.o -o $(OUTDIR)roofline_demo $(LIBS)

$(OUTDIR)roofline.o: $(OUTDIR) $(SOURCEDIR)roofline.cpp
	$(CXX) $(CXXFLAGS) -o $@ $(SOURCEDIR)roofline.cpp

$(OUTDIR):
	mkdir $(OUTDIR)

.PHONY: clean
clean:
	rm -r $(OUTDIR)
