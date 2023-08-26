#!/bin/sh

# Print out the sizes of functions in a linked executable that came
# from libt_cose.a. This doesn't work on ".a" because it gets confused
# by the separate ".o" files in the ".a". This can work on ".o" files,
# but only those are to be part of libqcbor.a

fun=`nm libt_cose.a | awk '/ [TtSs] /{printf "%s|", $3 }'; echo xxxxxxxxxxxxxx`

# It is not possible to get the size of the last symbol because nm
# doesn't output the end of the last item in the list.  This is
# usually not an issue when run against an executable because there is
# only one function of the whole linked library left off.


nm -n -t d -U $1 |\
grep ' [TtSs] ' |\
awk 'NR!=1{printf "%-45s %4s\n", name, $1 - offset }
     {offset=$1; name=$3}' |\
egrep $fun |\
sort -r -n  -k 2 | tee /tmp/sizex_sh.$$

cat /tmp/sizex_sh.$$ | awk '{sum += $2} END {print "total                                         " sum}'

rm -f /tmp/sizex_sh.$$
