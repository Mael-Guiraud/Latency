

all:
	pdflatex Manuscript.tex
	bibtex Manuscript
	pdflatex Manuscript.tex
	pdflatex Manuscript.tex
	pdftk couv.pdf Manuscript.pdf cat output Manuscrit.pdf
ifeq ($(USER),mael)
	echo $(USER)
	cp Manuscrit.pdf ../../Mael-Guiraud.github.io
	cd ../../Mael-Guiraud.github.io  && git commit -am "autocommit"  && git push; 
endif
	evince Manuscrit.pdf &

clean:
	rm -f *.aux
	rm -f *.mtc*
	rm -f Manuscript.bbl
	rm -f Manuscript.bbl
	rm -f Manuscript.blg
	rm -f Manuscript.maf
	rm -f Manuscript.out
	rm -f Manuscript.run.xml
	rm -f Manuscript.toc
	rm -f Manuscript-blx.bib
