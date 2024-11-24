#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct BoardState
{
    int* boardState;
    struct BoardState* parent;
} BoardState;

// Queue struct
typedef struct Queue
{
	BoardState** boardStates;
	int front;
	int tail;
	int capacity;
} Queue;

// Node struct
typedef struct Node
{
	BoardState* boardState;
	struct Node* next;
} Node;

// Hash Table struct to store unique board states.
typedef struct HashTable
{
	Node** buckets;
	int size;
} HashTable;

// Hash function to compute hash value for a given board state.
unsigned long hashFunction(BoardState *boardState, int k, int size) 
{
    unsigned long total = 0;

    for (int i = 0; i < k*k; i++) 
	{
        total = total + boardState->boardState[i];
        total = total * 101;                        // Multiply the total by 101 (a prime number)
    }
    return total % size;
}

// Function to print the state of the board.
void printBoard(int *board, int k)
{
	for (int i = 0; i < k; i++)
	{
		for (int j = 0; j < k; j++)
		{
			printf("%d ", board[i * k + j]);
		}
		printf("\n");		
	}
	printf("\n\n");
}

// Function to create boardState in struct.
BoardState* createBoardState(int* boardState, int k)
{
	BoardState* newBoardState = malloc(sizeof(BoardState));
	newBoardState->boardState = malloc(sizeof(int) * (k*k));
	memcpy(newBoardState->boardState, boardState, sizeof(int) * k*k);
	newBoardState->parent = NULL;

	return newBoardState;	
}

// Function to initialize the Queue data structure to hold the state of the board.
Queue* initializeQueue()
{
	Queue* queue = malloc(sizeof(Queue));
	queue->capacity = 10;
	queue->boardStates = malloc(sizeof(BoardState*) * queue->capacity);
	queue->front = 0;
	queue->tail = 0;

	return queue;
}

// Function to resize the queue.
void resizeQueue(Queue* queue)
{
    int newCapacity = queue->capacity * 2;
    BoardState** newBoardState = realloc(queue->boardStates, sizeof(BoardState*) * newCapacity);

    // Check if realloc was successful. Remove before submitting.
    if (newBoardState == NULL)
    {
        // If realloc fails, exit or handle the error appropriately
        printf("Malloc failed!\n");
        exit(1);  // Or handle it according to your needs
    }

    queue->boardStates = newBoardState;
    queue->capacity = newCapacity;
}

// Function to insert to queue.
void enqueue(Queue* queue, BoardState* boardState)
{
	if (queue->tail == queue->capacity)
	{
		// Print statement for debugging. Remove later
		printf("Queue is full. Resizing...\n");
		resizeQueue(queue);
	}

	queue->boardStates[queue->tail] = boardState;
	queue->tail++;
}

// Function to dequeue
BoardState* dequeue(Queue* queue)
{
	if (queue->front == queue->tail)
	{
		printf("Queue is empty!\n");
		return NULL;
	}
	
	BoardState* boardState = queue->boardStates[queue->front];
	queue->front++;

	return boardState;
}

// Function to print queue for debugging. Remove later.
void printQueue(Queue* queue, int k)
{
	int index = 0;

	while (index != queue->tail)
	{
		printf("Board: %d\n", index);
		BoardState* currentBoardState = queue->boardStates[index];
		printBoard(currentBoardState->boardState, k);

		index++;
	}
}

// Function to compare two board states in order to only insert unique board state to hash table.
bool isUniqueBoard(HashTable* hashTable, BoardState* boardState, int k) 
{
    int index = hashFunction(boardState, k, hashTable->size);

    Node* current = hashTable->buckets[index];
    
    while (current != NULL)
    {
        bool match = true;
        for (int i = 0; i < k * k; i++) 
        {
            if (current->boardState->boardState[i] != boardState->boardState[i]) 
            {
                match = false;
                break;
            }
        }

        if (match)
		{
            return false;
		}

        current = current->next;
    }
    return true;  // The board is unique
}

// Function to initialize Hash table.
HashTable* initializeHashTable()
{
	HashTable* hashTable = malloc(sizeof(HashTable));
	hashTable->size = 100;
	hashTable->buckets = malloc(sizeof(Node*) * hashTable->size);

	for (int i = 0; i < hashTable->size; i++)
	{
		hashTable->buckets[i] = NULL;
	}

	return hashTable;	
}

// Function to insert the unique board state to the hash table.
void insertToHashTable(HashTable* hashTable, BoardState* boardState, int k)
{
	int index = hashFunction(boardState, k, hashTable->size);

	Node* newNode = malloc(sizeof(Node));
	newNode->boardState = boardState;
	newNode->next = hashTable->buckets[index];

	hashTable->buckets[index] = newNode;
}

// Function to find the position of the empty tile (0).
int findEmptyTile(int* boardState, int k)
{
	for (int i = 0; i < k*k; i++)
	{
		if (boardState[i] == 0)
		{
			return i;
		}
	}

	return -1;	
}

// Function to swap two items
void swap(int* a, int* b) 
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Partition function.
int partition(int* arr, int low, int high) {
	int mid = low + (high - low)/2;
	int pivot = arr[mid];  // Taking middle element as the pivot.
	int i = low - 1;

	swap(&arr[mid], &arr[high]);    // Swapping the pivot with the last element.

	for (int j = low; j < high; j++) 
	{
		if (arr[j] != 0 && (arr[j] < pivot || (arr[j] == pivot))) 
		{
			i++;
			swap(&arr[i], &arr[j]);
		}
	}

	swap(&arr[i + 1], &arr[high]);  // Putting the pivot in the middle.
	return i + 1;   // Returning pivot's index
}

// Quicksort function.
void quickSort(int* arr, int low, int high) {
	if (low < high) 
	{
		int pivotIndex = partition(arr, low, high);
		quickSort(arr, low, pivotIndex - 1);
		quickSort(arr, pivotIndex + 1, high);
	}
}

// Function to find the goal state with the help of quicksort.
int* getGoalState(int* boardState, int k)
{
	int* goalState = malloc(sizeof(int) * (k*k));
	int index = 0;

	for (int i = 0; i < k*k; i++)
	{
		if (boardState[i] != 0)
		{
			goalState[index] = boardState[i];
			index++;
		}	
	}

	quickSort(goalState, 0, (k*k -2));
	goalState[k*k - 1] = 0;

	return goalState;
}

// Function to free the BoardState Struct.
void freeBoardStateStruct(BoardState* boardState)
{
	if (boardState != NULL)
	{
		free(boardState->boardState);
		free(boardState);
	}
}

// Function to enqueue the adjacent board states from the current position of empty tile. (Need to make it readable) (Also, need hashtable to check for uniqueness).
void getAdjacentBoardStates(BoardState* currentBoardState, int k, Queue* queue, HashTable* hashTable)
{
	int emptyTileIndex = findEmptyTile(currentBoardState->boardState, k);
	int row = emptyTileIndex / k;
	int column = emptyTileIndex % k;

	int directions[4][2] = {
		{-1, 0},
		{1, 0},
		{0, -1},
		{0, 1}
	};

	for (int i = 0; i < 4; i++) 
	{
        int newRow = row + directions[i][0];
        int newCol = column + directions[i][1];

        // Check if the move is within bounds
        if (newRow >= 0 && newRow < k && newCol >= 0 && newCol < k) 
		{
			BoardState* newBoardState = createBoardState(currentBoardState->boardState, k);
            // Swap the empty tile with the new position
            swap(&(newBoardState->boardState[emptyTileIndex]), &(newBoardState->boardState[newRow * k + newCol]));

			if (isUniqueBoard(hashTable, newBoardState, k))
			{
				newBoardState->parent = currentBoardState; // Set the parent to track the path
				enqueue(queue, newBoardState); // Enqueue the new state
				insertToHashTable(hashTable, newBoardState, k);
			} else
			{
				freeBoardStateStruct(newBoardState);
			}
        }
    }
	printf("Queue after getting its adjacent:\n");
	printQueue(queue, k);
}

// Function to check if we have reached the goal state.
bool isGoalState(BoardState* currentBoardState, int* goalState, int k)
{
	for (int i = 0; i < k*k; i++)
	{
		if (currentBoardState->boardState[i] != goalState[i])
		{
			return false;
		}
	}
	return true;	
}

// BFS function to get the shortest path. (Incomplete. First complete the hashtable)
BoardState* BFSTraversal(Queue* queue, HashTable* hashTable, int* goalState, int k)
{
	while (queue->front != queue->tail)
	{
		BoardState* currentBoardState = dequeue(queue);

		// Check if it's the goal state.
		if (isGoalState(currentBoardState, goalState, k))
		{
			printf("Goal State reached!\n");
			printBoard(currentBoardState->boardState, k);
			return currentBoardState;
		}

		if (isUniqueBoard(hashTable, currentBoardState, k))
		{
			insertToHashTable(hashTable, currentBoardState, k);
		}
		
		getAdjacentBoardStates(currentBoardState, k, queue, hashTable);
	}

	return NULL;
}

// Function to free the hash table
void freeHashTable(HashTable* hashTable) 
{
    for (int i = 0; i < hashTable->size; i++) 
    {
        Node* current = hashTable->buckets[i];
        while (current != NULL) 
        {
            Node* temp = current;
            current = current->next;
            freeBoardStateStruct(temp->boardState); // Free the BoardState as well
            free(temp); // Free the Node itself
        }
    }
    free(hashTable->buckets);
    free(hashTable);
}

void freePath(BoardState* goalState)
{
    BoardState* current = goalState;
    while (current != NULL)
    {
        BoardState* parent = current->parent;
        freeBoardStateStruct(current);
        current = parent;
    }
}

// Function to free all dynamically allocated memory
void freeMemory(Queue* queue, HashTable* hashTable, int* goalState, BoardState* solutionState)
{
    if (solutionState != NULL)
    {
        // Free the solution path first
        freePath(solutionState);
    }
    else
    {
        // If no solution was found, free all states in the queue
        for (int i = queue->front; i < queue->tail; i++)
        {
            if (queue->boardStates[i] != NULL) {
                freeBoardStateStruct(queue->boardStates[i]);
            }
        }
    }
    
    // Free queue structure
    free(queue->boardStates);
    free(queue);

    // Free hash table (nodes only, BoardStates already freed)
    freeHashTable(hashTable);

    // Free goal state
    free(goalState);
}


int main(int argc, char **argv)
{
	FILE *fp_in,*fp_out;
	
	fp_in = fopen(argv[1], "r");
	if (fp_in == NULL){
		printf("Could not open a file.\n");
		return -1;
	}
	
	fp_out = fopen(argv[2], "w");
	if (fp_out == NULL){
		printf("Could not open a file.\n");
		return -1;
	}
	
	char *line = NULL;
	size_t lineBuffSize = 0;
	ssize_t lineSize;
	int k;
	
	////////////////////////////////////////////////////
	// Read the input file and add to the initial board
	////////////////////////////////////////////////////

	getline(&line,&lineBuffSize,fp_in);		//ignore the first line in file, which is a comment
	fscanf(fp_in,"%d\n",&k);				//read size of the board
	//printf("k = %d\n", k); 				//make sure you read k properly for DEBUG purposes
	getline(&line,&lineBuffSize,fp_in);		//ignore the second line in file, which is a comment

	int initial_board[k*k];					//get kxk memory to hold the initial board
	for(int i=0;i<k*k;i++)
	{
		fscanf(fp_in,"%d ",&initial_board[i]);
	}
	fclose(fp_in);

	printf("Initial Board:\n\n");
	printBoard(initial_board, k); // Remove later

	// Get the goal state.
	int* goalState = getGoalState(initial_board, k);
	printf("Goal State:\n\n");
	printBoard(goalState, k); // Remove later

	//////////////////////////////////
	// do the rest to solve the puzzle
	//////////////////////////////////

	// Initialize Queue
	Queue* queue = initializeQueue();

	// Initialize hash table to store unique board states.
	HashTable* hashTable = initializeHashTable();
	
	BoardState* initialBoardState = createBoardState(initial_board, k);
	enqueue(queue, initialBoardState);

	// Call function for BFS.
	BoardState* solutionBoardState = BFSTraversal(queue, hashTable, goalState, k);

	/*
	//once you are done, you can use the code similar to the one below to print the output into file
	//if the puzzle is NOT solvable use something as follows
	fprintf(fp_out, "#moves\n");
	fprintf(fp_out, "no solution\n");
	
	//if it is solvable, then use something as follows:
	fprintf(fp_out, "#moves\n");
	//probably within a loop, or however you stored proper moves, print them one by one by leaving a space between moves, as below
	for(int i=0;i<numberOfMoves;i++)
		fprintf(fp_out, "%d ", move[i]);
	*/
	fclose(fp_out);

	freeMemory(queue, hashTable, goalState, solutionBoardState);
	free(line);

	return 0;

}
