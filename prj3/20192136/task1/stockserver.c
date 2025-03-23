/* 
 * echoservers.c - A concurrent echo server based on select
 */
/* $begin echoserversmain */
#include "csapp.h"

typedef struct { /* Represents a pool of connected descriptors */ //line:conc:echoservers:beginpool
    int maxfd;        /* Largest descriptor in read_set */   
    fd_set read_set;  /* Set of all active descriptors */
    fd_set ready_set; /* Subset of descriptors ready for reading  */
    int nready;       /* Number of ready descriptors from select */   
    int maxi;         /* Highwater index into client array */
    int clientfd[FD_SETSIZE];    /* Set of active descriptors */
    rio_t clientrio[FD_SETSIZE]; /* Set of active read buffers */
} pool; //line:conc:echoservers:endpool
/* $end echoserversmain */

void init_pool(int listenfd, pool *p);
void add_client(int connfd, pool *p);
void check_clients(pool *p);
/* $begin echoserversmain */


int byte_cnt = 0; /* Counts total bytes received by server */


typedef struct item{
    int ID;
    int left_stock;
    int price;
    int readcnt;
    struct item * left;
    struct item * right;
}item;

item * root = NULL;

void printStock(FILE * fp, item * cur){
    if(cur == NULL) return;
    printStock(fp,cur->left);
    fprintf(fp,"%d %d %d\n",cur->ID,cur->left_stock,cur->price);
    printStock(fp,cur->right);
	return;
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

// item* search(item* cur, int id){
//     while (cur != NULL) {
//         if (cur->ID == id) return cur;
//         else if (cur->ID < id) cur = cur->right;
//         else cur = cur->left;
//     }
//     return NULL;
// }

int main(int argc, char **argv)
{
    Signal(SIGINT,sigint_handler);
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    static pool pool; 
    char client_hostname[MAXLINE], client_port[MAXLINE];

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
    init_pool(listenfd, &pool); //line:conc:echoservers:initpool

    while (1) {
	/* Wait for listening/connected descriptor(s) to become ready */
	pool.ready_set = pool.read_set;
	pool.nready = Select(pool.maxfd+1, &pool.ready_set, NULL, NULL, NULL);

	if (FD_ISSET(listenfd, &pool.ready_set)) {
        clientlen = sizeof(struct sockaddr_storage);
	    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA*)& clientaddr, clientlen, client_hostname, MAXLINE,
				client_port, MAXLINE, 0);
	    add_client(connfd, &pool); //line:conc:echoservers:addclient
        printf("Connected to (cspro.sogang.ac.kr, %s)\n",client_port);
	}
	
	/* Echo a text line from each ready connected descriptor */ 
	check_clients(&pool); //line:conc:echoservers:checkclients
    }
}
/* $end echoserversmain */

/* $begin init_pool */
void init_pool(int listenfd, pool *p) 
{
    /* Initially, there are no connected descriptors */
    int i;
    p->maxi = -1;                   //line:conc:echoservers:beginempty
    for (i=0; i< FD_SETSIZE; i++)  
	p->clientfd[i] = -1;        //line:conc:echoservers:endempty

    /* Initially, listenfd is only member of select read set */
    p->maxfd = listenfd;            //line:conc:echoservers:begininit
    FD_ZERO(&p->read_set);
    FD_SET(listenfd, &p->read_set); //line:conc:echoservers:endinit
}
/* $end init_pool */

/* $begin add_client */
void add_client(int connfd, pool *p) 
{
    int i;
    p->nready--;
    for (i = 0; i < FD_SETSIZE; i++)  /* Find an available slot */
	if (p->clientfd[i] < 0) { 
	    /* Add connected descriptor to the pool */
	    p->clientfd[i] = connfd;                 //line:conc:echoservers:beginaddclient
	    Rio_readinitb(&p->clientrio[i], connfd); //line:conc:echoservers:endaddclient

	    /* Add the descriptor to descriptor set */
	    FD_SET(connfd, &p->read_set); //line:conc:echoservers:addconnfd

	    /* Update max descriptor and pool highwater mark */
	    if (connfd > p->maxfd) //line:conc:echoservers:beginmaxfd
		p->maxfd = connfd; //line:conc:echoservers:endmaxfd
	    if (i > p->maxi)       //line:conc:echoservers:beginmaxi
		p->maxi = i;       //line:conc:echoservers:endmaxi
	    break;
	}
    if (i == FD_SETSIZE) /* Couldn't find an empty slot */
	app_error("add_client error: Too many clients");
}
/* $end add_client */

/* $begin check_clients */
void check_clients(pool *p) 
{
    int i, connfd, n;
    char buf[MAXLINE];
    char str[MAXLINE]; // 터지면 크기 증가시키기
    int findID, stockCnt;
    rio_t rio;

    for (i = 0; (i <= p->maxi) && (p->nready > 0); i++) {
	connfd = p->clientfd[i];
	rio = p->clientrio[i];

	/* If the descriptor is ready, echo a text line from it */
	if ((connfd > 0) && (FD_ISSET(connfd, &p->ready_set))) { 
	    p->nready--;
	    if ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
		    byte_cnt += n; //line:conc:echoservers:beginecho
		    printf("Server received %d bytes on fd %d\n", n,connfd);
			strcat(str,buf);

            if(strncmp("show", buf, 4) == 0){
                show(root,str);
            }
            else if (strncmp("sell", buf, 4) == 0){ // sell [주식ID] [팔주식개수]
                findID = atoi(buf+5);
                stockCnt = atoi(strchr(buf+5,32));
                item * find = search(root,findID);
                if(find == NULL){
                    strcat(str,"Stock does not exist\n");
                }
                else{
                    strcat(str, "[sell] success\n");
                    find->left_stock += stockCnt;
                }
            }
            else if (strncmp("buy", buf, 3) == 0){ // buy [주식ID] [살주식개수]
                findID = atoi(buf+4);
                stockCnt = atoi(strchr(buf+4,32));
                item * find = search(root,findID);
                if(find == NULL){
                    strcat(str,"Stock does not exist\n");                    
                }
                else if (find->left_stock < stockCnt){
                    strcat(str,"Not enough left stock\n");
                }
                else{
                    find->left_stock -= stockCnt;
                    strcat(str,"[buy] success\n");
                }
                
            }
            // else if (strncmp("exit", buf, 4) == 0){
            //     raise(SIGINT);
            // }
		    Rio_writen(connfd, str, sizeof(str)); //line:conc:echoservers:endecho
            //client 개수 증가시켰을 때 터지면 sizeof(str)로 변경
            memset(str,0,sizeof(str));
            //str[0] = '\0';
	    }
	    /* EOF detected, remove descriptor from pool */
	    else { 
		Close(connfd); //line:conc:echoservers:closeconnfd
		FD_CLR(connfd, &p->read_set); //line:conc:echoservers:beginremove
		p->clientfd[i] = -1;          //line:conc:echoservers:endremove
	    }
	}
    }
}
/* $end check_clients */

