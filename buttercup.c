/*Buttercup - objective from a fixrank file, pull out original
fastq sequence information*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include "search.h"
#include <errno.h>

/*argument collection and start of agorithm*/

struct files {
	FILE *outfile;
	FILE *fixrank;
	FILE *fastq;
};

struct split_data {
	char **tokenize;
	int elements;
};

struct data {
	char *taxlevel;
	char *name;
};


void errorOut(struct files *f, struct data *d) {
	printf("Oh, shit\n");
	exit(-10);
}



/*last element is a single '\0' character*/
struct split_data *splitData(char *tokenize, const char *delim) {
	char **data;
	char *tmpToken = NULL, *tmpStringLoc, *tmp = strdup(tokenize);
	int elements = 0;
	struct split_data *info = (struct split_data*) malloc(sizeof(struct split_data));

	/*tmp is used to leave the original char * the same*/
	tmpToken = strtok_r(tmp, delim, &tmpStringLoc);
	
	while (tmpToken != NULL) {
		elements++;
		if (elements == 1) {
			(info->tokenize) = (char **)malloc(elements *sizeof(char *));
		} else {
			(info->tokenize) = realloc(info->tokenize, (elements+1) * sizeof(char *));
		}
		(info->tokenize)[elements-1] = strdup(tmpToken);
		tmpToken = strtok_r(NULL, delim, &tmpStringLoc);
	}
	(info->tokenize)[elements] = strdup("\0");
	info->elements = elements;
	return info;
 		
}


void DiveThroughFixrank(struct tree *t, FILE *fixrank, char *taxlevel, char *name) {
	char *line = NULL;
	size_t length;
	ssize_t read;
	char *s;
	char *tmp;
	long long int id;
	struct split_data *nameSearch = (struct split_data*)malloc(sizeof(struct split_data));
	struct split_data *idFirst = (struct split_data*)malloc(sizeof(struct split_data));
	struct split_data *idLast = (struct split_data*)malloc(sizeof(struct split_data));

	int i = 0;

	/*Read the entire file*/
	while ((read = getline(&line, &length, fixrank)) != -1) {
		nameSearch = splitData(line, "\t");		
		for (i = 0; i < nameSearch->elements; i++) {
			/*if there is a match, the next should be taxlevel*/	
			if (strcmp(name, (nameSearch->tokenize)[i]) == 0) {
				if (strcmp(taxlevel, (nameSearch->tokenize)[i+1]) == 0) {
					idLast = splitData((nameSearch->tokenize)[0], "|");
					idFirst = splitData((idLast->tokenize)[0], ":");	
					s = (char *)malloc(100*sizeof(char));
					sprintf(s, "%s%s%s", (idFirst->tokenize)[4], (idFirst->tokenize)[5], (idFirst->tokenize)[6]);
					id = strtoll(s, &tmp, 0);
					free(s);
					AddNode(&(t->root), id);
 
				}
				free((nameSearch->tokenize)[i]);
			}
		}

	}
}

void PullOutMatchedReads(struct tree *t, FILE *fastq, FILE *out) {

	int location = 0;
	int write = 0;
	char *line = NULL;
	size_t length;
	ssize_t read;
	struct split_data *idFirst = (struct split_data*)malloc(sizeof(struct split_data));
	struct split_data *idLast = (struct split_data*)malloc(sizeof(struct split_data));
	char *tmp;	
	char *s;
	long long int id = 0;

	while ((read = getline(&line, &length, fastq)) != -1) {
		if (location == 0) {
			idFirst = splitData(line, " ");
			idLast = splitData((idFirst->tokenize)[0], ":");
			s = (char *)malloc(100*sizeof(char));
			sprintf(s, "%s%s%s", (idLast->tokenize)[idLast->elements-3], (idLast->tokenize)[idLast->elements-2], (idLast->tokenize)[idLast->elements-1]);
			id = strtoll(s, &tmp, 0);
			write = Lookup(t->root, id);		
			location++;
		} else if (location == 3) {
			location = 0;
		} else {
			location++;
		}

		if (write) {
			fprintf(out, "%s", line);
		}	
		
	}
}
void CreateSearchTree(struct files *f, struct data *d) {
	struct tree *t = (struct tree *)malloc(sizeof(struct tree));
	t->root = NULL;	
	DiveThroughFixrank(t, f->fixrank, d->taxlevel, d->name);
	PullOutMatchedReads(t, f->fastq, f->outfile);


}
int main(int argc, char *argv[]) {

	int cmd_line_char, long_index;

	struct files requiredFiles;
	struct data taxInfo;

	const struct option longopts[] = {
		{"version", no_argument, 0, 'v'},
		{"help", no_argument, 0, 'h'},
		{"outfile", required_argument, 0, 'o'},
		{"fastq", required_argument, 0, 'f'},
		{"fixrank", required_argument, 0, 'r'},
		{"tax_level", required_argument, 0, 'l'},
		{"name", required_argument, 0, 'n',},
		{0, 0, 0, 0}
	};
	
	while ((cmd_line_char = getopt_long(argc, argv, "vho:f:r:l:n:", longopts, &long_index)) != EOF) {
		switch(cmd_line_char) {
			case 'o':
				requiredFiles.outfile = fopen(optarg, "w");
				if (requiredFiles.outfile == NULL) {
					errorOut(&requiredFiles, &taxInfo);
				}
				break;
			case 'f':
				requiredFiles.fastq = fopen(optarg, "r");
				if (requiredFiles.fastq == NULL) {
					errorOut(&requiredFiles, &taxInfo);
				}
				break;
			case 'r':
				requiredFiles.fixrank = fopen(optarg, "r");
				if (requiredFiles.fixrank == NULL) {
					errorOut(&requiredFiles, &taxInfo);
				}
				break;
			case 'l':
				taxInfo.taxlevel = strdup(optarg);
				break;
			case 'n':
				taxInfo.name = strdup(optarg);
				break;
		}
		
	}

	CreateSearchTree(&requiredFiles, &taxInfo);
	
	return 0;

}
