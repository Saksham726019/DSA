#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Added this header for ssize_t
#include <unistd.h>

#define BUFSIZE 256

typedef struct Node
{
    char* word;
    struct Node* next;
}Node;

typedef struct openHashTable
{
    int size;
    Node** buckets;
} openHashTable;


// Hash function to compute hash value for a given word on the ASCII value of its characters.
unsigned long hashFunction(char *word, int size) {
    unsigned long total = 0;                        // Initialize the total to 0
    for (int i = 0; i < strlen(word); i++) {
        total = total + word[i];                    // Add the ASCII value of the character to total
        total = total * 101;                        // Multiply the total by 101 (a prime number)
    }
    return total % size;                            // Return the hash value constrained by the size
}

// Function to print Hash Table for debugging purpose.
void printHashTable(openHashTable* hashTable) 
{
    for (int i = 0; i < hashTable->size; i++) 
    {
        Node* current = hashTable->buckets[i];

        if (current != NULL)
        {
            printf("Bucket %d: ", i);
            while (current != NULL) 
            {
                printf("%s -> ", current->word);
                current = current->next;
            }
            printf("NULL\n");
        } else 
        {
            printf("Bucket %d: NULL\n", i);
        }
    }
}


// Function to insert the dictionary words to hash table
void insertToHashTable(openHashTable* hashTable, char* wrd)
{
    // Let's get the hash value of the word first.
    unsigned long hashValue = hashFunction(wrd, hashTable->size);

    Node* head = hashTable->buckets[hashValue];

    if (head == NULL)
    {
        head = malloc(sizeof(Node));
        head->word = malloc(strlen(wrd) + 1);
        strcpy(head->word, wrd);
        head->next = NULL;
        hashTable->buckets[hashValue] = head;
    } else
    {
        Node* current = head;
        Node* previous = NULL;
        bool found = false;

        while (current != NULL)
        {
            if (strcmp(current->word, wrd) == 0)
            {
                found = true;
                break;
            }
            previous = current;
            current = current->next;   
        }
        
        if (found == false)
        {
            Node* newNode = malloc(sizeof(Node));
            newNode -> word = malloc(strlen(wrd) + 1);
            strcpy(newNode->word, wrd);
            newNode -> next = NULL;

            previous -> next = newNode;
        }
        
    }
}


// Function to check for misspelled words
bool isMisspelled(openHashTable* hashTable, char* word)
{
    // Let's get the hash value of the input word.
    unsigned long hashValue = hashFunction(word, hashTable->size);

    Node* current = hashTable->buckets[hashValue];

    while (current != NULL)
    {
        if (strcmp(current->word, word) == 0)
        {
            return false;
        }
        current = current -> next;
    }
    
    return true;
}


void swap(char *a, char *b)
{
    char temp = *a;
    *a = *b;
    *b = temp;
}

void shiftRight(char* word)
{
    int len = strlen(word);
    for (int i = strlen(word); i > 0; i--)
    {
        word[i] = word[i-1];
    }
    word[len + 1] = '\0';
}

void shiftLeft(char* word)
{
    int len = strlen(word);
    for (int i = 1; i < strlen(word); i++)
    {
        word[i-1] = word[i];
    }
    word[len - 1] = '\0';
}

void printSuggestions(openHashTable* hashTable, char* originalWord)
{
    // Initialize an array to store suggestions
    int capacity = 3;
    int size = 0;
    char** suggestions = malloc(sizeof(char*) * capacity);
    
    // First, we will copy the word into a separate variable.
    char* word = malloc(sizeof(char) * (strlen(originalWord) + 5));
    strcpy(word, originalWord);

    //////////////////////////////////
    // Step 1: Inverted adjacent pairs
    //////////////////////////////////

    for (int i = 0; i < strlen(originalWord) - 1; i++)
    {
        swap(&word[i], &word[i+1]);   // Swap the adjacent pair

        if (!isMisspelled(hashTable, word))
        {
            // Double the capacity if the array is full
            if (size >= capacity)
            {
                capacity *= 2;
                suggestions = realloc(suggestions, sizeof(char*) * capacity);
            }

            suggestions[size] = malloc(strlen(word) + 1);
            strcpy(suggestions[size], word);
            size++;
        }

        swap(&word[i], &word[i+1]);     // swap the same pair again to change the word back to how it was originally        
    }

    //////////////////////////////////////////////////////
    // Step 2: Missing letters at the beginning or the end
    //////////////////////////////////////////////////////

    // Let's start with the beginning by shifting each character to thr right and then inserting from a to z in the beginning.
    shiftRight(word);

    // Let's add character from 'a' to 'z' in the front.
    for (char c = 'a'; c <= 'z'; c++)
    {
        word[0] = c;   // Put the character in the front

        if (!isMisspelled(hashTable, word))
        {
            if (size >= capacity)
            {
                capacity *= 2;
                suggestions = realloc(suggestions, sizeof(char*) * capacity);
            }

            suggestions[size] = malloc(strlen(word) + 1);
            strcpy(suggestions[size], word);
            size++;    
        }
    }
    strcpy(word, originalWord);    // Get the original word back

    // Now, let's add characters 'a' to 'z' at the end.
    for (char c = 'a'; c <= 'z'; c++)
    {
        word[strlen(word)] = c;

        if (!isMisspelled(hashTable, word))
        {
            if (size >= capacity)
            {
                capacity *= 2;
                suggestions = realloc(suggestions, sizeof(char*) * capacity);
            }
            
            suggestions[size] = malloc(strlen(word) + 1);
            strcpy(suggestions[size], word);
            size++;
        }
        strcpy(word, originalWord);    // Get the original word back
    }

    /////////////////////////////////////////////////////
    // Step 3: Extra letters at the beginning or the end
    ////////////////////////////////////////////////////

    // Let's remove the first character of the string by shifting left.
    shiftLeft(word);

    if (!isMisspelled(hashTable, word))
    {
        if (size >= capacity)
        {
            capacity *= 2;
            suggestions = realloc(suggestions, sizeof(char*) * capacity);
        }

        suggestions[size] = malloc(strlen(word) + 1);
        strcpy(suggestions[size], word);
        size++;        
    }
    strcpy(word, originalWord);     // Get the original word back

    // Let's remove the last character of the string by declaring last index as '\0'
    word[strlen(word) - 1] = '\0';

    if (!isMisspelled(hashTable, word))
    {
        if (size >= capacity)
        {
            capacity *= 2;
            suggestions = realloc(suggestions, sizeof(char*) * capacity);
        }

        suggestions[size] = malloc(strlen(word) + 1);
        strcpy(suggestions[size], word);
        size++;        
    }
    strcpy(word, originalWord);     // Get the original word back

    //////////////////////////////////
    // Finally, print the suggestions
    //////////////////////////////////

    printf("Suggestions: ");
    for (int i = 0; i < size; i++)
    {
        printf("%s ", suggestions[i]);
    }

    ///////////////////////////////////////////////////////
    // Time to free the sugesstions array and originalWord.
    //////////////////////////////////////////////////////
    for (int i = 0; i < size; i++)
    {
        free(suggestions[i]);
    }
    free(suggestions);
    free(word);
}

// Function to free the hash table
void freeHashTable(openHashTable* hashTable) {
    for (int i = 0; i < hashTable->size; i++) {
        Node* current = hashTable->buckets[i];

        while (current != NULL) {
            Node* temp = current;
            current = current->next;
            free(temp->word);
            free(temp);
        }
    }
    free(hashTable->buckets);
    free(hashTable);
}


int main(int argc, char **argv)
{
	char *dictionaryFilePath = argv[1]; //this keeps the path to the dictionary file file
	char *inputFilePath = argv[2]; //this keeps the path to the input text file
	char *check = argv[3]; // this keeps the flag to whether we should insert mistyped words into dictionary or ignore
	int numOfWords=0; //this variable will tell us how much memory to allocate

	int insertToDictionary;
	if(strcmp(check,"add")==0)
		insertToDictionary = 1;
	else
		insertToDictionary = 0;
    
	////////////////////////////////////////////////////////////////////
	//read dictionary file
    FILE *fp = fopen(dictionaryFilePath, "r");
    char *line = NULL; //variable to be used for line counting
    size_t lineBuffSize = 0; //variable to be used for line counting
    ssize_t lineSize; //variable to be used for line counting

    //check if the file is accessible, just to make sure...
    if(fp == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        exit(1);
    }

    //First, let's count number of words in the dictionary.
    //This will help us know how much memory to allocate for our hash table
    while((lineSize = getline(&line,&lineBuffSize,fp)) !=-1)
        numOfWords++;
    
    //HINT: You can initialize your hash table here, since you know the size of the dictionary
    openHashTable* newOpenHashTable = malloc(sizeof(openHashTable));
    newOpenHashTable->buckets = malloc(sizeof(Node*) * numOfWords);
    newOpenHashTable->size = numOfWords;

    for (int i = 0; i < newOpenHashTable -> size; i++)
    {
        newOpenHashTable->buckets[i] = NULL;
    }
    
    //rewind file pointer to the beginning of the file, to be able to read it line by line.
    fseek(fp, 0, SEEK_SET);

    char wrd[BUFSIZE];
    for (int i = 0; i < numOfWords; i++)
    {
        fscanf(fp, "%s \n", wrd);
        
        //HINT: here is a good place to insert the words into your hash table
        insertToHashTable(newOpenHashTable, wrd);
    }
    fclose(fp);

    //printf("\nPrinting out the Hash Table:\n");
    //printHashTable(newOpenHashTable);
    
	////////////////////////////////////////////////////////////////////
	//read the input text file word by word
    fp = fopen(inputFilePath, "r");
	
	//check if the file is accessible, just to make sure...
	if(fp == NULL)
	{
		fprintf(stderr, "Error opening file\n");
		return -1;
	}

    //HINT: You can use a flag to indicate if there is a misspleed word or not, which is initially set to 1
	int noTypo=1;
    
	//read a line from the input file
	while((lineSize = getline(&line,&lineBuffSize,fp)) !=-1)
	{
		char *word;
        //These are the delimiters you are expected to check for. Nothing else is needed here.
		const char delimiter[]= " ,.:;!\n";

		//split the buffer by delimiters to read a single word
		word = strtok(line,delimiter); 
		
		//read the line word by word
		while(word!=NULL)
		{
            // You can print the words of the inpit file for Debug purposes, just to make sure you are loading the input text as intended
			//printf("%s\n",word);

            // HINT: Since this nested while loop will keep reading the input text word by word, here is a good place to check for misspelled words
            if (isMisspelled(newOpenHashTable, word))
            {
                noTypo = 0;
                printf("Misspelled word: %s\n",word);

                if (insertToDictionary == 1)
                {
                    insertToHashTable(newOpenHashTable, word);
                }
                
                printSuggestions(newOpenHashTable, word);
                printf("\n");
            }
            
			word = strtok(NULL,delimiter); 
		}
	}
	fclose(fp);
    
    //HINT: If the flag noTypo is not altered (which you should do in the loop above if there exists a word not in the dictionary), then you should print "No typo!"
    if(noTypo==1)
        printf("No typo!\n");
    

    // DON'T FORGET to free the memory that you allocated
    freeHashTable(newOpenHashTable);
    
	return 0;
}
