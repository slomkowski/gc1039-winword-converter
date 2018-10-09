#include <windows.h>

#include "convapi.h"

#include <fstream>
#include <iostream>

using namespace std;

extern "C" typedef long (PASCAL *lib_PFN_RTF)(long, long);

extern "C" typedef long PASCAL (*lib_InitConverter32)(HANDLE hWnd, char *szModule);

extern "C" typedef FCE  PASCAL (*lib_IsFormatCorrect32)(HANDLE ghszFile, HANDLE ghszClass);

extern "C" typedef FCE  PASCAL (*lib_ForeignToRtf32)(HANDLE ghszFile, void *pstgForeign, HANDLE ghBuff, HANDLE ghszClass, HANDLE ghszSubset, lib_PFN_RTF lpfnOut);

extern "C" typedef void PASCAL (*lib_GetReadNames)(HANDLE haszClass, HANDLE haszDescrip, HANDLE haszExt);

HGLOBAL StringToHGLOBAL(LPCSTR pstr) {
    HGLOBAL hMem = NULL;
    if (pstr != NULL) {
        hMem = GlobalAlloc(GHND, (lstrlenA(pstr) * 2) + 1);
        char *p = (char *) GlobalLock(hMem);
        if (p != NULL)
            lstrcpyA(p, pstr);
        GlobalUnlock(hMem);
    }
    return hMem;
}

HGLOBAL buffer;
ofstream rtfFileHandle("/tmp/foo.rtf");

PASCAL long callback(long cchBuff, long nPercent) {
    std::cout << "callback: " << cchBuff << " " << nPercent << std::endl;

    auto *p = (char *) GlobalLock(buffer);

    rtfFileHandle << p;

    GlobalUnlock(buffer);

    return 0;
}

int main(int argc, char *argv[]) {

    HMODULE hLib = LoadLibrary("Doswrd32.cnv");

    if (hLib == nullptr) {
        cerr << "Error: load DLL!\n";
        return 1;
    }

    auto GetProcAddressAndCheck = [&](LPCSTR procName) {
        auto farProc = GetProcAddress(hLib, procName);
        if (farProc == nullptr) {
            cerr << "Error: cannot find procedure " << procName;
        }
        return farProc;
    };

//    if(argc == 1) {
//        cerr << "provide argument!\n";
//        return 3;
//    }

    auto fInitConverter32 = (lib_InitConverter32) GetProcAddressAndCheck("InitConverter32");
    auto fIsFormatCorrect32 = (lib_IsFormatCorrect32) GetProcAddressAndCheck("IsFormatCorrect32");
    auto fForeignToRtf32 = (lib_ForeignToRtf32) GetProcAddressAndCheck("ForeignToRtf32");
//    auto fCchFetchLpszError = (lib_CchFetchLpszError) GetProcAddressAndCheck("CchFetchLpszError");
    auto fGetReadNames = (lib_GetReadNames) GetProcAddressAndCheck("GetReadNames");

    if (fInitConverter32(nullptr, nullptr) == 0) {
        cerr << "InitConverter32() failed.\n";
        return 2;
    }

//    {
////        HGLOBAL names = GlobalAlloc(GHND, 1024);
////        HGLOBAL descriptions = GlobalAlloc(GHND, 10 * 1024);
////        HGLOBAL extensions = GlobalAlloc(GHND, 10 *
//
//        char names[1024];
//        char descriptions[10 * 1024];
//
//
//        fGetReadNames(names, descriptions, extensions);
//
//        for(int i = 0; i < 1024; ++i) {
//            cout << ((char*) names)[i];
//        }
//    }
//
//    return 1;


    HGLOBAL m_hFileName;
    char buf[_MAX_PATH];
//    strcpy(buf, T2CA(inputFileBuffer));
    strcpy(buf, "Z:\\tmp\\G0101.TXT");
//
//    CharToOemA(buf, buf);
//    OemToCharA(buf, buf);
//    CharToOemA(buf, buf);

    m_hFileName = StringToHGLOBAL(buf);

    {
        HGLOBAL hDesc = GlobalAlloc(GHND, 1024);
        auto ret = fIsFormatCorrect32((HANDLE) m_hFileName, hDesc);
        GlobalFree(hDesc);

        if (ret != 1) {
            cerr << "IsFormatCorrect32 failed, return code " << ret << endl;
            return 2;
        }
    }

    {
        buffer = GlobalAlloc(GHND, 10 * 1024);

        auto ret = fForeignToRtf32(m_hFileName, nullptr, buffer, nullptr, nullptr, callback);

        if (buffer != NULL) {
            GlobalUnlock(buffer);
        }
        GlobalFree(buffer);

        if (ret != 0) {
            cerr << "ForeignToRtf32 failed, return code " << ret << endl;
            return 2;
        }

        cout << "ret: " << ret << endl;
    }

}

