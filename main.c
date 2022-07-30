#include <stdio.h>
#include <stdlib.h>

struct Childs{
    int cod;
    struct Childs *prox;
};

typedef struct Childs Childs;

struct Vertice{
    int cod; 
    int distance; 
    char status; //{"b": "black", "g": "grey", "w" = "white"}
    struct Vertice *parent;
    int timeStamp;
    Childs *childs;
};

typedef struct Vertice Vertice;

typedef struct{
    int nVertices;
    int nArestas; 
    Vertice *vertices;
}Grafo;

struct Queue{
    Vertice *vertice;
    struct Queue *prox;
};

typedef struct Queue Queue;

FILE *retornaArquivo() {
    char nome[101];
    FILE *fp;
    printf("Insira o nome(100) do arquivo que contem o grafo: ");
    setbuf(stdin, NULL);
    fgets(nome, 101, stdin);

    for (int i = 0; i<101; i++){
        if(nome[i] == '\n')
            nome[i] = '\0';
    }

    fp = fopen(nome, "r");

    if(fp == NULL){
        printf("Erro ao abrir o arquivo!");
        getchar();
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    if(ftell(fp) == 0){
        printf("arquivo vazio!");
        getchar();
        exit(1);
    }

    rewind(fp);

    return fp;
}

int potencia(int base, int expoente){
  int result = 1;

  if(expoente == 0){
    return 1;
  }

  for(int i = 0; i< expoente; i++){
    result = result*base;
  }

  return result;
}

FILE *lerLinha(FILE *fp, int *param1, int *param2){
    char *quantidade = NULL, c;
    int i = 0, n;

    *param1 = 0;
    *param2 = 0;

     while(1){
        c=fgetc(fp);
        if(c == '\n' || c == EOF){
            break;
        }

        if(c != ' '){
            i++;
            quantidade = realloc(quantidade, i*sizeof(char));
            quantidade[i-1] = c;
        }else{
            for(int j=1;j<=i;j++){ 
                n = quantidade[j-1] - 48;
                *param1 += n*potencia(10, i-j);           
            }

            i=0;
        }
    }

    for(int j=1;j<=i;j++){ 
        n = quantidade[j-1] - 48;
        *param2 += n*potencia(10, i-j);
    }

    return fp;
}

Vertice criaVertice(int cod){
    Vertice vertice;

    vertice.cod = cod;
    vertice.parent = NULL;
    vertice.status = 'w';
    vertice.distance = 0;
    vertice.timeStamp = 0;
    vertice.childs = NULL;

    return vertice;
}


Grafo *criaGrafo(int nVertices, int nArestas){
    
    int n, i=0;
    Grafo *grafo = malloc(sizeof(Grafo));
    Vertice *vertices;

    grafo->nVertices = nVertices;
    grafo->nArestas = nArestas;

    grafo->vertices =  malloc(nVertices*sizeof(Vertice));

    for(i = 0; i<nVertices; i++){
        grafo->vertices[i] = criaVertice(i);
    }   

    return grafo;
}

void montaAdjacentes(FILE *fp, Grafo *grafo){
    int parent, child;
    char c;
    Childs *novoAdj, *adj;

    while(!feof(fp)){
        fp = lerLinha(fp, &parent, &child);

        novoAdj = malloc(sizeof(Childs));
        novoAdj->cod = child;
        novoAdj->prox = NULL;

        adj = grafo->vertices[parent].childs;

        if(adj == NULL){
            grafo->vertices[parent].childs = novoAdj;
        }else{
            while(adj->prox != NULL){
                adj = adj->prox;
            }

            adj->prox = novoAdj;
        }
    }  
}

void printAdjacentes(Grafo *grafo){
    Childs *adj;

    for(int i = 0; i<grafo->nVertices; i++){
        printf("[%d] ", grafo->vertices[i].cod);
        adj = grafo->vertices[i].childs;
        while (adj!=NULL)
        {
            printf("-> %d ", adj->cod);
            adj = adj->prox;
        }

        printf("\n");
        
    }

}

Queue* criarQueue(){
    Queue* q = malloc(sizeof(Queue));
    q->prox = NULL;
    q->vertice = NULL;
    return q;
}

void enQueu(Queue *cabeca, Vertice *v){
    Queue *no = cabeca, *novoNo;

    novoNo = malloc(sizeof(Queue));
    novoNo->prox = NULL;
    novoNo->vertice = v;

    while (no->prox != NULL)
    {
        no = no->prox;
    }

    no->prox = novoNo;    
}

Vertice *deQueue(Queue *cabeca){
    Vertice *u;
    Queue *no = cabeca->prox;
    cabeca->prox = no->prox;
    u = no->vertice;
    return u;
}

void printQueue(Queue *cabeca){
    Queue *no = cabeca->prox;
    if(no == NULL){
        printf("Fila vazia!\n");
    }
    while(no != NULL){
        printf("%d | %c\n", no->vertice->cod, no->vertice->status);
        no = no->prox;
    }
}

void printDistance(Grafo *grafo){
    int  pai;
    printf("\n\n");
    for(int i = 0; i < grafo->nVertices; i++){

        if (grafo->vertices[i].parent == NULL){
            pai = -1;
        }else{
           pai = grafo->vertices[i].parent->cod;
        }
        //print([v, cor, pai, d]) - vizinhos
        printf("vertice: %d | status: %c | pai: %d | distancia: %d\n", grafo->vertices[i].cod, grafo->vertices[i].status, pai, grafo->vertices[i].distance);
        
        
        printf("\n");
    }
}

void BFS(Grafo *grafo, int origem){
    Vertice *s, *u, *v;
    Childs *c;
    Queue *cabeca;
    int j = 0;
    for (int i = 0; i < grafo->nVertices; i++)
    {
        grafo->vertices[i].distance = -1;
        grafo->vertices[i].status = 'w';
        grafo->vertices[i].parent = NULL;
    }

    s = &grafo->vertices[origem];
    s->status = 'g';
    s->distance = 0;
    s->parent = NULL;

    cabeca = criarQueue();
    enQueu(cabeca, s);
    printQueue(cabeca);
    while (cabeca->prox != NULL){
        u = deQueue(cabeca);
        c = u->childs;  

        while (c!=NULL)
        {
            v = &grafo->vertices[c->cod];
            if(v->status == 'w'){
                v->status = 'g';
                v->distance = u->distance+1;
                v->parent = u;
                enQueu(cabeca, v);
            }
            c = c->prox;
        }

        u->status = 'b';
        j++;
        
    }       
}

void DFS_VISIT(Grafo *grafo, Vertice *u, int *tempo){
    Childs *c;
    Vertice *v;
    *tempo = *tempo+1;

    u->distance = *tempo;
    u->status = 'g';

    c = u->childs;

    while (c != NULL){
        v = &grafo->vertices[c->cod];
        if(v->status == 'w'){
            v->parent = u;
            DFS_VISIT(grafo, v, tempo);
        }
        c = c->prox;
    }

    u->status = 'b';
    *tempo = *tempo+1;
    u->timeStamp = *tempo;
}

void DFS(Grafo *grafo){
    int tempo;
    Vertice *u;
    for(int i=0; i<grafo->nVertices; i++){
        u = &grafo->vertices[i];
        u->status = 'w';
        u->parent = NULL;
    }

    tempo = 0;

    for(int i=0; i<grafo->nVertices; i++){
        u = &grafo->vertices[i];
        
        if(u->status == 'w')
            DFS_VISIT(grafo, u, &tempo);
    }
}

void printTimeStamp(Grafo *grafo){
    int  pai;
    printf("\n\n");
    for(int i = 0; i < grafo->nVertices; i++){

        if (grafo->vertices[i].parent == NULL){
            pai = -1;
        }else{
           pai = grafo->vertices[i].parent->cod;
        }
        //print([v, cor, pai, d]) - vizinhos
        printf("vertice: %d | status: %c | pai: %d | distancia: %d | carimbo: %d\n", grafo->vertices[i].cod, grafo->vertices[i].status, pai, grafo->vertices[i].distance, grafo->vertices[i].timeStamp);
        
        
        printf("\n");
    }
}




int main() {
    int nVertices, nArestas;
    char str[9];
    Grafo *grafo;
    FILE *fp;

    fp = retornaArquivo();

    fp = lerLinha(fp, &nVertices, &nArestas);

    grafo = criaGrafo(nVertices, nArestas);
    montaAdjacentes(fp, grafo);
    printAdjacentes(grafo);
    printf("----------------------------------------------------");
    BFS(grafo, 0);
    printDistance(grafo);
    printf("----------------------------------------------------");
    DFS(grafo);
    printTimeStamp(grafo);


    printf("chegou");
}