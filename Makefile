
# Makefile for all

include		./Makefile.ini

all:
	cd src; make
	cd symbreg; make
	cd lawn; make
	cd skeleton; make
	cd ant; make
	cd crawling; make

clean:
	rm -f *~
	cd src; make clean
	cd symbreg; make clean
	cd lawn; make clean
	cd skeleton; make clean
	cd ant; make clean
	cd crawling; make clean

superclean: 
	rm -f *~
	cd src; make superclean
	cd symbreg; make superclean
	cd lawn; make superclean
	cd skeleton; make superclean
	cd ant; make superclean
	cd crawling; make superclean

# Strip debug information (if there is any). 
strip:
	cd src; make strip
	cd lawn; make strip
	cd skeleton; make strip
	cd ant; make strip
	cd symbreg; make strip
	cd crawling; make strip

install:
	$(INSTALL) -d $(INSTALLDIR)/include
	$(INSTALL) -d $(INSTALLDIR)/lib
	$(INSTALL) -f $(INSTALLDIR)/lib lib/$(GPLIBFILE)
	$(INSTALL) -f $(INSTALLDIR)/include include/gp.h
	$(INSTALL) -f $(INSTALLDIR)/include include/gpconfig.h

