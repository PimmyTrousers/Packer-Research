#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define MODE 0777

char* stub_crypt(FILE* hFile, int dwFileSize);

char CRYPTED_FILE[15] = "crypto.bin";
char UNCRYPTED_FILE[15] = "uncrpyto.bin";
char Key[9];

int main(int argc, char* argv[])
{
    Key[0] = 99;
    Key[1] = 114;
    Key[2] = 121;
    Key[3] = 112;
    Key[4] = 116;
    Key[5] = 111;
    Key[6] = 99;
    Key[7] = 114;
    Key[8] = 121;

    if (argc < 2)
    {
        // get crypted file name in current directory
        char cwd[1024];
        char un_cwd[1024];
        getcwd(cwd, sizeof(cwd));
        strcat(cwd, "/");
        strcat(cwd, CRYPTED_FILE);
        printf("new file, %s\n", cwd);

        // create location for new unencrypted file
        getcwd(un_cwd, sizeof(cwd));
        strcat(un_cwd, "/");
        strcat(un_cwd, UNCRYPTED_FILE);
       
        // open handle to crypted file
        FILE *hCryptedFile = fopen(cwd, "ab+");
        if (hCryptedFile == NULL)
        {
            fputs("Opening the file failed", stderr);
            exit(1);
        }

        // get file size of the crypted file
        fseek(hCryptedFile, 0, SEEK_END);
        int size = ftell(hCryptedFile);
        fseek(hCryptedFile, 0, SEEK_SET);

        // create the buffer with decrypted contents and close the handle to the crypted file
        char *lpFileBytes = stub_crypt(hCryptedFile, size);
        fclose(hCryptedFile);

        // create a handle to the unpacked executable 
        hCryptedFile = fopen(un_cwd, "wb");
        if (hCryptedFile == NULL)
        {
            fputs("Opening the file failed", stderr);
            exit(1);
        }

        // write result to unencrypted file
        int result = fwrite(lpFileBytes, size, 1, hCryptedFile);
        fclose(hCryptedFile);
        free(lpFileBytes);

        // change permissions of unpacked file, run it and finalyl remove it
        chmod(un_cwd, MODE);
        system(un_cwd);
        remove(un_cwd);
 }else{
       // packer routine
       // open file to pack
       FILE* hFile = fopen(argv[1], "ab+");
       if(hFile == NULL){
           fputs("Opening the file failed", stderr);
           exit(1);
       }
       // get file size
       fseek(hFile, 0, SEEK_END);
       int size = ftell(hFile);
       fseek(hFile, 0, SEEK_SET);

       // pack and get packed bytes
       char* lpFileBytes = stub_crypt(hFile, size);

       // get packed file name in current directory
       char cwd[1024];
       getcwd(cwd, sizeof(cwd));
       strcat(cwd, "/");
       strcat(cwd, CRYPTED_FILE);
       printf("%s", cwd);

       // open handle to new packed file
       FILE* hCryptedFile = fopen(cwd, "wb");

       //  write to packed file
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
