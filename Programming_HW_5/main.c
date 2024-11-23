#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Queue struct
typedef struct Queue
{
	int** boardStates;
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
	queue->boardStates = malloc(sizeof(int*) * queue->capacity);
	queue->front = 0;
	queue->tail = 0;

	return queue;
}

// Function to resize the queue.
void resizeQueue(Queue* queue)
{
    int newCapacity = queue->capacity * 2;
    int** newBoardState = realloc(queue->boardStates, sizeof(int*) * newCapacity);

    // Check if realloc was successful. Remove before submitting.
    if (newBoardState == NULL)
    {
        // If realloc fails, exit or handle the error appropriately
        printf("Memory allocation failed during queue resizing\n");
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

	queue->boardStates[queue->tail] = malloc(sizeof(int) * (k*k));
	
	// Copy the state of the board to the queue.
	for (int i = 0; i < k*k; i++)
	{
		queue->boardStates[queue->tail][i] = boardState[i];
	}

	queue->tail += 1;
}

// Function to dequeue
int* dequeue(Queue* queue)
{
	if (queue->front == queue->tail)
	{
		printf("Queue is empty!\n");
		return NULL;
	}
	
	int* boardState = queue->boardStates[queue->front];
	queue->front += 1;

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

// Function to find the goal state.
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

// Function to free all dynamically allocated memory
void freeMemory(Queue* queue, int* goalState)
{
	// Free each individual board state in the queue
	for (int i = queue->front; i < queue->tail; i++)
	{
		free(queue->boardStates[i]);
	}
	// Free the array that holds the pointers to board states
	free(queue->boardStates);
	// Free the queue structure itself
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

	getline(&line,&lineBuffSize,fp_in);//ignore the first line in file, which is a comment
	fscanf(fp_in,"%d\n",&k);//read size of the board
	//printf("k = %d\n", k); //make sure you read k properly for DEBUG purposes
	getline(&line,&lineBuffSize,fp_in);//ignore the second line in file, which is a comment

	int initial_board[k*k];//get kxk memory to hold the initial board
	for(int i=0;i<k*k;i++)
		fscanf(fp_in,"%d ",&initial_board[i]);
	printBoard(initial_board, k);//Assuming that I have a function to print the board, print it here to make sure I read the input board properly for DEBUG purposes
	fclose(fp_in);

	// Get the goal state.
	int* goalState = getGoalState(initial_board, k);
	//printBoard(goalState, k); Remove later

	//////////////////////////////////
	// do the rest to solve the puzzle
	//////////////////////////////////

	// Initialize Queue
	Queue* queue = initializeQueue();

	// Enqueue and dequeue for debugging. Remove later
	//enqueue(queue, initial_board, k);

	// int* dequeuedBoard = dequeue(queue);
	// printBoard(dequeuedBoard, k);
	// free(dequeuedBoard);

	// Printing empty tile location for debugging. Remove later.
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
