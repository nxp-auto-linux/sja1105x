#/bin/bash
FIRMWARENAME=sja1105p_cfg

mkdir -p output
python cfg_applicationBoard.py
python ethsw/intelhex.py output/cfg_applicationBoard.hex output/cfg_applicationBoard.bin
cp output/cfg_applicationBoard.bin /lib/firmware/$FIRMWARENAME.bin
