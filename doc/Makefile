.PHONY: clean update

# do pdflatex twice for table of contents

sassafras.pdf: sassafras.tex
	pdflatex sassafras.tex
	pdflatex sassafras.tex
	rm -f *.aux *.log *.synctex.gz *.toc

clean:
	rm -f *.aux *.log *.pdf *.synctex.gz *.toc

update:
	cp sassafras.html ../xcode/src/Base.lproj/Sassafras.help/Contents/Resources/English.lproj/index.html
