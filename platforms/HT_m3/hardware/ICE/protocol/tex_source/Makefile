PDFS = ../protocol.pdf ../old-protocol.pdf

all:	$(PDFS)

../old-protocol.pdf:	old-protocol.tex
	pdflatex $<
	pdflatex $<
	mv old-protocol.pdf ../

../protocol.pdf:	protocol.tex $(wildcard protocol-*.tex)
	pdflatex $<
	pdflatex $<
	pdflatex $<
	pdflatex $<
	mv protocol.pdf ../

clean:
	rm -f *.aux *.log *.out *.toc
	rm -f $(PDFS)
