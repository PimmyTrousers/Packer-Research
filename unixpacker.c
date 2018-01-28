#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* stub_crypt(FILE* hFile, int dwFileSize);

char CRYPTED_FILE[15] = "crypto.bin";
char Key[9] = "cryptocry";

int main(int argc, char* argv[])
{
 if(argc < 2){

 }else{
   // crypter routine
       // open file to crypt
       FILE* hFile = fopen(argv[1], "ab+");
       // get file size
       fseek(hFile, 0, SEEK_END);
       int size = ftell(hFile);
       printf("the size is %d", size);
       fseek(hFile, 0, SEEK_SET);

       // crypt and get crypted bytes
      char* lpFileBytes = stub_crypt(hFile, size);

       // get crypted file name in current directory
       char cwd[1024];
       getcwd(cwd, sizeof(cwd));
       strcat(cwd, "/");
       strcat(cwd, CRYPTED_FILE);
       printf("%s", cwd);
       // open handle to new crypted file
       FILE* hCryptedFile = fopen(cwd, "ab+");
      //  write to crypted file
      // WriteFile(hCryptedFile, lpFileBytes, dwFileSize, NULL, NULL);
      int result = fputs(lpFileBytes, hCryptedFile);
       fclose(hCryptedFile);
      //  free(lpFileBytes);
 }
}

char* stub_crypt(FILE* hFile, int dwFileSize)
{
    char *buffer;

    buffer = (char*)calloc(dwFileSize,sizeof(char));

    // read the file into the buffer
    if( 1!=fread( buffer , dwFileSize, 1 , hFile)){
      fclose(hFile),free(buffer),fputs("entire read fails",stderr),exit(1);
    }

    // apply XOR encryption
    int i;
    for (i = 0; i < dwFileSize; i++)
    {
        *(buffer + i) ^= Key[i % sizeof(Key)];
    }

    return buffer;
}
