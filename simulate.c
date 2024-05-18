#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//use adjacency list since it is an extremely sparse graph

typedef struct Edge{ //type for storing edge, contain name and pointer of next node
    char nextName;
    struct Edge* next;
}Edge;

typedef struct Node{ //type for storing node, contain name and head of chain
    int WCET;
    char name;
    Edge *head;
}Node;

typedef struct DAG{ //type for graph, contain array of all node
    int numberOfV;
    int deadline;
    Node* nodeArray;
    
    //two arrays for easy conversion of node name and array index;
    int nameToNumber[90]; //ASCII of 'Z' is 90, a stupid but simple way to hash letter to int in C
    char* numberToName;
}DAG;

Edge* newEdge(char nextName){
    Edge* newNode=(Edge*) malloc(sizeof(Edge));
    newNode->nextName=nextName;
    newNode->next=NULL;
    return newNode;
}

DAG* newDAG(int numberOfV, char nameOfV[]){
    srand(time(0));
    DAG* dag=(DAG*)malloc(sizeof(DAG));
    dag->numberOfV=numberOfV;
    dag->nodeArray=(Node*)malloc(numberOfV*sizeof(Node));
    dag->numberToName=(char*)malloc(numberOfV*sizeof(char));
    for (int i = 0; i < 90; i++) { //initialize hash array to -1
        dag->numberToName[i]=-1;
    }
    for (int i = 0; i < numberOfV; i++) {
        dag->nodeArray[i].head=NULL;
        dag->nodeArray[i].name=nameOfV[i];
        dag->nodeArray[i].WCET=(rand()%9)+2; //set WCET to a random number in [2,10]
        dag->nameToNumber[(int)nameOfV[i]]=i;
        dag->numberToName[i]=nameOfV[i];
    }
    return dag;
}

void setEdge(DAG* dag, int prev, int next){
  Edge* edge=newEdge(next);
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
    for (int i = 0; i < 2; i++) {
        int numberOfV=0;
        fscanf(data,"%d",&numberOfV); //get number of vertices
        char nameOfV[numberOfV];
        fscanf(data, "%s",nameOfV); //get name of vertices
        dag[i]=newDAG(numberOfV, nameOfV);
        for (int j = 0; j < numberOfV; j++) {
            char newEdgeHeadName;
            fscanf(data," %c",&newEdgeHeadName);
            char newEdgeHeadNum=dag[i]->nameToNumber[(int)newEdgeHeadName];
            for (int k = 0; k < numberOfV; k++) {
                int newEdgeNextExist=0;
                fscanf(data,"%d",&newEdgeNextExist);
                setEdge(dag[i],newEdgeHeadNum,k);
            }
            printf("\n");
        }
    }

    //set deadline
    dag[0]->deadline=45;
    dag[1]->deadline=60;

    //for (int i = 0; i < 6; i++) {
    //    printf("%c:",dag[0]->nodeArray[i].name);
    //    Edge* tempEdge=dag[0]->nodeArray[i].head;
    //    while (tempEdge->next!=NULL) {
    //        printf("%c ", tempEdge->nextName);
    //    }
    //    printf("\n");
    //}
    

    return 0;
}
