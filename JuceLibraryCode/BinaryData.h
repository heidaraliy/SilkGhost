/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   IconOff_png;
    const int            IconOff_pngSize = 14146;

    extern const char*   IconOn_png;
    const int            IconOn_pngSize = 14519;

    extern const char*   SilkGhost_png;
    const int            SilkGhost_pngSize = 33761;

    extern const char*   ArimoRegular_ttf;
    const int            ArimoRegular_ttfSize = 317652;

    extern const char*   VidalokaRegular_ttf;
    const int            VidalokaRegular_ttfSize = 82128;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 5;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
