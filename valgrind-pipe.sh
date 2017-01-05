#!/usr/bin/env bash

BLUE=`echo -e '\033[34m'`
PURPLE=`echo -e '\033[35m'`
BOLD=`echo -e '\033[1m'`
BG_BLUE=`echo -e '\033[46m'`
BG_YELLOW=`echo -e '\033[43m'`
BG_RED=`echo -e '\033[41m'`
GRAY=`echo -e '\033[1;37m'`
DARK_GRAY=`echo -e '\033[1;35m'`
NORMAL=`echo -e '\033[0m'`

SED='sed --unbuffered -r'

while read x; do
    echo -n $x \
        | $SED "s|$|\n|g" \
        | $SED "s/^==[0123456789]+==/$BOLD$BLUE:$NORMAL/g" \
        | $SED "s,(at|by) 0x[^ ]*: ([^(]*),$PURPLE\2$NORMAL,g" \
        | $SED "s,in .*/,in ,g" \
        | $SED "s,\(in [^)]*\),$BOLD&$NORMAL,g" \
        | $SED "s/\\w* SUMMARY/$BOLD&$NORMAL/g"
done

