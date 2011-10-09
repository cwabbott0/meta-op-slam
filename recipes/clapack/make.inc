
SHELL = /bin/sh
PLAT = _linux
LOADER = $(CC)
LOADOPTS = $(CLAGS)
NOOPT     = -O0
DRVCFLAGS = $(CFLAGS)
F2CCFLAGS = $(CFLAGS)
ARCH     = ar
ARCHFLAGS= cr
RANLIB   = ranlib
BLASLIB      = ../../libblas.a
LAPACKLIB    = liblapack.a
F2CLIB       = ../../F2CLIBS/libF77.a ../../F2CLIBS/libI77.a
TMGLIB       = tmglib$(PLAT).a
EIGSRCLIB    = eigsrc$(PLAT).a
LINSRCLIB    = linsrc$(PLAT).a
