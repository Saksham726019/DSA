#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct BoardState
{
    int* boardState;
	int emptyTileIndex;
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

// Function to create boardState in struct.
BoardState* createBoardState(int* boardState, int emptyTileIndex, int k)
{
	BoardState* newBoardState = malloc(sizeof(BoardState));
	newBoardState->boardState = malloc(sizeof(int) * (k*k));
	memcpy(newBoardState->boardState, boardState, sizeof(int) * k*k);
	newBoardState->emptyTileIndex = emptyTileIndex;
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

// Function to compare two board states in order to only insert unique board state to hash table.
bool isUniqueBoard(HashTable* hashTable, BoardState* boardState, int k) 
{
    int index = hashFunction(boardState, k, hashTable->size);

    Node* current = hashTable->buckets[index];
    
    while (current != NULL)
    {
		if (memcmp(current->boardState->boardState, boardState->boardState, sizeof(int) * k*k) == 0)
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
	hashTable->size = 20071;
	hashTable->buckets = malloc(sizeof(Node*) * hashTable->size);

	for (int i = 0; i < hashTable->size; i++)
	{
		hashTable->buckets[i] = NULL;
	}

	return hashTable;	
}

// Function to insert the board state to the hash table.
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

// Function to find the goal state.
void getGoalState(int goalState[], int k)
{
	int index = 0;

	for (int i = 0; i < k*k - 1; i++)
	{
		goalState[i] = i + 1;
	}

	goalState[k*k - 1] = 0;
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
	return memcmp(currentBoardState->boardState, goalState, sizeof(int) * k*k) == 0;
}

// Function to enqueue the adjacent board states from the current position of empty tile. Also checks for goal state (Need to make it readable) .
BoardState* getAdjacentBoardStates(BoardState* currentBoardState, int k, Queue* queue, HashTable* hashTable, int* goalState)
{
	int emptyTileIndex = currentBoardState->emptyTileIndex;
	int row = emptyTileIndex / k;
	int column = emptyTileIndex % k;

	// These are the directions that the empty tile can move at most.
	int directions[4][2] = 
	{
		{-1, 0},	// Up
		{1, 0},		// Down
		{0, -1},	// Left
		{0, 1}		// Right
	};

	for (int i = 0; i < 4; i++) 
	{
        int newRow = row + directions[i][0];
        int newCol = column + directions[i][1];

        // Check if the move doesn't cross the board boundary.
        if (newRow >= 0 && newRow < k && newCol >= 0 && newCol < k) 
		{
            // Swap the empty tile with the new position.
            swap(&(currentBoardState->boardState[emptyTileIndex]), &(currentBoardState->boardState[newRow * k + newCol]));

			// Check if the board already exists in the hash table. If not, create a new state and add to Queue and Hash Table.
			if (isUniqueBoard(hashTable, currentBoardState, k))
			{
				BoardState* newBoardState = createBoardState(currentBoardState->boardState, newRow * k + newCol, k);
				newBoardState->parent = currentBoardState; // Set the parent to track the path.
				enqueue(queue, newBoardState); // Enqueue the new state.
				insertToHashTable(hashTable, newBoardState, k);
				
				if (isGoalState(newBoardState, goalState, k))
				{
					// Restore the original state.
					swap(&currentBoardState->boardState[emptyTileIndex], &currentBoardState->boardState[newRow * k + newCol]);
					return newBoardState;
				}
			}
			// Restore the original state.
			swap(&currentBoardState->boardState[emptyTileIndex], &currentBoardState->boardState[newRow * k + newCol]);
        }
    }

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

	if (k % 2 == 1)		// If grid is odd
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

// main() function.
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

	getline(&line, &lineBuffSize, fp_in);		// ignore the first line in file, which is a comment
	fscanf(fp_in, "%d\n", &k);					// read size of the board
	getline(&line, &lineBuffSize, fp_in);		// ignore the second line in file, which is a comment

	int initial_board[k*k];					// get kxk memory to hold the initial board

	for(int i = 0; i < k*k; i++)
	{
		fscanf(fp_in, "%d ", &initial_board[i]);
	}
	fclose(fp_in);

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
	int goalState[k*k];
	getGoalState(goalState, k);

	// The initial board state
	int emptyTileIndex = findEmptyTile(initial_board, k);
	BoardState* initialBoardState = createBoardState(initial_board, emptyTileIndex, k);

	if (isGoalState(initialBoardState, goalState, k))
	{
		fprintf(fp_out, "#moves\n");

		fclose(fp_out);
		free(line);
		freeBoardStateStruct(initialBoardState);

		return 0;
	}
	

	// Initialize Queue
	Queue* queue = initializeQueue();

	// Initialize hash table to store unique board states.
	HashTable* hashTable = initializeHashTable();
	
	// Insert the initial board to queue and hash table.
	enqueue(queue, initialBoardState);
	insertToHashTable(hashTable, initialBoardState, k);

	// Start the BFS.
	BoardState* solutionBoardState = BFSTraversal(queue, hashTable, goalState, k);

	int numberOfMoves = 0;
	int moves[100];
	int index = 0;

	while (solutionBoardState != NULL) 
	{
		if (solutionBoardState->parent != NULL) 
		{
			// Get the index of the empty tile
			int emptyTileIndex = findEmptyTile(solutionBoardState->boardState, k);

			// Store the number that swapped with the empty tile
			moves[index++] = solutionBoardState->parent->boardState[emptyTileIndex];
		}

		solutionBoardState = solutionBoardState->parent;
		numberOfMoves++;
	}

	// Write the moves to output file.
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
