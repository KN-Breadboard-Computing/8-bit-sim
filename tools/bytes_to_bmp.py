import sys
from PIL import Image
def bytes_to_bmp():
    if len(sys.argv) < 3:
        print("Usage: python bytes_to_bmp.py <input_file> <output_file>")
        return

    img = Image.new('1', (128, 128))
    pixels = img.load()
    with open(sys.argv[1], "r") as f:
        char_num = 0
        for line in f:
            offset = 0
            character_bytes = line.strip().split(", ")
            for byt in character_bytes:
                byt1 = int(byt[2:], 16)
                for pos in range(0, 8):
                    x = (int(char_num / 16)) * 8 + offset
                    y = (char_num % 16) * 8 + pos
                    pixels[y, x] = byt1 >> pos & 0x01
                offset += 1
            char_num += 1
    img.save(sys.argv[2])
    print(f"Success: Converted '{sys.argv[1]}' to '{sys.argv[2]}'.")

if __name__ == "__main__":
    bytes_to_bmp()