

all:
	pdflatex Manuscript.tex
	bibtex Manuscript
	pdflatex Manuscript.tex
	pdflatex Manuscript.tex
	evince Manuscript.pdf &
ifeq ($(USER),mael)
	echo $(USER)
	cp Manuscript.pdf ../../Mael-Guiraud.github.io
	cd ../../Mael-Guiraud.github.io  && git commit -am "autocommit"  && git push; 
endif

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
