@echo off
@echo off
set "BIN_DIR=bin"
set "SDL_DIR=packages\SDL2"

:: 1. Setup bin directory
if not exist %BIN_DIR% mkdir %BIN_DIR%

:: 2. Identify SDL2 Architecture Path
:: We check for 64-bit first, then 32-bit
set "ARCH_DIR=%SDL_DIR%\x86_64-w64-mingw32"
if not exist "%ARCH_DIR%" set "ARCH_DIR=%SDL_DIR%\i686-w64-mingw32"

:: 3. Set Compiler Flags
:: Standardizing paths for D: drive compatibility
set "INCLUDES=-Ipackages/protocol -Ipackages/map"
set "SDL_INC=-I%ARCH_DIR%\include\SDL2"
set "SDL_LIB=-L%ARCH_DIR%\lib"

echo 🛠️  Building Server...
gcc services/game-server/src/server.c -o %BIN_DIR%\shank_server.exe %INCLUDES% -Iservices/game-server/src -lm

echo 🛠️  Building Client...
gcc apps/shank-fps/src/main.c -o %BIN_DIR%\shank_client.exe %INCLUDES% -Iapps/shank-fps/src %SDL_INC% %SDL_LIB% -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglu32 -lm

:: 4. Finalize
if exist "%BIN_DIR%\shank_client.exe" (
    copy "%ARCH_DIR%\bin\SDL2.dll" "%BIN_DIR%\SDL2.dll" /Y >nul
    echo ✅ SUCCESS: Binaries generated in /bin folder.
) else (
    echo ❌ BUILD FAILED: Check the GCC errors above.
)



