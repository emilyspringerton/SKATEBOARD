
import os
import datetime

OUTPUT_FILE = "SKATEBOARD_CONSTRUCT.txt"
IGNORE_DIRS = {".git", ".github", "__pycache__", "sdl2_win"}
INCLUDE_EXTS = {".c", ".h", ".py", ".md", ".yml", ".txt"}

def forge_construct():
    print(f"📜 Forging Construct: {OUTPUT_FILE}...")
    
    with open(OUTPUT_FILE, "w", encoding="utf-8") as outfile:
        # 1. Header
        outfile.write("=== SKATEBOARD CONSTRUCT ===\n")
        outfile.write(f"Generated: {datetime.datetime.now()}\n")
        outfile.write("Context: Full Repo Aggregation for LLM Hydration\n\n")

        # 2. File Tree
        outfile.write("--- DIRECTORY STRUCTURE ---\n")
        for root, dirs, files in os.walk("."):
            # Filter directories in-place
            dirs[:] = [d for d in dirs if d not in IGNORE_DIRS]
            level = root.replace(".", "").count(os.sep)
            indent = " " * 4 * (level)
            outfile.write(f"{indent}{os.path.basename(root)}/\n")
            subindent = " " * 4 * (level + 1)
            for f in files:
                outfile.write(f"{subindent}{f}\n")
        outfile.write("\n")

        # 3. The Content (The Meat)
        outfile.write("--- SOURCE CONTENTS ---\n")
        for root, dirs, files in os.walk("."):
            dirs[:] = [d for d in dirs if d not in IGNORE_DIRS]
            
            for filename in files:
                ext = os.path.splitext(filename)[1]
                if ext in INCLUDE_EXTS and filename != OUTPUT_FILE:
                    filepath = os.path.join(root, filename)
                    
                    outfile.write(f"\n{'='*60}\n")
                    outfile.write(f"FILE: {filepath}\n")
                    outfile.write(f"{'='*60}\n")
                    
                    try:
                        with open(filepath, "r", encoding="utf-8") as infile:
                            outfile.write(infile.read())
                    except Exception as e:
                        outfile.write(f"[Error reading file: {e}]")
                    
                    outfile.write("\n")

    print(f"✅ Construct Complete. Size: {os.path.getsize(OUTPUT_FILE)} bytes")

if __name__ == "__main__":
    forge_construct()
