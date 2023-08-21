.PHONY: sync clean

sassafras-manual.pdf: sassafras-manual.tex
	pdflatex sassafras-manual.tex
	pdflatex sassafras-manual.tex

sync:
	cp sassafras.html ../xcode/src/Base.lproj/Sassafras.help/Contents/Resources/English.lproj/index.html

clean:
	rm -f *.aux *.log *.synctex.gz *.toc
