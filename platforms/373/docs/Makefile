###############
# Documentation

DOCS=$(patsubst %.tex,%.pdf,$(wildcard *.tex))

doc:	$(DOCS)

%.pdf:	%.tex
	pdflatex $<
	pdflatex $<

clean-doc:
	rm -f *.log *.aux *.out

clean-doc-all: clean-doc
	rm -f *.pdf

clean: clean-doc

.PHONY: doc clean clean-doc clean-doc-all
