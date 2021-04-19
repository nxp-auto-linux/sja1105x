"""
The MIT License (MIT)
Copyright 2017-2021 NXP

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

from ethsw.intelhex import IntelHex
import struct

class Block_Generator(object):
    BlockSize = 64
    WordSize = 4
    
    def __init__(self, filename):
        self.srcfile = filename
        
    def __splitter(self, data, size = WordSize):
        return (data[pos:pos + size] for pos in xrange(0, len(data), size))
    
    def makeBlocks(self):
        blocks = []
        ihex = IntelHex()
        ihex.loadhex(self.srcfile)
        binArr = ihex.tobinarray()
        for chunk in (self.__splitter(binArr, (self.BlockSize*self.WordSize))):
            block = []
            for batch in (self.__splitter(chunk)):
                value = struct.unpack("<I", batch)
                block.append('%08X' % value)
            blocks.append(block)
        return blocks

class Converter:
    """
    Programs the configuration contained in a hex-file.
    @param config_file The hex-file containing the configuration.
    """
    def create_c_code( self, config_files, output_file='../src/NXP_SJA1105P_configStream.c'):
        n_configs = len(config_files)
        file = open( output_file,'w' )
        file.write("""/******************************************************************************
* INCLUDES
*****************************************************************************/
\n""")
        file.write("#include \"NXP_SJA1105P_config.h\"\n")
        file.write("#include \"NXP_SJA1105P_spi.h\"\n\n")        

        defines = "#define CONFIG_BASE_ADDR (0x20000U)  /**< Base address of the configuration area */\n"
        addr = "CONFIG_BASE_ADDR"

        defines += "#define N_CONFIGS %dU  /**< Number of configurations that can be loaded */\n" % n_configs

        functions = "extern uint8_t SJA1105P_loadConfig(uint8_t configIndex, uint8_t switchId)\n"
        functions += "{\n"
        functions += "\tuint8_t ret = 0;\n"
        functions += "\tuint8_t block;\n\n"


        config_list_assignment = []
        for config in range(n_configs):

            config_file = config_files[config]
            block_counter = 0
            config_data_lines = []
            block_length = []
            block_names = []
            blocks = Block_Generator(config_file).makeBlocks()
            for block in blocks:
                block_length.append(len(block))
                block_names.append("configBurst%d_%d" % (config, block_counter))
                line = "static uint32_t " + block_names[-1] + "[%d] = {" % (len(block))
                for word in block:
                    line += "0x%sU, " % word
                line = line[:-2] + "};"
                config_data_lines.append(line)
                config_list_assignment.append("p_configBurstList%d[%d] = " % (config, block_counter) + block_names[-1] + ";")
                block_counter += 1
            config_list_assignment.append("")
            functions += ("\t/* Automatically generated from " + config_file + " */\n")    
            defines += "#define N_BURSTS_CONFIG%d %dU  /**< Number of bursts in configuration %d */\n" % (config, block_counter, config)
            block_length_string = (', '.join(map(str, block_length)))
            block_pointer_string = (', '.join(block_names))
            functions += "\tuint32_t *p_configBurstList%d[N_BURSTS_CONFIG%d" % (config, config) + "];\n"
            functions += "\tconst uint8_t  k_burstLength%d[N_BURSTS_CONFIG%d] = {" % (config, config) + block_length_string + "};\n"
            for line in config_data_lines:
                functions += "\t" + line + "\n"
            functions += "\n"

        functions += "\tuint32_t **pp_configBurstList[N_CONFIGS];\n"
        functions += "\tconst uint8_t *kp_burstLength[N_CONFIGS];\n"

        functions += "\tconst uint8_t k_nBursts[N_CONFIGS] = {"
        for config in range(n_configs):
            functions += "N_BURSTS_CONFIG%d, " % config
        functions = functions[:-2] + "};\n\n"

        for line in config_list_assignment:
            functions += "\t" + line + "\n"

        for config in range(n_configs):
            functions += "\tpp_configBurstList[%d] = p_configBurstList%d;\n" % (config, config)
        functions += "\n"

        for config in range(n_configs):
            functions += "\tkp_burstLength[%d] = k_burstLength%d;\n" % (config, config)
        functions += "\n"

        functions += ("\tfor (block = 0; block < k_nBursts[configIndex]; block++)\n\t{\n")
        functions += ("\t\tif (SJA1105P_gpf_spiWrite32(switchId, kp_burstLength[configIndex][block], %s, pp_configBurstList[configIndex][block]) != 0U)\n\t\t{\n") % (addr + " + block")
        functions += "\t\t\tret = 1;\n"
        functions += "\t\t\tbreak;  /* configuration was unsuccessful */\n\t\t}\n\t}\n"

        functions += "\n\treturn ret;\n"
        functions += "}\n"

        file.write("""/******************************************************************************
* DEFINES
*****************************************************************************/
\n""")
        file.write(defines + "\n")
        file.write("""/******************************************************************************
* FUNCTIONS
*****************************************************************************/
\n""")
        file.write(functions)
        
        file.flush();
        file.close()
        
# Converter().create_c_code(["SJA1105P_ReferenceBoard_switch0.hex", "SJA1105P_ReferenceBoard_switch1.hex", "SJA1105Q_ReferenceBoard_switch0.hex", "SJA1105Q_ReferenceBoard_switch1.hex", "SJA1105R_ReferenceBoard_switch0.hex", "SJA1105R_ReferenceBoard_switch1.hex", "SJA1105S_ReferenceBoard_switch0.hex", "SJA1105S_ReferenceBoard_switch1.hex"], "test.c")
# Converter().create_c_code(["examplePQRS_SJA1105PR.hex", "examplePQRS_SJA1105QS.hex"], "hex_to_c.c")
