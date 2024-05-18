#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <time.h>

//use adjacency list since it is an extremely sparse graph

typedef struct Edge{ //type for storing edge, contain name and pointer of next node
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

    //two array for name<->index conversion
    char* indexToName;
    int* nameToIndex;
}DAG;

Edge* newEdge(char nextName){
    Edge* newNode=malloc(sizeof(Edge));
    newNode->nextName=nextName;
    newNode->next=NULL;
    return newNode;
}

DAG* newDAG(int numberOfV, char nameOfV[]){
    srand(time(0));
    DAG* dag=malloc(sizeof(DAG));
    dag->numberOfV=numberOfV;
    dag->nodeArray=malloc(numberOfV*sizeof(Node));
    dag->indexToName=malloc(numberOfV*sizeof(char));
    dag->nameToIndex=malloc(((int)'Z')*sizeof(int));
    for (int i = 0; i < numberOfV; i++) {
        dag->nodeArray[i].head=NULL;
        dag->nodeArray[i].name=nameOfV[i];
        dag->nodeArray[i].inDegree=0;
        dag->nodeArray[i].WCET=(rand()%9)+2; //set WCET to a random number in [2,10]
        dag->indexToName[i]=nameOfV[i];
        dag->nameToIndex[(int)nameOfV[i]]=i; //a simple & stupid hash
    }
    dag->deadline=0;
    return dag;
}

void setEdge(DAG* dag, int prev, int next){
  Edge* edge=newEdge(dag->indexToName[next]);
  edge->next=dag->nodeArray[prev].head;
  dag->nodeArray[prev].head=edge;
}

int main(){
    FILE *data=fopen("data.txt", "r");
    if (data==NULL) {
        printf("File open failed!\n");
        return 1;
    }
    int numberOfDAG=0;

    fscanf(data,"%d", &numberOfDAG);//get number of dags
    DAG* dag[numberOfDAG];
    printf("Number of DAG is %d.\n", numberOfDAG);

    //read from data.txt and store them in to DAG[2]
    for (int i = 0; i < 1; i++) {
        int numberOfV=0;
        fscanf(data,"%d",&numberOfV); //get number of vertices
        printf("Number of Vertices is %d\n", numberOfV);
        char nameOfV[numberOfV];
        fscanf(data, "%s",nameOfV); //get name of vertices
        //char nameOfV[6]={'A','B','C','D','E','F'};
        dag[i]=newDAG(numberOfV, nameOfV);
        for (int j = 0; j < numberOfV; j++) {
            char newEdgeHeadName;
            fscanf(data," %c",&newEdgeHeadName);
            printf("get new edge %c\n", newEdgeHeadName);
            for (int k = 0; k < numberOfV; k++) {
                int newEdgeNextExist=0;
                fscanf(data,"%d",&newEdgeNextExist);
                if (newEdgeNextExist) {
                    printf("I am setting edge from %d to %d\n",j,k);
                    setEdge(dag[i],j,k);
                    dag[i]->nodeArray[k].inDegree++;
                }
            }
        }
    }

    //set deadline
    dag[0]->deadline=45;
    //dag[1]->deadline=60;

    for (int i = 0; i < 6; i++) {
        printf("For %c:",dag[0]->nodeArray[i].name);
        Edge* tempEdge=dag[0]->nodeArray[i].head;
        while (tempEdge!=NULL) {
            printf("%c ", tempEdge->nextName);
            tempEdge=tempEdge->next;
        }
        printf("\n");
        printf("Indegree is %d\n",dag[0]->nodeArray[i].inDegree);
    }
    

    return 0;
}

