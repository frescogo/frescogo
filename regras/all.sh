#!/bin/sh

#pandoc regra-150.md -H deeplists.tex -o /tmp/x.pdf
#pdftoppm /tmp/x.pdf /tmp/x -png
#convert /tmp/x-1.png -trim /tmp/x.png
#convert /tmp/x.png -bordercolor White -border 8 regra-150.png
#eog regra-150.png

#pandoc regra-240.md -H deeplists.tex -o /tmp/x.pdf
#pdftoppm /tmp/x.pdf /tmp/x -png
#convert /tmp/x-1.png -trim /tmp/x.png
#convert /tmp/x.png -bordercolor White -border 8 regra-240.png
#eog regra-300.png

#pandoc regra-150-en.md -H deeplists.tex -o /tmp/x.pdf
#pdftoppm /tmp/x.pdf /tmp/x -png
#convert /tmp/x-1.png -trim /tmp/x.png
#convert /tmp/x.png -bordercolor White -border 8 regra-150-en.png
#eog regra-150-en.png

#pandoc regra-300-en.md -H deeplists.tex -o /tmp/x.pdf
#pdftoppm /tmp/x.pdf /tmp/x -png
#convert /tmp/x-1.png -trim /tmp/x.png
#convert /tmp/x.png -bordercolor White -border 8 regra-300-en.png
#eog regra-300-en.png

pandoc simples-240.md -H deeplists.tex -o /tmp/x.pdf
pdftoppm /tmp/x.pdf /tmp/x -png
convert /tmp/x-1.png -trim /tmp/x.png
convert /tmp/x.png -bordercolor White -border 8 simples-240.png
#eog simples-300.png

#pandoc simples-150.md -H deeplists.tex -o /tmp/x.pdf
#pdftoppm /tmp/x.pdf /tmp/x -png
#convert /tmp/x-1.png -trim /tmp/x.png
#convert /tmp/x.png -bordercolor White -border 8 simples-150.png
#eog simples-150.png
