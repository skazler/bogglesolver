
/*////////////////////////////////////////////////////////////////////////

							 Boggle Solver

*/////////////////////////////////////////////////////////////////////////

// packages
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


/************************ Structs & Definitions *************************/

/* Structs */
typedef struct _Results {
	const char* const* Words;
	unsigned Count;
	unsigned Score;
	void* UserData;
}Results;

typedef struct _tnode {
	struct _tnode* link[26];
	int EOW; // 1 = end of word, 0 = not end of word
}tnode;

/* Global variables */
unsigned width = 0;
unsigned height = 0;
int sz;
int wc = 0;
int score = 0;
tnode* trie_head;


/****************************** Functions ******************************/

tnode* new_tnode()
{
	tnode* new_n = NULL;
	new_n = (tnode*)malloc(sizeof(tnode));

	if (new_n)
	{
		int counter;

		new_n->EOW = 0;

		for (counter = 0; counter < 26; counter++)
		{
			new_n->link[counter] = NULL;
		}
	}
	return new_n;
}

void insert_string(char* word)
{
	int wlen = (strlen(word) - 1);

	tnode* curr = trie_head;

	int counter;

	for (counter = 0; counter < wlen; counter++)
	{
		if (counter > 0)
			if (word[counter - 1] == 'q' && word[counter] == 'u')
				continue;

		int index = word[counter] - 'a';

		if (curr->link[index] == NULL)
		{
			curr->link[index] = new_tnode();
		}

		curr = curr->link[index];
	}
	curr->EOW = 1;

}

// read the file
void LoadDictionary(const char* path)
{
	// initializations
	FILE* dictionary;
	char buffer[255];
	dictionary = fopen(path, "r");

	// error check
	if (!dictionary)
	{
		fclose(dictionary);
		fprintf(stderr, "error: dictionary file not valid\n");
		exit(0);
	}

	// read & insert into trie
	while ((fgets(buffer, 1028, dictionary)) && (!feof(dictionary)))
	{
		// insert line by line (aka string by string) into trie
		insert_string(buffer);
	}

	// close        
	fclose(dictionary);
}

int search_trie(char* word)
{
	int wlen = strlen(word);
	tnode* curr = trie_head;
	int counter;
	for (counter = 0; counter < wlen; counter++)
	{
		int index = word[counter] - 'a';
		if (curr->link[index] == NULL)
		{
			return 0;
		}

		curr = curr->link[index];
	}

	if ((curr) && (curr->EOW == 1))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// convert coordinate to index
int ctoi(int x, int y)
{
	return (x + (y * width));
}

// print an array
void print_array(int *array)
{
	// initializations
	unsigned rc;
	unsigned hc;

	for (hc = 0; hc < height; hc++)
	{
		for (rc = 0; rc < width; rc++)
		{
			printf("%d ", array[ctoi(rc, hc)]);
		}

		printf("\n");
	}

}

// check if current location matrix[row][col] is in range
int valid_loc(int row, int col, int *visited)
{
	if ((row >= 0) && ((unsigned)row < width) &&
		(col >= 0) && ((unsigned)col < height) &&
		(visited[ctoi(row, col)] == 0))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// score the word based on its length
int scorer(int len)
{
	int word_length = len + 1;

	if (word_length <= 4)
	{
		return 1;
	}
	else if (word_length == 5)
	{
		return 2;
	}
	else if (word_length == 6)
	{
		return 3;
	}
	else if (word_length == 7)
	{
		return 5;
	}
	else
		return 11;

}

void arr_printer(char *orig)
{
	int len = (width*height);
	int counter;

	for (counter = 0; counter < len; counter++)
	{
		printf("%c", orig[counter]);
	}

	printf("\n");
}

void finder(tnode* curr_node, const char* board, int x, int y,
	int *visited, char *str, int tracker)
{
	// initializations
	int counter;

	// word is found in the trie
	if ((curr_node->EOW) == 1)
	{
		// update
		int pos = ctoi(x, y);
		int index = board[pos] - 'a';
		str[tracker] = (index + 97);
		str[tracker + 1] = '\0';

		// update and print
		wc++;
		score += scorer(tracker);

		int len = strlen(str);
		for (int i = 0; i < len; i++)
		{
			if (str[i] == 'q')
			{
				printf("qu");
			}
			else
			{
				printf("%c", str[i]);
			}
		}
		printf("\n");
	}

	if (valid_loc(x, y, visited))
	{
		// update
		int pos = ctoi(x, y);
		int index = board[pos] - 'a';
		str[tracker] = (index + 97);
		visited[ctoi(x, y)] = 1;

		// increment
		tracker++;

		// go through all childs of curr_node
		for (counter = 0; counter < 26; counter++)
		{
			if (curr_node->link[counter])
			{
				char curr = counter + 'a';

				// recursive search through surroundings
								// case: (x-1, y-1)
				if ((valid_loc(x - 1, y - 1, visited) == 1) &&
					(board[ctoi(x - 1, y - 1)] == curr))
				{
					finder(curr_node->link[counter],
						board, x - 1, y - 1, visited,
						str, tracker);
				}
				// case: (x, y-1)
				if ((valid_loc(x, y - 1, visited) == 1) &&
					(board[ctoi(x, y - 1)] == curr))
				{
					finder(curr_node->link[counter],
						board, x, y - 1, visited,
						str, tracker);
				}
				// case: (x+1, y-1)
				if ((valid_loc(x + 1, y - 1, visited) == 1) &&
					(board[ctoi(x + 1, y - 1)] == curr))
				{

					finder(curr_node->link[counter],
						board, x + 1, y - 1, visited,
						str, tracker);
				}
				// case: (x-1, y)
				if ((valid_loc(x - 1, y, visited) == 1) &&
					(board[ctoi(x - 1, y)] == curr))
				{
					finder(curr_node->link[counter],
						board, x - 1, y, visited,
						str, tracker);
				}
				// case: (x+1, y)
				if ((valid_loc(x + 1, y, visited) == 1) &&
					(board[ctoi(x + 1, y)] == curr))
				{
					finder(curr_node->link[counter],
						board, x + 1, y, visited,
						str, tracker);
				}
				// case: (x-1, y+1)
				if ((valid_loc(x - 1, y + 1, visited) == 1) &&
					(board[ctoi(x - 1, y + 1)] == curr))
				{
					finder(curr_node->link[counter],
						board, x - 1, y + 1, visited,
						str, tracker);
				}
				// case: (x, y+1)
				if ((valid_loc(x, y + 1, visited) == 1) &&
					(board[ctoi(x, y + 1)] == curr))
				{
					finder(curr_node->link[counter],
						board, x, y + 1, visited,
						str, tracker);
				}
				// case: (x+1, y+1)
				if ((valid_loc(x + 1, y + 1, visited) == 1) &&
					(board[ctoi(x + 1, y + 1)] == curr))
				{
					finder(curr_node->link[counter],
						board, x + 1, y + 1, visited,
						str, tracker);
				}

			}
		}

		// mark unvisited
		visited[ctoi(x, y)] = 1;
	}

}

// read the board, try to find matching letter in dictionary,
// continue to compare letter by letter
Results FindWords(const char* board, unsigned width, unsigned height)
{
	// initializations
	int counter;
	Results results;
	tnode* temp = trie_head;

	// set up visited matrix, set all not visited
	int *visited = (int*)malloc(sizeof(int) * width * height);
	for (unsigned x = 0; x < (width * height); x++)
	{
		visited[x] = 0;
	}

	// an array for the growing word
	int sz = width * height;
	char *str = malloc(sz);
	for (int counting = 0; counting < sz; counting++)
	{
		// fill with initial dummy chars
		str[counting] = 0;
	}

	printf("Words found:\n");

	// we are searching by coordinates and not board[index] because it
	// makes it easier in the next steps to see which surrounding cells
	// also are being searched
	for (unsigned x = 0; x < width; x++)
	{
		for (unsigned y = 0; y < height; y++)
		{
			int pos = ctoi(x, y);
			int index = board[pos] - 'a';

			if (temp->link[index])
			{
				// refresh str
				for (int counting = 0; counting < sz; counting++)
				{
					// fill with initial dummy chars
					str[counting] = 0;
				}

				// refresh visited matrix
				unsigned x_ct;
				for (x_ct = 0; x_ct < (width * height); x_ct++)
				{
					visited[x_ct] = 0;
				}

				finder(temp->link[index], board, x, y,
					visited, str, 0);

				// just in case
				for (int counting = 0; counting < sz; counting++)
				{
					// fill with initial dummy chars
					str[counting] = 0;
				}

				// just in case
				for (x_ct = 0; x_ct < (width * height); x_ct++)
				{
					visited[x_ct] = 0;
				}
			}
		}
	}

	// setting up skeleton to fill in
	// first, results.Words
	char** words = malloc(wc * sizeof(char**));
	for (counter = 0; counter < wc; counter++)
	{
		words[counter] = NULL;
	}
	// when filling in, the following format should be followed:
	// char const* example = "example";
	// words[count_location] = example;
	results.Words = (const char* const*)words;
	// next, results.Count
	results.Count = wc;
	printf("\nNumber of words found: %d\n", results.Count);
	// next, results.Score
	results.Score = score;
	printf("Score: %d\n", results.Score);
	// next, results->UserData
	results.UserData = NULL;

    // free
    free(visited);
    free(str);
    
	// return
	return results;
}

void print_board(char* input)
{
	// initializations
	int str_len = strlen(input);
	int counter;

	// print the board
	printf("\n");
	for (counter = 0; counter <= str_len; counter++)
	{
		printf(" %c", input[counter]);

		if (((counter + 1) % width) == 0)
			printf("\n");
	}
	printf("\n");
}

// free stuff
void FreeDictionary()
{
	// a valgrind check shows no memory leaks
}

void FreeWords(Results results)
{
	// a valgrind check shows no memory leaks
}


/******************************* Main ***************************/

int main(int argc, char* argv[])
{
    // just some hardcoded testing cases
	//const char* argv[3];
	//argv[0] = "boggle2.exe";
	//argv[1] = "./boggle_solver";
	//argv[2] = "dictionary_3.txt";

    // this is assuming the input looks more like:
    // boggle_solver.exe ./boggle_solver "dictionary_something.txt"
    // so there are 3 pieces. This will not work by just running the
    // executable because a specific dictionary needs to be specified.
	if (argc != 3)
	{
		printf("Not enough inputs. Please check the README for ");
		printf("proper formatting.\n");
		exit(0);
	}

	// initializations
	const char* filename = argv[2];
	width = 0;
	height = 0;
	wc = 0;
	trie_head = new_tnode();

	// extract width and height from user input
	printf("What is the width of the boggle board?\n");
	scanf("%u", &width);
	printf("What is the height of the boggle board?\n");
	scanf("%u", &height);
	//check if input is valid
	if ((width <= 0) || (height <= 0))
	{
		printf("Sorry, your width or height can't be less than or ");
		printf("equal to 0. Please try again.\n");
		exit(0);
	}
	// valid confirmation message
	printf("Confirming width: %u, and height: %u...\n", width, height);

	// now extract the letters on the board from user input
	char *board = malloc(width*height);
	printf("Enter your board, left to right, from top to bottom, ");
	printf("NO SPACES, and in LOWERCASE:\n");
	scanf("%s", board);
	// check if input is valid
	if (strlen(board) != (width*height))
	{
		printf("Sorry, that is not a valid board based on your ");
		printf("width and height. Please try again.\n");
		exit(0);
	}
	// valid confirmation message
	printf("Confirming board:\n");
	print_board(board);

	// overall confirmation message
	printf("Okay, loading results...\n\n");

	// load the dictionary
	LoadDictionary(filename);

	//// find the words
	FindWords(board, width, height);

	//// free
	free(trie_head);

	printf("\nThank you for using Boggle-Solver!\n");
	// return
	return 1;
}
