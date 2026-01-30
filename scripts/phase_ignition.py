
import os

def forge_apps():
    # ShankPit FPS Source
    os.makedirs("apps/shank-fps/src", exist_ok=True)
    with open("apps/shank-fps/src/main.c", "w") as f:
        f.write("#include <stdio.h>\nint main() { printf(\"SHANKPIT FPS ONLINE\\n\"); return 0; }")
    
    # Empire Client
    os.makedirs("apps/lobby/src", exist_ok=True)
    with open("apps/lobby/src/main.c", "w") as f:
        f.write("#include <stdio.h>\nint main() { printf(\"EMPIRE ONLINE\\n\"); return 0; }")

    # Fishing Game
    os.makedirs("apps/fishing/src", exist_ok=True)
    with open("apps/fishing/src/main.c", "w") as f:
        f.write("#include <stdio.h>\nint main() { printf(\"FISHING ONLINE\\n\"); return 0; }")

if __name__ == "__main__":
    forge_apps()
