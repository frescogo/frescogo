#!/bin/sh

pandoc calendario.md -V geometry:margin=0.7in -V fontsize:11pt -s -o calendario.pdf
pandoc nacional.md   -V geometry:margin=0.7in -V fontsize:11pt -s -o nacional.pdf
convert calendario3.png -resize 25% calendario3.pdf
pdfunite calendario3.pdf calendario.pdf nacional.pdf all.pdf
