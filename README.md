# Notes on Packers 

`|Your File| --> |Crypter| --> |Encrpyte File|+|Stub|`

the stub is the section of the code that will provide the extraction and sometime the eecution of the progrma
## Scantime Packers
These types of packers are known as scantime due to their capability of obscuring data on disk which is where antivirus software can run a scan on the file with signature-based detection, for example. In this stage, the antivirus software will never be able to detect any malicious activity provided that the applied obfuscation is robust
## Runtime Packers
These packers will deobfuscate the data as it is needed rather than in whole compared to the Scantime Packer
## Packer and Stub Pseudo Code for a Scantime Packer
```
1. Check if there is a command line argument
+-> 2. If there is a command line argument, act as a crypter to crypt the file
|   3. Open the target file
|   4. Read file contents
|   5. Encrypt the file contents
|   6. Create a new file
|   7. Write the encrypted into the new file
|   8. Finish
|
+-> 2. If there is no command line argument, act as the stub
    3. Open encrypted file
    4. Read file contents
    5. Decrypt the file contents
    6. Create a temporary file
    7. Write the decrypted into the temporary file
    8. Execute the file
    9. Remove new file
    10. Finish
```

## Stub Pseudo Code for a Runtime Packer
```
1. Decrypt application
2. Create suspended process
3. Preserve process's thread context
4. Hollow out process's virtual memory space
5. Allocate virtual memory
6. Write application's header and sections into allocated memory
7. Set modified thread context
8. Resume process
9. Finish
```

## Instructions for unix packer
1. Compile both the hello.c and unixpacker.c with `gcc hello.c -o hello` and `gcc unixpacker.c -o unixpacker` 
2. Encrypt the hello binary with `./unixpacker hello`
3. You should now have a crypto.bin file which contains the encrypted contents
4. To run the encrypted program enter `./unixpacker`



