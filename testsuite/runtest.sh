#!/bin/sh

set -x

CHRPATH=../chrpath

retval=0

rm prog
make prog

if $CHRPATH -h ; then
    echo "success: chrpath -h worked."
else
    echo "error: chrpath -h failed."
    retval=1
fi

if $CHRPATH $0 ; then
    echo "error: chrpath on /bin/sh script worked."
    retval=1
else
    echo "success: chrpath on /bin/sh script failed."
fi

if $CHRPATH non-existant file ; then
    echo "error: chrpath on non-existant file worked."
    retval=1
else
    echo "success: chrpath on non-existant file failed."
fi

if $CHRPATH -l prog | grep -q 'R.*PATH=.*/usr/local/lib' ; then
    echo "success: chrpath listed current rpath."
else
    echo "error: chrpath unable to list current rpath."
    retval=1
fi

$CHRPATH -r '/usr/lib' prog > /dev/null

if $CHRPATH -l prog | grep -q 'R.*PATH=/usr/lib' ; then
    echo "success: chrpath changed rpath."
else
    echo "error: chrpath unable to change rpath."
    retval=1
fi

$CHRPATH -c prog > /dev/null

if $CHRPATH -l prog | grep -q 'RUNPATH=/usr/lib' ; then
    echo "success: chrpath converted rpath to runpath."
else
    # Not all archs support runpath, ie not a fatal error
    echo "warning: chrpath unable to convert rpath to runpath."
fi

$CHRPATH -d prog > /dev/null

if $CHRPATH -l prog | grep -q 'no rpath or runpath tag found' ; then
    echo "success: chrpath removed rpath and runpath successfully."
else
    echo "error: chrpath unable to remove rpath or runpath."
    retval=1
fi

exit $retval

