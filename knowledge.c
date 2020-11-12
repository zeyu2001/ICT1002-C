/*
 * ICT1002 (C Language) Group Project.
 *
 * This file implements the chatbot's knowledge base.
 *
 * knowledge_get() retrieves the response to a question.
 * knowledge_put() inserts a new response to a question.
 * knowledge_read() reads the knowledge base from a file.
 * knowledge_reset() erases all of the knowledge.
 * knowledge_write() saves the knowledge base in a file.
 *
 * You may add helper functions as necessary.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "chat1002.h"

/*
 * Get the root of the relevant BST, given the intent.
 * 
 * Input:
 * 	 intent		- the question word
 * 
 * Returns:
 * 	 the root of the BST corresponding to the intent, if valid
 *   KB_INVALID, if 'intent' is not a recognised question word
 */
KB_NODE **get_root(const char *intent)
{
	KB_NODE **root;

	if (compare_token(intent, "WHERE") == 0)
	{	
		root = &WHERE_root;
	}
	else if (compare_token(intent, "WHAT") == 0)
	{
		root = &WHAT_root;
	}
	else if (compare_token(intent, "WHO") == 0)
	{
		root = &WHO_root;
	}
	else
	{
		// Not a valid question word
		root = NULL;
	}
	return root;
}


/*
 * Get the response to a question.
 *
 * Input:
 *   intent   - the question word
 *   entity   - the entity
 *   response - a buffer to receive the response
 *   n        - the maximum number of characters to write to the response buffer
 *
 * Returns:
 *   KB_OK, if a response was found for the intent and entity (the response is copied to the response buffer)
 *   KB_CLOSESTMATCH, if the entity is not found, but a closest match is found
 *   KB_NOTFOUND, if no suitable response could be found
 *   KB_INVALID, if 'intent' is not a recognised question word
 */
int knowledge_get(const char *intent, const char *entity, char *response, int n) {

	/* Identify the intent */
	KB_NODE **root = get_root(intent);

	// Not a valid question word
	if (root == NULL)
	{
		return KB_INVALID;
	}

	KB_NODE *node = search(*root, entity);

	// Not found
	if (node == NULL)
	{
		return KB_NOTFOUND;
	}

	// Closest match
	else if (get_ascii_difference(entity, node->entity) > 0)
	{
		char answer[MAX_INPUT];
		prompt_user(answer, MAX_INPUT, "Sorry, I don't know %s is %s. Did you mean %s?", intent, entity, node->entity);
		
		// User accepts closest match
		if (compare_token(answer, "yes") == 0 || compare_token(answer, "y") == 0)
		{
			snprintf(response, MAX_RESPONSE, "%s", node->response);
			return KB_CLOSESTMATCH;
		}
		// User does not accept closest match
		else
		{
			return KB_NOTFOUND;
		}
	}
	else
	{
		snprintf(response, MAX_RESPONSE, "%s", node->response);
		return KB_OK;
	}
}


/*
 * Insert a new response to a question. If a response already exists for the
 * given intent and entity, it will be overwritten. Otherwise, it will be added
 * to the knowledge base.
 *
 * Input:
 *   intent    - the question word
 *   entity    - the entity
 *   response  - the response for this question and entity
 *
 * Returns:
 *   KB_FOUND, if successful
 *   KB_NOMEM, if there was a memory allocation failure
 *   KB_INVALID, if the intent is not a valid question word
 */
int knowledge_put(const char *intent, const char *entity, const char *response) {

	/* Identify the intent */
	KB_NODE **root = get_root(intent);

	// Not a valid question word
	if (root == NULL)
	{
		return KB_INVALID;
	}

	// Empty tree (set root to new node)
	if (*root == NULL)
	{
		*root = create_new_node(entity, response);
	}

	// Non-empty tree (insert new node into BST)
	else
	{
		int status = insert(*root, entity, response);	
		return status;
	}
	return KB_OK;
}


/*
 * Read a knowledge base from a file.
 *
 * Input:
 *   f - the file
 *
 * Returns: the number of entity/response pairs successful read from the file
 */
int knowledge_read(FILE *f) {

	/* to be implemented */

	return 0;
}


/*
 * Reset the knowledge base, removing all know entitities from all intents.
 */
void knowledge_reset() {

	/* to be implemented */

}


/*
 * Write the knowledge base to a file.
 *
 * Input:
 *   f - the file
 */
void knowledge_write(FILE *f) {

	/* to be implemented */

}
