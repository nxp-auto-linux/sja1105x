"""
The MIT License (MIT)
Copyright (c) 2017 NXP

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.

"""

import os
import argparse

from ethsw.configuration import Configuration
import ethsw.tables_sja1105pqrs
import ethsw.tables_sja1105

# Arguments parser
parser = argparse.ArgumentParser()
parser.add_argument("--hex", help="Hex file to load", default='simpleT_SJA1105T.hex')
args = parser.parse_args()


c = Configuration()
device_id = c.peek_device_id_hex(args.hex)

SJA1105QS_DEVICEID = 0xae00030e
SJA1105PR_DEVICEID = 0xaf00030e
SJA1105_DEVICEID = 0x9f00030e
SJA1105T_DEVICEID = 0x9e00030e

table_map = {SJA1105QS_DEVICEID: ethsw.tables_sja1105pqrs.layoutid_map,
            SJA1105PR_DEVICEID: ethsw.tables_sja1105pqrs.layoutid_map,
            SJA1105_DEVICEID: ethsw.tables_sja1105.layoutid_map,
            SJA1105T_DEVICEID: ethsw.tables_sja1105.layoutid_map,}

c.from_hex(args.hex, table_map[device_id])

print c