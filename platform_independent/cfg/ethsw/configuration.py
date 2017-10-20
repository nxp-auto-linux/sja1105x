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

import struct
import binascii
import intelhex
import prettytable

def make_table_by_id(tableid):
    return Table(tableid=tableid)

def make_table_by_layout(layout, layoutid_map):
    tableid = get_tableid_for_layout(layout, layoutid_map)
    return Table(layout, tableid)

def get_tableid_for_layout(layout, layoutid_map):
    tableids = [t[1] for t in layoutid_map if t[0] == layout]
    if len(tableids) == 0:
        Exception("No table id found for layout")
    assert len(tableids) == 1, "more than two tables for a given layout available: %s" %(str(tableids))
    return tableids[0]

def crc32(bytes):
    return binascii.crc32(bytes)& 0xffffffff

class Field (object):
    def __init__(self, l = None):
        self.name = None
        self.len = 0
        self.value = 0
        self.offset = 0

        if l is not None:
            self.from_list(l)

    def from_list(self, l):
        self.name = l[0]
        self.len = l[1]
        self.value = l[2]

    def __str__(self):
        return "%s: %x, len:%d, offset: %d" %(self.name, self.value, self.len, self.offset)

class Entry(object):
    def __init__(self, layout=None, data = None):
        self.fields = None
        self.len = 0
        self._process_layout(layout)

        if data is not None:
            for key, value in data.items():
                self[key] = value

    def has_key(self, name):
        if self._get_field_by_name(name) == None: return False
        return True

    def _gen_fields(self, layout):
        self.fields = [Field(l) for l in layout]

    def _process_layout(self, layout):
        assert layout is not None

        self._gen_fields(layout)

        bits = sum([field.len for field in self.fields])

        # align MSB of first field to MSB of the next 32 bit boundary
        words = (bits + 32 - 1) // 32
        self.len = words * 32

        top = (words * 32)

        #print self.len

        for field in self.fields:
            field.offset = top - field.len
            top = field.offset
            #print "%s, %d" %(field.name, field.offset)

    def _get_field_by_name(self, name):
        for field in self.fields:
            if field.name == name: return field
        return None

    def __setitem__(self, key, value):
        f = self._get_field_by_name(key)
        if f is None: raise KeyError('no Field %s in layout' % key)
        if not ((type(value) == int) or (type(value) == long)):
            print "Warning type of %s is not int or long but %s" %(key, type(value))
        f.value = value & ((1 << f.len) - 1)
        if value != f.value:
            print "WARNING: %s truncated" % key

    def __getitem__(self, key):
        f = self._get_field_by_name(key)
        if f is None: raise KeyError('no Field %s in layout' % key)
        return f.value

    def __str__(self):
        table = prettytable.PrettyTable(["Name", "Value", "Len", "Offset"])
        for f in self.fields:
            table.add_row((f.name, f.value, f.len, f.offset))
        return table.get_string()
        #s = '\n\t'+'\n\t'.join([str(f) for f in self.fields])
        #return 'ENTRY:' + s

    def __len__(self):
        return len(self.fields)

    def to_bytes(self):
        d = 0
        for field in self.fields:
            d |= field.value << field.offset

        byte_like = list()
        for i in range(self.len / 8):
            byte_like.append((d >> i*8) & 0xff)

        bytes = bytearray(byte_like)

        assert len(bytes) > 0
        return bytes

    def from_bytes(self, bytes):
        d = 0
        for b in reversed(bytes):
            d <<= 8
            d |= b

        for f in self.fields:
            f.value = (d >> f.offset) & ((1 << f.len) - 1)

class Table(object):
    def __init__(self,layout=None, tableid=-1):
        """

        :param tableid: the table id (block id)
        :param layout:  the layout as a list of fields (not an entry)
        :return:
        """
        self.tableid = tableid
        self.entries = list()
        self.layout = layout

        
    def __len__(self):
        return len(self.entries)

    def append(self, entry):
        if isinstance(entry, dict):
            self.entries.append(Entry(self.layout, entry))
        else:
            self.entries.append(entry)

    def to_bytes(self):
        payload_bytes = bytearray()
        for entry in self.entries:
            payload_bytes += entry.to_bytes()

        bytes = bytearray()
        bytes += struct.pack("<I", self.tableid << 24 )
        bytes += struct.pack("<I", len(payload_bytes) / 4 )
        bytes += struct.pack("<I", crc32(bytes))
        bytes += payload_bytes
        bytes += struct.pack("<I", crc32(payload_bytes))

        return bytes

    def _get_layouts_for_id(self, tableid, layoutid_map):
        layouts = [(x[0], x[2]) for x in layoutid_map if x[1] == tableid]
        assert len(layouts) > 0, "no layout found for tableid %d" % (tableid)
        return layouts

    def _select_layout(self, layouts, configuration, bytes):
        if len(layouts) == 1:
            return layouts[0][0]
        else:
            for layout, func in layouts:
                assert func is not None, "no selction function available"
                if func(configuration, bytes) == True: return layout

        return None



    def from_bytes(self, bytes, layoutid_map, configuration):
        # we dont know which layout exactly we see,
        # but to get the size any will do
        self.bytes = bytes

        layouts = self._get_layouts_for_id(self.tableid, layoutid_map)
        if len(layouts) > 1:
            print "INFO: Skipping table %d for now. Retry in second pass" %(self.tableid)
        else:
            self.second_stage(layoutid_map, configuration)

    def second_stage(self, layoutid_map, configuration):
        # we dont know which layout exactly we see,
        # but to get the size any will do

        bytes = self.bytes

        layouts = self._get_layouts_for_id(self.tableid, layoutid_map)

        bytes_per_entry = Entry(layouts[0][0]).len/4
        nentries = len(bytes) / bytes_per_entry


        while len(bytes) > 0:
            # peek at top most word.
            # in case there are multiple layouts, the MSB indicates which to use

            layout = self._select_layout(layouts, configuration, bytes)

            if layout is None:
                raise Exception("No layout for table %d in second pass found" %(self.tableid))

            e = Entry(layout)
            e.from_bytes(bytes[:bytes_per_entry])
            self.append(e)
            bytes = bytes[bytes_per_entry:]


    def __str__(self):
        output = "Table ID: %d #entries: %d\n" %(self.tableid, len(self.entries))

        for idx, entry in enumerate(self.entries):
            output += "======= entry #%d =======\n" %(idx)
            output += str(entry)+"\n"
        return output



class Configuration(object):
    def __init__(self, deviceid=0):
        self.deviceid = deviceid
        self.tables = list()

    def append(self, table):
        self.tables.append(table)

    def to_hex(self, filename):
        bytes = self.to_bytes()

        ihex = intelhex.IntelHex()
        ihex.extend(bytes)
        ihex.writefile(filename, write_start_addr=False, line_size=4)

    def from_hex(self, filename, layoutid_map):
        ihex = intelhex.IntelHex()
        ihex.loadhex(filename)
        bytes = ihex.tobinarray()
        assert len(bytes) % 4 == 0, "Hex file does contain an integer number of bytes"
        self.from_bytes(bytes, layoutid_map)

    def _decode_table(self, bytes, layoutid_map):
        tableid = struct.unpack("<I", bytes[0:4])[0] >> 24
        #print tableid
        length = struct.unpack("<I", bytes[4:8])[0]
        #print length
        crc1 = struct.unpack("<I", bytes[8:12])[0]
        bytes = bytes[12:(4+length)*4]

        table = Table(tableid=tableid)
        table.from_bytes(bytes, layoutid_map, self)
        self.append(table)
        return length

    def peek_device_id_hex(self, filename):
        ihex = intelhex.IntelHex()
        ihex.loadhex(filename)
        bytes = ihex.tobinarray()
        return self.peek_device_id(bytes)

    def peek_device_id(self, bytes):
        return struct.unpack("<I", bytes[0:4])[0]

    def from_bytes(self, bytes, layoutid_map):
        assert len(bytes) % 4 == 0
        print "Total bytes: %d" %(len(bytes))
        self.deviceid = self.peek_device_id(bytes)
        print "Device ID: %08x" % self.deviceid

        bytes = bytes[4:]

        while len(bytes) > 0:
            length = self._decode_table(bytes, layoutid_map)
            bytes = bytes[(4+length)*4:]

        for t in self.tables:
            t.second_stage(layoutid_map, self)

    def to_bytes(self):
        self.tables.sort(key=lambda x: x.tableid)
        # note that we write tables always in order of table ids
        # tttech tries also to do it that way, but fails for some tables
        # this is why the hex output will not look identical

        bytes = bytearray()
        bytes += struct.pack("<I", self.deviceid )

        for table in self.tables:
            bytes += table.to_bytes()

        bytes += struct.pack("<I", 0 )
        bytes += struct.pack("<I", 0 )

        bytes += struct.pack("<I", crc32(bytes))

        return bytes

    def __str__(self):
        output = "Configuration for switch device id: %08X\n" %(self.deviceid)
        for table in self.tables:
            output += str(table)

        return output

