/*
*@Author: Daniel Green
*Date: 5/21/26
*Concept: Huffman Coding
*Complexity: O(n+klogk) using heaps where k=27
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct {
    int freq[27];    // a-z + ' '
} CharMap;

typedef struct Node {
    char letter;
    int freq;

    struct Node *left;
    struct Node *right;
} Node;

typedef struct {
    Node **data;
    int size;
    int capacity;
} MinHeap;

Node* createNode(char letter, int freq) {
    Node *node = malloc(sizeof(Node));
    
    node->letter = letter;
    node->freq = freq;

    node->left = NULL;
    node->right = NULL;

    return node;
}

MinHeap* initHeap(int capacity) {
    MinHeap *heap = malloc(sizeof(MinHeap));

    heap->data = malloc(sizeof(Node*) * capacity);

    heap->size = 0;
    heap->capacity = capacity;

    return heap;
}

// Counts the frequencies of each character, a-z, treating any other character as ' '
// and store each character indexed 0-27 (27 being ' ') in freq[]
void char_freq(CharMap *map, char text[]) {
    for (int i=0; text[i] != '\0'; i++) {
        if (isalpha(text[i]))
            map->freq[tolower(text[i])-'a']++;
        else
            map->freq[26]++; // every other character is treated as whitespace.
    }
}

// helper to search frequency of character
int search_freq(CharMap *map, char target) {
    return map->freq[target-'a'];
}

void swap(Node **a, Node **b) {
    Node *temp = *a;
    *a = *b;
    *b = temp;
}

void percolateUp(MinHeap *heap, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;

        if (heap->data[parent]->freq <= heap->data[index]->freq)
            break;

        swap(&heap->data[parent], &heap->data[index]);

        index = parent;
    }
}

void percolateDown(MinHeap *heap, int index) {
    while (1) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;

        if (left < heap->size &&
            heap->data[left]->freq < heap->data[smallest]->freq)
            smallest = left;

        if (right < heap->size &&
            heap->data[right]->freq < heap->data[smallest]->freq)
            smallest = right;

        if (smallest == index)
            break;

        swap(&heap->data[index], &heap->data[smallest]);

        index = smallest;
    }
}

void insert(MinHeap *heap, Node *node) {
    if (heap->size >= heap->capacity) return;

    heap->data[heap->size] = node;

    heap->size++;
    percolateUp(heap, heap->size-1);
}

Node* removeMin(MinHeap *heap) {
    if (heap->size == 0)
        return NULL;

    Node *min = heap->data[0];

    heap->size--;

    if (heap->size > 0) {
        heap->data[0] = heap->data[heap->size];

        percolateDown(heap, 0);
    }

    return min;
}

MinHeap* buildHeap(CharMap *map) {
    MinHeap *heap = initHeap(27);

    for (int i = 0; i < 27; i++) {
        if (map->freq[i] > 0) {
            char c;

            if (i == 26)
                c = ' ';
            else
                c = 'a' + i;

            Node *node = createNode(c, map->freq[i]);

            insert(heap, node);
        }
    }

    return heap;
}

Node* buildHuffmanTree(MinHeap *heap) {
    while (heap->size > 1) {

        Node *left = removeMin(heap);
        Node *right = removeMin(heap);

        Node *parent = createNode('\0', left->freq + right->freq);

        parent->left = left;
        parent->right = right;

        insert(heap, parent);
    }

    return removeMin(heap);
}

void freeHeap(MinHeap *heap) {

    free(heap->data);

    free(heap);
}

void freeTree(Node *root) {

    if (root == NULL)
        return;

    freeTree(root->left);
    freeTree(root->right);

    free(root);
}

// generates bit sequence in table[][]
void generateCodes(Node *root, char code[], int depth, char table[27][64]) {
    if (root == NULL)
        return;

    // leaf node
    if (root->left == NULL && root->right == NULL) {
        if (depth == 0) {
            code[0] = '0';
            depth = 1;
        }

        code[depth] = '\0';
        int index = root->letter == ' ' ? 26 : root->letter - 'a';
        
        strcpy(table[index], code);
        
        return;
    }

    // go left
    code[depth] = '0';
    generateCodes(root->left, code, depth + 1, table);

    // go right
    code[depth] = '1';
    generateCodes(root->right, code, depth + 1, table);
}

// WRITING TO FILE
//+========================================================================
typedef struct {
    FILE *f;
    unsigned char byte;
    int count;
} BitWriter;

// writes single bit into file
void writeBit(BitWriter *bw, int bit) {
    bw->byte <<= 1;
    bw->byte |= bit;
    bw->count++;

    if (bw->count == 8) {
        fputc(bw->byte, bw->f);
        bw->byte = 0;
        bw->count = 0;
    }
}

// flush remaining bits
void flushBits(BitWriter *bw) {
    if (bw->count > 0) {
        bw->byte <<= (8 - bw->count);
        fputc(bw->byte, bw->f);
    }
}

// converts text to bit sequence
void encode(char *text, char table[27][64], FILE *out) {
    int len = strlen(text);
    fwrite(&len, sizeof(int), 1, out);

    BitWriter bw = {out, 0, 0};

    for (int i = 0; text[i]; i++) {

        int idx;

        if (isalpha(text[i]))
            idx = tolower(text[i]) - 'a';
        else
            idx = 26;

        char *code = table[idx];

        for (int j = 0; code[j]; j++) {
            writeBit(&bw, code[j] == '1');
        }
    }

    flushBits(&bw);
}
//+========================================================================

// READING FILE
//+========================================================================
// converts bit sequence to plaintext
void decode(Node *root, FILE *in) {
    int originalLen;
    fread(&originalLen, sizeof(int), 1, in);

    // tree contains only one character
    if (root->left == NULL && root->right == NULL) {

        for (int i = 0; i < originalLen; i++)
            putchar(root->letter);

        printf("\n");
        return;
    }

    Node *cur = root;
    int byte;
    int decoded = 0;

    while ((byte = fgetc(in)) != EOF && decoded < originalLen) {

        for (int i = 7; i >= 0 && decoded < originalLen; i--) {

            int bit = (byte >> i) & 1;

            cur = bit ? cur->right : cur->left;

            if (!cur->left && !cur->right) {

                putchar(cur->letter);

                decoded++;

                cur = root;
            }
        }
    }

    printf("\n");
}
//+========================================================================

// MAIN
//+========================================================================
int main()
{
    char text[1000];
    printf("Enter text: \n");
    scanf("%999[^\n]", text);
    
    CharMap map = {0};
    char_freq(&map, text);

    MinHeap *heap = buildHeap(&map);

    Node *root = buildHuffmanTree(heap);

    char code[64];
    char table[27][64] = {0};
    
    generateCodes(root, code, 0, table);

    FILE *out = fopen("compressed.bin", "wb");
    encode(text, table, out);
    fclose(out);
    
    // shows that decoding works
    FILE *in = fopen("compressed.bin", "rb");
    printf("\nDecoded:\n");
    decode(root, in);
    fclose(in);

    freeTree(root);
    freeHeap(heap);
    return 0;
}
//+========================================================================
