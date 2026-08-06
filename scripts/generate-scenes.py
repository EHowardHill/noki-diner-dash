#!/usr/bin/env python3
import json
import re
import sys
from pathlib import Path


def process_portrait(val: str):
    """
    Parses a portrait entry from TSV and returns a C++ pointer expression
    and optional header file include. If the value represents null/none,
    it strictly returns nullptr.
    """
    if val is None:
        return "nullptr", None

    val = val.strip()
    if not val:
        return "nullptr", None

    # Look only at the first whitespace-delimited token. This protects us
    # against columns that have been accidentally merged together by a
    # malformed TSV row (e.g. "nullptr nullptr 0 ...").
    tokens = val.split()
    first_token = tokens[0] if tokens else ""

    # Explicitly check for null-like values to avoid generating bogus sprite pointers
    if first_token.lower() in ("nullptr", "null", "0", "none", "nil", "false", ""):
        return "nullptr", None

    # Handle formats like 'spr_noki', '&bn::sprite_items::spr_noki', etc.
    if "sprite_items::" in first_token:
        sprite_name = first_token.split("sprite_items::")[-1].strip("& ")
    elif first_token.startswith("&"):
        sprite_name = first_token[1:].strip()
    else:
        sprite_name = first_token

    # Double-check the extracted name itself isn't a null keyword
    if not sprite_name or sprite_name.lower() in (
        "nullptr",
        "null",
        "0",
        "none",
        "nil",
    ):
        return "nullptr", None

    include_header = f"bn_sprite_items_{sprite_name}.h"
    cpp_ref = f"&bn::sprite_items::{sprite_name}"
    return cpp_ref, include_header


def parse_tsv_line(line: str, line_num: int):
    """
    Parses a single TSV line into one or more Line struct data dictionaries.
    The first three columns are text entries; the remaining columns configure
    portrait/frame/visibility settings shared across all texts in the row.
    """
    line = line.rstrip("\r\n")
    if not line:
        return None

    if "\t" in line:
        parts = line.split("\t")
    else:
        parts = re.split(r"\s{2,}", line)

    parts = [p.strip() for p in parts]

    # Defaults now account for 3 text columns + 7 portrait columns
    defaults = [
        "",
        "",
        "",
        "nullptr",
        "nullptr",
        "0",
        "0",
        "false",
        "false",
        "false",
        "false",
    ]
    while len(parts) < len(defaults):
        parts.append(defaults[len(parts)])

    text1 = parts[0]
    text2 = parts[1]
    text3 = parts[2]
    left_portrait_raw = parts[3]
    right_portrait_raw = parts[4]

    try:
        left_frame = int(parts[5])
    except ValueError:
        left_frame = 0

    try:
        right_frame = int(parts[6])
    except ValueError:
        right_frame = 0

    left_vis = "true" if parts[7].lower() in ("true", "1", "yes", "t") else "false"
    right_vis = "true" if parts[8].lower() in ("true", "1", "yes", "t") else "false"
    left_flip = "true" if parts[9].lower() in ("true", "1", "yes", "t") else "false"
    right_flip = "true" if parts[10].lower() in ("true", "1", "yes", "t") else "false"

    left_ptr, left_inc = process_portrait(left_portrait_raw)
    right_ptr, right_inc = process_portrait(right_portrait_raw)

    includes = [inc for inc in (left_inc, right_inc) if inc]

    # Build one entry per non-empty text column
    entries = []
    for text in (text1, text2, text3):
        if not text:
            continue
        entries.append(
            {
                "text": text,
                "left_portrait": left_ptr,
                "right_portrait": right_ptr,
                "left_portrait_frame": left_frame,
                "right_portrait_frame": right_frame,
                "left_portrait_visible": left_vis,
                "right_portrait_visible": right_vis,
                "left_portrait_flipped": left_flip,
                "right_portrait_flipped": right_flip,
                "includes": includes,
            }
        )

    return entries if entries else None


def generate_scene_files(scene_tsv_path: Path, include_dir: Path, src_dir: Path):
    scene_id = scene_tsv_path.stem  # e.g. 'scene01'

    lines_data = []
    required_includes = set()

    with open(scene_tsv_path, "r", encoding="utf-8") as f:
        for idx, line in enumerate(f, 1):
            if not line.strip() or line.startswith("#"):
                continue
            parsed = parse_tsv_line(line, idx)
            if parsed:
                for entry in parsed:
                    lines_data.append(entry)
                    for inc in entry["includes"]:
                        required_includes.add(inc)

    if not lines_data:
        print(f"Warning: '{scene_tsv_path.name}' is empty or contains no valid lines.")

    # 1. Output Header File: include_auto/scene01.h
    header_path = include_dir / f"{scene_id}.h"
    header_content = f"""#pragma once

#include "scene.h"

extern const Scene {scene_id};
"""
    with open(header_path, "w", encoding="utf-8") as f:
        f.write(header_content)

    # 2. Output Source File: src_auto/scene01.cpp
    src_path = src_dir / f"{scene_id}.cpp"

    includes_code = "\n".join(f'#include "{inc}"' for inc in sorted(required_includes))
    if includes_code:
        includes_code += "\n"

    lines_struct_cpp = []
    for item in lines_data:
        escaped_text = json.dumps(item["text"])
        lines_struct_cpp.append(f"""    {{
        .text = {escaped_text},
        .left_portrait = {item['left_portrait']},
        .right_portrait = {item['right_portrait']},
        .left_portrait_frame = {item['left_portrait_frame']},
        .right_portrait_frame = {item['right_portrait_frame']},
        .left_portrait_visible = {item['left_portrait_visible']},
        .right_portrait_visible = {item['right_portrait_visible']},
        .left_portrait_flipped = {item['left_portrait_flipped']},
        .right_portrait_flipped = {item['right_portrait_flipped']}
    }}""")

    lines_array_body = ",\n".join(lines_struct_cpp)

    src_content = f"""#include "{scene_id}.h"
{includes_code}
static const Line {scene_id}_lines[] = {{
{lines_array_body}
}};

const Scene {scene_id} = {{
    .lines = {scene_id}_lines,
    .lines_count = {len(lines_data)}
}};
"""
    with open(src_path, "w", encoding="utf-8") as f:
        f.write(src_content)

    return scene_id


def main():
    script_dir = Path(__file__).parent.resolve()
    project_root = script_dir.parent if script_dir.name == "scripts" else script_dir

    scenes_dir = project_root / "scenes"
    include_auto_dir = project_root / "include_auto"
    src_auto_dir = project_root / "src_auto"

    include_auto_dir.mkdir(parents=True, exist_ok=True)
    src_auto_dir.mkdir(parents=True, exist_ok=True)

    if not scenes_dir.exists():
        print(f"Error: Scenes directory '{scenes_dir}' does not exist.")
        sys.exit(1)

    tsv_files = sorted(scenes_dir.glob("*.tsv"))
    if not tsv_files:
        print("No .tsv files found in scenes/")
        return

    generated_scenes = []
    for tsv_file in tsv_files:
        scene_id = generate_scene_files(tsv_file, include_auto_dir, src_auto_dir)
        generated_scenes.append(scene_id)
        print(f"Generated {scene_id}.h and {scene_id}.cpp from {tsv_file.name}")

    # Generate master include file: include_auto/scenes.h
    master_header_path = include_auto_dir / "scenes.h"
    master_includes = "\n".join(f'#include "{sid}.h"' for sid in generated_scenes)
    master_header_content = f"""#pragma once

{master_includes}
"""
    with open(master_header_path, "w", encoding="utf-8") as f:
        f.write(master_header_content)

    print(f"Generated master header: {master_header_path.name}")


if __name__ == "__main__":
    main()
