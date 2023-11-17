.PHONY: default clean

HELPBOOK = ../xcode/src/Base.lproj/Sassafras.help/Contents/Resources/English.lproj/index.html

default:
	make sassafras.pdf
	make $(HELPBOOK)

sassafras.pdf: sassafras.tex
	pdflatex sassafras.tex
	pdflatex sassafras.tex
	pdflatex sassafras.tex

$(HELPBOOK): sassafras.html
	cp sassafras.html $(HELPBOOK)

clean:
	rm -f *.aux *.log *.synctex.gz *.toc
