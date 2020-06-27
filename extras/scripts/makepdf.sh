#!/bin/bash

echo ":::: CLEAN BUILD OF $1 ::::"

rm -f ${1}.toc
rm -f ${1}.run.xml
rm -f ${1}.out
rm -f ${1}.nlo
rm -f ${1}.log
rm -f ${1}.bcf
rm -f ${1}.aux
rm -f ${1}.nls
rm -f ${1}.ilg
rm -f ${1}.idx
rm -f ${1}.blg
rm -f ${1}.bbl

pdflatex ${1}.tex > /dev/null
if [ -e "${1}.bcf" ]
then
  biber -q $1
fi

if [ -e "${1}.nlo" ]
then
  makeindex -q ${1}.nlo -s nomencl.ist -o ${1}.nls
fi

python extras/scripts/compile_refs.py main > /dev/null
pdflatex ${1}.tex > /dev/null
pdflatex ${1}.tex > /dev/null
# cf. https://tex.stackexchange.com/questions/27878
pdflatex -shell-escape -interaction=nonstopmode -file-line-error "${1}.tex" | grep -i ".*:[0-9]*:.*\|warning" 