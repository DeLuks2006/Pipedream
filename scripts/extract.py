#!/usr/bin/env python3
# -*- coding:utf-8 -*-
import pefile
import argparse

# Stolen and slightly modified from TitanLdr
# https://github.com/kyleavery/TitanLdr/blob/master/python3/extract.py

end_marker: bytes = b'END-PIPEDREAM'

if __name__ == "__main__":
    parser = argparse.ArgumentParser( description="Extracts shellcode from a PE" )
    parser.add_argument( '-f', required = True, help = 'Path to the source executable', type = str );
    parser.add_argument( '-o', required = True, help = 'Path to store the output raw binary', type = str );
    option = parser.parse_args()

    print('[i] Reading PE')
    binary = pefile.PE( option.f )
    print('[i] Reading first section')
    section = binary.sections[0].get_data()
    
    print('[i] Attempting to find end-marker')
    marker = section.find( end_marker )

    if marker != None:
        print('[i] Carving out the shellcode')
        section_raw = section[ : marker ]
        print('[i] Opening output file')
        file = open( option.o, '+wb' )
        print('[i] Writing Shellcode to output file')
        file.write( section_raw )
        print('[i] Closing file')
        file.close()
    else:
        print('[x] No end marker :/')
