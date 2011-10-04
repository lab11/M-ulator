###############
# Documentation

doc:	README.pdf

README.pdf:	README.tex
	pdflatex README.tex
	pdflatex README.tex

clean-doc:
	rm -f README.log README.aux README.out

clean-doc-all: clean-doc
	rm -f README.pdf

clean: clean-doc

.PHONY: doc clean clean-doc clean-doc-all
