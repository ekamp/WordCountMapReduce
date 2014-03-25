#!/bin/bash 

# split a file into N pieces, each with the same number of lines

if [ $# != 2 ]; then
	echo "usage $0 filename num_splits" >&2
	exit 1
fi

typeset -i total start perfile part end maxpart

total=`wc -l <"$1"`
maxpart=$2-1
echo total lines = $total
perfile=$total/$2
start=1
part=0

while [ $start -le $total ]; do 
	end=$start+$perfile-1
	if [ $end -gt $total ]; then end=$total; fi
	if [ $part -eq $maxpart ]; then end=$total; fi
	echo sed -n "${start},${end}p" '<'$1 '>'$1.$part
	sed -n "${start},${end}p" <$1 >$1.$part
	part=$part+1
	start=$end+1
done
