/* 
 * echoservert_pre.c - A prethreaded concurrent echo server
 */
/* $begin echoservertpremain */
#include "csapp.h"
#include "sbuf.h"
#define NTHREADS  4
#define SBUFSIZE  16

typedef struct {
    int *buf;          /* Buffer array */         
    int n;             /* Maximum number of slots */
    int front;         /* buf[(front+1)%n] is first item */
    int rear;          /* buf[rear%n] is last item */
    sem_t mutex;       /* Protects accesses to buf */
    sem_t slots;       /* Counts available slots */
    sem_t items;       /* Counts available items */
} sbuf_t;

typedef struct item{
    int ID;
    int left_stock;
    int price;
    int readcnt;
    sem_t mutex;
}item;

void sbuf_init(sbuf_t *sp, int n);
void sbuf_deinit(sbuf_t *sp);
void sbuf_insert(sbuf_t *sp, int item);
int sbuf_remove(sbuf_t *sp);

void echo_cnt(int connfd);
void *thread(void *vargp);

sbuf_t sbuf; /* Shared buffer of connected descriptors */
item * root = NULL;

void sbuf_init(sbuf_t *sp, int n)
{
    sp->buf = Calloc(n, sizeof(int)); 
    sp->n = n;                       /* Buffer holds max of n items */
    sp->front = sp->rear = 0;        /* Empty buffer iff front == rear */
    Sem_init(&sp->mutex, 0, 1);      /* Binary semaphore for locking */
    Sem_init(&sp->slots, 0, n);      /* Initially, buf has n empty slots */
    Sem_init(&sp->items, 0, 0);      /* Initially, buf has zero data items */
}

void sbuf_deinit(sbuf_t *sp)
{
    Free(sp->buf);
}

void sbuf_insert(sbuf_t *sp, int item)
{
    P(&sp->slots);                          /* Wait for available slot */
    P(&sp->mutex);                          /* Lock the buffer */
    sp->buf[(++sp->rear)%(sp->n)] = item;   /* Insert the item */
    V(&sp->mutex);                          /* Unlock the buffer */
    V(&sp->items);                          /* Announce available item */
}

int sbuf_remove(sbuf_t *sp)
{
    int item;
    P(&sp->items);                          /* Wait for available item */
    P(&sp->mutex);                          /* Lock the buffer */
    item = sp->buf[(++sp->front)%(sp->n)];  /* Remove the item */
    V(&sp->mutex);                          /* Unlock the buffer */
    V(&sp->slots);                          /* Announce available slot */
    return item;
}


void sigint_handler(int sig){
    FILE * fp = fopen("stock.txt","w");
    if(!fp){
        printf("file error\n");
        exit(1);
    }
    printStock(fp,root);
    fclose(fp);
    exit(1);
}

void printStock(FILE * fp, item * cur){
    if(cur == NULL) return;
    printStock(fp,cur->left);
    fprintf(fp,"%d %d %d\n",cur->ID,cur->left_stock,cur->price);
    printStock(fp,cur->right);
	return;
}

item * insert(item* cur, item* i){
    if(cur == NULL){
        cur = i;
        return cur;
    }
    if(cur->ID <= i->ID){
        cur->right = insert(cur->right,i);
    }
    else{
        cur->left = insert(cur->left,i);
    }
    return cur;
}

void show(item * cur, char * str){
    if(cur == NULL) return;
    char temp[MAXLINE];
    show(cur->left,str);
    sprintf(temp,"%d %d %d\n",cur->ID,cur->left_stock,cur->price);
	strcat(str,temp);
    show(cur->right,str);
}

item * search(item * cur, int id){
    if(cur == NULL) return NULL;

    if(cur->ID == id) return cur;
    else if(cur->ID <= id) search(cur->right,id);
    else search(cur->left,id);
}

int main(int argc, char **argv) 
{
    int i, listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid; 

    if (argc != 2) {
	fprintf(stderr, "usage: %s <port>\n", argv[0]);
	exit(0);
    }
    
    FILE * fp = fopen("stock.txt","r");
    if(!fp){
        printf("file error\n");
        exit(1);
    }

    int sid,sleft,sprice;
    while(fscanf(fp,"%d %d %d",&sid,&sleft,&sprice) != EOF){
        item * i = (item *)malloc(sizeof(item));
        i->ID = sid;
        i->left_stock = sleft;
        i->price = sprice;
        i->left = NULL;
        i->right = NULL;
        root = insert(root,i);
    }
    fclose(fp);

    listenfd = Open_listenfd(argv[1]);
    sbuf_init(&sbuf, SBUFSIZE); //line:conc:pre:initsbuf
    for (i = 0; i < NTHREADS; i++)
        Pthread_create(&tid, NULL, thread, NULL);
    while (1) { 
        clientlen = sizeof(struct sockaddr_storage);
	    connfd = Accept(listenfd, (SA *) &clientaddr, &clientlen);
        Getnameinfo((SA*)& clientaddr, clientlen, client_hostname, MAXLINE,
				client_port, MAXLINE, 0);
	    sbuf_insert(&sbuf, connfd); /* Insert connfd in buffer */
        printf("Connected to (cspro.sogang.ac.kr, %s)\n",client_port);
    }

    return 0;
}


void *thread(void *vargp) 
{  
    Pthread_detach(pthread_self()); 
    while (1) { 
	    int connfd = sbuf_remove(&sbuf);
	    echo_cnt(connfd);
	    Close(connfd);
    }
}
/* $end echoservertpremain */
