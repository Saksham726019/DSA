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


/* Heap (priority queue)
 ● The root of the tree is in A[0]
 ● Parent of node at index n is at index (n − 1)/2
 ● Left Child of node at index n is at index (n + 1) * 2 − 1
 ● Right Child of node at index n is at index (n + 1) * 2
*/

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

int parent(Heap* heap, int index)
{
  int parentIndex = (int)((index - 1)/2);

  return parentIndex;
}

void upHeap(Heap* heap, int index)
{
  if (index != 0)
  {
    int parentIndex = parent(heap, index);
    int parentFrequency = heap -> array[parentIndex]->frequency;

    if (parentFrequency > heap -> array[index]->frequency)
    {
      swap(&heap->array[parentIndex], &heap->array[index]);
      upHeap(heap, parentIndex);
    }  
  }
}

void insertToHeap(Heap* heap, Node* node)
{
  heap -> array[heap -> size] = node;
  upHeap(heap, heap -> size);
  heap -> size++;
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
    inputTextFilePath = argv[2];
    codeTableFilePath = argv[3];
    encodedTextFilePath = argv[4];

  } else if (strcmp(mode, "decode") == 0)
  {
    codeTableFilePath = argv[2];
    encodedTextFilePath = argv[3];
    decodedTextFilePath = argv[4];
  }
  
  /*----------------ENCODER-----------------------*/
  /*----------------------------------------------*/
  /*To read the input text file, you might want to use a code as follows*/
  FILE *inputFile = fopen(inputTextFilePath, "r");
  if (inputFile == NULL)
  {
    printf("Could not open file to read: %s\n",inputTextFilePath);
    return -1;
  }

	// initialize the code table, which will store the frequency of each character,
	// and eventually the binary code. We are allocating a space of 256 in the
	// table, and the character's ASCII code will serve as hashing the index for
	// that character.
	CodeTable *codeTable = malloc(sizeof(CodeTable)*256);
	//set counters to zero initially
	for(int i = 0; i < 256; i++)
  {
		codeTable[i].frequency = 0;
  }

	int totalNumOfCharacters=0;
  char c;
  //read the file character by character
  while ((c = fgetc(inputFile)) != EOF && c!='\n')
  {
    //increase the frequency of the character by one
    codeTable[c].frequency++;
    //increase the total character count
    totalNumOfCharacters++;

  }
  fclose(inputFile);

  // We will create a NodeArray that will store all the characters we have in the CodeTable and then generate a heap (priority queue)
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

  // Print the heap for debugging purpose. (Delete after debugging)
  for (int i = 0; i < heap->size; i++) {
      printf("Node %d: Character = %c, Frequency = %u\n", i, heap->array[i]->character, heap->array[i]->frequency);
  }
  
    
  // now that you have collected the frequency of each character that is present 
  // in the input file, you need to generate the code table.
  
  // Hint: when initializing the heap to be used in the generation of Huffman
  // 		 tree, keep in mind that you know how many unique characters have
  //		 appeared in the input text. You can get this number by going through
  //		 your codeTable...
  
  /*----------------------------------------------*/ 
  /* to write the code table into the file, you might want to use a code as follows
  FILE *codeTableFile = fopen(codeTableFilePath, "w");
  if (codeTableFile == NULL)
  {
      printf("Could not open file to write: %s\n",codeTableFilePath);
      return;
  }

  // Write the code table into file:
  for(int i = numOfCharacters - 1; i >= 0; i--)
  {
    fprintf(codeTableFile, "%c\t%s\t%d\n", characters[i], characterCodes[i], characterFrequencies[i]); 
  }
  fclose(codeTableFile);
  
  /*----------------------------------------------
  // To print the statistics about the compression, use print statements as follows
  printf("Original: %d bits\n", uncompressed*8); //assuming that you store the number of characters in variable "uncompressed". *8 is because ASCII table uses 8 bits to represent each character
  printf("Compressed: %d bits\n", compressed_size); //assuming that you store the number of bits (i.e., 0/1s) of encoded text in variable "compressed_size"
  printf("Compression Ratio: %.2f%%\n", (float)compressed_size/((float)uncompressed*8)*100); //This line will print the compression ration in percentages, up to 2 decimals.
  
  /*----------------------------------------------*/
  // to write encoded version of the text in 0/1 form into text file, you can use a code similar to fprintf statment above that is suggested for writing code table to the file.
  
  
  /*----------------DECODER-----------------------*/
  /*----------------------------------------------*/
  // When decoding, you will need to:
  // 1) read code table: you can use fscanf() function, since the code table file is well structured. Alternatively, you can also use the read statements from above as was used for reading input text file.
  // 2) read encoded text, which is a single line consisting of 0/1 characters: This file is better be read character by character, for which you can use a code similar to getc() code above
  // 3) write the decoded text into file: for that, you can write it into the file using a code similar to fprintf() usages exemplified above.

  return 0;
}
