all:
	pdflatex Manuscript.tex
	bibtex Manuscript
	pdflatex Manuscript.tex
	pdflatex Manuscript.tex
	evince Manuscript.pdf &


clean:
	rm -f *.aux
	rm -f *.mtc*