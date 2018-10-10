#include <windows.h>

#include "convapi.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace std;

extern "C" typedef long (PASCAL *lib_PFN_RTF)(long, long);

extern "C" typedef long PASCAL (*lib_InitConverter32)(HANDLE hWnd, char *szModule);

extern "C" typedef FCE  PASCAL (*lib_IsFormatCorrect32)(HANDLE ghszFile, HANDLE ghszClass);

extern "C" typedef FCE  PASCAL (*lib_ForeignToRtf32)(HANDLE ghszFile, void *pstgForeign, HANDLE ghBuff, HANDLE ghszClass, HANDLE ghszSubset, lib_PFN_RTF lpfnOut);

extern "C" typedef void PASCAL (*lib_GetReadNames)(HANDLE haszClass, HANDLE haszDescrip, HANDLE haszExt);

HGLOBAL buffer;
ofstream rtfFileHandle;

PASCAL long callback(long cchBuff, long nPercent) {

    auto *p = (char *) GlobalLock(buffer);

    rtfFileHandle.write(p, cchBuff);

    GlobalUnlock(buffer);

    return 0;
}

int main(int argc, char *argv[]) {

    if (argc < 4) {
        cerr << "Usage: {cnv file} {input file} {output RTF file}\n";
        return 3;
    }

    HMODULE hLib = LoadLibrary(argv[1]);

    if (hLib == nullptr) {
        throw runtime_error(string("Error: load library ") + argv[1]);
    }

    auto GetProcAddressAndCheck = [&](LPCSTR procName) {
        auto farProc = GetProcAddress(hLib, procName);
        if (farProc == nullptr) {
            throw runtime_error(string("Error: cannot find procedure ") + procName);
        }
        return farProc;
    };

    auto fInitConverter32 = (lib_InitConverter32) GetProcAddressAndCheck("InitConverter32");
    auto fIsFormatCorrect32 = (lib_IsFormatCorrect32) GetProcAddressAndCheck("IsFormatCorrect32");
    auto fForeignToRtf32 = (lib_ForeignToRtf32) GetProcAddressAndCheck("ForeignToRtf32");
    auto fGetReadNames = (lib_GetReadNames) GetProcAddressAndCheck("GetReadNames");

    if (fInitConverter32(nullptr, nullptr) == 0) {
        throw runtime_error("InitConverter32() failed");
    }


    HGLOBAL m_hFileName = GlobalAlloc(GHND, _MAX_PATH + 1);

    auto *p = (char *) GlobalLock(m_hFileName);
    lstrcpynA(p, argv[2], _MAX_PATH);
    GlobalUnlock(m_hFileName);

    {
        HGLOBAL hDesc = GlobalAlloc(GHND, 1024);
        auto ret = fIsFormatCorrect32((HANDLE) m_hFileName, hDesc);
        GlobalFree(hDesc);

        if (ret != 1) {
            throw runtime_error(string("IsFormatCorrect32 failed, return code ") + to_string(ret));
        }
    }

    {
        rtfFileHandle.open(argv[3], ios::binary | ios::trunc | ios::out);

        buffer = GlobalAlloc(GHND, 10 * 1024);

        auto ret = fForeignToRtf32(m_hFileName, nullptr, buffer, nullptr, nullptr, callback);

        if (buffer != nullptr) {
            GlobalUnlock(buffer);
        }
        GlobalFree(buffer);

        if (ret != 0) {
            throw runtime_error(string("ForeignToRtf32 failed, return code ") + to_string(ret));
        }

        rtfFileHandle.close();
    }
}

