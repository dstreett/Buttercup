#include <stdio.h>
#include <stdlib.h>

struct node {
	long long int id;
	struct node *left;
	struct node *right;
    int count;
}; 

struct tree {
	struct node *root;
    int count;
};

int Lookup(struct node *n, long long int id);
void AddNode(struct node **n, long long int id);
