.PHONY: clean

# do pdflatex twice for table of contents

sassafras.pdf: sassafras.tex
	pdflatex sassafras.tex
	pdflatex sassafras.tex
	rm -f *.aux *.log *.synctex.gz *.toc

clean:
	rm -f *.aux *.log *.pdf *.synctex.gz *.toc
