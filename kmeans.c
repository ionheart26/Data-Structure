#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#define MAX_ITERATION 20
struct node {
	float* value;
	struct node* next;
};
struct node** list;

//insert node after trail
void insert(struct node** list, struct node* trail, struct node* node) {
	if (trail) {
		node->next = trail->next;
		trail->next = node;
	}
	else {
		node->next = *list;
		*list = node;
	}
}

//remove node after trail
void erase(struct node** list, struct node* trail) {
	if (trail)
		trail->next = trail->next->next;
	else
		(*list) = (*list)->next;
}

void concatenate(struct node** list1, struct node* list2) {
	if (*list1) {
		struct node* last;
		for (last = *list1; last->next; last = last->next);
		last->next = list2;
	}
	else {
		*list1 = list2;
	}
}

//return nearest centroid index
int nearest(const float* data, const int k, const int feature_num) {
	int idist = 0;
	double dist = 0;
	//initialize as the first centroid distance
	for (int i = 0; i < feature_num; i++) {
		dist += (list[0]->value[i] - data[i]) * (list[0]->value[i] - data[i]);
	}
	//find minimum
	for (int i = 1; i < k; i++) {
		double temp = 0;
		for (int j = 0; j < feature_num; j++) {
			temp += (list[i]->value[j] - data[j]) * (list[i]->value[j] - data[j]);
		}
		if (temp < dist) {
			dist = temp;
			idist = i;
		}
	}
	return idist;
}

void print_centroids(int k, int feature_num) {
	printf("===================================start==============================\n");
	for (int i = 0; i < k; i++) {
		printf("list%d:", i);
		for (int j = 0; j < feature_num; j++) {
			printf("%f ", list[i]->value[j]);
		}
		printf("\n");
	}
	printf("===================================end==============================\n");
}

int main() {
	FILE* in;
	int n, feature_num, k;
	srand(1000);

	/**********************INPUT************************/
	in = fopen("input.txt", "r");
	fscanf(in, "%d %d %d", &n, &feature_num, &k);
	list = (struct node**)malloc(sizeof(struct node*) * k);
	//insert centroids
	for (int i = 0; i < k; i++) {
		list[i] = NULL;
		float* data = (float*)malloc(sizeof(float) * feature_num);
		for (int j = 0; j < feature_num; j++)
			data[j] = rand();
		struct node* temp = (struct node*)malloc(sizeof(struct node));
		temp->value = data;
		temp->next = NULL;
		insert(&list[i], NULL, temp);
	}
	//insert input
	for (int i = 0; i < n; i++) {
		float* data = (float*)malloc(sizeof(float) * feature_num);
		for (int j = 0; j < feature_num; j++) {
			fscanf(in, "%f", &data[j]);
		}
		struct node* temp = (struct node*)malloc(sizeof(struct node));
		temp->value = data;
		temp->next = NULL;
		insert(&list[0], list[0], temp);
	}
	fclose(in);

	print_centroids(k, feature_num);
	/**********************CLUSTERING************************/
	struct node** _list = (struct node**)malloc(sizeof(struct node*) * k);
	for (int i = 0; i < MAX_ITERATION; i++) {
		/***************************************
		traverse lists and reconstruct them.
		1. insert each nodes into proper _list.
		2. concatenate list and _list.
		****************************************/
		for (int i = 0; i < k; i++)
			_list[i] = NULL;
		for (int i = 0; i < k; i++) {
			struct node* trail = list[i];
			struct node* temp = list[i]->next;
			if (!temp) continue;
			struct node* follow = temp->next;
			while (temp != NULL) {
				int idx = nearest(temp->value, k, feature_num);
				if (idx == i) {
					trail = temp;
					temp = follow;
					if (follow)
						follow = follow->next;
					continue;
				}
				erase(&list[i], trail);
				insert(&_list[idx], NULL, temp);
				temp = follow;
				if (follow)
					follow = follow->next;
			}
		}
		for(int i = 0; i < k; i++)
			concatenate(&list[i], _list[i]);


		//calculate centroids
		for (int i = 0; i < k; i++) {
			//initialize centroid to zero
			for (int j = 0; j < feature_num; j++) {
				list[i]->value[j] = 0;
			}
			//sum up
			int cnt = 0;
			struct node* temp;
			for (temp = list[i]->next; temp; temp = temp->next) {
				cnt++;
				for (int j = 0; j < feature_num; j++) {
					list[i]->value[j] += temp->value[j];
				}
			}
			//divide
			if (cnt) {
				for (int j = 0; j < feature_num; j++)
					list[i]->value[j] /= cnt;
			}
		}
		print_centroids(k, feature_num);
	}

	/**********************TEST OUTPUT************************/
	FILE* out = fopen("output.txt", "w");
	for (int i = 0; i < k; i++) {
		fprintf(out, "%d\n", i);
		struct node* temp;
		for (temp = list[i]->next; temp; temp = temp->next) {
			for (int j = 0; j < feature_num; j++) {
				fprintf(out, "%f ", temp->value[j]);
			}
			fprintf(out, "\n");
		}
	}
	fclose(out);

	/**********************EXIT************************/
	free(_list);
	return 0;
}