#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "chat1002.h"

/*
 * Display the list in a readable format. 
 * 
 * Input:
 *   head       - the ptr to the head of the linked list
 */
int display_list(LIST_NODE *head) 
{
   LIST_NODE *curr_ptr = head;

   printf("[ ");

   while(curr_ptr != NULL) {
      printf("(%s, %s) ", curr_ptr->entity, curr_ptr->response);
      curr_ptr = curr_ptr->next_ptr;
   }
	
   printf(" ]\n\n");

   return 0;
}

/* 
 * Insert a node into the linked list, using insertion sort.
 * 
 * Input:
 *   entity     - the entity attribute of the new node
 *   response   - the response attribute of the new node
 * 
 * Returns:
 *   KB_OK, if successful
 *   KB_NOMEM, if there was a memory allocation failure
 */
int insert_to_list(LIST_NODE **head, const char *entity, const char *response) {
   
    // Create a new node
    LIST_NODE *new_node = malloc(sizeof(LIST_NODE));

    if (new_node == NULL)
    {
        return KB_NOMEM;
    }
    
    strcpy(new_node->entity, entity);
    strcpy(new_node->response, response);

    // Perform insertion sort
    LIST_NODE *curr_ptr = *head;
    LIST_NODE *prev_ptr = NULL;
    while (curr_ptr && compare_token(entity, curr_ptr->entity) == 1)
    {
        prev_ptr = curr_ptr;
        curr_ptr = curr_ptr->next_ptr;
    }
    // Point the new node to the suitable next ptr
    new_node->next_ptr = curr_ptr;

    // Inserted at the first position in the list
    if (curr_ptr == *head)
    {
        // Update the head
        *head = new_node;
    }
    else
    {
        // Point the previous node to the new node
        prev_ptr->next_ptr = new_node;
    }
    
    return KB_OK;
}

/*
 * Convert the linked list into a balanced BST
 * 
 * Input:
 *   n              - the size of the linked list
 *   mem_error      - set to true if there is a memory allocation failure
 * 
 * Returns:
 *   the root of the balanced BST
 */
KB_NODE *convert_to_balanced_bst(LIST_NODE **head, int n, bool *mem_error) {
    if (n <= 0) 
    {
        return NULL;
    }
    
    // Recursively construct the left subtree (bottom-up)
    // Left subtree has n/2 nodes
    KB_NODE *left_subtree = convert_to_balanced_bst(head, n/2, mem_error);
    
    // Create the root node
    KB_NODE *root = create_new_node((*head)->entity, (*head)->response);
    if (root == NULL)
    {
        *mem_error = true;
    }

    // Move to next node in the linked list
    // Note that
    *head = (*head)->next_ptr;
    
    // Set left child
    root->left_child = left_subtree;
    
    // Recursively construct the right subtree (bottom up)
    // Right subtree has n (total) - n/2 (left subtree) - 1 (root) nodes
    root->right_child = convert_to_balanced_bst(head, n - n/2 - 1, mem_error);
    
    return root;
}

KB_NODE* balanced_bst(LIST_NODE *head, bool *mem_error) {
    int n = 0;
    LIST_NODE *curr_node = head;

    // Count the number of nodes in the list
    while (curr_node != NULL) {
        n++;
        curr_node = curr_node->next_ptr;
    }
    return convert_to_balanced_bst(&head, n, mem_error);
}

/*
 * Clear the contents of the Linked List
 * 
 * Input
 *   head       - the head of the list
 */
void reset_list(LIST_NODE *head) {

    if (head == NULL)
    {
        return;
    }

    LIST_NODE *curr_node = head;
    LIST_NODE *next_node = NULL;

    while (curr_node != NULL) {
        next_node = curr_node->next_ptr;
        free(curr_node);
        curr_node = next_node;
    }
}

/* 
 * Runs a series of test cases on linkedlist.c
 * 
 * NOTE: this function does not check for memory allocation failures.
 * When 'faking' mallocs for testing purposes, do not run this function.
 */
int linkedlist_tests()
{
    printf("== BEGIN linkedlist.c TESTS ==\n\n");
    LIST_NODE *WHAT_head = NULL;

    insert_to_list(&WHAT_head, "ICT1001", "Introduction to ICT.");
    insert_to_list(&WHAT_head, "ICT1002", "Programming Fundamentals.");
    insert_to_list(&WHAT_head, "ICT1004", "Web Systems and Technologies.");

    display_list(WHAT_head);

    reset_list(WHAT_head);

    printf("== END linkedlist.c TESTS ==\n\n");

    return 0;
}
