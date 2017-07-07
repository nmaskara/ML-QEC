#!/bin/bash
for p in 0.05 0.06 0.07 0.08 0.09 0.1 0.11 0.12 0.13 0.14 0.15
do
	./gendata square 5 100000 $p
	python trainmodel.py square 5 100000 $p 100 32
done