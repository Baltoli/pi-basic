TEST=$1
shift
exec $TEST 1>&2
