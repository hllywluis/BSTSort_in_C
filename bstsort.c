/*
 * Luis Bauza
 *
 * There's a lot going in this program, so comments will be provided above every function. However, in summary, this
 * program constructs a binary search tree using dynamic memory for nodes and their keys, as well as either printing out
 * the output to the command line or creating a file and "printing" the output to the file using a dynamically allocated
 * buffer. Finally, the tree is deleted and the files are closed.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/**
 * A basic structure for each Node, with a key and left and right nodes. The times integer is used for counting string
 * occurrences.
 */
struct Node {
    char* key;
    int times;
    struct Node *left, *right;
};

/**
 * Traverses the binary search tree using In-Order Traversal. Also either prints out or writes each Node's contents to
 * the terminal or a file.
 *
 * @param root The Node that we want to traverse.
 * @param toWrite Whether we are writing to a file or not.
 * @param outputFile The output file (or NULL) that we are writing to.
 */
void inOrderTraversal(struct Node *root, bool toWrite, FILE* outputFile) {
    if (root != NULL) {
        inOrderTraversal(root->left, toWrite, outputFile);
        if (!toWrite) {
            printf("%d %s\n", root->times, root->key);
        }
        else {
            char* buffer = malloc(sizeof(root->times) + sizeof(root->key));
            sprintf(buffer, "%d %s\n", root->times, root->key);
            fputs(buffer, outputFile);
            free(buffer);
        }
        inOrderTraversal(root->right, toWrite, outputFile);
    }
}

/**
 * Simply frees up both the dynamically allocated memory taken up by the key and by the Node.
 * @param root The node that we want to delete.
 */
void deleteNode(struct Node* root) {
    if (root != NULL) {
        free(root->key);
        free(root);
    }
}

/**
 * Traverses the tree in Post-Order and deletes every Node in the tree.
 * @param root The root of the tree that we want to delete.
 */
void postOrderDelete(struct Node* root) {
    if (root != NULL) {
        postOrderDelete(root->left);
        postOrderDelete(root->right);
        deleteNode(root);
    }
}

/**
 * Creates a new Node if there are no Nodes in the tree (if the tree is NULL).
 * @param key The key that we are inserting into the Node.
 * @return The temporary Node to be assigned to the Node variable.
 */
struct Node *createNewNode(char* key) {
    struct Node *temp = malloc(sizeof(struct Node));
    temp->key = malloc(sizeof(key));
    temp->key = strdup(key);
    temp->times = 1;
    temp->left = temp->right = NULL;
    return temp;
}

/**
 * Compares strings and returns 1 if stringOne is greater than stringTwo, or returns 0 if both strings are the same.
 *
 * @param stringOne The first string to compare.
 * @param stringTwo The second string to compare.
 * @return Either -1, 0, or 1 depending on the comparison.
 */
int compareStrings(char stringOne[], char stringTwo[]) {
    int i = 0;
    for (; ; i++) {
        if (stringOne[i] != stringTwo[i]) {
            return stringOne[i] < stringTwo[i] ? -1 : 1; // conditional statement, easy to use
        }
        if (stringOne[i] == '\0') { // if the end of the string is reached
            return 0;
        }
    }
}

/**
 * Inserts a given key into a given Node.
 *
 * @param node The Node to insert into.
 * @param key The key to insert.
 * @return The modified Node with key inserted.
 */
struct Node *insertIntoNode(struct Node* node, char* key) {
    if (node == NULL) {
       return createNewNode(key);
    }

    int result = compareStrings(key, node->key);

    if (result < 0) {
        node->left = insertIntoNode(node->left, key);
    } else if (result > 0) {
        node->right = insertIntoNode(node->right, key);
    } else {
        node->times++;
    }
    return node;
}

int main(int argc, char* argv[]) {
    // Declaring flags and associated variables.
    int option;
    int cFlag = 0;
    int oFlag = 0;
    char currentLine[101];
    char* currentLineTemp;
    char* outputFileName;
    struct Node* root = NULL;
    FILE* inputFile;
    FILE* outputFile;

    static char usage[] = "usage: bstsort [-c][-o output_file_name][input_file_name]"; // usage statement

    while ((option = getopt(argc, argv, ":co:")) != -1) {
        switch (option) {
            case 'c':
                cFlag = 1;
                break;
            case 'o':
                oFlag = 1;
                outputFileName = optarg;
                break;
            case '?':
                fprintf(stderr, "Unknown option -%c\n%s", optopt, usage);
                exit(1);
        }
    }

    inputFile = fopen(argv[argc - 1], "r");

    if (inputFile == NULL) {
        fprintf(stderr, "Cannot find file \"%s\"!\n", argv[argc - 1]);
        exit(1);
    }

    if (cFlag == 1) {
        while (fgets(currentLine, 101, inputFile) != NULL) {
            currentLineTemp = strtok(currentLine, "\n");
            if (root == NULL) {
                root = insertIntoNode(root, currentLineTemp);
            }
            else {
                insertIntoNode(root, currentLineTemp);
            }
        }
        if (oFlag != 1) { // no output if the user wants output to be stored in a file
            inOrderTraversal(root, false, NULL);
        }
    } else {
        while (fgets(currentLine, 101, inputFile) != NULL) {
            currentLineTemp = strtok(currentLine, "\n");
            int i = 0;
            for (; currentLineTemp[i]; i++) {
                currentLineTemp[i] = tolower(currentLineTemp[i]);
            }
            if (root == NULL) {
                root = insertIntoNode(root, currentLineTemp);
            }
            else {
                insertIntoNode(root, currentLineTemp);
            }
        }
        if (oFlag != 1) { // no output if the user wants output to be stored in a file
            inOrderTraversal(root, false, NULL);
        }
    }

    if (oFlag == 1) {
        outputFile = fopen(outputFileName, "w");
        if (outputFile == NULL) {
            fprintf(stderr, "Cannot create file \"%s\"!\n", outputFileName);
            exit(1);
        }
        inOrderTraversal(root, true, outputFile);
        fclose(outputFile);
    }

    postOrderDelete(root);
    fclose(inputFile);

    return 0;
}