#include <StormLib.h>
#include <cstdio>
#include <cstring>
#include <vector>
#include <zlib.h>

// Read every archived entry, not merely the MPQ directory. Output is a TSV
// manifest (name, size, resource type, resource version) for package validation.
int main(int argc, char** argv) {
    if (argc != 2) return 2;
    HANDLE archive = nullptr;
    if (!SFileOpenArchive(argv[1], 0, MPQ_OPEN_READ_ONLY, &archive)) return 3;
    SFILE_FIND_DATA data;
    HANDLE find = SFileFindFirstFile(archive, "*", &data, nullptr);
    if (!find) return 4;
    int failed = 0;
    do {
        HANDLE file = nullptr;
        if (!SFileOpenFileEx(archive, data.cFileName, SFILE_OPEN_FROM_MPQ, &file)) { ++failed; continue; }
        DWORD size = SFileGetFileSize(file, nullptr), read = 0;
        // OTR/MPQ archives intentionally contain empty helper entries (for
        // example, skeletal limb pointer symbols).  StormLib reports a
        // zero-byte read for them, which is not an archive read failure.
        std::vector<unsigned char> bytes(size);
        if (size != 0 && (!SFileReadFile(file, bytes.data(), size, &read, nullptr) || read != size)) ++failed;
        unsigned type = 0, version = 0;
        if (size >= 64) {
            std::memcpy(&type, bytes.data() + 4, 4);
            std::memcpy(&version, bytes.data() + 8, 4);
        }
        const auto checksum = crc32(0, size == 0 ? nullptr : bytes.data(), size);
        std::printf("%s\t%u\t%08X\t%u\t%08lX\n", data.cFileName, size, type, version, checksum);
        SFileCloseFile(file);
    } while (SFileFindNextFile(find, &data));
    SFileFindClose(find);
    SFileCloseArchive(archive);
    return failed ? 5 : 0;
}
