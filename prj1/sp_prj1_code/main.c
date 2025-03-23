#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "list.h"
#include "bitmap.h"
#include "hash.h"

struct list l_arr[10];
struct hash h_arr[10];
struct bitmap b_arr[10];

bool less_func(const struct list_elem *a, const struct list_elem *b, void * aux){
    struct list_item * ai = list_entry(a, struct list_item, elem);
    struct list_item * bi = list_entry(b, struct list_item, elem);
    if(ai->data < bi->data) return true;
    else return false;
}


void list_swap(struct list * list, int a, int b){
    int tmp;
    struct list_elem * ea = &list->head;
    struct list_elem * eb = &list->head;
    struct list_item * ia = (struct list_item *)malloc(sizeof(struct list_item));
    struct list_item * ib = (struct list_item *)malloc(sizeof(struct list_item));
    for(int i=0; i<=a ;i++) ea = list_next(ea);
    for(int i=0; i<=b ;i++) eb = list_next(eb);
    ia = list_entry(ea,struct list_item, elem);
    ib = list_entry(eb,struct list_item, elem);
    tmp = ia->data;
    ia->data = ib->data;
    ib->data = tmp;
}

bool hash_less(const struct hash_elem *a, const struct hash_elem *b, void * aux){
    return a->data < b->data;
}

unsigned hash_func (const struct hash_elem *e, void * aux){    
    return hash_int(e->data);
}

void list_shuffle(struct list* list){
    struct list_elem * ele = &list->head;
    struct list_elem * jele = &list->head;
    int size = list_size(list);
    int j;
    for(int i=0; i<size; i++){
        j = rand() % (i+1);
        list_swap(list,i,j);
    }
}

void hash_print(struct hash_elem * h){
    printf("%d ",h->data);
}

void hash_square(struct hash_elem *h){
    h->data = (h->data) * (h->data);
}

void hash_triple(struct hash_elem *h){
    h->data = (h->data) * (h->data) * (h->data);
}

void h_delete(struct hash_elem * he, void *aux){
    list_remove(&he->list_elem);
}

void bitmap_expand(struct bitmap * bm, size_t size){
    bm->bits = realloc(bm->bits,bm->bit_cnt + size);
    bm->bit_cnt += size;
}

int main(int argc, char * argv[]){
    char line[40];
    char *command;
    char *type;
    char *name;
    char *name2;
    int num;
    int idx;
    int num2;
    int aux = NULL;
    fgets(line,40,stdin);
    line[strlen(line)-1] = 0;
    command = strtok(line," ");
    type = strtok(NULL," ");
    name = strtok(NULL," ");
    if(type[0] == 'l'){
        bool (*less)(const struct list_elem *a, const struct list_elem *b, void*aux) = less_func;
        list_init(&l_arr[name[4]-'0']);
        while(1){
            fgets(line,40,stdin);
            line[strlen(line)-1] = 0;
            command = strtok(line," ");
            if(!strcmp(command,"quit")) break;
            if(!strcmp(command,"create")){
                type = strtok(NULL," ");
                name = strtok(NULL," ");
                list_init(&l_arr[name[4] - '0']);
            }
            else if (!strcmp(command,"delete")){
                name = strtok(NULL," ");
                list_init(&l_arr[name[4]-'0']);
            }
            else if (!strcmp(command,"list_shuffle")){
                name = strtok(NULL," ");
                list_shuffle(&l_arr[name[4]-'0']);
            }
            else if (!strcmp(command,"list_swap")){
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                num2 = atoi(strtok(NULL," "));
                list_swap(&l_arr[name[4]-'0'],num,num2);
            }
            else if (!strcmp(command,"list_push_back")){
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                struct list_item *i = (struct list_item*)malloc(sizeof(struct list_item));
                i->data = num;
                list_push_back(&l_arr[name[4]-'0'],&(i->elem));
            }
            else if (!strcmp(command,"list_push_front")){
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                struct list_item *i = (struct list_item *)malloc(sizeof(struct list_item));
                i->data = num;
                list_push_front(&l_arr[name[4] - '0'],&(i->elem));
            }
            else if (!strcmp(command,"list_front")){ // segfault
                name = strtok(NULL," ");
                struct list_elem * e = list_front(&l_arr[name[4]-'0']); //boom
                struct list_item * a = list_entry(e,struct list_item, elem);
                printf("%d\n",a->data);
            }
            else if (!strcmp(command,"list_back")){
                name = strtok(NULL," ");
                struct list_elem * e = list_back(&l_arr[name[4]-'0']);
                struct list_item * a = list_entry(e,struct list_item, elem);
                printf("%d\n",a->data); 
            }
            else if (!strcmp(command,"list_insert_ordered")){
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                struct list_item * i = (struct list_item *)malloc(sizeof(struct list_item));
                i->data = num;
                list_insert_ordered(&l_arr[name[4] - '0'],&(i->elem),less,aux);
            }
            else if (!strcmp(command,"list_insert")){
                name = strtok(NULL," ");
                idx = atoi(strtok(NULL," "));
                num = atoi(strtok(NULL," "));
                struct list_elem * before = &l_arr[name[4]-'0'].head;
                struct list_item * i = (struct list_item*)malloc(sizeof(struct list_item));
                for(int i=0; i<=idx ;i++){
                    before = list_next(before);
                }
                i->data = num;
                if(before == &l_arr[name[4]-'0'].head) list_push_front(&l_arr[name[4]-'0'],&i->elem);
                else list_insert(before,&i->elem);
            }
            else if (!strcmp(command,"list_pop_back")){
                name = strtok(NULL," ");
                list_pop_back(&l_arr[name[4] - '0']);
            }
            else if (!strcmp(command,"list_pop_front")){
                name = strtok(NULL," ");
                list_pop_front(&l_arr[name[4] - '0']);
            }
            else if (!strcmp(command,"list_remove")){
                name = strtok(NULL," ");
                idx = atoi(strtok(NULL," "));
                struct list_elem * elem = &l_arr[name[4]-'0'].head;
                for(int i=0; i<=idx ;i++){
                    elem = list_next(elem);
                }
                list_remove(elem);
            }
            else if (!strcmp(command,"list_empty")){
                name = strtok(NULL, " ");
                if(list_empty(&l_arr[name[4] - '0'])) printf("true\n");
                else printf("false\n");
            }
            else if (!strcmp(command,"list_size")){
                name = strtok(NULL," ");
                num = list_size(&l_arr[name[4] - '0']);
                printf("%d\n",num);
            }
            else if (!strcmp(command,"list_max")){
                name = strtok(NULL," ");
                struct list_elem * ele = list_max(&l_arr[name[4] - '0'],less,NULL);
                struct list_item *i = list_entry(ele,struct list_item, elem);
                printf("%d\n",i->data);
            }
            else if (!strcmp(command,"list_min")){
                name = strtok(NULL," ");
                struct list_elem * ele = list_min(&l_arr[name[4] - '0'],less,NULL);
                struct list_item *i = list_entry(ele,struct list_item, elem);
                printf("%d\n",i->data);
            }
            else if (!strcmp(command,"list_reverse")){
                name = strtok(NULL," ");
                list_reverse(&l_arr[name[4] - '0']);
            }
            else if (!strcmp(command,"list_sort")){
                name = strtok(NULL," ");
                list_sort(&l_arr[name[4] - '0'],less,aux);
            }
            else if (!strcmp(command,"list_splice")){
                name = strtok(NULL," ");
                idx = atoi(strtok(NULL," "));
                name2 = strtok(NULL, " ");
                int first = atoi(strtok(NULL, " "));
                int last = atoi(strtok(NULL," "));
                struct list_elem * elem = &l_arr[name[4]-'0'].head;
                for(int i=0; i<=idx ;i++){
                    elem = list_next(elem);
                }
                struct list_elem * f = &l_arr[name2[4]-'0'].head;
                for(int i=0; i<=first ;i++){
                    f = list_next(f);
                }
                struct list_elem * l = &l_arr[name2[4]-'0'].head;
                for(int i=0; i<=last ;i++){
                    l = list_next(l);
                }

                list_splice(elem,f,l);
            }
            else if (!strcmp(command,"dumpdata")){
                name = strtok(NULL," ");
                struct list_elem * e = list_begin(&l_arr[name[4]-'0']);
                struct list_item * it;
                if(e == list_end(&l_arr[name[4]-'0'])) continue;
                for(;e != list_end(&l_arr[name[4]-'0']); e = list_next(e)){
                    it = list_entry(e,struct list_item, elem);
                    printf("%d ",it->data);
                }
                printf("\n");
            }
            else {      //list_unique
                name = strtok(NULL, " ");
                name2 = strtok(NULL, " ");
                if(name2 == NULL) list_unique(&l_arr[name[4] - '0'], NULL,less,aux);
                else list_unique(&l_arr[name[4] - '0'],&l_arr[name2[4] - '0'],less,aux);
            }
        }
    }
    else if(type[0] == 'h'){
        bool (*less)(const struct hash_elem *a, const struct hash_elem *b, void*aux) = hash_less;
        hash_init(&h_arr[name[4]-'0'],hash_func,less,aux);
        while(1){
            fgets(line,40,stdin);
            line[strlen(line)-1] = 0;
            command = strtok(line," ");
            if(!strcmp(command,"quit")) break;
            if(!strcmp(command,"create")){
                type = strtok(NULL," ");
                name = strtok(NULL," ");
                hash_init(&h_arr[name[4]-'0'],hash_func,less,aux);
            }
            else if (!strcmp(command,"delete")){
                name = strtok(NULL, " ");
                hash_init(&h_arr[name[4]-'0'],hash_func,less,aux);
            }
            else if (!strcmp(command,"dumpdata")){
                name = strtok(NULL," ");
                if(h_arr[name[4]-'0'].elem_cnt != 0){
                    hash_apply(&h_arr[name[4]-'0'],*hash_print);
                    printf("\n");
                }
            }
            else if (!strcmp(command,"hash_insert")){
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                struct hash_elem * nhash = (struct hash_elem *)malloc(sizeof(struct hash_elem));
                nhash->data = num;
                hash_insert(&h_arr[name[4]-'0'],nhash);
            }
            else if (!strcmp(command,"hash_apply")){
                name = strtok(NULL," ");
                name2 = strtok(NULL," ");
                if(!strcmp(name2,"square")) hash_apply(&h_arr[name[4]-'0'],*hash_square);
                if(!strcmp(name2,"triple")) hash_apply(&h_arr[name[4]-'0'],*hash_triple);
            }
            else if (!strcmp(command,"hash_delete")){
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                struct hash_elem * he = (struct hash_elem *)malloc(sizeof(struct hash_elem));
                he->data = num;
                hash_delete(&h_arr[name[4]-'0'],he);
            }
            else if (!strcmp(command,"hash_empty")){
                name = strtok(NULL, " ");
                if(hash_empty(&h_arr[name[4]-'0'])) printf("true\n");
                else printf("false\n");
            }
            else if (!strcmp(command,"hash_size")){
                name = strtok(NULL," ");
                printf("%d\n",hash_size(&h_arr[name[4]-'0']));
            }
            else if (!strcmp(command,"hash_clear")){
                name = strtok(NULL," ");
                hash_clear(&h_arr[name[4]-'0'],*h_delete);
            }
            else if (!strcmp(command,"hash_find")){
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                struct hash_elem * he = (struct hash_elem *)malloc(sizeof(struct hash_elem));
                he->data = num;
                he = hash_find(&h_arr[name[4]-'0'],he);
                if(he != NULL) printf("%d\n",he->data);
            }
            else{   //hash_replace
                name = strtok(NULL," ");
                num= atoi(strtok(NULL," "));
                struct hash_elem * he = (struct hash_elem *)malloc(sizeof(struct hash_elem));
                he->data = num;
                hash_replace(&h_arr[name[4]-'0'],he);
            }
        }
    }
    else{
        num = atoi(strtok(NULL," "));
        struct bitmap * bp = bitmap_create((size_t)num);
        b_arr[name[2]-'0'].bit_cnt = bp->bit_cnt;
        b_arr[name[2]-'0'].bits = bp->bits;
        while(1){
            fgets(line,40,stdin);
            line[strlen(line)-1] = 0;
            command = strtok(line," ");
            if(!strcmp(command,"quit")) break;
            if(!strcmp(command,"create")){
                type = strtok(NULL," ");
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                struct bitmap * bp = bitmap_create((size_t)num);
                b_arr[name[2]-'0'].bit_cnt = bp->bit_cnt;
                b_arr[name[2]-'0'].bits = bp->bits;
            }
            else if (!strcmp(command,"dumpdata")){
                name = strtok(NULL," ");
                
                for(size_t i=0 ;i< b_arr[name[2]-'0'].bit_cnt; i++){
                    if(bitmap_test(&b_arr[name[2]-'0'],i)) printf("1");
                    else printf("0");
                }
                printf("\n");
            }
            else if (!strcmp(command,"bitmap_mark")){
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                bitmap_mark(&b_arr[name[2]-'0'],(size_t)num);
            }
            else if (!strcmp(command,"bitmap_all")){
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                num2 = atoi(strtok(NULL," "));
                if(bitmap_all(&b_arr[name[2]-'0'],(size_t)num,(size_t)num2)) printf("true\n");
                else printf("false\n");
            }
            else if (!strcmp(command,"delete")){
                name = strtok(NULL," ");
                b_arr[name[2]-'0'].bits = NULL;
                b_arr[name[2]-'0'].bit_cnt = 0;
            }
            else if (!strcmp(command,"bitmap_any")){
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                num2 = atoi(strtok(NULL," "));
                if(bitmap_any(&b_arr[name[2]-'0'],(size_t)num,(size_t)num2)) printf("true\n");
                else printf("false\n");
            }
            else if (!strcmp(command,"bitmap_contains")){
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                num2 = atoi(strtok(NULL," "));
                name2 = strtok(NULL," ");
                bool value;
                if(!strcmp(name2,"true")) value = true;
                else value = false;
                if(bitmap_contains(&b_arr[name[2]-'0'],(size_t)num,(size_t)num2,value)) printf("true\n");
                else printf("false\n");
            }
            else if (!strcmp(command,"bitmap_count")){
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                num2 = atoi(strtok(NULL," "));
                name2 = strtok(NULL," ");
                bool value;
                if(!strcmp(name2,"true")) value = true;
                else value = false;
                printf("%zu\n",bitmap_count(&b_arr[name[2]-'0'],num,num2,value));
            }
            else if (!strcmp(command,"bitmap_dump")){
                name = strtok(NULL," ");
                bitmap_dump(&b_arr[name[2]-'0']);
            }
            else if (!strcmp(command,"bitmap_set_all")){
                name = strtok(NULL," ");
                name2 = strtok(NULL," ");
                if(name2[0] == 't') bitmap_set_all(&b_arr[name[2]-'0'],true);
                else bitmap_set_all(&b_arr[name[2]-'0'],false);;
            }
            else if (!strcmp(command,"bitmap_flip")){
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                bitmap_flip(&b_arr[name[2]-'0'],num);
            }
            else if (!strcmp(command,"bitmap_none")){
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                num2 = atoi(strtok(NULL," "));
                if(bitmap_none(&b_arr[name[2]-'0'],num,num2)) printf("true\n");
                else printf("false\n");
            }
            else if (!strcmp(command,"bitmap_reset")){
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                bitmap_reset(&b_arr[name[2]-'0'],num);
            }
            else if (!strcmp(command,"bitmap_scan_and_flip")){
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                num2 = atoi(strtok(NULL," "));
                name2 = strtok(NULL," ");
                if(name2[0] == 't')printf("%zu\n",bitmap_scan_and_flip(&b_arr[name[2]-'0'],num,num2,true));
                else printf("%zu\n",bitmap_scan_and_flip(&b_arr[name[2]-'0'],num,num2,false));
            }
            else if (!strcmp(command,"bitmap_scan")){
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                num2 = atoi(strtok(NULL," "));
                name2 = strtok(NULL," ");
                if(name2[0] == 't')printf("%zu\n",bitmap_scan(&b_arr[name[2]-'0'],num,num2,true));
                else printf("%zu\n",bitmap_scan(&b_arr[name[2]-'0'],num,num2,false));
            }
            else if (!strcmp(command,"bitmap_set_multiple")){
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                num2 = atoi(strtok(NULL," "));
                name2 = strtok(NULL," ");
                if(name2[0] == 't') bitmap_set_multiple(&b_arr[name[2]-'0'],num,num2,true);
                else bitmap_set_multiple(&b_arr[name[2]-'0'],num,num2,false);
            }
            else if (!strcmp(command,"bitmap_set")){
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                name2 = strtok(NULL," ");
                if(name2[0] == 't') bitmap_set(&b_arr[name[2]-'0'],num,true);
                else bitmap_set(&b_arr[name[2]-'0'],num,false);
            }
            else if (!strcmp(command,"bitmap_size")){
                name = strtok(NULL," ");
                printf("%zu\n",bitmap_size(&b_arr[name[2]-'0']));
            }
            else if (!strcmp(command,"bitmap_expand")){
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                bitmap_expand(&b_arr[name[2]-'0'],num);                
            }
            else{   //bitmap_test
                name = strtok(NULL," ");
                num = atoi(strtok(NULL," "));
                if(bitmap_test(&b_arr[name[2]-'0'],num)) printf("true\n");
                else printf("false\n");
            }
        }
    }
    return 0;
}