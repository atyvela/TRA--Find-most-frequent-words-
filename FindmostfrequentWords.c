#include <stdio.h>
#include <ctype.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

# define MAX_CHARS 26 
# define MAX_WORD_SIZE 30 

 #include<stdbool.h>
 #include<stdlib.h>


typedef struct TrieNode TrieNode;
struct TrieNode
{
    bool isEnd; // Onko sana päättynyt
    
    unsigned lkm;  // Sanan esiintymiskertojen lukumäärämäärä
    
    int indexMinHeap; // sanan indeksi listasssa
    
    TrieNode* child[MAX_CHARS]; // kirjaimet a-zn
};

 

struct min_heap_node
{
    TrieNode* root; // hakupuun juuri
    
    unsigned lkm; //  lukumäärä
    
    char* sana; // etsittävä sana
};

typedef struct min_heap_node min_heap_node;
typedef struct MinHeap MinHeap;

struct MinHeap
{
    unsigned koko; // listan(heap) koko
    
    int sanalkm; // montako sanaa listassa on
    
    min_heap_node* array; //  lista "nodeista" eli sanoista
};
 

TrieNode* newTrieNode()
{
 
    TrieNode* trieNode = (TrieNode*)malloc(sizeof(TrieNode));
 
    
    trieNode->isEnd = 0;
    trieNode->lkm = 0;
    trieNode->indexMinHeap = -1;
    
    int i;
    
    for(i = 0; i < MAX_CHARS; ++i)
        trieNode->child[i] = NULL;
 
    return trieNode;
}
 
// tekee listan halutulla koolla ( haluttu sanojen määrä, tässätapauksessa 100
MinHeap* create_min_heap(int koko)
{
    MinHeap* minHeap =(MinHeap*)malloc(sizeof(MinHeap*));

    minHeap->koko = koko;
    minHeap->sanalkm  = 0;
 
    minHeap->array =(min_heap_node*)malloc(sizeof(min_heap_node)*koko);
 
    return minHeap;
}
 
// Vaihtaa heapin kaksi pienintä nodea, minheapify tarvii tätä//tämä olikin harjoitus esimerkissä
void exchange(min_heap_node* a, min_heap_node* b)
{
    min_heap_node tmp = *a;
    *a = *b;
    *b = tmp;
}
 

//minheapify funktio
void min_heapify(MinHeap* minHeap, int idx)
{

 
    int left = idx +1;
    int right = idx + 2;
    int smallest = idx;
    
    if (left < minHeap->sanalkm && minHeap->array[left].lkm < minHeap->array[smallest].lkm){
        smallest = left;
    }
    if (right < minHeap->sanalkm && minHeap->array[right]. lkm < minHeap->array[smallest].lkm){
        smallest = right;
    }
    if(smallest != idx)
    {
       
        minHeap->array[smallest]. root->indexMinHeap = idx;
        minHeap->array[idx]. root->indexMinHeap = smallest;
 
        
        exchange(&minHeap->array[smallest], &minHeap->array[idx]);
 
        min_heapify(minHeap, smallest);
    }
}
 
// rakentaa heapin
void build_min_heap( MinHeap* minHeap )
{
    
    int i = 100;
    int n = minHeap->sanalkm;
 
    for(i = n - 1 / 2; i >= 1; --i){
        min_heapify(minHeap, i);
    }
}
 
//siirtää sanan heappiin
void lisaa_minHeap(MinHeap* minHeap, TrieNode** root, const char* sana)
{
    //sana on jo listassa, lisätään lukumäärää
    if( (*root)->indexMinHeap != -1)
    {
        ++( minHeap->array[(*root)->indexMinHeap].lkm);
 
        // percolate down
        min_heapify(minHeap,(*root)->indexMinHeap);
    }
 
    //sanaa ei ole listassa ja listassa on tilaa uudelle sanalle
    else if(minHeap->sanalkm < minHeap->koko)
    {
        int sanalkm = minHeap->sanalkm;
        
        minHeap->array[sanalkm].lkm = (*root)->lkm;
        minHeap->array[sanalkm].sana = (char *)malloc(sizeof(char)*strlen(sana) + 1);
        
        strcpy(minHeap->array[sanalkm].sana, sana);
 
        minHeap->array[sanalkm]. root = *root;
        
        (*root)->indexMinHeap = minHeap->sanalkm;
 
        ++(minHeap->sanalkm);
        
        build_min_heap(minHeap);
    }
 
    // Sanaa ei ole listassa, lista on täysi ja lukumäärä on suurempi kuin root noden lukumäärä(root node vähiten yleisin sana listassa
    //laitetaan uusi sana rootin tilalle
    else if ((*root)->lkm > minHeap->array[0]. lkm)
    {
 
        minHeap->array[0].root->indexMinHeap = -1;
        minHeap->array[0].root = *root;
        minHeap->array[0].root->indexMinHeap = 0;
        minHeap->array[0].lkm = (*root)->lkm;
 

        free( minHeap->array[0].sana);
        
        minHeap->array[0].sana = (char *) malloc(sizeof(char)* strlen(sana) + 1);
        
        strcpy(minHeap->array[0].sana, sana);
 
        min_heapify (minHeap, 0);
    }
}
 
//Lisää uuden sanan hmyös trie:hin
void lisaa (TrieNode** root, MinHeap* minHeap, const char* sana, const char* sana1)
{

    if (*root == NULL){
        *root = newTrieNode();
    }
    // tarkistaa jatkuuko sana vielä
    if (*sana != '\0'){
        lisaa (&((*root)->child[tolower(*sana) - 96]), minHeap, sana + 1, sana1); //tolowerin tarvi tähänkin vaikka isot ja pienet kirjaimet samaistetaan ja 
    }                                                                                 // välimerkit poistetaan jo tiedostoa lukiessa.
    else
    {
        // sana on olemassa lisää lukumäärää
        if ((*root)->isEnd ){
            ++((*root)->lkm);
        }
        else
        {
            (*root)->isEnd = 1;
            (*root)->lkm = 1;
        }
 
        
        lisaa_minHeap(minHeap, root, sana1);
    }
}
 
 

void lisaa_molemmat(const char *sana, TrieNode** root, MinHeap* minHeap) //funktio jolla kutsutaan molemmat lisäykset, piti tehdä näin että sain oikeat   
{                                                                       //muuttujat välittymään kullekkin funktiolle.
    lisaa(root, minHeap, sana, sana);
}



 

void print_words( FILE* tiedosto, int n)
{
    int i = n;
    // halutun kokoinen (n) heap, tässä tapauksessa 100
    MinHeap* minHeap = create_min_heap(n);
    
    
    TrieNode* root = NULL;
 
    // varastoi yhden sanan kerrallaan suluissa sanan  maksimi pituus
    char sana2[150];
 
    // lukee sanat tiedostosta,
    while(fscanf(tiedosto, "%[A-Za-z]", sana2) != EOF){ // poistaa välimerkit ja lukee isot ja pienet kirjaimet yhtäsuurina
        fscanf(tiedosto, "%*[^A-Za-z]"); 
        lisaa_molemmat(sana2, &root, minHeap);
    }


    while(i--) //tämä oli pakko tehdä while loopilla, muuten kuluneen ajan tulostus ei toiminut jostain syystä
    {
        printf( "%s : %d\n", minHeap->array[i].sana, minHeap->array[i].lkm ); 
    }
    
}


//pääohjelma
int main()
{
    FILE *tiedosto; //haluttu tekstitiedosto
    int n =100; // sanojen määrä, samalla heapin koko
    char tnimi[20]={0}; //tiedoston nimi
    clock_t alku, loppu; //kello
    double aika; //kello

    
    while(tiedosto == NULL){ //luetaan syötettä niin kauan että tiedosto löytyy
        printf("Syötä tekstitiedoston nimi\n");
        scanf("%s", tnimi);
        tiedosto =fopen(tnimi, "r" );
    }
    
    if (tiedosto == NULL){
        
        printf ("tiedosto puuttuu");
    }
    else{
        alku=clock();
        print_words(tiedosto, n);
        getchar();

    }
    loppu=clock();
    aika = (double)(loppu - alku)/CLOCKS_PER_SEC;
    printf("%\nAikaa kului %f", aika);
    fclose(tiedosto);
    return 0;
}