#include<stdio.h>

int main(){
    int list[6];
    int list2[10001];

    for(int i = 0; i < 6; i++){
        scanf("%d", &list[i]);
    }

    int count = 0;
    for(int i = 0; i < 6; i++){
        if(list[i] % 2 != 0){
            list2[count] = list[i];
            count++;
        }
    }

    int total = 0;
    for(int i = 0; i < count; i++){
        total += list2[i];
    }

    printf("%.3f\n", (float)total/count);

    return 0;
}