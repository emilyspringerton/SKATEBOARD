@echo off
setlocal enabledelayedexpansion

:: --- CONFIG ---
set "BIN_DIR=bin"
set "TOOL_DIR=packages"
set "SDL_DIR=packages\SDL2"
set "COMPILER_URL=https://github.com/skeeto/w64devkit/releases/download/v1.23.0/w64devkit-1.23.0.zip"

if not exist %BIN_DIR% mkdir %BIN_DIR%
if not exist %TOOL_DIR% mkdir %TOOL_DIR%

:: 1. DOWNLOAD COMPILER IF MISSING
if not exist "%TOOL_DIR%\w64devkit\bin\gcc.exe" (
    echo [!] GCC missing. Downloading portable w64devkit...
    powershell -Command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri '%COMPILER_URL%' -OutFile 'compiler.zip'"
    echo [!] Extracting Compiler...
    powershell -Command "Expand-Archive -Path 'compiler.zip' -DestinationPath '%TOOL_DIR%' -Force"
    del compiler.zip
    echo [OK] Compiler ready.
)

:: Set path for this session
set "PATH=%CD%\%TOOL_DIR%\w64devkit\bin;%PATH%"

:: 2. VERIFY SDL2
if not exist "%SDL_DIR%" (
    echo [!] SDL2 directory missing in %SDL_DIR%. 
    echo [!] Please ensure your SDL2 Mingw files are there.
    pause
    exit /b
)

:: 3. PATHS
set "ARCH_DIR=%SDL_DIR%\x86_64-w64-mingw32"
set "INCLUDES=-Ipackages/protocol -Ipackages/simulation -Iapps/shank-fps/src"
set "SDL_FLAGS=-I%ARCH_DIR%\include\SDL2 -L%ARCH_DIR%\lib -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglu32 -lm"

echo 🛠️ Building Witch Engine: Turtle Module...
gcc apps/turtle/src/main.c -o %BIN_DIR%/turtle.exe %INCLUDES% %SDL_FLAGS%

echo 🛠️ Building Witch Engine: Text Editor...
gcc apps/editor/src/main.c -o %BIN_DIR%/editor.exe %INCLUDES% %SDL_FLAGS%

if exist "%BIN_DIR%\editor.exe" (
    copy "%ARCH_DIR%\bin\SDL2.dll" "%BIN_DIR%\SDL2.dll" /Y >nul
    echo ✅ SUCCESS: Turtle and Editor are ready in /bin
) else (
    echo ❌ Build Failed. Check GCC output above.
)

pause
