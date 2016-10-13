#include "search.h"

void AddNode(struct node **n, long long int id) {
	if (*n == NULL) {
		*n = (struct node *)malloc(sizeof(struct node));
		(*n)->id = id;
		(*n)->left = NULL;
		(*n)->right = NULL;
        (*n)->count = 0;
	} else {
        /*if (((*n)->id) == id) {
            //printf("%llu\n", id);
        }*/
        
		if (((*n)->id) > id) {
			AddNode(&((*n)->left), id);
		} else {
			AddNode(&((*n)->right), id);
		}
	}
}

int Lookup(struct node *n, long long int id) {

	if (n == NULL) {
		return 0;
	} else if (n->id == id) {
        /*if (n->count != 0) {
            printf("bad %llu\n", id);
        }*/
        (n->count)++;
		return 1;
	} else {
		if (n->id > id) {
			return Lookup(n->left, id);
		} else {
			return Lookup(n->right, id);
		}
	}
}
