all : gengraphs

gengraphs :
	make -C graphs

finalize :
	make -C graphs finalize

show : 
	make -C .. show

clean :
	rm -f *.log *.dvi *.aux *.toc *.lof *.lot *.out *.bbl *.blg
	make -C graphs clean
