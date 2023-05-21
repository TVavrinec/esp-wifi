#!/usr/bin/env python
import os
import sys
import argparse

# parse command line arguments
parser = argparse.ArgumentParser(description='Create a LittleFS image.')
parser.add_argument('input_dir', help='input directory')
parser.add_argument('-o', '--output', help='output file name (default: spiffs.bin)', default='spiffs.bin')
parser.add_argument('-s', '--sector-size', type=int, help='SPI Flash sector size (default: 4096)', default=4096)
parser.add_argument('-p', '--page-size', type=int, help='SPI Flash page size (default: 256)', default=256)
args = parser.parse_args()

# check that input directory exists
if not os.path.exists(args.input_dir):
    print(f"error: input directory '{args.input_dir}' does not exist")
    sys.exit(1)

# run esptool to create the image
os.system(f"python -m esp32s2spi -d {args.input_dir} -p {args.page_size} -s {args.sector_size} -o {args.output}")
