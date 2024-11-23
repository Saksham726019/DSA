#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
	printf("\n");
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

// Function to create boardState in struct.
BoardState* createBoardState(int* boardState, int k)
{
	BoardState* newBoardState = malloc(sizeof(BoardState));
	newBoardState->boardState = malloc(sizeof(int) * (k*k));
	memcpy(newBoardState->boardState, boardState, sizeof(int) * k*k);
	newBoardState->parent = NULL;

	return newBoardState;	
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
void enqueue(Queue* queue, int *boardState, int k)
{
	if (queue->tail == queue->capacity)
	{
		// Print statement for debugging. Remove later
		printf("Queue is full. Resizing...\n");
		resizeQueue(queue);
	}

	BoardState* newBoardState = createBoardState(boardState, k);
	queue->boardStates[queue->tail] = newBoardState;
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

// Function to enqueue the adjacent board states from the current position of empty tile. (Need to make it readable).
void getAdjacentBoardStates(BoardState* currentBoardState, int k, Queue* queue)
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
            // Swap the empty tile with the new position
            swap(&currentBoardState->boardState[emptyTileIndex], &currentBoardState->boardState[newRow * k + newCol]);

            // Create a new board state and enqueue it
            BoardState* newState = createBoardState(currentBoardState->boardState, k);
            newState->parent = currentBoardState; // Set the parent to track the path

            enqueue(queue, newState->boardState, k); // Enqueue the new state

            // Undo the swap for the next iteration
            swap(&currentBoardState->boardState[emptyTileIndex], &currentBoardState->boardState[newRow * k + newCol]);
        }
    }
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

// Function to free all dynamically allocated memory
void freeMemory(Queue* queue, int* goalState)
{
	for (int i = queue->front; i < queue->tail; i++)
	{
		freeBoardStateStruct(queue->boardStates[i]);
	}
	free(queue->boardStates);
	free(queue);

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
	
	// Enqueue and dequeue for debugging. Remove later
	enqueue(queue, initial_board, k);

	// Make a function for BFS.

	// printf("Dequeued board:\n\n");
	// BoardState* dequeuedBoard = dequeue(queue);
	// printBoard(dequeuedBoard->boardState, k);
	// freeBoardStateStruct(dequeuedBoard);

	// // Printing empty tile location for debugging. Remove later.
	// int emptyTileIndex = findEmptyTile(initial_board, k);
	// int row = emptyTileIndex / k;
	// int colum = emptyTileIndex % k;
	// printf("Empty Tile location: (%d, %d)\n", row, colum);

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

	freeMemory(queue, goalState);
	free(line);

	return 0;

}
