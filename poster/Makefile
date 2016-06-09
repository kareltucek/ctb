all: 
	./process.sh
	pdflatex poster.tex

show : all
	mupdf poster.pdf

clean:
	rm -f *.log *.dvi *.aux *.toc *.lof *.lot *.out *.bbl *.blg *.pdf
	rm -rf figures2

