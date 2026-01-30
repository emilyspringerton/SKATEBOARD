# Meta-Layer for Code Generation
import os

print("🔥 Generating Empire of the Worlds source...")

# 1. Define Content (Safe Mode)
# We build the string line-by-line to ensure no hidden newlines break the C code.
physics_h = "#ifndef PHYSICS_H\n"
physics_h += "#define PHYSICS_H\n"
physics_h += "#define GRAVITY 9.81f\n"
physics_h += "#endif\n"

main_c = "#include <stdio.h>\n"
main_c += "#include \"physics.h\"\n\n"
main_c += "int main() {\n"
main_c += "    printf(\"EMPIRE OF THE WORLDS: ONLINE\\n\");\n"
main_c += "    printf(\"Gravity Constant: %f\\n\", GRAVITY);\n"
main_c += "    return 0;\n"
main_c += "}\n"

# 2. Build Directory Structure
dirs = ["apps/lobby/src", "packages/common", "packages/simulation"]
for d in dirs:
    os.makedirs(d, exist_ok=True)

# 3. Write Files
with open("apps/lobby/src/main.c", "w") as f:
    f.write(main_c)

with open("packages/common/physics.h", "w") as f:
    f.write(physics_h)

# Touch other headers to be safe
with open("packages/simulation/local_game.h", "w") as f:
    f.write("// Placeholder")

print("📄 Source files generated (Clean Syntax).")
