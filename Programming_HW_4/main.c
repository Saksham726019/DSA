#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// a struct to keep frequency and binary code representation of a character
typedef struct CodeTable{
  unsigned int frequency;
  char *binary_code;
} CodeTable;

typedef struct Node
{
  unsigned frequency;
  unsigned char character;
  struct Node* left;
  struct Node* right;
} Node;

typedef struct Heap
{
  int capacity;
  int size;
  Node** array;
} Heap;

void swap(Node** a, Node** b) 
{
  Node* temp = *a;
  *a = *b;
  *b = temp;
}

Heap* initializeHeap(int capacity)
{
  Heap* heap = malloc(sizeof(Heap));
  heap -> capacity = capacity;
  heap -> size = 0;
  heap -> array = malloc(sizeof(Node*) * capacity);

  return heap;
}

// This function returns the paren't index of the current node/index
int parent(Heap* heap, int index)
{
  int parentIndex = (int)((index - 1)/2);

  return parentIndex;
}

// This function returns the minimum child's index of the current index/node
int minChild(Heap* heap, int index)
{
  int leftChildIndex = ((index + 1) * 2) - 1;
  int rightChildIndex = (index + 1) * 2;

  if (heap -> size-1 >= rightChildIndex)
  {
    if (heap -> array[rightChildIndex]->frequency >= heap -> array[leftChildIndex]->frequency)
    {
      return leftChildIndex;

    } else
    {
      return rightChildIndex;
    }

  } else if (heap -> size - 1 == leftChildIndex)
  {
    return leftChildIndex;

  } else
  {
    return -1;
  }
}

void upHeap(Heap* heap, int index)
{
  if (index != 0)
  {
    int parentIndex = parent(heap, index);    // Get the parent's index of this current node/index
    int parentFrequency = heap -> array[parentIndex]->frequency;

    if (parentFrequency > heap -> array[index]->frequency)    // If parent's frequency is bigger than its child's, swap
    {
      swap(&heap->array[parentIndex], &heap->array[index]);
      upHeap(heap, parentIndex);    // Recursively keep on fixing the heap property
    }  
  }
}

void downHeap(Heap* heap, int index)
{
  if (!(heap -> size-1 < ((index + 1)*2) - 1))
  {
    int minChildIndex = minChild(heap, index);    // Get the minimum child index of this node/index

    if (heap -> array[index]->frequency > heap -> array[minChildIndex]->frequency)    // If parent is greater than child, swap.
    {
      swap(&heap -> array[index], &heap -> array[minChildIndex]);
      downHeap(heap, minChildIndex);    // Recursively downheap until the min heap property is fixed.
    }
  } 
}

void insertToHeap(Heap* heap, Node* node)
{
  heap -> array[heap -> size] = node;   // Append at the end of heap
  upHeap(heap, heap -> size);           // Fix the heap property
  heap -> size++;
}

Node* extractMin(Heap* heap)
{
  Node* min = heap -> array[0];   // Root is the smallest node as it's the min heap.

  swap(&heap -> array[0], &heap -> array[(heap -> size-1)]);
  heap -> size--;
  downHeap(heap, 0);    // Fix the heap property

  return min;
}

// Print Huffman tree characters in pre-order for debugging. Remove later
void printHuffmanTree(Node* root) {
  if (root == NULL) 
  {
    return;
  }

  if (root->character != '\0') {
    printf("%c ", root->character);
  } else {
    printf("NULL ");
  }

  // Recursively traverse the left and right subtrees
  printHuffmanTree(root->left);
  printHuffmanTree(root->right);
}


// This function builds a huffman Tree. We will add labels only after the tree is built. At last, the heap will only have one node, which is a root of the tree.
void buildHuffmanTree(Heap* heap)
{
  while (heap->size > 1)
  {
    // Extract two minimum nodes from the min heap.
    Node* min_1 = extractMin(heap);
    Node* min_2 = extractMin(heap);

    // We will make a new node by combining the two min nodes' frequencies
    Node* new_node = malloc(sizeof(Node));
    new_node->character = '\0';
    new_node->frequency = (min_1->frequency) + (min_2->frequency);
    new_node->left = min_1;
    new_node->right = min_2;

    insertToHeap(heap, new_node);   // Insert to the heap again.
  }

  printHuffmanTree(heap->array[0]);
  printf("\n");
}

// This function will label the edges of huffman tree. Left edges as 0 and right edges as 1. Then, we will store the binary code in the codeTable once we hit the leaf node.
void labelHuffmanEdges(Node* node, CodeTable* codeTable, char* binaryArray, int depth)
{
  if (node != NULL)
  {
    if (node->left == NULL && node->right == NULL)    // We hit the leaf node, which has a character.
    {
      codeTable[node->character].binary_code = malloc(sizeof(char) * (depth + 1));
      strncpy(codeTable[node->character].binary_code, binaryArray, depth);    // Store the binary code for that character in the codeTable
      codeTable[node->character].binary_code[depth] = '\0';

      // Print statement for debugging. Remove later
      printf("%c\t%s\t%u\n", node->character, codeTable[node->character].binary_code, node->frequency);

      return;
    }

    // Traverse left and add '0' along the way.
    binaryArray[depth] = '0';
    labelHuffmanEdges(node->left, codeTable, binaryArray, depth + 1);

    // Traverse right and add '1' along the way.
    binaryArray[depth] = '1';
    labelHuffmanEdges(node->right, codeTable, binaryArray, depth + 1);
  }
}

// Partition function.
int partition(Node** arr, int low, int high) {
  int mid = low + (high - low)/2;
  Node* pivot = arr[mid];  // Taking middle element as the pivot.
  int i = low - 1;

  swap(&arr[mid], &arr[high]);    // Swapping the pivot with the last element.

  for (int j = low; j < high; j++) {
    if (arr[j]->frequency < pivot->frequency || (arr[j]->frequency == pivot->frequency && arr[j]->character < pivot->character)) 
    {
      i++;
      swap(&arr[i], &arr[j]);
    }
  }

  swap(&arr[i + 1], &arr[high]);  // Putting the pivot in the middle.
  return i + 1;   // Returning pivot's index
}

// Quicksort function.
void quickSort(Node** arr, int low, int high) {
  if (low < high) {
    int pivotIndex = partition(arr, low, high);
    quickSort(arr, low, pivotIndex - 1);
    quickSort(arr, pivotIndex + 1, high);
  }
}

// Calculate compressed size.
int calculateCompressedSize(CodeTable* codeTable, Node** nodeArray, int nodeCount)
{
  int compressed_size = 0;

  for (int i = 0; i < nodeCount; i++)
  {
    compressed_size += strlen(codeTable[nodeArray[i]->character].binary_code) * (nodeArray[i]->frequency);
  }
  
  return compressed_size;
}

// This function will write the encoded version of the input text file into text file.
void writeEncodedToFile(char* inputTextFilePath, char* encodedTextFilePath, CodeTable* codeTable)
{
  FILE* inputFile = fopen(inputTextFilePath, "r");

  if (inputFile == NULL)
  {
    printf("Could not open file to read: %s\n",inputTextFilePath);
    return;
  }

  FILE* encodeFile = fopen(encodedTextFilePath, "w");

  if (encodeFile == NULL)
  {
    printf("Could not open file to read: %s\n",encodedTextFilePath);
    return;
  }

  char c;
  while ((c = fgetc(inputFile)) != EOF && c != '\n')
  {
    fprintf(encodeFile, "%s", codeTable[c].binary_code);
  }

  fclose(inputFile);
  fclose(encodeFile);
}

// Free all the dynamically allocated memory.
void freeMemory(Heap* heap, CodeTable* codeTable, Node** nodeArray, char* binaryArray)
{
  for (int i = 0; i < heap -> size; i++)
  {
    free(heap -> array[i]);
  }

  free(heap -> array);
  free(heap);

  for (int i = 0; i < 256; i++)
  {
    if (codeTable[i].binary_code != NULL)
    {
      free(codeTable[i].binary_code);
    }
  }

  free(codeTable);
  free(nodeArray);
  free(binaryArray);
}


int main(int argc, char **argv)
{
  char* mode = argv[1];
  char* inputTextFilePath;
  char* codeTableFilePath;
  char* encodedTextFilePath;
  char* decodedTextFilePath;

  //argv[1] will be "encode" or "decode" signifying the mode of the program
  if (strcmp(mode, "encode") == 0)
  {
    /*----------------ENCODER-----------------------*/

    inputTextFilePath = argv[2];
    codeTableFilePath = argv[3];
    encodedTextFilePath = argv[4];

    // Read the input text file
    FILE *inputFile = fopen(inputTextFilePath, "r");

    if (inputFile == NULL)
    {
      printf("Could not open file to read: %s\n",inputTextFilePath);
      return -1;
    }

    // Initialize the code table, which will store the frequency of each character, and eventually the binary code. We are allocating a space of 256 in the table.
    // The character's ASCII code will serve as hashing the index for that character.
    CodeTable *codeTable = malloc(sizeof(CodeTable)*256);

    //set counters to zero initially
    for(int i = 0; i < 256; i++)
    {
      codeTable[i].frequency = 0;
    }

    int totalNumOfCharacters=0;
    char c;

    // read the file character by character
    while ((c = fgetc(inputFile)) != EOF && c!='\n')
    {
      //increase the frequency of the character by one
      codeTable[c].frequency++;
      //increase the total character count
      totalNumOfCharacters++;
    }
    fclose(inputFile);

    // We will create a NodeArray that will store all the characters we have in the CodeTable with frequency > 0 and then generate a min heap (priority queue)
    // nodeArray is basically codeTable, but only contains characters with frequency > 0. That way, we don't have to deal with characters with frequcny < 0 while sorting.
    Node** nodeArray = malloc(sizeof(Node*) * totalNumOfCharacters);
    int nodeCount = 0;

    for (int i = 0; i < 256; i++)
    {
      if (codeTable[i].frequency > 0)
      {
        Node* newNode = malloc(sizeof(Node));
        newNode -> character = i;
        newNode -> frequency = codeTable[i].frequency;
        newNode -> left = NULL;
        newNode -> right = NULL;

        nodeArray[nodeCount] = newNode;
        nodeCount++;
      }
    }

    // We will build the min heap.
    Heap* heap = initializeHeap(nodeCount);

    // We will insert to heap.
    for (int i = 0; i < nodeCount; i++)
    {
      insertToHeap(heap, nodeArray[i]);
    }

    // We will now build Huffman Tree
    buildHuffmanTree(heap);

    // We will label the edges of huffman tree. Left edges as 0 and right edges as 1. Then, we will store the binary code in the codeTable once we hit the leaf node
    Node* root = heap->array[0];
    char* binaryArray = malloc(sizeof(char)*256);
    labelHuffmanEdges(root, codeTable, binaryArray, 0);

    // Now that the codeTable has binary_codes for each character with frequency > 0, we will write to the codeTableFile
    FILE *codeTableFile = fopen(codeTableFilePath, "w");
    if (codeTableFile == NULL)
    {
      printf("Could not open file to write: %s\n",codeTableFilePath);
      return -1;
    }

    // Sort the nodeArray in descending order. 
    quickSort(nodeArray, 0, nodeCount-1);

    // Write the nodeArray into codeTable file. 
    for(int i = nodeCount - 1; i >= 0; i--)
    {
      fprintf(codeTableFile, "%c\t%s\t%u\n", nodeArray[i]->character, codeTable[nodeArray[i]->character].binary_code, nodeArray[i]->frequency);
    }
    fclose(codeTableFile);

    int compressed_size = calculateCompressedSize(codeTable, nodeArray, nodeCount);

    // To print the statistics about the compression, use print statements as follows
    printf("Original: %d bits\n", totalNumOfCharacters*8);
    printf("Compressed: %d bits\n", compressed_size); //assuming that you store the number of bits (i.e., 0/1s) of encoded text in variable "compressed_size"
    printf("Compression Ratio: %.2f%%\n", (float)compressed_size/((float)totalNumOfCharacters*8)*100); //This line will print the compression ration in percentages, up to 2 decimals.
    
    // Write encoded version of the text in 0/1 form into text file.
    writeEncodedToFile(inputTextFilePath, encodedTextFilePath, codeTable);

    // Time to free all the memory
    freeMemory(heap, codeTable, nodeArray, binaryArray);

  } else if (strcmp(mode, "decode") == 0)
  {
    /*----------------DECODER-----------------------*/
    
    codeTableFilePath = argv[2];
    encodedTextFilePath = argv[3];
    decodedTextFilePath = argv[4];
  
    /*----------------------------------------------*/
    // When decoding, you will need to:
    // 1) read code table: you can use fscanf() function, since the code table file is well structured. Alternatively, you can also use the read statements from above as was used for reading input text file.
    // 2) read encoded text, which is a single line consisting of 0/1 characters: This file is better be read character by character, for which you can use a code similar to getc() code above
    // 3) write the decoded text into file: for that, you can write it into the file using a code similar to fprintf() usages exemplified above.
  }

  return 0;
}
