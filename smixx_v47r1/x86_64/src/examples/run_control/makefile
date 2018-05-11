#
# Makefile: builds the Proxies for RUN_CON
#

CC = cc
CXX = CC
MAKE = gmake
CFLAGS = $(FLAGS) -g -I. -I$(DIMDIR)/dim -I$(SMIDIR)/smixx
EXTRALIBS =

ifeq ($(OS),Solaris)
	EXTRALIBS = -lpthread -lposix4 -lsocket -lnsl
	FLAGS += -mt
endif
ifeq ($(OS),Linux)
	EXTRALIBS = -lpthread
	CXX = g++
endif

all:	$(ODIR)/logger	$(ODIR)/evt_builder

$(ODIR)/logger : logger.c
	$(CC) $(CFLAGS) logger.c -L$(SMIDIR)/$(ODIR) -lsmi -L$(DIMDIR)/$(ODIR) -ldim $(EXTRALIBS) -o $(ODIR)/logger
$(ODIR)/evt_builder : evt_builder.c
	$(CC) $(CFLAGS) evt_builder.c -L$(SMIDIR)/$(ODIR) -lsmi -L$(DIMDIR)/$(ODIR) -ldim $(EXTRALIBS) -o $(ODIR)/evt_builder

$(ODIR)/logger_cpp : logger_cpp.cxx
	$(CXX) $(CFLAGS) logger_cpp.cxx -L$(SMIDIR)/$(ODIR) -lsmi -L$(DIMDIR)/$(ODIR) -ldim $(EXTRALIBS) -o $(ODIR)/logger_cpp
$(ODIR)/evt_builder_cpp : evt_builder_cpp.cxx
	$(CXX) $(CFLAGS) evt_builder_cpp.cxx -L$(SMIDIR)/$(ODIR) -lsmi -L$(DIMDIR)/$(ODIR) -ldim $(EXTRALIBS) -o $(ODIR)/evt_builder_cpp
