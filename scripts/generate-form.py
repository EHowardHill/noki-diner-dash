import os

ROOT = "."

def bundle_source_code():
    # --- Configuration ---
    output_filename = "source.txt"
    # Add or remove file extensions here that you want to include in the output
    target_extensions = [".cpp", ".hpp", ".c", ".h", ".py", ".sh"]

    # Directories to ignore so they don't clutter your structure or output
    ignored_dirs = {
        ".git",
        "__pycache__",
        "venv",
        ".idea",
        ".vscode",
        "scripts",
        "Butano",
    }
    # ---------------------

    all_file_paths = []

    with open(output_filename, "w", encoding="utf-8") as outfile:
        # Phase 1: Write the file structure
        outfile.write("=========================================\n")
        outfile.write("DIRECTORY STRUCTURE\n")
        outfile.write("=========================================\n\n")

        for root, dirs, files in os.walk(ROOT):
            # Modify dirs in-place to skip ignored directories
            dirs[:] = [d for d in dirs if d not in ignored_dirs]

            # Calculate indentation based on depth
            level = root.replace(".", "").count(os.sep)
            indent = " " * 4 * level

            # Print the directory name
            folder_name = os.path.basename(root) if os.path.basename(root) else "."
            outfile.write(f"{indent}{folder_name}/\n")

            subindent = " " * 4 * (level + 1)
            for file in files:
                # Skip the output file itself so we don't list it or read it
                if file == output_filename:
                    continue

                outfile.write(f"{subindent}{file}\n")
                all_file_paths.append(os.path.join(root, file))

        outfile.write("\n\n")
        outfile.write("=========================================\n")
        outfile.write("FILE CONTENTS\n")
        outfile.write("=========================================\n\n")

        # Phase 2: Read and append targeted file contents
        for file_path in all_file_paths:
            _, ext = os.path.splitext(file_path)

            if ext in target_extensions:
                outfile.write(f"{'-' * 60}\n")
                outfile.write(f"--- File: {file_path}\n")
                outfile.write(f"{'-' * 60}\n")

                try:
                    # Using errors='replace' to avoid crashing on non-UTF-8 characters
                    with open(
                        file_path, "r", encoding="utf-8", errors="replace"
                    ) as infile:
                        outfile.write(infile.read())
                except Exception as e:
                    outfile.write(f"[Error reading file: {e}]\n")

                outfile.write("\n\n")

    print(f"Success: Directory structure and source files written to {output_filename}")


if __name__ == "__main__":
    bundle_source_code()
