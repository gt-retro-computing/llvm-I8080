#!/usr/bin/env python
from __future__ import print_function, division

import sys

def to_intel_hex(memory, width):
    record_type = "00"
    output = ""
    for address, data in enumerate(memory):
        hex_data = "{:02X}".format(data).zfill(width)

        checksum = width // 2
        lol_address = address
        while lol_address > 0:
            checksum = checksum + lol_address & 0xff
            lol_address = lol_address >> 8

        while data > 0:
            checksum = checksum + data & 0xff
            data = data >> 8

        checksum = (-checksum) & 0xff

        line = ":{:02X}{:04X}{}{}{:02X}\n".format(width // 2, address, record_type, hex_data, checksum)
        output += line
    output += ":00000001FF\n"
    return output

f = open(sys.argv[1], 'r').read()
for i, c in enumerate(f):
    if i % 4 > 0:
        print(' ', end='')
    print('0x%02x' % ord(c), end='')
    if i % 4 == 3:
        print()

memories = []
for i in range(0, len(f), 4):
    memories.append((ord(f[i]) << 24) | (ord(f[i+1]) << 16) | (ord(f[i+2]) << 8) | ord(f[i+3]))

# print(memories)
# print(to_intel_hex(memories, 4))

with open('a.hex', 'w') as fh:
    fh.write(to_intel_hex(memories, 8))

