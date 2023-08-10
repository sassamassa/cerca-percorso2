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

// Funzione per aggiungere un'auto all'albero
void aggiungi_auto(TreeNode* root, int km, int autonomia_auto) {
    TreeNode* stationNode = searchStation(root, km);

    if (stationNode != NULL) {
        insertHeap(stationNode->max_heap, autonomia_auto);
        printf("aggiunta\n");
    } else {
        printf("non aggiunta\n");
    }
}

// Funzione per rimuovere un'auto dall'albero
void rottama_auto(TreeNode* root, int km, int autonomia_auto) {
    TreeNode* stationNode = searchStation(root, km);

    if (stationNode != NULL) {
        int index;
        for (index = 0; index < stationNode->max_heap->car_size; index++) {
            if (stationNode->max_heap->cars[index] == autonomia_auto) {
                break;
            }
        }

        if (index < stationNode->max_heap->car_size) {
            stationNode->max_heap->cars[index] = stationNode->max_heap->cars[stationNode->max_heap->car_size - 1];
            stationNode->max_heap->car_size--;

            heapifyDown(stationNode->max_heap, index);

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


void inorderTraversal(TreeNode* root, TreeNode** array, int* index) {
    if (root == NULL) {
        return;
    }

    inorderTraversal(root->left, array, index);
    array[*index] = root;
    (*index)++;
    inorderTraversal(root->right, array, index);
}

TreeNode** treeToArray(TreeNode* root, int* size) {
    int index = 0;
    // Count the number of nodes in the tree
    void countNodes(TreeNode* node) {
        if (node == NULL) {
            return;
        }
        (*size)++;
        countNodes(node->left);
        countNodes(node->right);
    }
    countNodes(root);

    // Create an array to store the nodes
    TreeNode** array = (TreeNode**)malloc(*size * sizeof(TreeNode*));
    index = 0;

    // Traverse the tree and populate the array
    inorderTraversal(root, array, &index);

    return array;
}



//ritorna indice del target nel nodeArray
int binarySearchNodeArray(TreeNode** nodeArray, int size, int target) {
    int low = 0;
    int high = size - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        if (nodeArray[mid]->km == target) {
            return mid; // Il nodo è stato trovato, restituisci l'indice
        } else if (nodeArray[mid]->km < target) {
            low = mid + 1; // Cerca nella metà destra
        } else {
            high = mid - 1; // Cerca nella metà sinistra
        }
    }

    return -1; // Il nodo non è stato trovato
}

void reconstructPath(int predecessors[], int startNode, int currentNode, TreeNode** nodeArray) {
    if (currentNode == startNode) {
        printf("%d", nodeArray[currentNode]->km);
        return;
    }

    reconstructPath(predecessors, startNode, predecessors[currentNode], nodeArray);
    printf(" %d", nodeArray[currentNode]->km);
}



void pianifica_percorso(TreeNode* root, int da, int a) {
    TreeNode** nodeArray = treeToArray(root, &numero_stazioni);
    int costo[numero_stazioni];
    int precedente[numero_stazioni];
    // Inizializza array dei costi a numero molto grande e quello dei precedenti a NULL
    for (int i = 0; i < numero_stazioni; i++) {
        costo[i] = INT_MAX;
        precedente[i] = -1;
    }

    int startNode = binarySearchNodeArray(nodeArray, numero_stazioni, da); //indice in nodearray del nodo di partenza
    int endNode = binarySearchNodeArray(nodeArray, numero_stazioni, a); //indice in nodeArray del nodo di arrivo
    int m;
    costo[startNode] = 0;
    // Gestisci il caso in cui startNode->key < endNode->key
    if (startNode < endNode) {

        while (nodeArray[startNode]->km < nodeArray[endNode]->km) {
            m = 1;
            while (startNode + m < numero_stazioni &&
                   nodeArray[startNode]->km + getMax(nodeArray[startNode]->max_heap) >= nodeArray[startNode +m ]->km) {
                if (costo[startNode] < costo[startNode + m] && costo[startNode + m] > costo[startNode] + 1) {
                    costo[startNode + m] = costo[startNode] + 1;
                    precedente[startNode + m] = startNode;
                }
                m++;
            }
            startNode++;
        }
        if (precedente[endNode] == -1) {
            printf("nessun percorso\n");
        } else {
            reconstructPath(precedente, binarySearchNodeArray(nodeArray, numero_stazioni, da), endNode, nodeArray);
            printf("\n");

        }

    } else { // Gestisci il caso in cui startNode->key > endNode->key
        while (startNode > endNode) {
            m = 1;
            while (startNode - m >= endNode &&
                   nodeArray[startNode]->km - getMax(nodeArray[startNode]->max_heap) <= nodeArray[startNode - m]->km) {
                if (costo[startNode] < costo[startNode - m]) {
                    costo[startNode - m] = costo[startNode] + 1;
                    precedente[startNode - m] = startNode;
                }
                m++;
            }
            startNode--;
        }
        if (precedente[endNode] == -1) {
            printf("nessun percorso\n");
        } else {
            reconstructPath(precedente, binarySearchNodeArray(nodeArray, numero_stazioni, da), endNode, nodeArray);
            printf("\n" );
        }
    }
}


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
        } else if (strcmp(comando, DELETE_STATION) == 0) {
            if(scanf("%d", &km)!=1){
                break;
            }
            demolisci_stazione(km);
        } else if (strcmp(comando, ADD_CAR) == 0) {
            if(scanf("%d %d", &km, &autonomia_auto)!=2){
                break;
            }
            aggiungi_auto(stazioni,km, autonomia_auto);
        } else if (strcmp(comando, DELETE_CAR) == 0) {
            if(scanf("%d %d", &km, &autonomia_auto)!=2){
                break;
            }
            rottama_auto(stazioni, km, autonomia_auto);
        } else if (strcmp(comando, PERCORSO) == 0) {
            if(scanf("%d %d", &da, &a)!=2){
                break;
            }
            pianifica_percorso(stazioni, da, a);
        }
    } while (k > 0);
    return 0;
}
