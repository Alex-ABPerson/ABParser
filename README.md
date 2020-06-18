# ABSoftware.ABParser

This is the main implementation of ABParser, which is a parsing utility which can be used to help make applications that have to read through strings easier.

## Usage & Development

All information about ABParser is primilarly documented on the **ABSoftware Docs**, please look at that at: https://abworld.ml/docs/abparser

Here is some extra information about compiling the project itself:

## Implementation

ABParser is designed to be as fast as possible. As a result, the core of ABParser (ABSoftware.ABParser.Core) is written in C++ natively, which is then accessed via `DllImport` on the C# ABSoftware.ABParser.

## Compiling

**NOTE: When using the library, ensure "#include DEBUG" in commented out in the "Debugging.h" to remove logging if you don't need it, and when developing for the library, always ensure this is commented out before commiting!**

ABParser is a header library, meaning it is all entirely contained within header files, this is necessary because it uses templates to allow usage across multiple characters.

This means that all you have to do is download the header files and include them in your projects and it will work!

However, if you wish to compile the C# parts of ABParser, you'll need to keep this information in mind as that's done through a dynamic library called `ABSoftware.ABParser.Core.ManagedInterop`.

ABParser is designed to work on Windows and Linux (via Mono) - Mac OS X is not currently supported, but there are plans to add support for it. This means that the native component must be compiled for all of them. These are the extensions for each platform:

- DLL - Windows
- SO - Linux
- DYLIB - Mac OS X **Work in progress**

When compiling, you must make sure that in `ExportedMethods.cpp` in the `ManagedInterop`, `#define COMPILE_DLL` either on or off (depending on whether you are compiling to a DLL file on Windows or not).

Then, if you're compiling for Windows, it's recommended that you use MSBuild. Because, the `.vcxproj` has been configured to place `__cdecl` on all of the methods, as well as automatic copying and various other things. And if you're not using the MSBuild it won't pick up on that, causing things to not work. MSBuild is configured to compile straight into the same directory as the C# testing programs.

If you're compiling for Linux or MacOSX, there's a `makefile` on the C++ project, which is already configured (provided you don't add any files).
Simply run `make compileAll` to compile everything, and `make clean` to clean-up.

Currently the only thing the makefile compiles is the *C++ parts*, you'll have to compile the C# parts manually if you need them.

### 32-bit or 64-bit

**NOTE: Any CPU is not supported.**

One thing you need to be aware of is compiling for 32-bit vs 64-bit systems.

On Windows, changing the solution's settings will cause everything to change correctly.

On other platforms, using the makefile, you can switch between 32-bit and 64-bit simply by commenting out the first line in the makefile.

## Testing

On Windows, testing the project is straight-forward and can be done straight from Visual Studio.

On other platforms, to properly test the C# parts of ABParser, you need to ensure you have `dotnet` installed.

Then, within the makefile, you can run `make runUnitTests` to run the unit tests ABParser provides to ensure everything still works after you've made changes.

There are also some other modes like `make runConsoleApp` which will launch the `ABSoftware.ABParser.Testing.ConsoleApp` project.

## License

ABSoftware.ABParser is licensed under the MIT License
See the LICENSE file for more information.
