![GitHub Release](https://img.shields.io/github/v/release/IanE-Official/rehash-sfse-asi-loader)

# 📑 SFSE ASI Loader

SFSE plugin which safely preloads ASI mods with logging forked from Doodlez by Ian E.

## ⚙ Requirements

- [CMake](https://cmake.org/)
  - Add this to your `PATH`
- [DKUtil](https://github.com/gottyduke/DKUtil)
  - Init & update with git submodule
- [SFSE](https://github.com/ianpatt/sfse)
  - Init & update with git submodule
- [PowerShell](https://github.com/PowerShell/PowerShell/releases/latest)
- [Vcpkg](https://github.com/microsoft/vcpkg)
  - Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
- [Visual Studio Community 2022](https://visualstudio.microsoft.com/)
  - Desktop development with C++
- [Starfield Steam Distribution](#-deployment)
  - Add the environment variable `SFPath` with the value as the path to the game installation

## To install / use this mod as an end-user:

- Download latest compiled version from [NexusMods](https://www.nexusmods.com/starfield/mods/8055?tab=files) or my [Github Releases](https://github.com/IanE-Official/rehash-sfse-asi-loader/releases)
- To use just place expand in your Starfield data directory or just copy the DLL to `{StarfieldDirectory}/Data/SFSE/Plugins/` and run the game

## Get started from source

### 💻 Register Visual Studio Community 2022 as a Generator

- Open `x64 Native Tools Command Prompt`
- Run `cmake`
- Close the cmd window

### 🔨 Building

To build from source (unrecommended):

```
git clone https://github.com/IanE-Official/rehash-sfse-asi-loader.git --branch {version} Plugin
cd Plugin
git submodule init
git submodule update --remote
.\build-release.ps1
```

> Change {version} to the newest version of starfield you are trying to build for (currently 1.8.88).

> Please note, failure to set the environment variables as described in requirements section WILL result in the build failing.

### 📦 Deployment

To get started on adding custom deploy rules, check out the [default examples](Plugin/dist/rules).
| action | usage |
| --------- | ------------------------------------------------------------ |
| `base` | set variable `params[0]` with value `params[1]` |
| `copy` | copy `params[0]` to `params[1]` |
| `copy_if` | do `copy` if file exists |
| `package` | add `params[0..-1]` list of sources to zip file `params[-1]` |
| `remove` | remove `params` list of sources |
| `script` | execute raw powershell script |

The following base variables are provided by default:

```
cmake_output    // this is the binary output path
dist            // this is the dist folder path, also the working directory of deployer script
project_name    // project name same as CMakeLists
project_version // project version same as CMakeLists
```

Deploy actions can be enabled by build configuration(`debug`, `release`, `relwithdebinfo`, etc)

### ➕ DKUtil addon

This project bundles [DKUtil](https://github.com/gottyduke/DKUtil).

## 📖 License

[MIT](LICENSE)

## ❓ Credits

- [Doodlez's original Mod](https://www.nexusmods.com/starfield/mods/857) which this was forked from.
- [Ryan for his commonLibSSE code](https://github.com/Ryan-rsm-McKenzie/CommonLibSSE) which was referenced in DKUtil.
- [ianpatt's starfield script extender](https://github.com/ianpatt/sfse).
- [Original plugin template](https://github.com/gottyduke/PluginTemplate)
