#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "../Unicode/unicode.h"

#define TRUE 1
#define FALSE 0

#define TYPE_Null 0
#define TYPE_True 1
#define TYPE_False 2
#define TYPE_Long 4
#define TYPE_Double 8
#define TYPE_String 16
#define TYPE_Element 32
#define TYPE_Object 64
#define TYPE_Array 128

int error;
char * str;
char quotes;
int num_tabs;

typedef struct Element {
	int type;
	void * value;
	struct Element * next;
} Element;

typedef struct Pair {
	char * key;
	Element * element;
	struct Pair * next;
} Pair;

typedef struct Object {
	Pair * pair;
	struct Object * next;
} Object;

Object * jInit();

int advanceptr();
int object(Object *);
int members(Pair *);
int pair(Pair *);
int string();
int value(Element *);
int isHex(char);
int value_true(Element *);
int value_false(Element *);
int value_null(Element *);
int number();
int value_int();
int digits();
int array(Element *);
int elements(Element *);

void jDestroy(Object *);
void jDestroyObject(Object *);
void jDestroyPair(Pair *);
void jDestroyElement(Element *);

void jString(FILE *, Object *);
void jStringObject(FILE *, Object *);
void jStringPair(FILE *, Pair *);
void jStringElement(FILE *, Element *);
void jStringAddTabs(FILE *);
