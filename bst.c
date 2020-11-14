#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "chat1002.h"


/*
 * Find the absolute ASCII difference between two strings. 
 * Used for finding the closest match in the BST if a match is not found.
 * 
 * Input:
 *   str1       - the first string
 *   str2       - the second string
 * 
 * Returns:
 *   the absolute ASCII difference between str1 and str2.
 */
int get_ascii_difference(const char *str1, const char *str2)
{
    int difference = 0;

    int i = 0;
    int j = 0;

    while (str1[i] != '\0' && str2[j] != '\0')
    {
        if (str1[i] != str2[j])
        {
            // Add ASCII differece if characters are different
            difference += abs(tolower(str1[i]) - tolower(str2[i]));
        }

        i++;
        j++;
    }

    // Now only either str1 or str2 still has characters left to read.
    // Add the remaining characters as ASCII difference.

    while (str1[i] != '\0')
    {
        difference += (int) tolower(str1[i++]);
    }

    while (str2[j] != '\0')
    {
        difference += (int) tolower(str2[j++]);
    }

    return difference;
}


/* 
 * Iteratively searches for the node with <entity>. If <entity> is not found, 
 * the closest node (in terms of absolute difference in ASCII value) is the result.
 * 
 * Input:
 *   root       - the root of the BST
 *   entity     - the entity to search for
 * 
 * Returns:
 *   the pointer to the node, if found
 *   the pointer to the closest node to <entity>, if not found.
 *   NULL, if not found
 */
KB_NODE *search(KB_NODE *root, const char *entity)
{

    KB_NODE *closest_so_far = NULL;
    int difference_so_far;
    int curr_diff;

    bool done = false;
    while (!done)
    {
        // Not found
        if (root == NULL)
        {
            done = true;
            if (difference_so_far < MAX_DIFFERENCE)
            {
                root = closest_so_far;
            }
        }
        // Found
        else if (compare_token(entity, root->entity) == 0)
        {
            done = true;
        }
        // Greater than (traverse to right subtree)
        else if (compare_token(entity, root->entity) == 1)
        {
            curr_diff = get_ascii_difference(entity, root->entity);
            if (closest_so_far == NULL || curr_diff < difference_so_far)
            {
                difference_so_far = curr_diff;
                closest_so_far = root;
            }
            root = root->right_child;
        }
        // Less than (traverse to left subtree)
        else
        {
            curr_diff = get_ascii_difference(entity, root->entity);
            if (closest_so_far == NULL || curr_diff < difference_so_far)
            {
                difference_so_far = curr_diff;
                closest_so_far = root;
            }
            root = root->left_child;
        }
    }
    return root;
}

/* 
 * Creates a new node, and returns its pointer.
 * 
 * Input:
 *   entity     - the entity attribute of the new node
 *   response   - the response attribute of the new node
 * 
 * Returns:
 *   the pointer to the new node, if successful
 *   NULL, if unsuccessful
 */
KB_NODE *create_new_node(const char *entity, const char *response)
{
    KB_NODE *new_node;
    new_node = malloc(sizeof(struct node));
     
    // Memory allocation failure
    if (new_node == NULL)
    {
        return NULL;
    }

    /* Set the entity and response attributes */
    strcpy(new_node->entity, entity);
    strcpy(new_node->response, response);

    /* New nodes are always leaves */
    new_node->left_child = NULL;
    new_node->right_child = NULL;

    return new_node;
}

/* 
 * Inserts a new node with <entity> and <response> to the BST.
 * 
 * Input:
 *   entity     - the entity attribute of the new node
 *   response   - the response attribute of the new node
 * 
 * Returns:
 *   KB_OK, if successful
 *   KB_NOMEM, if there was a memory allocation failure
 */
int insert(KB_NODE *root, const char *entity, const char *response)
{
    int status = KB_OK;
    bool done = false;
    while (!done)
    {
        // Equal (update the response)
        if (compare_token(entity, root->entity) == 0)
        {
            strcpy(root->response, response);
            done = true;
        }

        // Greater than (traverse to right subtree)
        else if (compare_token(entity, root->entity) == 1)
        {
            // Found location to insert
            if (root->right_child == NULL)
            {
                root->right_child = create_new_node(entity, response);

                // Memory allocation error
                if (root->right_child == NULL)
                {
                    status = KB_NOMEM;
                }
                done = true;
            }

            // Continue traversing
            else
            {
                root = root->right_child;
            }
        }
        // Less than (traverse to left subtree)
        else if (compare_token(entity, root->entity) == -1)
        {
            // Found location to insert
            if (root->left_child == NULL)
            {
                root->left_child = create_new_node(entity, response);

                // Memory allocation error
                if (root->left_child == NULL)
                {
                    status = KB_NOMEM;
                }
                done = true;
            }
            
            // Continue traversing
            else
            {
                root = root->left_child;
            }
        }
    }
    return status;
}
/*
 * Recursively clear the contents of the BST
 * 
 * Input
 *   root       - the root of the BST
 */
int reset(KB_NODE *root) 
{
    // Recursively traverse left subtree
    if (root->left_child != NULL)
    {
        reset(root->left_child);
    }
    // Recursively traverse right subtree
    if (root->right_child != NULL)
    {
        reset(root->right_child);
    }

    // Deallocate the memory previously allocated to the node
    free(root);

    return 0;
}

/* 
 * Performs a recursive reverse in-order (descending order) write to file.
 * 
 * Input:
 *   f          - the file to write to
 */
void reverse_in_order_write(KB_NODE *root, FILE *f)
{
    if (root != NULL) 
        {
            reverse_in_order_write(root->right_child, f);        // visiting right child
            
            // Write entity and response
            fprintf(f, "%s=%s\n", root->entity, root->response);

            reverse_in_order_write(root->left_child, f);         // visiting left child
        }
}

/* 
 * Performs a recursive in-order traversal (for visualization and testing purposes).
 * 
 * Input:
 *   root       - the root of the BST
 */
int in_order(KB_NODE *root)
{
    if (root != NULL) 
    {
        in_order(root->left_child);         // visiting left child
        printf(" %s ", root->entity);       // printing key at root
        in_order(root->right_child);        // visiting right child
    }
    return 0;
}

/* 
 * Testing functions to print the tree to console.
 * The root is printed as the leftmost item.
 */
void put_padding(char ch, int n) {
    int i;
  
    for (i = 0; i < n; i++)
        putchar(ch) ;
}

void print_tree(struct node *root, int level) {
  
    if (root == NULL) 
    {
        put_padding('\t', level * 3);
        puts("~");
    } 
    else 
    {
        print_tree(root->right_child, level + 1);
        put_padding('\t', level * 3);
        printf("%s\n", root->entity);
        print_tree(root->left_child, level + 1);
    }
}

/* 
 * Runs a series of test cases on bst.c
 * 
 * NOTE: this function does not check for memory allocation failures.
 * When 'faking' mallocs for testing purposes, do not run this function.
 */
int bst_tests()
{
    printf("== BEGIN bst.c TESTS ==\n\n");
    /*
                        ICT1003
                        /     \
                       /       \
                      /         \
                  ICT1001     ICT1004
                     \         /    \
                      \       /      \
                   ICT1002  ICT1005  SIT
    */
    WHAT_root = create_new_node("ICT1003", "Computer Organisation and Architecture.");
    insert(WHAT_root, "ICT1001", "Introduction to ICT.");
    insert(WHAT_root, "ICT1002", "Programming Fundamentals.");
    insert(WHAT_root, "ICT1004", "Web Systems and Technologies.");
    insert(WHAT_root, "SIT", "SIT is an autonomous university in Singapore.");
    insert(WHAT_root, "ICT1005", "Mathematics and Statistics for ICT.");
    
    printf(" -- In-order Traversal (WHAT):");
    in_order(WHAT_root);
    printf("-- \n\n");

    KB_NODE *WHAT_ICT1004 = search(WHAT_root, "ICT1004");
    printf("Entity: %s, Response: %s\n", WHAT_ICT1004->entity, WHAT_ICT1004->response);

    KB_NODE *WHAT_SIT = search(WHAT_root, "SIT");
    printf("Entity: %s, Response: %s\n\n", WHAT_SIT->entity, WHAT_SIT->response);

    WHO_root = create_new_node("Frank Guan", "Frank teaches the C section of ICT1002.");
    insert(WHO_root, "Wang Zhengkui", "Zhengkui teaches the Python section of ICT1002.");
    
    printf(" -- In-order Traversal (WHO):");
    in_order(WHO_root);
    printf("-- \n\n");

    KB_NODE *WHO_Frank = search(WHO_root, "Frank Guan");
    printf("Entity: %s\nResponse: %s\n\n", WHO_Frank->entity, WHO_Frank->response);

    printf("RESET\n\n");
    knowledge_reset();

    in_order(WHO_root);
    in_order(WHAT_root);

    printf("LOADING sample.sorted.ini\n");
    knowledge_read(fopen("sample.sorted.ini", "r"));
    printf("LOADED sample.sorted.ini\n\n");

    printf(" \n-- WHAT TREE\n");
    print_tree(WHAT_root, 0);

    printf(" \n-- WHO TREE\n");
	print_tree(WHO_root, 0);

    printf(" \n-- WHERE TREE\n");
	print_tree(WHERE_root, 0);

    printf("\n -- In-order Traversal (WHO):");
    in_order(WHO_root);
    printf("-- \n\n");

    printf(" -- In-order Traversal (WHAT):");
    in_order(WHAT_root);
    printf("-- \n\n");

    printf(" -- In-order Traversal (WHERE):");
    in_order(WHERE_root);
    printf(" -- \n\n");

    printf("RESET\n\n");
    knowledge_reset();

    printf("== END bst.c TESTS ==\n\n");
    return 0;
}