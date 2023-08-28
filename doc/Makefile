.PHONY: sync clean

sassafras.pdf: sassafras.tex
	pdflatex sassafras.tex
	pdflatex sassafras.tex

sync:
	cp sassafras.html ../xcode/src/Base.lproj/Sassafras.help/Contents/Resources/English.lproj/index.html

clean:
	rm -f *.aux *.log *.synctex.gz *.toc
