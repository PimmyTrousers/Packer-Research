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
     // get crypted file name in current directory
     char cwd[1024];
     getcwd(cwd, sizeof(cwd));
     strcat(cwd, "/");
     strcat(cwd, CRYPTED_FILE);
     printf("%s\n", cwd);
     // open handle to new decrypted file
     FILE *hCryptedFile = fopen(cwd, "ab+");
     if (hCryptedFile == NULL)
     {
         fputs("Opening the file failed", stderr);
         exit(1);
     }

     // get file size, remove the file and close the handle
     fseek(hCryptedFile, 0, SEEK_END);
     int size = ftell(hCryptedFile);
     printf("The size is 0x%x\n", size);
     fseek(hCryptedFile, 0, SEEK_SET);

     char *lpFileBytes = stub_crypt(hCryptedFile, size);

     fclose(hCryptedFile);
     hCryptedFile = fopen(cwd, "wb");
     // Create the file
     if (hCryptedFile == NULL)
     {
         fputs("Opening the file failed", stderr);
         exit(1);
     }

     // write result to file 
     int result = fwrite(lpFileBytes, size, 1, hCryptedFile);
     fclose(hCryptedFile);
     free(lpFileBytes);

 }else{
   // crypter routine
       // open file to crypt
       FILE* hFile = fopen(argv[1], "ab+");
       if(hFile == NULL){
           fputs("Opening the file failed", stderr);
           exit(1);
       }
       // get file size
       fseek(hFile, 0, SEEK_END);
       int size = ftell(hFile);
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
      int result = fwrite(lpFileBytes, size, 1, hCryptedFile);
      fclose(hCryptedFile);
      free(lpFileBytes);
 }
}

char* stub_crypt(FILE* hFile, int dwFileSize)
{
    char *buffer;

    buffer = (char*)calloc(dwFileSize,sizeof(char));

    // read the file into the buffer
    printf("The size is 0x%x\n", dwFileSize);
    if( 1!=fread( buffer , dwFileSize, 1 , hFile)){
      fclose(hFile);
      free(buffer);
      fputs("entire read fails",stderr);
      exit(1);
    }

    // apply XOR encryption
    int i;
    for (i = 0; i < dwFileSize; i++)
    {
        *(buffer + i) ^= Key[i % sizeof(Key)];
    }

    return buffer;
}
