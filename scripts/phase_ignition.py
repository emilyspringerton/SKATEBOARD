# Meta-Layer for Code Generation
import os

print("🔥 Generating Empire of the Worlds source...")

# 1. Define Content
main_c = '''#include <stdio.h>
#include "physics.h" 

int main() {
    printf("EMPIRE OF THE WORLDS: ONLINE\n");
    printf("Gravity Constant: %f\n", GRAVITY);
    return 0;
}'''

physics_h = '''#ifndef PHYSICS_H
#define PHYSICS_H
#define GRAVITY 9.81f
#endif
'''

# 2. Build Directory Structure
dirs = ["apps/lobby/src", "packages/common", "packages/simulation"]
for d in dirs:
    os.makedirs(d, exist_ok=True)
    print(f"✅ Created: {d}")

# 3. Write Files
with open("apps/lobby/src/main.c", "w") as f:
    f.write(main_c)

with open("packages/common/physics.h", "w") as f:
    f.write(physics_h)

# Touch other headers to be safe
with open("packages/simulation/local_game.h", "w") as f:
    f.write("// Placeholder")

print("📄 Source files generated.")
