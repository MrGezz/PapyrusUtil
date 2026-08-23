# PapyrusUtil SE

This is the SE version of the source code.

Depends on Boost, Jsoncpp, and the latest version of SKSE64.

Note: I'm not a C++ programmer. This was mainly all written after Googling basics and then applying the results here as necessary.
I also haven't very reliably kept up my local repo of it for the past 6-ish years. This was basically just a quick open source where I added every file I thought maybe relevant to the repo and pushed it. There is almost certainly old and unused files here. Sorry.

Will try to clean this up and push a LE branch later(tm).

## 4.7 — Skyrim SE 1.7.99 (IcZ fork, 2026-08-23)

What changed against upstream 4.5 (1.6.1130):

- **Upstream PR #1 is merged** (osmelmc, 2022, never actioned by eeveelo): `MiscUtil.cpp`
  `ScanCellObjects` now null-checks `ObjRef->baseForm` before reading `formType` — a cell
  holding a reference whose base form is not loaded crashed the game — and `MiscUtil.psc`
  gains `ScanCellObjectsOfAnyTypeInList(FormList, ObjectReference, float)` and
  `ScanCellObjectsOfAnyTypeInArray(Form[], ObjectReference, float)`, two Papyrus-side scans
  that match any of several base objects instead of one form type. Both are pure additions:
  no existing signature changed, so `MiscUtil.pex` stays a drop-in replacement.

- `versionlibdb.h` reads Address Library **file format 5** (the dense `uint32 offset[id]` table
  meh321 introduced with the 1.7.99 library) as well as format 2. The six IDs the plugin uses
  (50809, 29619, 12057, 69166, 37398+0x47, 53984+0x103) all exist in the official 1.7.99 library
  and the two mid-function hook sites still hold the same `jmp`/`call` instructions.
- `main.cpp`: `compatibleVersions = { RUNTIME_VERSION_1_7_99 }`, plugin version 3;
  `Plugin.h`: `PAPYRUSUTIL_VERSION 47`. The DLL stays runtime-pinned on purpose — the game
  structs come from the skse64 headers it is compiled against.
- `MiscUtil.cpp`: `MenuManager::showMenus` is private in skse64 2.3.0, use `ShowMenus(bool)`.
- `PapyrusUtil.vcxproj` rewritten: x64 only, toolset v145 (VS 2026), Windows SDK 10, /MT,
  C++17, no more `E:\Libs` paths or references to the SKSE Visual Studio projects. Every
  dependency path is an overridable property; `PapyrusUtil.rc` / `resource.h` (which the old
  project referenced but never contained) carry the file version.
- `vcpkg.json` for Boost (filesystem, algorithm, container, random, lexical_cast, array,
  iterator, range); jsoncpp is the bundled amalgamation.

### Building

1. Checkouts next to each other as in the IcZ workspace: `skse64` (ianpatt, 2.3.0) and `common`
   (ianpatt) two levels above this folder, or pass `-p:SKSE64Root=... -p:CommonRoot=...`.
2. SKSE libraries: `common.lib`, `skse64_common.lib` and a **static** `skse64_1_7_99_static.lib`
   made from the skse64 DLL's object files without `skse64.obj` (the SKSE entry point):
   `lib.exe /OUT:skse64_1_7_99_static.lib <build>\skse64\skse64.dir\Release\*.obj` minus that
   one. The CMake build of skse64 only produces the DLL and its 2 KB import library, which does
   not carry the game globals (`LookupFormByID`, `g_skyrimVM`, …) a plugin built from the SKSE
   sources needs. Default location: `..\Runtime 1.7.99\SKSE64 2.3.0\lib\`, or `-p:SKSELibDir=...`.
3. Boost: `vcpkg install --triplet x64-windows-static` in this folder (manifest mode), or point
   `-p:VcpkgInstalled=<root>\x64-windows-static\` at an existing install. Library names are in
   `BoostLibs` (`boost_*-vc145-mt-x64-1_91.lib`); change them if the toolset or Boost differs.
4. `msbuild PapyrusUtil.vcxproj -p:Configuration=Release -p:Platform=x64` → `bin\x64\Release\PapyrusUtil.dll`.

`RUNTIME_VERSION` (`SKSERuntimeVersion`, default `0x1070630` = 1.7.99) must match the skse64
objects in the static library.

### Verified

Loaded by SKSE 2.3.0 on SkyrimSE.exe 1.7.99.0 with the official `versionlib-1-7-99-0.bin`:
`PapyrusUtilDev.log` reports "Loaded database for SkyrimSE.exe version 1.7.99.0", hooks
installed, functions registered; game reached the main menu. Not yet exercised beyond that
(package overrides, storage serialization) — run the Campfire/Frostfall MCM and a save/load
cycle before shipping.
