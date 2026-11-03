#include<stdio.h>

int main(){
    char str[100];
    
    printf("input a string: ");
    scanf("%s", str);

    char c = '6';
    int len = 0;
    while (c != '\0')
    {
        c = str[len];
        len++;
    }
    printf("len:%d", len);
}