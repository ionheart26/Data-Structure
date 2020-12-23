/*****************************************************
Assume that class is labeled as sequential integers.
Range: 0 - MAX_CLASS_NUM
ex. 0, 1, 2
*****************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#define K 7
#define MAX_CLASS_NUM 50
#define SWAP(a, b) {int temp = a; a = b; b = temp;}

int main() {
	FILE* in;
	int n, feature_num, m;
	float** x_train, ** x_test;
	int* y_train, * y_test;

	/**********************TRAIN INPUT************************/
	in = fopen("data.txt", "r");
	fscanf(in, "%d %d", &n, &feature_num);
	x_train = (float**)malloc(sizeof(float*) * n);
	y_train = (int*)malloc(sizeof(int) * n);
	for (int i = 0; i < n; i++) {
		x_train[i] = (float*)malloc(sizeof(float) * feature_num);
		for (int j = 0; j < feature_num; j++) {
			fscanf(in, "%f", &x_train[i][j]);
		}
		fscanf(in, "%d", &y_train[i]);
	}
	fclose(in);

	/**********************TEST INPUT************************/
	in = fopen("test.txt", "r");
	fscanf(in, "%d", &m);
	x_test = (float**)malloc(sizeof(float*) * m);
	for (int i = 0; i < m; i++) {
		x_test[i] = (float*)malloc(sizeof(float) * feature_num);
		for (int j = 0; j < feature_num; j++) {
			fscanf(in, "%f", &x_test[i][j]);
		}
	}
	fclose(in);

	/**********************PREDICTION************************/
	y_test = (int*)malloc(sizeof(int) * m);
	float* dist = (float*)malloc(sizeof(float) * n);
	int* sorted_class = (int*)malloc(sizeof(int) * n);
	int* class = (int*)malloc(sizeof(int) * MAX_CLASS_NUM);
	for (int i = 0; i < m; i++) {
		//CALCULATE DISTANCE(SQUARE)
		for (int j = 0; j < n; j++) {
			dist[j] = 0;
			for (int k = 0; k < feature_num; k++) {
				dist[j] += (x_train[j][k] - x_test[i][k]) * (x_train[j][k] - x_test[i][k]);
			}
		}

		//SORT
		for (int i = 0; i < n; i++) {
			sorted_class[i] = y_train[i];
		}
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n - i - 1; j++) {
				if (dist[j] > dist[j + 1]) {
					SWAP(dist[j], dist[j + 1]);
					SWAP(sorted_class[j], sorted_class[j + 1]);
				}
			}
		}
		//COUNT
		for (int i = 0; i < MAX_CLASS_NUM; i++) {
			class[i] = 0;
		}
		for (int i = 0; i < K; i++) {
			class[sorted_class[i]]++;
		}
		int max = 0, prediction;
		for (int i = 0; i < MAX_CLASS_NUM; i++) {
			if (max < class[i]) {
				max = class[i];
				prediction = i;
			}
		}
		y_test[i] = prediction;
	}
	free(dist);
	free(sorted_class);
	free(class);

	/**********************TEST OUTPUT************************/
	FILE* out = fopen("output.txt", "w");
	for (int i = 0; i < m; i++) {
		fprintf(out, "%d\n", y_test[i]);
	}

	/**********************EXIT************************/
	for (int i = 0; i < n; i++) {
		free(x_train[i]);
	}
	free(x_train);
	free(y_train);
	for (int i = 0; i < m; i++) {
		free(x_test[i]);
	}
	free(x_test);
	free(y_test);
	fclose(out);
	return 0;
}