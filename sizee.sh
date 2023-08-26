#!/bin/sh
#
# This prints out the sizes of *all* the functions in the 
# file name given. 
#
# This doesn't work completely correctly on a ".a" file. It works best
# on linked executables where the issue of missing one last symbol is
# usually not an issue. It also works on ".o" files.

# It is not possible to get the size of the last symbol because nm
# doesn't output the end of the last item in the list.  This is
# usually not an issue when run against an executable because there is
# only one function of the whole linked library left off.


nm -n -t d -U $1 |\
grep ' [TtSs] ' |\
awk 'NR!=1{printf "%-45s %4s\n", name, $1 - offset }
     {offset=$1; name=$3}' |\
grep -v __mh_execute |\
sort -r -n  -k 2 | tee /tmp/sizex_sh.$$

cat /tmp/sizex_sh.$$ | awk '{sum += $2} END {print "total                                         " sum}'

rm -f /tmp/sizex_sh.$$
