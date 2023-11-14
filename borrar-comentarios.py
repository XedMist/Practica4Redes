#!/bin/python3

import sys
import os
import re

for input_file in sys.argv[1:]:
    input_dirname  = os.path.dirname(input_file)
    input_filename = os.path.basename(input_file)
    input_split    = os.path.splitext(input_filename)

    output_file = input_dirname if input_dirname else '.'
    output_file += '/' + input_split[0] + '-SinComentarios' + input_split[1]

    with open(input_file, "r") as input, open(output_file, "w") as output:
        # Multiline comment
        output_line = re.sub(r'/\*[\S\s]*?\*/', '', input.read())
        # One line comment
        output_line = re.sub(r'^\s+//.*\n|//.*', '', output_line, flags=re.MULTILINE)
        # Trailing whitespace
        output_line = re.sub(r'[ \t]+$', '', output_line, flags=re.MULTILINE)

        output.write(output_line)
