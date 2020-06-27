#!/bin/bash

echo "# Executing latex build..."

python extras/scripts/compile_refs.py main > /dev/null
pdflatex -8bit main.tex > /dev/null
makeindex main.nlo -s nomencl.ist -o main.els
pdflatex -8bit main.tex > /dev/null
# mv main.tex tmp.tex
mv main.pdf PI.pdf
# rm main.*
# mv tmp.tex main.tex

echo "# Done. PDF file saved to .aux/PI.pdf"