############################################################################
# file:			Makefile																
# authors:	Matúš Liščinský xlisci02								
#						Vladimír Marcin xmarci10									
############################################################################

compile:
	cd src && $(MAKE) && mv blockeditor ../blockeditor

run: compile
	./blockeditor

clean: 
	rm -rf ./doc/* xlisci02-xmarci10.zip 
	cd src && make clean

doxygen:
	cd src && doxygen Doxyfile

pack: clean
	zip xlisci02-xmarci10.zip -r src doc examples README.txt Makefile
