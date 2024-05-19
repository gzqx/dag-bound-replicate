#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>

#define INF 999999 //999999 is effectively larger than any number in this program

//use adjacency list since it is an extremely sparse graph

typedef struct Edge{ //type for storing edge, contain name and pointer of next node
    int nextIndex;
    char nextName;
    struct Edge* next;
}Edge;

//typedef struct InverseEdge{ //for easy calculation of in-degree
//    char prevName;
//    struct InverseEdge* prev;
//}InverseEdge;

typedef struct Node{ //type for storing node, contain name and head of chain
    int WCET;
    char name;
    Edge *head;
    int inDegree;
}Node;

typedef struct DAG{ //type for graph, contain array of all node
    int numberOfV;
    int deadline;
    Node* nodeArray;
    int Vol;

    //two array for name<->index conversion
    char* indexToName;
}DAG;

Edge* newEdge(char nextName, int nextIndex){
    Edge* newNode=malloc(sizeof(Edge));
    newNode->nextName=nextName;
    newNode->nextIndex=nextIndex;
    newNode->next=NULL;
    return newNode;
}

DAG* newDAG(int numberOfV, char nameOfV[]){
    DAG* dag=malloc(sizeof(DAG));
    dag->numberOfV=numberOfV;
    dag->nodeArray=malloc(numberOfV*sizeof(Node));
    dag->indexToName=malloc(numberOfV*sizeof(char));
    dag->Vol=0;
    for (int i = 0; i < numberOfV; i++) {
        dag->nodeArray[i].head=NULL;
        dag->nodeArray[i].name=nameOfV[i];
        dag->nodeArray[i].inDegree=0;
        dag->nodeArray[i].WCET=(rand()%9)+2; //set WCET to a random number in [2,10]
        dag->indexToName[i]=nameOfV[i];
        dag->Vol+=dag->nodeArray[i].WCET;
    }
    dag->deadline=0;
    return dag;
}

void setEdge(DAG* dag, int prev, int next){
  Edge* edge=newEdge(dag->indexToName[next],next);
  edge->next=dag->nodeArray[prev].head;
  dag->nodeArray[prev].head=edge;
}


typedef struct PathVertex{
    int vertex;
    int WCET;
    struct PathVertex* next;
    struct PathVertex* prev;
}PathVertex;

typedef struct Path{
    int length;
    struct PathVertex* head;
    struct PathVertex* tail;
}Path;

typedef struct PathList{
    int k;
    Path** pathArray;
}PathList;

DAG* topololgicalSorting(DAG* dag){
    //the input data has already been sorted
    return dag;
}

PathVertex* newPathVertex(DAG* dag, int vertex){
    PathVertex* Vertex=malloc(sizeof(PathVertex));
    Vertex->vertex=vertex;
    Vertex->WCET=dag->nodeArray[vertex].WCET;
    Vertex->next=NULL;
    Vertex->prev=NULL;
    return Vertex;
}

Path* newPath(DAG* dag, int head){
    Path* path=malloc(sizeof(Path));
    PathVertex* headVertex=newPathVertex(dag, head);
    path->head=headVertex;
    path->tail=headVertex;
    path->length=0+headVertex->WCET;
    return path;
}

Path* shiftPath(Path* path, DAG* dag, int newHead){
    PathVertex* NewHead=newPathVertex(dag, newHead);
    path->head->prev=NewHead;
    NewHead->next=path->head->prev;
    path->head=NewHead;
    path->length+=NewHead->WCET;
    return path;
}

Path* appendPath(Path* path, DAG* dag, int newTail){
    PathVertex* NewTail=newPathVertex(dag, newTail);
    path->tail->next=NewTail;
    NewTail->prev=path->tail;
    path->tail=NewTail;
    path->length+=NewTail->WCET;
    return path;
}

PathList* newPathList(){
    PathList* pathList=malloc(sizeof(PathList));
    pathList->k=0;
    pathList->pathArray=NULL;
    return pathList;
}

void addPath(PathList* pathList,Path* path){
    pathList->k++;
    pathList->pathArray=realloc(pathList->pathArray, pathList->k*sizeof(Path*));
    pathList->pathArray[pathList->k-1]=path;
}

Path* longestPathFromVertex(DAG* dag, int sourceVertex){
    int* maxLength=malloc(dag->numberOfV*sizeof(int));
    int* recordPath=malloc(dag->numberOfV*sizeof(int));
    for (int i = 0; i < dag->numberOfV; i++) {
        maxLength[i]=-INF;
        recordPath[i]=-1;
    }
    maxLength[sourceVertex]=dag->nodeArray[sourceVertex].WCET;
    for (int i = 0; i < dag->numberOfV; i++) {
        Edge* adVertex=dag->nodeArray[i].head;
        while (adVertex!=NULL) {
            int j=adVertex->nextIndex;
            int a=maxLength[j];
            int b=maxLength[i]+dag->nodeArray[j].WCET;
            maxLength[j]=a>b?a:b;
            recordPath[i]=a>b?recordPath[i]:j;
            //printf("Current dealing with vertex %d and its adjacent %d: maxlength of %d is %d. maxlength of %d is %d+%d. Thus maxlength now is %d.\n Thus longest path to %d is from %d.\n",i,adVertex->nextIndex,j,a,i,maxLength[i],dag->nodeArray[j].WCET,maxLength[j],j,recordPath[j]);
            adVertex=adVertex->next;
        }
    }

    Path* longestPath=newPath(dag, sourceVertex);
    int iterate=sourceVertex;
    while (recordPath[iterate]!=-1) {
        iterate=recordPath[iterate];
        appendPath(longestPath, dag, iterate);
    }
    return longestPath;
}

Path* longestPathInDAG(DAG* dag){
    Path* longestPath=NULL;
    for (int i = 0; i < dag->numberOfV; i++) {
        Path* longestPathTemp=longestPathFromVertex(dag, i);
        if (longestPath==NULL || longestPathTemp->length>longestPath->length) {
            longestPath=longestPathTemp;
        }else{
            free(longestPathTemp);
        }
    }
    return longestPath;
}

void printPath(Path* path){
    PathVertex* v=path->head;
    printf("Following is a path of length %d:\n",path->length);
    while (v!=NULL) {
        printf("%d ",v->vertex);
        v=v->next;
    }
    printf("\n");
}

void printWCET(DAG* dag){
    for (int i = 0; i < dag->numberOfV; i++) {
        printf("%c:%d\n",dag->indexToName[i],dag->nodeArray[i].WCET);
    }
}

void resDAG(DAG* dag, Path* lambda){
    PathVertex* v=lambda->head;
    while (v!=NULL) {
        dag->nodeArray[v->vertex].WCET=0;
        dag->Vol-=v->WCET;
        v=v->next;
    }
}

void pathRemove(Path* path, PathVertex* v){
    PathVertex* a=v->prev;
    PathVertex* b=v->next;
    if (a==NULL) {
        path->head=b;
        b->prev=a;
    }else if (b==NULL) {
        path->tail=a;
        a->next=b;
    }else{
        a->next=b;
        b->prev=a;
    }
    path->length=path->length-v->WCET;
}


int main(){
    const int m=3; //number of processors
    const int runTime=10; //run 10 times
    srand(time(0));

    FILE *data=fopen("data.txt", "r");
    if (data==NULL) {
        printf("File open failed!\n");
        return 1;
    }
    int numberOfDAG=0;

    fscanf(data,"%d", &numberOfDAG);//get number of dags
    DAG* dag[numberOfDAG];
    //printf("Number of DAG is %d.\n", numberOfDAG);

    //read from data.txt and store them in to DAG[2]
    for (int i = 0; i < numberOfDAG; i++) {
        int numberOfV=0;
        fscanf(data,"%d",&numberOfV); //get number of vertices
        //printf("Number of Vertices is %d\n", numberOfV);
        char nameOfV[numberOfV];
        fscanf(data, "%s",nameOfV); //get name of vertices
        //char nameOfV[6]={'A','B','C','D','E','F'};
        dag[i]=newDAG(numberOfV, nameOfV);
        for (int j = 0; j < numberOfV; j++) {
            char newEdgeHeadName;
            fscanf(data," %c",&newEdgeHeadName);
            //printf("get new edge %c\n", newEdgeHeadName);
            for (int k = 0; k < numberOfV; k++) {
                int newEdgeNextExist=0;
                fscanf(data,"%d",&newEdgeNextExist);
                if (newEdgeNextExist) {
                    //printf("I am setting edge from %d to %d\n",j,k);
                    setEdge(dag[i],j,k);
                    dag[i]->nodeArray[k].inDegree++;
                }
            }
        }
    }

    //set deadline
    dag[0]->deadline=45;
    dag[1]->deadline=60;

    PathList* generalizedPathList=newPathList();

    //Algorithm 2
    for (int d = 0; d < numberOfDAG; d++) { 
            int originVolDAG=dag[d]->Vol;
            while (dag[d]->Vol!=0) {
                Path* lambda=longestPathInDAG(dag[d]);
                PathVertex* v=lambda->head;
                while (v!=NULL && lambda->length!=0) {
                    if (v->WCET==0) {
                        pathRemove(lambda, v);
                    }
                    v=v->next;
                }
                addPath(generalizedPathList, lambda);
                resDAG(dag[d], lambda);
            }
            //Theorem 2
            int R=INF;
            int k=generalizedPathList->k<(m-1)?(generalizedPathList->k):(m-1);
            for (int j = 0; j < k; j++) {
                //len(G)=len(lambda_0)
                int sumOfLen=0;
                for (int i = 0; i < j; i++) {
                    sumOfLen+=generalizedPathList->pathArray[i]->length;
                }
                int bound=(generalizedPathList->pathArray[0]->length)+(originVolDAG-sumOfLen)/(m-j);
                //printf("bound is %d+(%d-%d)/(%d-%d)", (generalizedPathList->pathArray[0]->length),originVolDAG,sumOfLen,m,j);
                R=R<bound?R:bound;
            }

            printf("\n");
            printf("===================\n");
            printf("The response time of DAG G%d is %d\n",d+1,R);
            printf("It ");
            if (R>dag[d]->deadline) {
                printf("failed ");
            }else{
                printf("succeeded ");
            }
            printf("in meeting the deadline\n");
            printf("===================\n");
            printf("\n");

    }

    //free all memory used
    for (int i = 0; i < numberOfDAG; i++) {
        for (int j = 0; j < dag[i]->numberOfV; j++) {
            Edge* temp=dag[i]->nodeArray[j].head;
            while (temp!=NULL) {
                Edge* tempTemp=temp->next;
                free(temp);
                temp=NULL;
                temp=tempTemp;
            }
        }
        free(dag[i]->nodeArray);
        dag[i]->nodeArray=NULL;
        free(dag[i]);
        dag[i]=NULL;
    }

    return 0;
}

