#!/bin/bash

# 
# Copyright (c) 2020, Laurence Lundblade. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# See BSD-3-Clause license in README.md
#


# TODO: Add code size evaluations
# TODO: add command line options to show more or show less output


function stringpermutations {
    local prefix=$1 # the prefix is the first argument
    local theset=$2 # the set left to process is the second argument
  
    # Output the new prefix
    echo "$prefix"

    # Loop over each item in the set adding it to the prefix
    # All items eventually get added to the prefix and the set
    # goes to nothing. Prefixes are what is output
    for i in $theset; do

        # Make a new prefix by appending the item from the set to it
        local newprefix="$prefix ${i}" 

        # Update the set by removing one more item from it 
        if [[ ! $theset = *[\ ]* ]]; then
            theset=""
        else
            theset=${theset#* }
        fi

        if [[ ! -z "$theset" ]]; then
           # The set is not empty, recurse to process it
           stringpermutations "$newprefix" "$theset"
        else
           # The set is empty, just output the new prefix
           echo "$newprefix"
        fi

        #echo "set:" $set
        #echo "pre:" $pre
    done
}


echo "===================================="

set="-DT_COSE_DISABLE_SHORT_CIRCUIT_SIGN -DT_COSE_DISABLE_CONTENT_TYPE -DT_COSE_DISABLE_ES512 -DT_COSE_DISABLE_ES384"
stringpermutations "" "$set" > /tmp/b.$$

for target_crypto in test ossl psa; 
do
   m="Makefile.$target_crypto"

   while read compile_options; do
      echo "$m $compile_options"
      make -f $m clean > /dev/null
      # Throw away stdout, but not stderr because that's were compiler warnings and erros show
      # All the extra error check options here so they are run before releases
      make -f $m "CMD_LINE=$compile_options -Wall -pedantic-errors -Wextra -Wshadow -Wparentheses -Wconversion -xc -std=c99"   2>&1 >/dev/null | grep -v 'ar: creating'
      ./t_cose_test > /tmp/bb.$$
      grep SUMMARY /tmp/bb.$$
   done < /tmp/b.$$
done
rm -f /tmp/b.$$ /tmp/bb.$$


