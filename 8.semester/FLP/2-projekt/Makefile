# projekt: flp20-log, rubikova kocka
# autor: Matus Liscinsky
# login: xlisci02
# mail: xlisci02@stud.fit.vutbr.cz
# datum: 26.4.2020


PROG=flp20-log
GOAL=start
TESTDIR=test
ZIPFILES=Makefile README.md $(PROG).pl $(TESTDIR)
# $(SRCDIR) $(DOCDIR) $(TESTDIR)

all:
	swipl -q -g $(GOAL) -o $(PROG) -c $(PROG).pl
clean:
	rm $(PROG)
test:	all
	python3 $(TESTDIR)/test.py
pack:	all clean
	zip flp-log-xlisci02.zip $(ZIPFILES) -r -q
