#include<stdio.h>
#include<stdlib.h>
#include<time.h>

typedef struct student{
    char name;
    char sex;
    int age;
    struct student *next;
} student;

int randint(int a, int b){
    return rand()%(b-a)+a;
}

void random_st(student *s){
    s->sex = randint(0,2)?'f':'m';
    s->age = randint(10,22);
    s->name = 'n';
}

void print_st(student s){
    printf("name=%c\n",s.name);
    printf("age=%d\n",s.age);
    printf("sex=%c\n",s.sex);
    putchar('\n');
}

int main()
{
    srand(time(NULL));
    int t=0;
    scanf("%d",&t);
    printf("total student = %d\n",t);

    student *head = NULL;
    student *tail = NULL;

    for(int i=0;i<t;i++){
        printf("init student=%d\n",i);

        student *ns = malloc(sizeof(student));  // 堆上分配
        if(ns  NULL){ perror("malloc"); return 1; }

        random_st(ns);
        ns->next = NULL;

        if(head  NULL){
            head = ns;      // 第一个节点
        } else {
            tail->next = ns; // 接在尾部
        }
        tail = ns;
    }

    printf("init done, print.\n");
    for(student *p = head; p != NULL; p = p->next){
        print_st(*p);
    }

    // 别忘了释放
    student *p = head;
    while(p){
        student *tmp = p->next;
        free(p);
        p = tmp;
    }

    printf("program finished\n");
    return 0;
}