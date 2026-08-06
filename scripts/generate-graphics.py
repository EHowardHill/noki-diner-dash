import os
import shutil
import json
import struct


def get_bmp_width(filepath):
    """Reads the width of a BMP file directly from its header."""
    with open(filepath, "rb") as f:
        # The width in a standard BMP header is a 4-byte integer starting at byte 18
        f.seek(18)
        # Unpack as a little-endian signed integer
        width = struct.unpack("<i", f.read(4))[0]
        return width


def main():
    raw_dir = "graphics_raw"
    out_dir = "graphics"

    # Ensure the output directory exists
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    # Check if the raw directory exists
    if not os.path.exists(raw_dir):
        print(f"Error: The directory '{raw_dir}' does not exist.")
        return

    # Process each file in the raw directory
    for filename in os.listdir(raw_dir):
        if not filename.lower().endswith(".bmp"):
            continue

        src_path = os.path.join(raw_dir, filename)
        dest_path = os.path.join(out_dir, filename)

        # 1. Copy the .bmp file to the graphics folder
        shutil.copy2(src_path, dest_path)

        # 2. Get the width of the BMP to use as the "height" field
        try:
            width = get_bmp_width(src_path)
        except Exception as e:
            print(f"Failed to read width for {filename}: {e}")
            continue

        # 3. Determine the JSON configuration based on the prefix
        json_data = None
        if filename.startswith("spr_"):
            json_data = {"type": "sprite", "height": width}
        elif filename.startswith("bg_"):
            json_data = {"type": "regular_bg", "height": width}
        else:
            print(f"Skipping JSON creation for {filename}: Unknown prefix.")
            continue

        # 4. Write the corresponding .json file
        json_filename = os.path.splitext(filename)[0] + ".json"
        json_path = os.path.join(out_dir, json_filename)

        with open(json_path, "w") as json_file:
            json.dump(json_data, json_file, indent=4)


if __name__ == "__main__":
    main()
