# =======================================
# ARGUMENT HANDLING
# =======================================
param (
    [Parameter(Mandatory=$false)][switch]$setup,
    [Parameter(Mandatory=$false)][string]$name,
    [Parameter(Mandatory=$false)][switch]$update,
    [Parameter(Mandatory=$false)][string]$rename
)

# =======================================
# CONFIGURATIONS
# =======================================
$ROOT_FOLDER = "../"
$GAME_FOLDER = "../Game"
$ENGINE_FOLDER = "../Engine"
$BUILD_FOLDER = "../build"

$GAME_BUILD_FOLDER = "../build"
$ENGINE_BUILD_FOLDER = "./build"


$BIN_FOLDER = "../build/bin"
$DEFAULT_GAME_NAME = "KobaltGame"
$CMAKE_GENERATOR = "Visual Studio 17 2022"
$ARCH = "x64"



$MODE = if ($setup) { "setup" } elseif ($update) { "update" } elseif ($rename) { "rename" } else { $null }
$GAME_NAME = if ($name) { $name } else { $DEFAULT_GAME_NAME }
$NEW_NAME = $rename

function Show-Usage {
    Write-Host "Usage:"
    Write-Host "  .\build.ps1 -setup [-name GameName]"
    Write-Host "  .\build.ps1 -update"
    exit 1
}

if (-not $MODE) {
    Write-Host "Error: No mode specified"
    Show-Usage
}

Write-Host "Mode: $MODE"
Write-Host "Game Name: $GAME_NAME"
if ($NEW_NAME) { Write-Host "New Name: $NEW_NAME" }

# =======================================
# SETUP MODE: Initializes the game project
# =======================================
if ($MODE -eq "setup") {
    Write-Host "Setting up the project with game name: $GAME_NAME"
    
    # Create necessary folders
    if (-not (Test-Path $GAME_FOLDER)) {
        Write-Host "Creating game folder: $GAME_FOLDER"
        New-Item -Path $GAME_FOLDER -ItemType Directory
        New-Item -Path "$GAME_FOLDER\src" -ItemType Directory

        if (-not (Get-ChildItem "$GAME_FOLDER\src\*.cpp")) {
            Write-Host "Creating default main.cpp..."
            @"
#include <iostream>

int main() {
    std::cout << "Welcome to $GAME_NAME!" << std::endl;
    return 0;
}
"@ | Set-Content "$GAME_FOLDER\src\main.cpp"
        }
    }

    # Create CMakeLists.txt in the game folder
    $TEMPLATE_FILE = "RootCMakeLists.template.txt"
    $DESTINATION_FILE = "../CMakeLists.txt"

    Write-Host "Creating the CMakeLists.txt by replacing placeholders"
    $content = Get-Content $TEMPLATE_FILE -Raw
    $content = $content -replace "GAME_NAME", $GAME_NAME
    $content | Set-Content $DESTINATION_FILE

    $TEMPLATE_FILE = "GameCMakeLists.template.txt"
    $DESTINATION_FILE = "../game/CMakeLists.txt"

    $content = Get-Content $TEMPLATE_FILE -Raw
    #$content = $content -replace "GAME_NAME", $GAME_NAME
    $content | Set-Content $DESTINATION_FILE

    $content = Get-Content "vcpkg.json" -Raw
    $content | Set-Content "../vcpkg.json"

    if (-not (Test-Path $BIN_FOLDER)) {
        Write-Host "Creating build folder: $BIN_FOLDER"
        New-Item -Path $BIN_FOLDER -ItemType Directory
    }

    # Run CMake to generate solution
    Write-Host "Generating solution..."
    Push-Location "../"
    cmake -G $CMAKE_GENERATOR -A $ARCH -S ./ -T host=x64
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Error: CMake failed to generate the solution!"
        Pop-Location
        exit 1
    }

    # Check for the solution file
    Write-Host "Looking for solution file: ../$GAME_NAME.sln"
    Pop-Location
    Write-Host "Setup complete! The solution is available as '$GAME_NAME.sln' in the current folder."
    exit 0
}

# Add other modes (update, rename) here if needed