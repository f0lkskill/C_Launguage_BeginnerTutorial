#include<stdio.h>
#include<stdlib.h>

typedef struct ch
{
    char dat;
    struct ch *next;
    struct ch *last;
}ch;

typedef struct str
{
    struct ch *head;
    struct ch *tail;
}str;

void create(str *s){
    char c;

    printf("input a text: ");
    while (1)
    {
        c = getchar();
        if(c == '\n') break;
        
        ch *cp = (ch *)malloc(sizeof(ch));
        cp -> dat = c;
        cp -> next = NULL;
        cp -> last = NULL;

        if(s->head == NULL) s -> head = cp;
        else{
            s -> tail -> next = cp;
            cp -> last = s -> tail;
        }
        s -> tail = cp;
    }
}

void print(str *s){
    ch *h = s -> head;
    while (h)
    {
        printf("%c",h->dat);
        h = h->next;
    }
    putchar('\n');
}

void connect(str *s1, str *s2){
    s1->tail->next = s2->head;
    s2->head->last = s1->tail;
    s1->tail = s2->tail;
}

void append(str *s, char c){
    ch *cp = (ch *)malloc(sizeof(ch));
    cp->dat = c;
    cp->next = NULL;
    cp->last = s->tail;
    s->tail->next = cp;
    s->tail = cp;
}

void replace(str *s, char old, char new){
    ch *np = s->head;
    while (np)
    {
        if(np->dat==old)np->dat=new;
        np = np -> next;
    }
}

void insert_str(str *s1, str *s2, int i){
    ch *np = s1->head;
    while (i-- && np->next)
        np = np->next;
    ch *ep = np -> next;

    np -> next = s2 -> head;
    s2 -> head -> last = np;
    
    if(ep){
        ep -> last = s2 -> tail;
        s2 -> tail -> next = ep;
    }
}

str * cut(str *s, int st, int en){
    str *ns;
    ch *sp = s -> head;
    ch *ep = s -> head;
    while (st-- && sp)
        sp = sp -> next;
    while(en-- && ep)
        ep = ep -> next;
    ns->head = sp;
    ns->tail = ep;
    ns->tail->next = NULL;
    s->head = sp;
    s->tail = ep;
    return ns;
}

int count(str *s, str *ns){
    ch *np = s->head;
    int total = 0;
    while(np){
        ch *nnp = ns->head;
        ch *snnp = np;
        int sf = 0;
        while(nnp && snnp)
        {
            if(!(nnp->dat == snnp->dat))break;
            if(nnp->next == NULL)sf=1;
            nnp = nnp->next;
            snnp = snnp->next;
        }
        if(sf)total ++;
        np=np->next;
    }
    return total;
}

int main(){
    str str1={NULL, NULL}, str2={NULL, NULL};
    create(&str1);
    print(&str1);

    // create(&str2);
    // print(&str2);

    // insert_str(&str1, &str2, 2);

    // print(&str1);

    // append(&str1, '6');

    // print(&str1);

    // replace(&str1, '6', '7');

    // print(&str1);

    create(&str2);

    printf("%d", count(&str1, &str2));
}