#!/bin/sh

fun=`nm libt_cose.a | awk '/ [TtSs] /{printf "%s|", $3 }'; echo xxxxxxxxxxxxxx`

nm -n -t d -U $1 |\
grep ' [TtSs] ' |\
awk 'NR!=1{printf "%-40s %4s\n", name, $1 - offset }
     {offset=$1; name=$3}' |\
egrep $fun | sort -r -n  -k 2 | tee /tmp/qsizes.$$

cat /tmp/qsizes.$$ | awk '{sum += $2} END {print "total                                    " sum}'




