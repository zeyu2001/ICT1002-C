/*
 * ICT1002 (C Language) Group Project.
 *
 * This file implements the behaviour of the chatbot. The main entry point to
 * this module is the chatbot_main() function, which identifies the intent
 * using the chatbot_is_*() functions then invokes the matching chatbot_do_*()
 * function to carry out the intent.
 *
 * chatbot_main() and chatbot_do_*() have the same method signature, which
 * works as described here.
 *
 * Input parameters:
 *   inc      - the number of words in the question
 *   inv      - an array of pointers to each word in the question
 *   response - a buffer to receive the response
 *   n        - the size of the response buffer
 *
 * The first word indicates the intent. If the intent is not recognised, the
 * chatbot should respond with "I do not understand [intent]." or similar, and
 * ignore the rest of the input.
 *
 * If the second word may be a part of speech that makes sense for the intent.
 *    - for WHAT, WHERE and WHO, it may be "is" or "are".
 *    - for SAVE, it may be "as" or "to".
 *    - for LOAD, it may be "from".
 * The word is otherwise ignored and may be omitted.
 *
 * The remainder of the input (including the second word, if it is not one of the
 * above) is the entity.
 *
 * The chatbot's answer should be stored in the output buffer, and be no longer
 * than n characters long (you can use snprintf() to do this). The contents of
 * this buffer will be printed by the main loop.
 *
 * The behaviour of the other functions is described individually in a comment
 * immediately before the function declaration.
 *
 * You can rename the chatbot and the user by changing chatbot_botname() and
 * chatbot_username(), respectively. The main loop will print the strings
 * returned by these functions at the start of each line.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "chat1002.h"


/*
 * Get the name of the chatbot.
 *
 * Returns: the name of the chatbot as a null-terminated string
 */
const char *chatbot_botname() {

	return "Marc";

}


/*
 * Get the name of the user.
 *
 * Returns: the name of the user as a null-terminated string
 */
const char *chatbot_username() {

	return "User";

}


/*
 * Get a response to user input.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop (i.e. it detected the EXIT intent)
 */
int chatbot_main(int inc, char *inv[], char *response, int n) {

	/* check for empty input */
	if (inc < 1) {

		int chosen_resp = rand() % 5;

		switch(chosen_resp) {
			case 0:
				snprintf(response, n, "Awkward...");
				break;
			case 1:
				snprintf(response, n, "Try asking me to tell you a riddle.");
				break;
			case 2:
				snprintf(response, n, "Try asking me a question.");
				break;
			case 3:
				snprintf(response, n, "Try asking me to tell you a joke.");
				break;
		    case 4:
		        snprintf(response, n, "Try asking me to tell you a fact.");
		        break;
		}
		return 0;
	}

	/* look for an intent and invoke the corresponding do_* function */
	if (chatbot_is_exit(inv[0]))
		return chatbot_do_exit(inc, inv, response, n);
	else if (chatbot_is_smalltalk(inv[0]))
		return chatbot_do_smalltalk(inc, inv, response, n);
	else if (chatbot_is_load(inv[0]))
		return chatbot_do_load(inc, inv, response, n);
	else if (chatbot_is_question(inv[0]))
		return chatbot_do_question(inc, inv, response, n);
	else if (chatbot_is_reset(inv[0]))
		return chatbot_do_reset(inc, inv, response, n);
	else if (chatbot_is_save(inv[0]))
		return chatbot_do_save(inc, inv, response, n);
	else {
		snprintf(response, n, "I don't understand \"%s\".", inv[0]);
		return 0;
	}

}


/*
 * Determine whether an intent is EXIT.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "exit" or "quit"
 *  0, otherwise
 */
int chatbot_is_exit(const char *intent) {

	return compare_token(intent, "exit") == 0 || compare_token(intent, "quit") == 0;

}


/*
 * Perform the EXIT intent.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   1 (the chatbot stops chatting after the intent is "exit" or "quit")
 */
int chatbot_do_exit(int inc, char *inv[], char *response, int n) {

	snprintf(response, n, "Goodbye!");

	return 1;

}


/*
 * Determine whether an intent is LOAD.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "load"
 *  0, otherwise
 */
int chatbot_is_load(const char *intent) {

	return compare_token(intent, "load") == 0;

}


/*
 * Load a chatbot's knowledge base from a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after loading knowledge)
 */
int chatbot_do_load(int inc, char *inv[], char *response, int n) {

	FILE *in_file;
	char *filename;

	if (inc >= 3 && compare_token(inv[1], "from") == 0)
	{
		filename = inv[2];
		in_file = fopen(inv[2], "r");
	}
	else if (inc >= 2)
	{
		filename = inv[1];
		in_file = fopen(inv[1], "r");
	}
	else
	{
		snprintf(response, MAX_RESPONSE, "Please enter a filename.");
	}
	

	if (in_file == NULL)
	{
		snprintf(response, MAX_RESPONSE, "File '%s' does not exist.", filename);
	}
	else
	{
		int num_responses = knowledge_read(in_file);

		// Note that error codes are -ve, so this will not conflict with normal return values which are +ve
		if (num_responses == KB_NOMEM)
		{
			snprintf(response, MAX_RESPONSE, "Memory allocation failure.");
		}
		
		// Successful
		else
		{
			snprintf(response, MAX_RESPONSE, "Read %d responses from %s.", num_responses, filename);
		}
	}

	return 0;

}


/*
 * Determine whether an intent is a question.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "what", "where", or "who"
 *  0, otherwise
 */
int chatbot_is_question(const char *intent) {

	return compare_token(intent, "what") == 0 || compare_token(intent, "where") == 0 || compare_token(intent, "who") == 0;

}

/* 
 * From inv, get the entity. inv[1] may contain "is" or "are"; if so, it is skipped.
 * The remainder of the words form the entity.
 * 
 * Returns
 * 	 the entity, as a char array, if valid input
 * 	 NULL, if invalid input
 */
char *get_entity(int inc, char *inv[])
{
	int i;

	/* Craft Entity */
	static char entity[MAX_ENTITY] = "";

	// Only include inv[1] if it is not "is" or "are"
	if (inc >= 2 && compare_token(inv[1], "is") != 0 && compare_token(inv[1], "are") != 0)
	{
		strcpy(entity, inv[1]);
		i = 2;
	}
	// Exclude inv[1] otherwise
	else if (inc >= 3)
	{
		strcpy(entity, inv[2]);
		i = 3;
	}
	// Invalid input, expected an entity
	else
	{
		return NULL;
	}
	
	while (i < inc)
	{
		strcat(entity, " ");
		strcat(entity, inv[i]);
		i++;
	}
	return entity;
}

/*
 * Answer a question.
 *
 * inv[0] contains the the question word.
 * inv[1] may contain "is" or "are"; if so, it is skipped.
 * The remainder of the words form the entity.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after a question)
 */
int chatbot_do_question(int inc, char *inv[], char *response, int n) {

	char *entity = get_entity(inc, inv);
	if (entity == NULL)
	{
		snprintf(response, MAX_RESPONSE, "Please provide an entity.");
		return 0;
	}

	// Craft question: 
	// I don't know. INTENT is/are ENTITY?
	char question[MAX_RESPONSE];
	strcpy(question, "I don't know. ");
	strcat(question, inv[0]);

	// Include 'is' / 'are' when reflecting the question back to the user
	if (compare_token(inv[1], "is") == 0 || compare_token(inv[1], "are") == 0)
	{
		strcat(question, " ");
		strcat(question, inv[1]);
	}

	strcat(question, " ");
	strcat(question, entity);
	strcat(question, "?");

	int status = knowledge_get(inv[0], entity, response, MAX_RESPONSE);
	if (status == KB_INVALID)
	{
		snprintf(response, MAX_RESPONSE, "%s", "Invalid question.");
	}
	else if (status == KB_NOTFOUND)
	{
		char input[MAX_INPUT];
		prompt_user(input, MAX_INPUT, "%s", question);
		
		status = knowledge_put(inv[0], entity, input);

		if (status == KB_NOMEM)
		{
			snprintf(response, MAX_RESPONSE, "%s", "Memory allocation failure.");
		}
		else
		{
			snprintf(response, MAX_RESPONSE, "%s", "Thank you.");
		}
	}
	return 0;
}


/*
 * Determine whether an intent is RESET.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "reset"
 *  0, otherwise
 */
int chatbot_is_reset(const char *intent) {

	return compare_token(intent, "reset") == 0;

}


/*
 * Reset the chatbot.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after reset)
 */
int chatbot_do_reset(int inc, char *inv[], char *response, int n) {

	knowledge_reset();
	snprintf(response, MAX_RESPONSE, "%s", "Reset successful.");

	return 0;

}


/*
 * Determine whether an intent is SAVE.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "what", "where", or "who"
 *  0, otherwise
 */
int chatbot_is_save(const char *intent) {

	return compare_token(intent, "save") == 0;

}


/*
 * Save the chatbot's knowledge to a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after saving knowledge)
 */
int chatbot_do_save(int inc, char *inv[], char *response, int n) {

	FILE *out_file;
	char *filename;

	if (inc >= 3 && (compare_token(inv[1], "to") == 0 || compare_token(inv[1], "as") == 0))
	{
		filename = inv[2];
		out_file = fopen(inv[2], "w");
	}
	else if (inc >= 2)
	{
		filename = inv[1];
		out_file = fopen(inv[1], "w");
	}
	else
	{
		snprintf(response, MAX_RESPONSE, "Please enter a filename.");
	}

	knowledge_write(out_file);

	snprintf(response, MAX_RESPONSE, "My knowledge has been saved to %s.", filename);

	return 0;

}


/*
 * Determine which an intent is smalltalk.
 *
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is the first word of one of the smalltalk phrases
 *  0, otherwise
 */
int chatbot_is_smalltalk(const char *intent) {

	return compare_token("Hello", intent) == 0 ||
		compare_token("It's", intent) == 0 ||
		compare_token("I'm", intent) == 0 ||
		compare_token("You're", intent) == 0 ||
		compare_token("Yes", intent) == 0 ||
		compare_token("No", intent) == 0 ||
		compare_token("Good", intent) == 0 ||
		compare_token("Goodbye", intent) == 0 ||
		compare_token("Tell", intent) == 0;
}

/*
 * Reflects the user's message back at them. 
 */

int get_reflection(char *reflection, char *inv[], int inc)
{
	if (inc >= 2)
		{
			strcpy(reflection, inv[1]);
		}
		for (int i = 2; i < inc; i++)
		{
			strcat(reflection, " ");

			// Speak from perspective of the chatbot
			if (compare_token(inv[i], "am") == 0) { strcat(reflection, "are"); } 
			else if (compare_token(inv[i], "was") == 0) { strcat(reflection, "were"); } 
			else if (compare_token(inv[i], "i") == 0) { strcat(reflection, "you"); } 
			else if (compare_token(inv[i], "i'd") == 0) { strcat(reflection, "you'd"); } 
			else if (compare_token(inv[i], "i've") == 0) { strcat(reflection, "you've"); } 
			else if (compare_token(inv[i], "i'll") == 0) { strcat(reflection, "you'll"); } 
			else if (compare_token(inv[i], "my") == 0) { strcat(reflection, "your"); } 
			else if (compare_token(inv[i], "are") == 0) { strcat(reflection, "am"); } 
			else if (compare_token(inv[i], "you've") == 0) { strcat(reflection, "I've"); } 
			else if (compare_token(inv[i], "you'll") == 0) { strcat(reflection, "I'll"); } 
			else if (compare_token(inv[i], "your") == 0) { strcat(reflection, "my"); } 
			else if (compare_token(inv[i], "yours") == 0) { strcat(reflection, "mine"); } 
			else if (compare_token(inv[i], "you") == 0) { strcat(reflection, "me"); } 
			else if (compare_token(inv[i], "me") == 0) { strcat(reflection, "you"); } 
			else
			{
				strcat(reflection, inv[i]);
			}
			
		}
	return 0;
}

/*
 * Respond to smalltalk.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop chatting (e.g. the smalltalk was "goodbye" etc.)
 */
int chatbot_do_smalltalk(int inc, char *inv[], char *response, int n) {

	if (compare_token("Hello", inv[0]) == 0) {
		snprintf(response, n, "Hellooooooooo :)");

	} else if (compare_token("It's", inv[0]) == 0){
		int chosen_resp = rand() % 4;
		char reflection[MAX_ENTITY];
		
		get_reflection(reflection, inv, inc);

		switch(chosen_resp) {
			case 0:
				snprintf(response, n, "%s indeed!", reflection);
				break;
			case 1:
				snprintf(response, n, "If I told you that it probably isn't %s, what would you feel?", reflection);
				break;
			case 2:
				snprintf(response, n, "It could well be that it's %s.", reflection);
				break;
			case 3:
				snprintf(response, n, "You seem very certain.");
				break;
		}

	} else if (compare_token("I'm", inv[0]) == 0){
		int chosen_resp = rand() % 4;
		char reflection[MAX_ENTITY];
		
		get_reflection(reflection, inv, inc);

		switch(chosen_resp) {
			case 0:
				snprintf(response, n, "How does being %s make you feel?", reflection);
				break;
			case 1:
				snprintf(response, n, "Do you enjoy being %s?", reflection);
				break;
			case 2:
				snprintf(response, n, "Why do you tell me you’re %s?", reflection);
				break;
			case 3:
				snprintf(response, n, "Why do you think you’re %s?", reflection);
				break;
		}

	} else if (compare_token("You're", inv[0]) == 0){
		int chosen_resp = rand() % 4;
		char reflection[MAX_ENTITY];
		
		get_reflection(reflection, inv, inc);

		switch(chosen_resp) {
			case 0:
				snprintf(response, n, "Why do you think I am %s?", reflection);
				break;
			case 1:
				snprintf(response, n, "Does it please you to think that I’m %s?", reflection);
				break;
			case 2:
				snprintf(response, n, "Perhaps you would like me to be %s.", reflection);
				break;
			case 3:
				snprintf(response, n, "Are we talking about you, or me?");
				break;
		}

	} else if (compare_token("Yes", inv[0]) == 0){

		int chosen_resp = rand() % 4;

		switch(chosen_resp) {
			case 0:
				snprintf(response, n, "You seem quite sure.");
				break;
			case 1:
				snprintf(response, n, "I think so too!");
				break;
			case 2:
				snprintf(response, n, "Indeed.");
				break;
			case 3:
				snprintf(response, n, "I see.");
				break;
		}

	} else if (compare_token("No", inv[0]) == 0){

		int chosen_resp = rand() % 4;

		switch(chosen_resp) {
			case 0:
				snprintf(response, n, "You seem quite sure.");
				break;
			case 1:
				snprintf(response, n, "Oh.");
				break;
			case 2:
				snprintf(response, n, "I thought so too.");
				break;
			case 3:
				snprintf(response, n, "I see.");
				break;
		}

	} else if (compare_token("Good", inv[0]) == 0){

		char reflection[MAX_ENTITY];
		get_reflection(reflection, inv, inc);

		snprintf(response, n, "Excellent %s", reflection);

	} else if (compare_token("Goodbye", inv[0]) == 0){
		snprintf(response, n, "Have an excellent day!");\
		return 1;

	} else if (compare_token("Tell", inv[0]) == 0){
		int chosen_resp = rand() % 4;
		char answer[MAX_INPUT];
		memset(answer, 0, MAX_INPUT);
		if (compare_token("riddle", inv[inc-1]) == 0) {
			switch(chosen_resp) {
				case 0:
					prompt_user(answer, MAX_INPUT, "When is a door not a door?");
					if (compare_token("When it is a jar", answer) != 0) {
						snprintf(response, n, "Actually... the answer was 'when it is a jar'. HAHA!!!!!");
					} else {
						snprintf(response, n, "You are right! Excellent job!");
					}
					break;
				case 1:
					prompt_user(answer, MAX_INPUT, "Not chest or box is now discussed. Money can be held in it, but just as we test its metal, within it there is rust?");
					if (compare_token("Trust", answer) != 0) {
						snprintf(response, n, "Actually... the answer was 'trust'. HAHA!!!!!");
					} else {
						snprintf(response, n, "You are right! Excellent job!");
					}
					break;
				case 2:
					prompt_user(answer, MAX_INPUT, "The more you take, the more you leave behind. What am I?");
					if (compare_token("Steps", answer) != 0) {
						snprintf(response, n, "Actually... the answer was 'Footsteps'. HAHA!!!!!");
					} else
					{
						snprintf(response, n, "You are right! Excellent job!");
					}
					break;
				case 3:
					prompt_user(answer, MAX_INPUT, "What belongs to you, but other people use it more than you?");
					if (compare_token("Name", answer) != 0) {
						snprintf(response, n, "Actually... the answer was 'Your name'. HAHA!!!!!");
					} else
					{
						snprintf(response, n, "You are right! Excellent job!");
					}
					break;
										
			}
		} else if (compare_token("joke", inv[inc-1]) == 0) {
			switch(chosen_resp) {
				case 0:
					snprintf(response, n, "I don't like comic books, they have too many issues.");
					break;
				case 1:
					snprintf(response, n, "I was once asked what drove me to be a programmer. I replied, Grab.");
					break;
				case 2:
					snprintf(response, n, "What is the best thing about Switzerland? I don't know, but the flag is a big plus.");
					break;
				case 3:
					snprintf(response, n, "Why do we tell actors to break a leg? Because every play has a cast.");
			}
		} else if (compare_token("fact", inv[inc-1]) == 0) {
			switch(chosen_resp) {
				case 0:
					snprintf(response, n, "Do you know that in 1986, Apple launched a clothing line?");
					break;
				case 1:
					snprintf(response, n, "Do you know that Google rents out goats?");
					break;
				case 2:
					snprintf(response, n, "Do you know that we breathe about 20000 times a day?");
					break;
				case 3:
					snprintf(response, n, "Do you know that the first fast food restaurant is A&W?");
					break;
			}
		} else {
			snprintf(response, n, "Sorry, I can only tell you jokes, riddles or facts!");
		}
	}
	return 0;

}
