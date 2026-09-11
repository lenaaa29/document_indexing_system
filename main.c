#include "search_index.h"

int main(){
	// open input and output files
	FILE *fin = fopen("indexare.in", "r");
	FILE *fout = fopen("indexare.out", "w");

	// check if files were opened successfully
	if (!fin || !fout){
		if (!fin){
			fprintf(stderr, "Error: could not open input file.\n");
		}
		if (!fout){
			fprintf(stderr, "Error: could not open output file.\n");
		}
		return 1;
	}

	// initialize the main system structure
	System *sys = createSystem(fout);

	if (!sys){
		fprintf(stderr, "Error: failed to initialize system.\n");
		fclose(fin);
		fclose(fout);
		return 1;
	}

	// read the number of commands
	int m;
	if (fscanf(fin, "%d", &m) != 1){
		fprintf(stderr, "Error: invalid command count.\n");
		fclose(fin);
		fclose(fout);
		return 1;
	}

	// consume the newline after the number
	fgetc(fin);

	// process each command from the input
	for (int i = 0; i < m; i++){
		// read the entire command line
		char *line = read_line(fin);
		if (!line){
			fprintf(stderr, "Error: failed to read command line.\n");
			break;
		}

		// remove trailing newline characters
		line[strcspn(line, "\r\n")] = '\0';

		// skip empty lines
		if (line[0] == '\0'){
			free(line);
			i--;
			continue;
		}

		// extract the command token
		char *cmd = strtok(line, " ");
		if (!cmd){
			free(line);
			continue;
		}

		if (strcmp(cmd, "ADD") == 0){
			char *id = strtok(NULL, " ");
			char *scoreStr = strtok(NULL, " ");
			char *tStr = strtok(NULL, " ");

			// transform from string (char*) to int
			if (id && scoreStr && tStr){
				int score = atoi(scoreStr);
				int t = atoi(tStr);

				// allocate array for the keywords
				char **words = malloc(t * sizeof(char *));

				if (!words){
					fprintf(stderr, "Error: memory allocation failed.\n");
					free(line);
					continue;
				}

				// read all keywords
				for (int j = 0; j < t; j++){
					char *w = strtok(NULL, " ");
					words[j] = duplicateString(w);

					if (!words[j]){
						fprintf(stderr, "Error: memory allocation failed.\n");
						for (int k = 0; k < j; k++){
							free(words[k]);
						}

						free(words);
						free(line);
						continue;
					}
				}

				// add the file to the system
				ADD(sys, id, score, t, words);

				// free temporary keyword copies
				for (int j = 0; j < t; j++){
					free(words[j]);
				}
				free(words);
			}
		} else if (strcmp(cmd, "DEL") == 0){
			char *id = strtok(NULL, " ");
			if (id){
				//delete the file from the system
				DEL(sys, id);
			}
		} else if (strcmp(cmd, "ADDKW") == 0){
			char *id = strtok(NULL, " ");
			char *word = strtok(NULL, " ");
			if (id && word){
				// add a keyword to an existing file
				ADDKW(sys, id, word);
			}
		} else if (strcmp(cmd, "DELKW") == 0){
			char *id = strtok(NULL, " ");
			char *word = strtok(NULL, " ");
			if (id && word){
				// remove a keyword from a file
				DELKW(sys, id, word);
			}
		} else if (strcmp(cmd, "FIND") == 0){
			char *word = strtok(NULL, " ");
			if (word){
				// find all documents containing a keyword
				FIND(sys, word);
			}
		} else if (strcmp(cmd, "TOPK") == 0){
			char *word = strtok(NULL, " ");
			char *kStr = strtok(NULL, " ");
			if (word && kStr){
				// retrieve top k documents for a keyword
				int k = atoi(kStr);
				TOPK(sys, word, k);
			}
		} else if (strcmp(cmd, "PRINT") == 0){
			// print the tree with all related files
			PRINT(sys);
		} else if (strcmp(cmd, "PREFIX") == 0){
			char *prefix = strtok(NULL, " ");
			if (prefix){
				// list all keywords starting with a given prefix
				PREFIX(sys, prefix);
			}
		} else {
			fprintf(stderr, "Error: unknown command '%s'.\n", cmd);
		}
		// free the line buffer
		free(line);
	}

	// free all system resources
	freeSystem(sys);

	// close files
	fclose(fin);
	fclose(fout);
	return 0;
}