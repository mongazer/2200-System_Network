#!/bin/bash

for i in `ls ./traces/`; do
	echo ""
	echo "----$i----"
		./vm-sim < ./traces/$i
	echo ""
done;