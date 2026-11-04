#include<stdio.h>

int main(){
    FILE *f1;
    char nc;
    f1 = fopen("test.log", "r+");
    while ((nc = fgetc(f1))!=EOF)
    {
        fputc(nc, stdout);
    }
    fclose(f1);
    return 0;
}