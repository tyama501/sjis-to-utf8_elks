#BASEDIR=..
BASEDIR=$(TOPDIR)/elkscmd

include $(BASEDIR)/Make.defs

###############################################################################
#
# Include standard packaging commands.

include $(BASEDIR)/Make.rules

###############################################################################

LOCALFLAGS=-D_POSIX_SOURCE

PRGS = sjisutf8

all: $(PRGS)

sjisutf8: sjis_to_utf8.o
	$(LD) $(LDFLAGS) -o sjisutf8 sjis_to_utf8.o $(LDLIBS)

install: $(PRGS)
	$(INSTALL) $(PRGS) $(DESTDIR)/bin

clean:
	rm -f $(PRGS) *.o
