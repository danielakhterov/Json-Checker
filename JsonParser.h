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

int error;
char * str;
char quotes;
int num_tabs;

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
void jDestroyAllObjects(Object *);
void jDestroyObject(Object *);
void jDestroyAllPairs(Pair *);
void jDestroyPair(Pair *);
void jDestroyAllElements(Element *);
void jDestroyElement(Element *);

void jString(Object *, char **);
void jStringObject(Object *, char **);
void jStringPair(Pair *, char **);
void jStringElement(Element *, char **);
void jStringAddTabs(char **);

char jHasProperty(Object *, char *);
int jGetValue(Pair *, char *, Element **);
int jGetAsLong(Element *, long *);
int jGetAsDouble(Element *, double *);
int jGetAsString(Element *, char **);
int jGetAsBool(Element *, char *);
int jGetAsObject(Element *, Object **);
int jGetElementFromArray(Element *, Element **, int);

int jAddPair(Pair **, char *, Element *);
int jAddElement(Element **, Element * ne, int n);
int jRemovePair(Pair **, char *);
int jRemoveElement(Element **, int n);

int jLongArrayToList(Element **, long *, size_t);
int jDoubleArrayToList(Element **, double *, size_t);

int strcat_realloc(char **, char *);
