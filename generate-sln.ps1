Remove-Item $PSScriptRoot/build -Recurse -Force -ErrorAction:SilentlyContinue -Confirm:$False | Out-Null
& cmake -B $PSScriptRoot/build -S $PSScriptRoot/Plugin --preset=build-release-msvc-msvc -DCMAKE_TOOLCHAIN_FILE="$PSScriptRoot/extern/vcpkg/scripts/buildsystems/vcpkg.cmake"
