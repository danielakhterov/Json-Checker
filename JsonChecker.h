#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "../Unicode/unicode.h"

#define TRUE 1
#define FALSE 0

#define Null 0
#define True 1
#define False 2
#define Long 4
#define Double 8
#define String 16
#define Element 32
#define Array 128

int error;

char * str;

typedef struct Element {
	int type;
	int length;
	void * value;
}

typedef struct JSON {
	struct Element * element;
	struct JSON * next;
} JSON;

void advanceptr();
int object();
int members();
int pair();
int string();
int value();
int chars();
int isHex(char);
int value_true();
int value_false();
int value_null();
int number();
int value_int();
int digits();
int array();
int elements();
