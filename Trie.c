#define _CRT_SECURE_NO_WARNINGS
//#include <windows.h>
//#define CHECK_TIME_START QueryPerformanceFrequency ((_LARGE_INTEGER*)&freq); QueryPerformanceCounter((_LARGE_INTEGER*)&start)
//#define CHECK_TIME_END(a) QueryPerformanceCounter((_LARGE_INTEGER*)&end); a=(float)((float) (end - start)/freq)
//__int64 start, freq, end;
//double result_time;
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define ALPHABETS 26
#define CHARACTERS 30 //, ' ? .
#define WORDS 600
#define LENGTH 20
#define TARGET 15
#define PRINT 7
#define FILENAME 50
#define INDICES 15

struct Trie {
	int is_terminal;
	int value[INDICES];
	int value_size;
	struct Trie* children[CHARACTERS];
};
struct Trie* root;

struct Trie* new_node(void);
void insert(char* key, int value, struct Trie* trie);
void find(char* key, int* arr, int* size, struct Trie* trie);
static int ctoi(char c);
static void gather(int* arr, int* arr_size, struct Trie* trie);
//void new_insert(char* key, int value);
int main() {
	char words[WORDS][LENGTH];
	int n = 0;
	char key[LENGTH];
	int arr[TARGET];
	int arr_size = 0;
	FILE* fp;
	char name[FILENAME];
	root = new_node();

	/* File Input */
	while (1) {
		printf("Enter file name: ");
		scanf("%s", name);
		//	sprintf(name, "%s.txt", name);
		fp = fopen(name, "r");
		if (fp == NULL) {
			printf("FILE OPEN ERROR: no such file\n");
		}
		else break;
	}
	while (1) {
		char input[LENGTH];
		int state = fscanf(fp, "%s", input);
		if (state == -1) break;
		strcpy(words[n++], input);
	}
	fclose(fp);
//	CHECK_TIME_START;
	for (int i = 0; i < n; i++) {
		insert(words[i], i, root);
	}
/*	CHECK_TIME_END(result_time);
	printf("insert(recursive): %lfs\n", result_time);

	CHECK_TIME_START;
	for (int i = 0; i < n; i++) {
		new_insert(words[i], i);
	}
	CHECK_TIME_END(result_time);
	printf("insert(iterative): %lfs\n", result_time);
*/

	/* Keyword Input */
	printf("Enter keyword: ");
	scanf("%s", key);
	find(key, arr, &arr_size, root);
	for (int i = 0; i < arr_size; i++) {
		printf("%d: ", i + 1);
		int index = arr[i];
		for (int j = 0; j < PRINT; j++) {
			if (index + j >= n) break;
			printf("%s ", words[index + j]);
		}
		printf("\n");
	}
	return 0;
}

struct Trie* new_node() {
	struct Trie* node = (struct Trie*)malloc(sizeof(struct Trie));
	node->is_terminal = 0;
	node->value_size = 0;
	for (int i = 0; i < CHARACTERS; i++)
		node->children[i] = NULL;
	return node;
}

void insert(char* key, int value, struct Trie* trie) {
	if (*key == '\0') {
		trie->is_terminal = 1;
		trie->value[(trie->value_size)++] = value;
		return;
	}
	int index = ctoi(*key);
	if (trie->children[index] == NULL)
		trie->children[index] = new_node();
	insert(key + 1, value, trie->children[index]);
}

void find(char* key, int* arr, int* arr_size, struct Trie* trie) {
	if (*key == '\0') {
		gather(arr, arr_size, trie);
		return;
	}

	int index = ctoi(*key);
	if (trie->children[index] == NULL)
		return;
	return find(key + 1, arr, arr_size, trie->children[index]);
}

static int
ctoi(char c) {
	if (c >= 'a' && c <= 'z')
		return c - 'a';
	else if (c >= 'A' && c <= 'Z')
		return c - 'A';
	switch (c) {
	case ',': return ALPHABETS;
	case '\'': return ALPHABETS + 1;
	case '?': return ALPHABETS + 2;
	case '.': return ALPHABETS + 3;
	}
}

static void
gather(int* arr, int* arr_size, struct Trie* trie) {
	if (trie->is_terminal) {
		memcpy(&arr[*arr_size], &trie->value, sizeof(int) * trie->value_size);
		(*arr_size) += trie->value_size;
	}
	for (int i = 0; i < CHARACTERS; i++) {
		if (trie->children[i] != NULL) {
			gather(arr, arr_size, trie->children[i]);
		}
	}
}

/*
void new_insert(char* key, int value) {
	struct Trie* trie = root;
	int index = ctoi(*key);
	while (*key) {
		if (trie->children[index] == NULL)
			trie->children[index] = new_node();
		trie = trie->children[index];
		key++;
	}
	trie->is_terminal = 1;
}
*/