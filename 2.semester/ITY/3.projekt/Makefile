#File		Makefile
#Author		Matus Liscinsky
#Login		xlisci02
#Mail		xlisci02@stud.fit.vutbr.cz
#Date		01.04.2017

NAME=proj3

all:
	latex $(NAME).tex
	latex $(NAME).tex
	dvips -t a4 $(NAME).dvi
	ps2pdf -sPAPERSIZE=a4 $(NAME).ps
clean:
	-rm -f $(NAME).aux $(NAME).ps $(NAME).dvi $(NAME).log				
