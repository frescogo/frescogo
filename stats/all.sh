#!/bin/sh

# ./all.sh ../Jogos/20190405/
# :%!sort -r -k3

for i in $1/serial_*.txt; do
    #echo $i
    base=`basename $i .txt`
    lua5.3  parse.lua $i /tmp
    python3 histogram.py "/tmp/$base.py" $1
done
