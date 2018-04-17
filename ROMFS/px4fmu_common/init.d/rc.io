#
# Init PX4IO interface
#

echo "Executing rc.io"

#
# Declare local script variables and initialize default values
#

#
# Allow PX4IO to recover from midair restarts.
#
px4io recovery

#
# Adjust PX4IO update rate limit
#
set PX4IO_LIMIT 400

if px4io limit ${PX4IO_LIMIT}
then
fi

unset PX4IO_LIMIT