/*
 * ICT1002 (C Language) Group Project.
 *
 * This file contains the definitions and function prototypes for all of
 * features of the ICT1002 chatbot.
 */
 
#ifndef _CHAT1002_H
#define _CHAT1002_H

#include <stdio.h>

/* the maximum number of characters we expect in a line of input (including the terminating null)  */
#define MAX_INPUT    256

/* the maximum number of characters allowed in the name of an intent (including the terminating null)  */
#define MAX_INTENT   32

/* the maximum number of characters allowed in the name of an entity (including the terminating null)  */
#define MAX_ENTITY   64

/* the maximum number of characters allowed in a response (including the terminating null) */
#define MAX_RESPONSE 256

/* return codes for knowledge_get() and knowledge_put() */
#define KB_OK               0
#define KB_CLOSESTMATCH     1
#define KB_NOTFOUND        -1
#define KB_INVALID         -2
#define KB_NOMEM           -3
 
/* functions defined in main.c */
int compare_token(const char *token1, const char *token2);
void prompt_user(char *buf, int n, const char *format, ...);

/* functions defined in chatbot.c */
const char *chatbot_botname();
const char *chatbot_username();
int chatbot_main(int inc, char *inv[], char *response, int n);
int chatbot_is_exit(const char *intent);
int chatbot_do_exit(int inc, char *inv[], char *response, int n);
int chatbot_is_load(const char *intent);
int chatbot_do_load(int inc, char *inv[], char *response, int n);
int chatbot_is_question(const char *intent);
int chatbot_do_question(int inc, char *inv[], char *response, int n);
int chatbot_is_reset(const char *intent);
int chatbot_do_reset(int inc, char *inv[], char *response, int n);
int chatbot_is_save(const char *intent);
int chatbot_do_save(int inc, char *inv[], char *response, int n);
int chatbot_is_smalltalk(const char *intent);
int chatbot_do_smalltalk(int inc, char *inv[], char *resonse, int n);

char *get_entity(int inc, char *inv[]);

/* functions defined in knowledge.c */
int knowledge_get(const char *intent, const char *entity, char *response, int n);
int knowledge_put(const char *intent, const char *entity, const char *response);
void knowledge_reset();
int knowledge_read(FILE *f);
void knowledge_write(FILE *f);

/* FOR TESTING ONLY: uncomment to 'fake' malloc and test memory allocation failures */
//#define malloc(...) NULL

/* BINARY SEARCH TREE
–––––––––––––––––––––––––––––––––––––––––––––––––– */
/* BST node */
typedef struct node
{
    char entity[MAX_ENTITY];        // the entity (key for the BST)
    char response[MAX_RESPONSE];    // the response for this entity
    struct node *right_child;       // right child
    struct node *left_child;        // left child
} KB_NODE;

/* root pointers for WHERE, WHAT and WHO trees */
KB_NODE *WHERE_root;
KB_NODE *WHAT_root;
KB_NODE *WHO_root;

/* the maximum ASCII difference to accept the closest match */
#define MAX_DIFFERENCE  200

/* functions defined in bst.c */
int get_ascii_difference(const char *str1, const char *str2);
KB_NODE *search(KB_NODE *root, const char *entity);
KB_NODE *create_new_node(const char *entity, const char *response);
int insert(KB_NODE *root, const char *entity, const char *response);
int reset(KB_NODE *root);
void reverse_in_order_write(KB_NODE *root, FILE *f);
int in_order(KB_NODE *root);
int bst_tests();

void put_padding (char ch, int n);
void print_tree (struct node *root, int level);

/* LINKED LIST
–––––––––––––––––––––––––––––––––––––––––––––––––– */
typedef struct list_node
{
    char entity[MAX_ENTITY];        // the entity (key for the sorted linked list)
    char response[MAX_RESPONSE];    // the response for this entity
    struct list_node *next_ptr;     // ptr to the next node 
} LIST_NODE;

/* head pointers for WHERE, WHAT and WHO lists */
LIST_NODE *WHERE_head;
LIST_NODE *WHAT_head;
LIST_NODE *WHO_head;

/* functions defined in linkedlist.c */
int display_list(LIST_NODE *head);
int insert_to_list(LIST_NODE **head, const char *entity, const char *response);
KB_NODE *convert_to_balanced_bst(LIST_NODE **head, int n);
KB_NODE *balanced_bst(LIST_NODE *head);
void reset_list(LIST_NODE *head);
int linkedlist_tests();

#endif