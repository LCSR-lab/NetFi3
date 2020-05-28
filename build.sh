#!/bin/bash

echo "# Executing latex build..."

pdflatex -output-directory=.aux main.tex >/dev/null


echo "# Done. PDF file saved to .aux/main.pdf"