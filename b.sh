#!/bin/bash

# 
# Copyright (c) 2020,2022, Laurence Lundblade. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# See BSD-3-Clause license in README.md
#

GCC=/usr/local/bin/gcc-10

# ----- Calculate and display code size ------------------------------

# This compiles the library and links it against t_cose_call_all.c so
# all t_cose code is linked. Then the size of each function and static
# data is displayed along with a total.
#
# This is done for a minimum size where all T_COSE_DISABLEs are set.
# The minimum uses gcc because it generally produces smaller code.
# This is also done for a maximum size.
#
# There are improvements to be made here. This should run for all the
# different crypto libraries because they produce different results
# and because some libraries support things that others don't (e.g.,
# EdDSA) and vice versa.
#
# It would also be nice to give a break down that separates signing,
# encryption, mac and separates the creation from the decoding. For
# example, sign encode, sign decode, sign encode and ecode.

echo "============ sizes ========================"

echo "===== Min ======"
make -f tdv/Makefile.min "CC=$GCC" clean > /dev/null
make -f tdv/Makefile.min > /dev/null
tdv/sizes.sh t_cose_call_all

echo "===== Max ======"
make -f tdv/Makefile.min clean > /dev/null
make -f tdv/Makefile.max > /dev/null
tdv/sizes.sh t_cose_call_all


echo
echo "============ C++ ========================"


# ----- Compile with strict warnings and for c++ ---------------------

# The t_cose makefiles uses a minimum of compiler flags so that it will
# work out-of-the-box with a wide variety of compilers. Here is where
# compilation with a full set of warning flags is performed. This set
# of flags has been thought through for gcc and clang/llvm and gives
# the necessary coverage without being too idiosynchratic and strict.
warn_flags="-Wall"
warn_flags+=" -Wextra"
warn_flags+=" -Wpedantic"
warn_flags+=" -Wshadow"
warn_flags+=" -Wconversion"
warn_flags+=" -Wcast-qual"

cpp_warn_flags="$warn_flags"
cpp_warn_flags+=" -std=c++11"

# Build for C++ 
make -f tdv/Makefile.min clean > /dev/null
make -f tdv/Makefile.max t_cose_call_all_cpp "CMD_LINE=$warn_flags" "CXX_CMD_LINE=$cpp_warn_flags"
#make -f tdv/Makefile.max clean > /dev/null
#make -f tdv/Makefile.max inc_all_ossl "CMD_LINE=$warn_flags" "CXX_CMD_LINE=$cpp_warn_flags"


echo
echo "============ strict compiler flags ========================"

# Add these after the C++ tests
warn_flags+=" -std=c99"
warn_flags+=" -xc"
warn_flags+=" -Wstrict-prototypes"

# Make once with gcc. The big fan out below uses the default compiler.
# If gcc is not available, this check can be skipped. The default
# compiler is used for the big fan out so it always works
make -f Makefile.test clean > /dev/null
make -f Makefile.test "CMD_LINE=$warn_flags" "CC=$GCC" 2>&1 | grep -v 'ar: creating'
make -f Makefile.ossl clean > /dev/null
make -f Makefile.ossl "CMD_LINE=$warn_flags" "CC=$GCC" 2>&1 | grep -v 'ar: creating'
make -f Makefile.psa clean > /dev/null
make -f Makefile.psa "CMD_LINE=$warn_flags" "CC=$GCC" 2>&1 | grep -v 'ar: creating'

echo
echo "============== fan out ======================"


# ----- Function for all #define permutations ------------------------

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
    done
}


# ----- Compile and test all #define permutations --------------------

set="-DT_COSE_DISABLE_SHORT_CIRCUIT_SIGN"
set+=" -DT_COSE_DISABLE_CONTENT_TYPE"
set+=" -DT_COSE_DISABLE_ES512"
set+=" -DT_COSE_DISABLE_ES384"
set+=" -DT_COSE_DISABLE_COSE_SIGN"
set+=" -DT_COSE_DISABLE_AES_KW"
set+=" -DT_COSE_DISABLE_EDDSA"
set+=" -DT_COSE_DISABLE_PS256"
set+=" -DT_COSE_DISABLE_PS384"
set+=" -DT_COSE_DISABLE_PS512"

stringpermutations "" "$set" > /tmp/b.$$

for target_crypto in test ossl psa; 
do
   m="Makefile.$target_crypto"

   while read compile_options; do
      echo "$m $compile_options"
      make -f $m clean > /dev/null
      # Throw away stdout, but not stderr because that's were compiler warnings and erros show
      # All the extra error check options here so they are run before releases
      make -f $m "CMD_LINE=$compile_options $warn_flags" 2>&1 >/dev/null | grep -v 'ar: creating'
      ./t_cose_test > /tmp/bb.$$
      grep SUMMARY /tmp/bb.$$
   done < /tmp/b.$$
done
rm -f /tmp/b.$$ /tmp/bb.$$


