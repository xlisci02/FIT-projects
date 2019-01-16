#File		Makefile
#Author		Matúš Liščinský 
#Login		xlisci02
#Mail		xlisci02@stud.fit.vutbr.cz
#Date		15.04.2017

NAME=proj4

all:
	latex $(NAME).tex
	bibtex $(NAME).aux
	latex $(NAME).tex
	latex $(NAME).tex
	dvips -t a4 $(NAME).dvi
	ps2pdf $(NAME).ps

clean:
	rm -f $(NAME).aux $(NAME).dvi $(NAME).ps $(NAME).bbl $(NAME).blg $(NAME).log
