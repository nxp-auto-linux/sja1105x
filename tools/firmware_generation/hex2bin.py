      
"""
This converter class is used to convert Intel HEX config files to binary file
to be loaded as a firmware.
"""
from struct import *
#from socket import *
from struct import pack
from struct import unpack
from time import sleep

    


class ConfigHexFile(object):
    class IntelHexLine(object):
        def __init__(self, intel_hex_line):
            self.start_symbol = intel_hex_line[0]
            self.record_type = None
            self.byte_count = None
            self.address = None
            self.data = None
            self.checksum = None
            if self.start_symbol == ':':
                self.byte_count = int(intel_hex_line[1:3],16)
                self.address = intel_hex_line[3:7]
                self.record_type = intel_hex_line[7:9]
                self.checksum = intel_hex_line[(9 + 2 * self.byte_count):(11 + 2 * self.byte_count)]
                if self.record_type == "00":
                    self.data = ""
                    for i in range(0, self.byte_count):
                        self.data = self.data + intel_hex_line[7 + 2 * (self.byte_count - i):9 + 2 * (self.byte_count - i)]

        def __str__(self):
            return "IntelHexLine type=%s bytes=%d addr=%s data=%s chk=%s" % (self.record_type, self.byte_count, self.address, self.data, self.checksum)
        def get_address(self):
            return int(self.address, 16)
        def get_byte_count(self):
            return self.byte_count

    def __init__(self, filename):
        """
        ConfigHexFile  stores a switch configuration.
        @param filename  Filename of a intel hex file containing the configuration.
        """
        self.lines = []
        with open(filename) as f:
            for line in f:
                self.lines.append(self.IntelHexLine(line))
                #print(self.lines[len(self.lines) - 1])

    def count_data_words(self):
        """
        Get number of data words read from the configuration file.
        These are the records of type "00" in the input file.
        @return number of data words in configuration file
        """
        num_words = 0
        for line in self.lines:
            if line.record_type == "00":
                num_words = num_words + 1
        return num_words


class Hex2BinConverter:
    """
    Programs the configuration contained in a hex-file.
    @param config_file The hex-file containing the configuration.
    @param be1_le0 format of output:  1 for BigEndian 0 for LittleEndian  
    """


    def create_bin_file(self, config_file, output_file, be1_le0=0):


        file = open(output_file, 'wb') # open as binary 
       
        #split HEX file in structured lines
        cfg = ConfigHexFile(config_file)
        offset = 0
        expected_offset = 0 
        for line in cfg.lines:
          if line.record_type == "00":
            offset = line.get_address()
            if offset != expected_offset:
                print("Error in Hex file at offset {} expected {}\n".format(offset, expected_offset))
            else:
                byte_count = line.get_byte_count()
                expected_offset = offset + byte_count
                if (byte_count & 3) != 0:
                    print("Error lines are expected to contain whole numbers of 32 bit words\n")
                else:
                    word_count = byte_count // 4
                    for i in range (word_count):
                        val = 0
                        if be1_le0:
                            # write as big enddian
                            val = pack('>4B', 
                                       int(line.data[4*i+0 : 4*i+2],16), 
                                       int(line.data[4*i+4 : 4*i+6],16), 
                                       int(line.data[4*i+2 : 4*i+4],16), 
                                       int(line.data[4*i+6 : 4*i+8],16) )
                        else:
                            # write as little enddian
                            val = pack('<4B', 
                                       int(line.data[4*i+6 : 4*i+8],16), 
                                       int(line.data[4*i+4 : 4*i+6],16), 
                                       int(line.data[4*i+2 : 4*i+4],16), 
                                       int(line.data[4*i+0 : 4*i+2],16) )
                        
                        file.write(val)


        file.flush();
        file.close()
    

