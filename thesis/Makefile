all: content thesis 

content: 
	make -C contents

finalize :
	make -C contents finalize

show : finalize thesis
	mupdf thesis.pdf

showplain : finalize thesis
	mupdf thesis.pdf

# LaTeX must be run multiple times to get references right
thesis: thesis.tex $(wildcard *.tex) bibliography.bib
	pdflatex $<
	bibtex thesis
	pdflatex $<
	pdflatex $<

clean:
	rm -f *.log *.dvi *.aux *.toc *.lof *.lot *.out *.bbl *.blg
	rm -f thesis.pdf
	make -C contents clean
