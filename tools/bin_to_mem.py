from argparse import ArgumentParser
from pathlib import Path

parser = ArgumentParser(description="Converts .bin files (raw bytes) to .mem files (ASCII hex numbers) for consumption by Verilog's $readmemh()")
parser.add_argument("input", help="Path to the .bin file to convert")
parser.add_argument("--output", dest="out_file", default=None, help="Output path")

args = parser.parse_args()

bin_path = Path(args.input)
out_path = Path(args.out_file) if args.out_file is not None else Path(bin_path.with_suffix('.mem'))

with open(bin_path, 'rb') as bin_file, open(out_path, 'w') as out_file:
    for chunk in iter(lambda: bin_file.read(8), b''):
        line = map(lambda b: '{:02x}'.format(b), chunk)
        out_file.write(' '.join(line))
        out_file.write('\n')
