#include <stdio.h>
void main(void)
{
    int a[]={12,30,15,57,69},*pa=&a[3];
    while (*pa!=30)
        printf("%d,", *(--pa));
}