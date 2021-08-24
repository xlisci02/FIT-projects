# projekt: plg-2-nka
# autor: Matus Liscinsky
# login: xlisci02
# mail: xlisci02@stud.fit.vutbr.cz
# datum: 2.4.2020


SRCDIR=src
TESTDIR=test
DOCDIR=doc
MAIN=$(SRCDIR)/Main.hs
PROG=plg-2-nka
ZIPFILES=Makefile $(SRCDIR) $(DOCDIR) $(TESTDIR)

all:
	ghc $(MAIN) --make -o $(PROG) -i$(SRCDIR)
clean:
	rm -rf $(PROG) $(SRCDIR)/*.hi $(SRCDIR)/*.o
test:	all
	python3 $(TESTDIR)/test.py
pack: all clean
	zip flp-fun-xlisci02.zip $(ZIPFILES) -r -q
