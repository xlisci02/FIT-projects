#FILE		Makefile
#Author		Matus Liscinsky
#Login		xlisci02
#Mail		xlisci02@stud.fit.vutbr.cz
#Date		14.03.2017

NAME=proj2

all:
	latex $(NAME).tex
	latex $(NAME).tex
	dvips -t a4 $(NAME).dvi
	ps2pdf $(NAME).ps
clean:
	-rm -f $(NAME).aux $(NAME).ps $(NAME).dvi $(NAME).log				
