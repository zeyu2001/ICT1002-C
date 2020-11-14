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
#include <stdbool.h>
#include "chat1002.h"

/*
 * Get the root of the relevant BST, given the intent.
 * 
 * Input:
 * 	 intent		- the question word
 * 
 * Returns:
 * 	 the root of the BST corresponding to the intent, if valid
 *   NULL, if 'intent' is not a recognised question word
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
 * Get the head of the relevant linked list, given the intent.
 * 
 * Input:
 * 	 intent		- the question word
 * 
 * Returns:
 * 	 the root of the BST corresponding to the intent, if valid
 *   NULL, if 'intent' is not a recognised question word
 */
LIST_NODE **get_head(const char *intent)
{
	LIST_NODE **head;

	if (compare_token(intent, "WHERE") == 0)
	{	
		head = &WHERE_head;
	}
	else if (compare_token(intent, "WHAT") == 0)
	{
		head = &WHAT_head;
	}
	else if (compare_token(intent, "WHO") == 0)
	{
		head = &WHO_head;
	}
	else
	{
		// Not a valid question word
		head = NULL;
	}
	return head;
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

	// Closest match found
	else if (get_ascii_difference(entity, node->entity) > 0)
	{
		char answer[MAX_INPUT];
		prompt_user(answer, MAX_INPUT, "Sorry, I don't know about %s. Did you mean %s? (yes/no)", entity, node->entity);
		
		// User accepts closest match
		if (compare_token(answer, "yes") == 0 || compare_token(answer, "y") == 0)
		{
			snprintf(response, MAX_RESPONSE, "%s", node->response);
			return KB_CLOSESTMATCH;
		}
		// User does not accept closest match
		else if (compare_token(answer, "no") == 0 || compare_token(answer, "n") == 0)
		{
			// Calling function will prompt the user to provide knowledge
			return KB_NOTFOUND;
		}
		// Invalid input
		else 
		{
			snprintf(response, MAX_RESPONSE, "I dont understand '%s'", answer);

			// Ends the current transaction
			return KB_CLOSESTMATCH;
		}
	}
	// Found
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
 *   KB_OK, if successful
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

		// Memory allocation failure
		if (*root == NULL)
		{
			return KB_NOMEM;
		}
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
 *   f 				- the file
 *	 response 		- the response buffer for error reporting
 *
 * Returns: 
 * 	 the number of entity/response pairs successful read from the file,
 * 	 or KB_NOMEM if there was a memory allocation failure
 */
int knowledge_read(FILE *f) {
	
	// Assume that no line of the file is longer than 
	// MAX_ENTITY + 1 + MAX_RESPONSE + 1 characters

	int buff_size = MAX_ENTITY + 1 + MAX_RESPONSE + 1;

	char line[buff_size];
	int line_length;

	int count = 0;

	char entity[MAX_ENTITY];
	char response[MAX_RESPONSE];
	int status;

	// The longest intent is WHERE, which is 5 characters long
	char section_name[6] = "";
	bool valid_section = false;

	// Read from file
	while (fgets(line, buff_size, f) != NULL) 
	{ 
		// Account for "\r\n" at end of line
		line[strcspn(line, "\r\n")] = 0;
		line_length = strlen(line);

		// Empty line
		if (line_length == '\0' && line[0] == '\n') {
			// Ignore
        	continue;
    	}

		// Get section heading
		if (line[0] == '[' && line[line_length - 1] == ']')
		{
			snprintf(section_name, 6, "%s", strtok(line + 1, "]"));
			if (chatbot_is_question(section_name))
			{
				valid_section = true;
			}
			// Ignore anything after ']'
			continue;
		}

		// If the file contains a section heading that does not correspond 
		// to an intent understood by the chatbot, the whole section should 
		// be ignored
		if (valid_section)
		{
			// Lines that do not contain either '=' or square brackets should be ignored
			if (strchr(line, '=') == NULL)
			{
				continue;
			}

			// Extract entity from '=' delimiter
			snprintf(entity, MAX_ENTITY, "%s", strtok(line, "="));
			// Extract response after '=' and before "\r\n"
			snprintf(response, MAX_RESPONSE, "%s", strtok(NULL, "\r\n"));

			// Insert entity-response pair into linked list
			LIST_NODE **head = get_head(section_name);
			status = insert_to_list(head, entity, response);

			if (status != KB_OK)
			{
				return status;
			}

			// Count number of entity-response pairs
			count++;
		}
	} 

	// Convert linked list to balanced BST
	WHAT_root = balanced_bst(WHAT_head);
	WHO_root = balanced_bst(WHO_head);
	WHERE_root = balanced_bst(WHERE_head);

	// Linked list is no longer needed after conversion
	reset_list(WHAT_head);
	reset_list(WHO_head);
	reset_list(WHERE_head);

	WHAT_head = WHO_head = WHERE_head = NULL;

	fclose(f);

	return count;
}


/*
 * Reset the knowledge base, removing all known entities from all intents.
 */
void knowledge_reset() {
	if (WHAT_root != NULL)
	{
		reset(WHAT_root);
	}
	if (WHERE_root != NULL)
	{
		reset(WHERE_root);
	}
	if (WHO_root != NULL)
	{
		reset(WHO_root);
	}
	WHAT_root = WHERE_root = WHO_root = NULL;
}


/*
 * Write the knowledge base to a file.
 *
 * Input:
 *   f - the file
 */
void knowledge_write(FILE *f) {

	fprintf(f, "[what]\n");
	reverse_in_order_write(WHAT_root, f);

	fprintf(f, "\n[where]\n");
	reverse_in_order_write(WHERE_root, f);

	fprintf(f, "\n[who]\n");
	reverse_in_order_write(WHO_root, f);

	fclose(f);
}
