CHRPATH=../chrpath

retval=0

rm prog
make prog > /dev/null

if $CHRPATH -l prog | grep -q 'RPATH=/usr/local/lib' ; then
    echo "success: chrpath listed current rpath."
else
    echo "error: chrpath unable to list current rpath."
    retval=1
fi

$CHRPATH -r '/usr/lib' prog > /dev/null

if $CHRPATH -l prog | grep -q 'RPATH=/usr/lib' ; then
    echo "success: chrpath changed rpath."
else
    echo "error: chrpath unable to change rpath."
    retval=1
fi

$CHRPATH -c prog > /dev/null

if $CHRPATH -l prog | grep -q 'RUNPATH=/usr/lib' ; then
    echo "success: chrpath converted rpath to runpath."
else
    echo "error: chrpath unable to convert rpath to runpath."
    retval=1
fi

$CHRPATH -d prog > /dev/null

if $CHRPATH -l prog | grep -q 'no rpath or runpath tag found' ; then
    echo "success: chrpath removed rpath and runpath successfully."
else
    echo "error: chrpath unable to remove rpath or runpath."
    retval=1
fi

exit $retval

