//README: entropy계산시 log2대신 단순 log함수를 사용하기로 한다. 크기만 비교하면 되기 때문이다.
#define _CRT_SECURE_NO_WARNINGS
#define LableN 2
#define ValueN 2
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
struct node {
	int is_leaf;
	int prediction;
	int feature_index;
	struct node* link[ValueN];
};
struct node* root;
void create_node(struct node* node, int** X, int* Y, int r, int c, int* feature_index);
void main()
{
	int n, m; //n: the number of data, m: the number of features
	int** trainX, **testX;
	int* trainY, *testY;
	int* feature_index;
	FILE* fp = fopen("data.txt", "r");
	//Train input
	fscanf(fp, "%d %d", &n, &m);
	trainX = (int**)malloc(sizeof(int*) * n);
	trainY = (int*)malloc(sizeof(int) * n);
	feature_index = (int*)malloc(sizeof(int) * m);
	for (int i = 0; i < n; i++)
		trainX[i] = (int*)malloc(sizeof(int) * m);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			fscanf(fp, "%d", &trainX[i][j]);
		}
		fscanf(fp, "%d", &trainY[i]);
	}
	fclose(fp);
	for (int i = 0; i < m; i++)
		feature_index[i] = i;
	//Train
	create_node(&root, trainX, trainY, n, m, feature_index);

	//Test input
	fp = fopen("test.txt", "r");
	fscanf(fp, "%d", &n);
	testX = (int**)malloc(sizeof(int*) * n);
	testY = (int*)malloc(sizeof(int) * n);
	for (int i = 0; i < n; i++) {
		testX[i] = (int*)malloc(sizeof(int) * m);
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			fscanf(fp, "%d", &testX[i][j]);
		}
	}
	fclose(fp);
	//Test
	struct node* node;
	for (int i = 0; i < n; i++) {
		node = root;
		while (1) {
			if (node->is_leaf) {
				testY[i] = node->prediction;
				break;
			}
			int feature = node->feature_index;
			int value = testX[i][feature];
			node = node->link[value];
		}
	}
	fp = fopen("output.txt", "w");
	for (int i = 0; i < n; i++)
		fprintf(fp, "%d\n", testY[i]);
	fclose(fp);
	return;
}

/***********************************************************************************
	Create node at NODE given X and Y
	If there is no more feature to use or entropy is zero, then create leaf node
	FEATURE_INDEX indicates real index of feature
************************************************************************************/
void create_node(struct node** node, int** X, int* Y, int r, int c, int* feature_index) {
	*node = (struct node*)malloc(sizeof(struct node));

	//if no more feature, find majority and create leaf node
	if (c == 0) {
		int cnt = 0;
		for (int i = 0; i < r; i++) {
			if (Y[i] == 0) cnt++;
		}
		(*node)->is_leaf = 1;
		(*node)->prediction = cnt >= r ? 0 : 1;
		goto EXIT;
	}
	/*
	
		Find the feature that leads to minmum entropy
	
	*/
	int minIndex = c;
	double minEntropy = -1;
	int count[ValueN][LableN];
	for (int index = 0; index < c; index++) {
		memset(count, 0, sizeof(int) * ValueN * LableN);
		for (int i = 0; i < r; i++) {
			int value = X[i][index];
			int label = Y[i];
			count[value][label]++;
		}
		double entropy_temp[ValueN];
		double entropy;
		double p;
		for (int j = 0; j < ValueN; j++) {
			p = (double)count[j][0] / (count[j][0] + count[j][1]);
			if (p == 0 || p == 1)
				entropy_temp[j] = 0;
			else
				entropy_temp[j] = -p * log(p) + -(1 - p) * log(1 - p);
		}
		p = ((double)count[0][0] + count[0][1]) / r;
		if (p == 0 || p == 1)
			entropy = 0;
		else
			entropy = p * entropy_temp[0] + (1 - p) * entropy_temp[1];
		if (minEntropy == -1 || minEntropy > entropy) {
			minIndex = index;
			minEntropy = entropy;
			if (minEntropy == 0) break;
		}
	}
	/*
	
		create node
		1. check if entropy is zero and thus this node should be a leaf node
		2. else create node and distribute data to call recursion
	
	*/
	if (minEntropy == 0) {
		(*node)->is_leaf = 1;
		(*node)->prediction = Y[0];
		goto EXIT;
	}

	(*node)->is_leaf = 0;
	(*node)->feature_index = feature_index[minIndex];
	int nextr[ValueN] = { 0 };
	int** nextX[ValueN];
	int* nextY[ValueN];
	for (int i = 0; i < r; i++) {
		int value = X[i][minIndex];
		nextr[value]++;
	}
	for (int i = 0; i < ValueN; i++) {
		nextX[i] = (int**)malloc(sizeof(int*) * nextr[i]);
		nextY[i] = (int*)malloc(sizeof(int) * nextr[i]);
	}
	int indexr[ValueN] = { 0 };
	for (int i = 0; i < r; i++) {
		int value = X[i][minIndex];
		nextX[value][indexr[value]] = (int*)malloc(sizeof(int) * (c - 1));
		int indexc = 0;
		for (int j = 0; j < c; j++) {
			if (j != minIndex) {
				nextX[value][indexr[value]][indexc++] = X[i][j];
			}
		}
		nextY[value][indexr[value]] = Y[i];
		indexr[value]++;
	}
	int* nextFeatureIndex = (int*)malloc(sizeof(int) * (c - 1));
	int index = 0;
	for (int i = 0; i < c; i++) {
		if (i != minIndex)
			nextFeatureIndex[index++] = feature_index[i];
	}
	for (int i = 0; i < ValueN; i++)
		create_node(&(*node)->link[i], nextX[i], nextY[i], nextr[i], c - 1, nextFeatureIndex);

EXIT:
	//free
	for (int i = 0; i < r; i++)
		free(X[i]);
	free(X);
	free(Y);
	//free(feature_index);//to free correctly, nextFeatureIndex should also be defined array of ValueN
	return;
}