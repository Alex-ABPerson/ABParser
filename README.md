# ABSoftware.ABParser

This is the main implementation of ABParser, which is a parsing utility which can be used to help make applications that have to read through strings easier.

For more information on how to use ABParser and how to develop ABParser, follow this guide here on the *ABSoftware* website.
**TODO: Fill in page here**

Here is a brief summary of the information there:

## Implementation

ABParser is designed to be as fast as possible. As a result, the core of ABParser (ABSoftware.ABParser.Core) is written in C++ natively, which is then accessed via `DllImport` on the C# ABSoftware.ABParser.

## Compiling

ABParser is designed to work on Windows and Linux (via Mono) - Mac OS X is not currently supported, but there are plans to add support for it. This means that the native component must be compilied for all of them. These are the extensions for each platform:

- DLL - Windows
- SO - Linux

When compiling, you must make sure that in the header file `PlatformImplementation.h` you have `#define COMPILE_DLL` either on or off (depending on whether you are compiling to a DLL file).

Then, if you're compiling for Windows, it's recommended that you use MSBuild Because, the `.vcxproj` has been configured to place `__cdecl` on all of the methods. And if you're not using the MSBuild it won't pick up on that, causing the `DllImport` to not work. MSBuild is configured to compile straight into the same directory as the C# testing program.

If you're compiling for Linux, there's a `makefile` on the C++ project, which is already configured (provided you don't add any files). So, just type "make compileAndCopy" which will compile the project and copy it to the C# test project. You can also use "make testWithMono" which will copy the files and actually launch the project!

## License

ABSoftware.ABParser is licensed under the MIT License
See the LICENSE file for more information.