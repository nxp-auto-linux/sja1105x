# Copyright 2017 NXP

KERN=4.1.26-rt29
OPT="max_hz=5000 verbosity=1"
rmmod sja1105.ko
insmod /lib/modules/`uname -r`/extra/sja1105.ko ${OPT}
