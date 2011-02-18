#!/bin/sh

# -
# Copyright (c) 2009-2010 by Fstorage Team.
# Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
# -

parallel=4

printf "Cleaning old compilation... "
make -j${parallel} clean 
make -j${parallel} distclean 
printf "done.\n"
printf "Configuring... "
autoreconf -i --force
./configure
printf "done.\n"
printf "Compiling everything... "
make -j${parallel} 
printf "done.\n"
