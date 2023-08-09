#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#define MAX 20

char ADD_STATION[18]= "aggiungi-stazione";
char DELETE_STATION[19]= "demolisci-stazione";
char ADD_CAR[14]="aggiungi-auto";
char DELETE_CAR[13]="rottama-auto";
char PERCORSO[19]="pianifica-percorso";

int numero_stazioni = 0;

//Definisco elemento dell'heap
typedef struct {
    int *cars;
    int car_size;
    int capacity;
} parco_auto;

typedef struct TreeNode {
    int km;
    parco_auto * max_heap;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

TreeNode* stazioni = NULL;


// Funzione per creare un nuovo max heap
parco_auto * createHeap() {
    parco_auto * el = (parco_auto *)malloc(sizeof(parco_auto ));
    el->cars = (int*)malloc(8 * sizeof(int));
    el->car_size = 0;
    el->capacity = 8;
    return el;
}

// Funzione per liberare la memoria del max heap
void freeHeap(parco_auto * el) {
    free(el->cars);
    free(el);
}

// Funzione per raddoppiare la grandezza del max heap
void resizeHeap(parco_auto * el) {
    int newCapacity = el->capacity * 2;
    el->cars = (int*)realloc(el->cars, newCapacity * sizeof(int));
    el->capacity = newCapacity;
}

// Funzione per heapify-down per mantenere il max heap property
void heapifyDown(parco_auto * el, int index) {
    int maxIndex = index;
    int leftChild = 2 * index + 1;
    int rightChild = 2 * index + 2;

    if (leftChild < el->car_size && el->cars[leftChild] > el->cars[maxIndex]) {
        maxIndex = leftChild;
    }

    if (rightChild < el->car_size && el->cars[rightChild] > el->cars[maxIndex]) {
        maxIndex = rightChild;
    }

    if (maxIndex != index) {
        int temp = el->cars[index];
        el->cars[index] = el->cars[maxIndex];
        el->cars[maxIndex] = temp;
        heapifyDown(el, maxIndex);
    }
}

// Funzione per eseguire l'inserimento nel max heap
void insertHeap(parco_auto * el, int value) {
    if (el->car_size >= el->capacity) {
        resizeHeap(el);
    }

    // Inseriamo il nuovo elemento all'ultimo posto
    el->cars[el->car_size] = value;
    int currentIndex = el->car_size;
    el->car_size++;

    // Sistemiamo l'elemento nell'heap risalendo finché non rispetta la condizione del max heap
    while (currentIndex > 0 && el->cars[currentIndex] > el->cars[(currentIndex - 1) / 2]) {
        int parentIndex = (currentIndex - 1) / 2;
        int temp = el->cars[currentIndex];
        el->cars[currentIndex] = el->cars[parentIndex];
        el->cars[parentIndex] = temp;
        currentIndex = parentIndex;
    }
}

// Funzione per inserire un nuovo nodo nell'albero
TreeNode* insertNode(TreeNode* root, TreeNode* newNode) {
    if (root == NULL) {
        return newNode;
    }

    if (newNode->km < root->km) {
        root->left = insertNode(root->left, newNode);
    } else if (newNode->km > root->km) {
        root->right = insertNode(root->right, newNode);
    }

    return root;
}


// Funzione per la ricerca di una stazione nell'albero
TreeNode* searchStation(TreeNode* node, int km) {
    if (node == NULL || node->km == km) {
        return node;
    }

    if (km < node->km) {
        return searchStation(node->left, km);
    } else {
        return searchStation(node->right, km);
    }
}

// Funzione per aggiungere una nuova stazione all'albero
void aggiungi_stazione(int km, int *automobili, int na) {
    if (searchStation(stazioni, km) != NULL) {
        printf("non aggiunta\n");
    } else {
        TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
        newNode->km = km;
        newNode->max_heap = createHeap();
        newNode->left = NULL;
        newNode->right = NULL;

        for (int i = 0; i < na; i++) {
            insertHeap(newNode->max_heap, automobili[i]);
        }

        stazioni = insertNode(stazioni, newNode); // Inserisce il nuovo nodo nell'albero
        printf("aggiunta\n");

    }
}

// Funzione per trovare il nodo con il valore minimo nell'albero
TreeNode* minValueNode(TreeNode* node) {
    TreeNode* current = node;
    while (current && current->left != NULL) {
        current = current->left;
    }
    return current;
}

// Funzione per eliminare un nodo dall'albero
TreeNode* deleteNode(TreeNode* root, int km) {
    if (root == NULL) {
        return root;
    }

    if (km < root->km) {
        root->left = deleteNode(root->left, km);
    } else if (km > root->km) {
        root->right = deleteNode(root->right, km);
    } else {
        if (root->left == NULL) {
            TreeNode* temp = root->right;
            freeHeap(root->max_heap);
            free(root);
            return temp;
        } else if (root->right == NULL) {
            TreeNode* temp = root->left;
            freeHeap(root->max_heap);
            free(root);
            return temp;
        }

        TreeNode* temp = minValueNode(root->right);
        root->km = temp->km;
        root->right = deleteNode(root->right, temp->km);
    }
    return root;
}


// Funzione per demolire una stazione dall'albero
void demolisci_stazione(int km) {
    TreeNode* stationNode = searchStation(stazioni, km);

    if (stationNode != NULL) {
        stazioni = deleteNode(stazioni, km); // Elimina il nodo dalla radice
        printf("demolita\n");
    } else {
        printf("non demolita\n");
    }
}


/*
//Prima trovo la stazione giusta nella lista, poi la aggiungo nel corrispettivo max heap
void aggiungi_auto(int km, int autonomia_auto){
    int index = ricerca_stazione(km);
    if(index != -1){
        insertHeap(stazioni[index].max_heap, autonomia_auto);
        printf("aggiunta\n");
    } else {
        printf("non aggiunta\n");
    }
}

//Funzione che viene chiamata per eliminare un' auto dalla stazione al chilometro km
void rottama_auto(int km, int autonomia_auto) {
    int index = ricerca_stazione(km);
    if (index != -1) {
        // Trova l'elemento da eliminare nell'heap
        int i;
        for (i = 0; i < stazioni[index].max_heap->car_size; i++) {
            if (stazioni[index].max_heap->cars[i] == autonomia_auto) {
                break;
            }
        }

        if (i < stazioni[index].max_heap->car_size) {
            // Rimuovi l'elemento dall'heap spostando l'ultimo elemento in posizione i e riduci la dimensione dell'heap
            stazioni[index].max_heap->cars[i] = stazioni[index].max_heap->cars[stazioni[index].max_heap->car_size - 1];
            stazioni[index].max_heap->car_size--;

            // Esegui heapify-down per mantenere la proprietà del max heap
            heapifyDown(stazioni[index].max_heap, i);

            printf("rottamata\n");
        } else {
            printf("non rottamata\n");
        }
    } else {
        printf("non rottamata\n");
    }
}


//Ritorna il massimo di un heap
int getMax(parco_auto * el) {
    if (el->car_size > 0) {
        return el->cars[0]; // Il massimo elemento si trova sempre nella radice dell'heap
    } else {
        return INT_MIN;
    }
}


void reconstructPath(int predecessors[], int startNode, int endNode) {
    int pathLength = 0;
    int* path = (int*)malloc(numero_stazioni * sizeof(int)); // Allocazione iniziale

    int currentNode = endNode;
    while (currentNode != startNode) {
        path[pathLength] = currentNode;
        pathLength++;
        currentNode = predecessors[currentNode];
    }

    path[pathLength] = startNode;
    pathLength++;

    for (int i = pathLength - 1; i > 0; i--) {
        printf("%d ", stazioni[path[i]].integer);
    }
    printf("%d\n", stazioni[path[0]].integer);

    free(path);
}



//funzione chiamata per pianificare il percorso migliore
void pianifica_percorso(int da, int a){

    int costo[numero_stazioni], precedente[numero_stazioni];
    //Inizializzo array dei costi a numero molto grande e quello dei precedenti a -1
    for (int i = 0; i < numero_stazioni; i++) {
        costo[i] = INT_MAX;
        precedente [i] = -1;
    }

    int start = ricerca_stazione(da); //indice nell'array della stazione di partenza
    int end = ricerca_stazione(a); // indice nell'array della stazione di arrivo
    if(start == -1 || end ==-1){
        printf("nessun percorso\n");
    }

    int m;
    costo[start] = 0;
    //gestisco prima il caso in cui start < end
    if(start < end) {
        while (stazioni[start].integer != stazioni[end].integer) {
            m = 1;
            while (start + m < numero_stazioni &&
                   stazioni[start].integer + getMax(stazioni[start].max_heap) >= stazioni[start + m].integer) {
                if (costo[start] < costo[start + m] && costo[start + m] > costo[start]+1) {
                    costo[start + m] = costo[start]+1;
                    precedente[start + m] = start;
                }
                m++;
            }

            start++;
        }
        if(precedente[end] == -1){
            printf("nessun percorso\n");
        }else{
            reconstructPath(precedente, ricerca_stazione(da), end);
        }

    }else { // Gestisco il caso in cui start > end
        while (start > end) {
            m = 1;
            while (start - m >= end && stazioni[start].integer - getMax(stazioni[start].max_heap) <= stazioni[start - m].integer) {
                if (costo[start] < costo[start - m]) {
                    costo[start - m] = costo[start] + 1;
                    precedente[start - m] = start;
                }
                m++;
            }
            start--;
        }
        if(precedente[end] == -1){
            printf("nessun percorso\n");
        }else{
            reconstructPath(precedente, ricerca_stazione(da), end);

        }
    }

}*/

// Funzione per la stampa in ordine crescente dell'albero
void printTreeInOrder(TreeNode* node) {
    if (node != NULL) {
        printTreeInOrder(node->left);
        printf("Stazione: %d\n", node->km);
        printf("Autonomie: ");
        for (int i = 0; i < node->max_heap->car_size; i++) {
            printf("%d ", node->max_heap->cars[i]);
        }
        printf("\n\n");
        printTreeInOrder(node->right);
    }
}


int main() {
    int k, km, na, da, a, autonomia_auto;
    char *comando = calloc(MAX, sizeof(char));
    do {
        k = scanf(" %s ", comando);
        if(k == -1){
            break;
        }
        if (strcmp(comando, ADD_STATION) == 0) {
            if(scanf("%d %d", &km, &na)!= 2){
                break;
            }
            int autonomia[512]; // Allocazione dinamica di autonomia
            for (int i = 0; i < na; i++) {
                if(scanf("%d", &autonomia[i])!=1){
                    break;
                }
            }
            aggiungi_stazione(km, autonomia, na);
            printTreeInOrder(stazioni);
        } else if (strcmp(comando, DELETE_STATION) == 0) {
            if(scanf("%d", &km)!=1){
                break;
            }
            demolisci_stazione(km);
            printTreeInOrder(stazioni);
        } else if (strcmp(comando, ADD_CAR) == 0) {
            if(scanf("%d %d", &km, &autonomia_auto)!=2){
                break;
            }
            printf("Aggiungo un auto\n");
            //aggiungi_auto(km, autonomia_auto);
        } else if (strcmp(comando, DELETE_CAR) == 0) {
            if(scanf("%d %d", &km, &autonomia_auto)!=2){
                break;
            }
            printf("Rottamo auto\n");
            //rottama_auto(km, autonomia_auto);
        } else if (strcmp(comando, PERCORSO) == 0) {
            if(scanf("%d %d", &da, &a)!=2){
                break;
            }
            //pianifica_percorso(da, a);
            printf("Pianifico percorso\n");
        }
    } while (k > 0);
    return 0;
}
