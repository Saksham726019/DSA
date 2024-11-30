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

// Node struct
typedef struct Node
{
	BoardState* boardState;
	struct Node* next;
} Node;

// Queue struct
typedef struct Queue
{
	Node* front;
	Node* tail;
} Queue;

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
        total = total * 31 + boardState->boardState[i];
    }
    return total % size;
}

// Function to print the state of the board. Remove later.
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
	queue->front = NULL;
	queue->tail = NULL;

	return queue;
}

// Function to insert to queue.
void enqueue(Queue* queue, BoardState* boardState)
{
    Node* newNode = malloc(sizeof(Node));
    newNode->boardState = boardState;
    newNode->next = NULL;

	if (queue->tail == NULL)
    {
        queue->front = newNode;
        queue->tail = newNode;
    } else
    {
        queue->tail->next = newNode;
        queue->tail = newNode;
    }
}

// Function to dequeue
BoardState* dequeue(Queue* queue)
{
	if (queue->front == NULL)
	{
		printf("Queue is empty!\n");
		return NULL;
	}
	
	Node* tempNode = queue->front;
    BoardState* boardState = tempNode->boardState;

    queue->front = queue->front->next;

    if (queue->front == NULL)
    {
        queue->tail = NULL;
    }
    
    free(tempNode);

	return boardState;
}

// Function to print queue for debugging. Well, it's incorrect as front is not the 0th index after dequeue. Remove later.
void printQueue(Queue* queue, int k)
{
	int index = 0;
    Node* current = queue->front;

	while (current != NULL)
	{
		printf("Queue Board: %d\n", index);
		printBoard(current->boardState->boardState, k);
        current = current->next;
		index++;
	}
    printf("\n");
}

// Function to print Hash Table for debugging purpose. Remove later.
void printHashTable(HashTable* hashTable, int k)
{
    for (int i = 0; i < hashTable->size; i++) 
    {
        Node* current = hashTable->buckets[i];

        if (current != NULL)
        {
            printf("Bucket %d:\n", i);
            while (current != NULL) 
            {
                printBoard(current->boardState->boardState, k);
                current = current->next;
            }
            printf("=========================\n");
        }
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
    return true;
}

// Function to initialize Hash table.
HashTable* initializeHashTable()
{
	HashTable* hashTable = malloc(sizeof(HashTable));
	hashTable->size = 1009;
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

// Function to swap two items.
void swap(int* a, int* b) 
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Function to find the goal state with the help of quicksort.
int* getGoalState(int boardState[], int k)
{
	int* goalState = malloc(sizeof(int) * (k*k));
	int index = 0;

	for (int i = 0; i < k*k - 1; i++)
	{
		goalState[i] = i + 1;
	}

	goalState[k*k - 1] = 0;

	return goalState;
}

// Function to free the BoardState Struct.
void freeBoardStateStruct(BoardState* boardState)
{
	if (boardState != NULL)
	{
		if (boardState->boardState)
		{
			free(boardState->boardState);
		}
		free(boardState);
	}
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

// Function to enqueue the adjacent board states from the current position of empty tile. Also checks for goal state (Need to make it readable) .
BoardState* getAdjacentBoardStates(BoardState* currentBoardState, int k, Queue* queue, HashTable* hashTable, int* goalState)
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
				
				if (isGoalState(newBoardState, goalState, k))
				{
					//printf("=====================Goal State reached!====================\n\n");
					return newBoardState;
				}
				
			} else
			{
				freeBoardStateStruct(newBoardState);
			}
        }
    }
	// printf("Queue after getting its adjacent:\n");
	// printQueue(queue, k);

	return NULL;
}

// BFS traversal to get the shortest path to solve the puzzle.
BoardState* BFSTraversal(Queue* queue, HashTable* hashTable, int* goalState, int k)
{
	BoardState* solutionState = NULL;	
	while (queue->front != NULL && solutionState == NULL)
	{
		BoardState* currentBoardState = dequeue(queue);
		solutionState = getAdjacentBoardStates(currentBoardState, k, queue, hashTable, goalState);
	}
	return solutionState;
}

// Function to check if the puzzle is solvable or not. Inversion technique.
bool isSolvable(int boardState[], int k)
{
    int inversionCount = 0;
	int emptyTileIndex = findEmptyTile(boardState, k);
	int row = emptyTileIndex / k;
	int column = emptyTileIndex % k;

    for (int i = 0; i < k*k - 1; i++)
    {
        for (int j = i+1; j < k*k; j++)
        {
            if (boardState[i] != 0 && boardState[j] != 0 && boardState[i] > boardState[j])
            {
                inversionCount++;
            }
        }
    }

	// printf("Row: %d\n", row);
	// printf("Inversion count: %d\n\n", inversionCount);

	if (row % 2 == 1)		// If grid is odd
	{
		return (inversionCount % 2 == 0); 
	} else		// If grid is even
	{
		// 1. Odd inversion and empty tile is on even row from bottom.
		// 2. Even inversion and empty tile is on odd row from bottom.
		if ((k - row) % 2 == 0)
		{
			return (inversionCount % 2 == 1);
		} else
		{
			return(inversionCount % 2 == 0);
		}
	}
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

	getline(&line,&lineBuffSize,fp_in);		// ignore the first line in file, which is a comment
	fscanf(fp_in,"%d\n",&k);				// read size of the board
	getline(&line,&lineBuffSize,fp_in);		// ignore the second line in file, which is a comment

	int initial_board[k*k];					// get kxk memory to hold the initial board

	for(int i = 0; i < k*k; i++)
	{
		fscanf(fp_in, "%d ", &initial_board[i]);
	}
	fclose(fp_in);

	// printf("Initial Board:\n\n");
	// printBoard(initial_board, k); // Remove later

    // Check if this puzzle is solvable or not
    if (!isSolvable(initial_board, k))
    {
        fprintf(fp_out, "#moves\n");
        fprintf(fp_out, "no solution\n");

        fclose(fp_out);
        free(line);

        return 0;
    }

	//////////////////////////////////
	// do the rest to solve the puzzle
	//////////////////////////////////

	// Get the goal state.
	int* goalState = getGoalState(initial_board, k);
	// printf("Goal State:\n\n");
	// printBoard(goalState, k); // Remove later

	// Initialize Queue
	Queue* queue = initializeQueue();

	// Initialize hash table to store unique board states.
	HashTable* hashTable = initializeHashTable();
	
	// Insert the initial board to queue and hash table.
	BoardState* initialBoardState = createBoardState(initial_board, k);
	enqueue(queue, initialBoardState);
	insertToHashTable(hashTable, initialBoardState, k);

	// Start the BFS.
	BoardState* solutionBoardState = BFSTraversal(queue, hashTable, goalState, k);

	// Printing for debugging. Remove later.
	// printQueue(queue, k);
	// printHashTable(hashTable, k);

	// printf("Child to Parent Heirarchy\n");
	int numberOfMoves = 0;
	int moves[100];
	int index = 0;

    while (solutionBoardState != NULL)
    {
		// printBoard(solutionBoardState->boardState, k);
		int emptyTileIndex = findEmptyTile(solutionBoardState->boardState, k);

        solutionBoardState = solutionBoardState->parent;
		if (solutionBoardState != NULL)
		{
			moves[index] = solutionBoardState->boardState[emptyTileIndex];		// Here, we will get which number swapped with the empty tile.
			index++;
		}

		numberOfMoves++;
    }

	// printf("Number of moves: %d\n", numberOfMoves - 1);
	// printf("Moves: ");
	// for (int i = numberOfMoves - 2; i >= 0; i--)
	// {
	// 	printf("%d ", moves[i]);
	// }

	fprintf(fp_out, "#moves\n");
	for(int i = numberOfMoves - 2; i >= 0; i--)
	{
		fprintf(fp_out, "%d ", moves[i]);
	}
	fclose(fp_out);

	//////////////////////////////////
	// Free all the memory allocated
	//////////////////////////////////

	free(line);
	free(goalState);

    // Free all elements in the hash table including boardstates.
    for (int i = 0; i < hashTable->size; i++) 
	{
        Node* current = hashTable->buckets[i];

        while (current != NULL) 
		{
            Node* temp = current;
            if (temp->boardState != NULL)
            {
                freeBoardStateStruct(temp->boardState);
                temp->boardState = NULL;
            }
            current = current->next;
            free(temp);
        }
    }
    free(hashTable->buckets);
    free(hashTable);

    // Free all nodes in the queue linked-list
    Node* current = queue->front;
    while (current != NULL)
    {
        Node* temp = current;
        current = current->next;
        free(temp);
    }
    
    // Free the queue struct
    free(queue);

	return 0;
}
