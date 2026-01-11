#BASEDIR=..
BASEDIR=$(TOPDIR)/elkscmd

include $(BASEDIR)/Makefile-rules

###############################################################################

#LOCALFLAGS =

PRGS = sjisutf8

all: $(PRGS)

sjisutf8: sjis_to_utf8.o
	$(LD) $(LDFLAGS) -maout-heap=0xffff -o sjisutf8 sjis_to_utf8.o $(TINYPRINTF) $(LDLIBS)

install: $(PRGS)
	$(INSTALL) $(PRGS) $(DESTDIR)/bin

clean:
	$(RM) $(PRGS) *.o
