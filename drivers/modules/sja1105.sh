# Copyright 2017 NXP

#!/bin/sh
### BEGIN INIT INFO
# Provides:          sja1105
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     3 5
# Default-Stop:
# Short-Description: Loading sja1105 module
# Description:       Inserts SJA1105PQRS kernel module. This enables ethernet.
### END INIT INFO

OPT="max_hz=5000 verbosity=1"
lsmod | grep sja1105 > /dev/null && rmmod sja1105
insmod /lib/modules/`uname -r`/extra/sja1105.ko ${OPT}
