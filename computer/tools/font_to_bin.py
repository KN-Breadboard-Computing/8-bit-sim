import sys
from PIL import Image
import numpy as np

# Converts a 128x128 black and white image to a binary file
# each pixel is represented by a single bit
# 0 -> black
# 1 -> white

def main():
    if len(sys.argv) < 3:
        print("Usage: python font_to_bin.py <input_file> <output_file>")
        return

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    try:
        img = Image.open(input_file)
    except FileNotFoundError:
        print(f"Error: File '{input_file}' does not exist.")
        return
    except Exception as e:
        print(f"Error: {e}.")
        return

    if img.size != (128, 128):
        print(f"Error: Image must be 128x128 pixels. The provided file '{input_file}' is of size: {img.size[0]}x{img.size[1]}.")
        return

    img = img.convert('L')
    img_data = list(img.getdata())
    if any((pixel != 0 and pixel != 255) for pixel in img_data):
        print(f"Error: Image must be black (0) and white (255) but found other colors: {set(img_data)}.")
        return

    bin_img = np.array(img).reshape((128, 128)).flatten()
    bitwise_bin_img = [0 if pixel == 0 else 1 for pixel in bin_img]

    print(bitwise_bin_img)

    with open(output_file, 'wb') as f:
        for square_idx in range(0, 256):
            x_offset = (square_idx % 16) * 8
            y_offset = (square_idx // 16) * 8
            for i in range(8):
                byte = 0
                for j in range(8):
                    byte |= bitwise_bin_img[x_offset + j + (y_offset + i) * 128] << (7 - j)
                f.write(byte.to_bytes(1, byteorder='big'))

    print(f"Success: Converted '{input_file}' to '{output_file}'.")

if __name__ == "__main__":
    main()
