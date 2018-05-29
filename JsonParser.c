#include "JsonParser.h"

int advanceptr(){
	int cw = u_strmbw(str);

	if(cw < 0)
		return -1;
	else
		str += cw;

	if(!quotes && (*str == ' ' || *str == '\r' || *str == '\n' || *str == '\t'))
		return cw + advanceptr();
	return cw;
}
Object * jInit(){
	quotes = FALSE;
	Object * obj = malloc(sizeof(Object));
	memset(obj, 0, sizeof(Object));

	if(*str == '['){
		while(*str){
			advanceptr();
			if(object(obj) < 0){
				free(obj);
				return NULL;
			}

			if(*str != ',')
				break;
		}

		if(*str != ']'){
			free(obj);
			return NULL;
		}

		return obj;
	}
	else if(*str == '{'){
		if(object(obj) < 0){
			free(obj);
			return NULL;
		}
		return obj;
	}

	return NULL;
}
int object(Object * obj){
	printf("object()\n");
	if(*str != '{')
		return -1;

	obj->pair = malloc(sizeof(Pair));
	memset(obj->pair, 0, sizeof(Pair));

	advanceptr();

	if(members(obj->pair) < 0){
		free(obj->pair);
		return -1;
	}

	if(*str != '}'){
		free(obj->pair);
		return -1;
	}

	advanceptr();

	return 0;
}
int members(Pair * p){
	printf("members()\n");
	if(*str == '"'){
		if(pair(p) < 0)
			return -1;
	}else{
		return -1;
	}

	if(*str == ','){
		advanceptr();

		if(*str != '"'){
			free(p);
			return -1;
		}

		p->next = malloc(sizeof(Pair));
		memset(p->next, 0, sizeof(Pair));

		if(members(p->next) < 0){
			free(p->next);
			free(p);
			return -1;
		}
		return 0;
	}

	return 0;
}
int pair(Pair * p){
	printf("pair()\n");
	char * temp = str;
	char * holder;

	if(string() <= 0)
		return -1;

	p->key = malloc(str - temp);
	memset(p->key, 0, str - temp);
	memcpy(p->key, temp + 1, str - temp - 1);

	advanceptr();

	if(*str != ':'){
		free(p->key);
		return -1;
	}

	advanceptr();

	p->element = malloc(sizeof(Element));
	memset(p->element, 0, sizeof(Element));

	if(value(p->element) < 0){
		free(p->key);
		free(p->element);
		return -1;
	}

	if(p->element->type == TYPE_String)
		advanceptr();

	return 0;
}
int string(){
	printf("string()\n");
	int n = 0;

	if(*str != '"')
		return -1;

	quotes = !quotes;
	advanceptr();

	while(*str){
		if(*str != '"' && *str != '\\'){
			advanceptr();
			n++;
		}else if(*str == '\\'){
			advanceptr();
			switch(*str){
				case '"':
				case '\\':
				case '/':
				case 'f':
				case 'n':
				case 'r':
				case 't':
					advanceptr();
					n++;
					break;
				case 'u':
					advanceptr();
					for(int i = 0; i < 4; i++){
						if(!isHex(*str))
							return -1;
						advanceptr();
					}
					break;
				default: return -1;
			}
		}else{
			break;
		}
	}

	if(*str != '"')
		return -1;

	quotes = !quotes;

	return n;
}
int value(Element * element){
	printf("value()\n");
	char * temp = str;
	char * value_str;

	if(*str == '"'){
		element->type = TYPE_String;

		if(string() < 0)
			return -1;

		element->value = malloc(str - temp);
		memset(element->value, 0, str - temp);
		memcpy(element->value, temp + 1, str - temp - 1);

		return 0;

	}else if((*str >= '0' && *str <= '9') || *str == '-' || *str == '.'){
		int n = number();

		if(n < 0)
			return n;
		if(n == 0){
			printf("Long\n");
			element->type = TYPE_Long;
			element->value = malloc(sizeof(long));
			value_str = malloc((str - temp) + 1);
			memset(value_str, 0, (str - temp) + 1);
			memcpy(value_str, temp, str - temp);
			sscanf(value_str, "%ld", element->value);
		}else if(n > 0){
			printf("Double\n");
			element->type = TYPE_Double;
			element->value = malloc(sizeof(double));
			value_str = malloc((str - temp) + 1);
			memset(value_str, 0, (str - temp) + 1);
			memcpy(value_str, temp, str - temp);
			sscanf(value_str, "%lf", element->value);
		}
		return n;
	}else if(*str == '{'){
		int o = 0;
		element->type = TYPE_Object;

		element->value = malloc(sizeof(Object));
		memset(element->value, 0, sizeof(Object));

		if(object(element->value) < 0){
			free(element->value);
			return -1;
		}
		return 0;
	}else if(*str == '['){
		element->type = TYPE_Array;

		element->value = malloc(sizeof(Element));
		memset(element->value, 0, sizeof(Element));

		if(array(element->value) < 0){
			printf("Array returned -1\n");
			free(element->value);
			return -1;
		}
		return 0;
	}else if(*str == 't'){
		element->type = TYPE_True;
		return value_true(element);
	}else if(*str == 'f'){
		element->type = TYPE_False;
		return value_false(element);
	}else if(*str == 'n'){
		element->type = TYPE_Null;
		return value_null(element);
	}
	// advanceptr();
	return 0;
}
int number(){
	printf("number()\n");
	int n = 0;
	int r = 0;
	
	n = value_int();
	printf("value_int returned %d\n", n);
	if(n < 0)
		return -1;

	if(*str == '.'){
		printf("decimal found\n");
		advanceptr();
		if(digits() < 0)
			return -1;
		r = 1;
	}

	if(*str == 'e' || *str == 'E'){
		advanceptr();
		if(*str != '+' || *str != '-')
			advanceptr();

		if(digits() < 0 ){

		}
		r = 1;
	}
	return n + r;
}
int value_int(){
	printf("value_int()\n");
	if(*str == '0'){
		advanceptr();
		return 0;
	}else if(*str >= '1' && *str <= '9'){
		advanceptr();
		return 0 + digits();
	}else if(*str == '-'){
		advanceptr();
		return 0 + value_int();
	}

	return -1;
}
int digits(){
	printf("digits()\n");
	if(*str >= '0' && *str <= '9'){
		advanceptr();

		if(*str >= '0' && *str <= '9')
			return 0 + digits();

		return 0;
	}

	return 0;
}
int array(Element * element){
	printf("array()\n");

	if(*str != '[')
		return -1;
	advanceptr();

	if(elements(element) < 0)
		return -1;

	if(*str != ']')
		return -1;
	advanceptr();

	return 0;
}
int elements(Element * element){
	printf("elements()\n");
	if(*str == '"' || *str == 't' || *str == 'f' || *str == 'n' || (*str >= '0' && *str <= '9') || *str == '-' || *str == '{' || *str == '['){
		if(value(element) < 0){
			free(element->value);
			free(element->next);
			free(element);
			return -1;
		}

		if(element->type == TYPE_String)
			advanceptr();

		if(*str == ','){
			advanceptr();
			element->next = malloc(sizeof(Element));
			memset(element->next, 0, sizeof(Element));
			if(elements(element->next) < 0){
				free(element->next);
				free(element);
				return -1;
			}
		}else{
			element->next = NULL;
		}
	}

	return 0;
}
int isHex(char c){
	if((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') || (c >= '0' && c <= '9'))
		return TRUE;
	return FALSE;
}
int value_true(Element * element){
	printf("value_true()\n");
	if(*str != 't')
		return -1;
	advanceptr();

	if(*str != 'r')
		return -1;
	advanceptr();

	if(*str != 'u')
		return -1;
	advanceptr();

	if(*str != 'e')
		return -1;
	advanceptr();

	element->value = malloc(1);
	*(char *)(element->value) = TRUE;

	return 0;
}
int value_false(Element * element){
	printf("value_false()\n");
	if(*str != 'f')
		return -1;
	advanceptr();

	if(*str != 'a')
		return -1;
	advanceptr();

	if(*str != 'l')
		return -1;
	advanceptr();

	if(*str != 's')
		return -1;
	advanceptr();

	if(*str != 'e')
		return -1;
	advanceptr();

	element->value = malloc(1);
	*(char *)(element->value) = FALSE;

	return 0;
}
int value_null(Element * element){
	printf("value_null()\n");
	if(*str != 'n')
		return -1;
	advanceptr();

	if(*str != 'u')
		return -1;
	advanceptr();

	if(*str != 'l')
		return -1;
	advanceptr();

	if(*str != 'l')
		return -1;
	advanceptr();

	return 0;
}
void jDestroy(Object * obj){
	jDestroyAllObjects(obj);
}
void jDestroyAllObjects(Object * obj){
	if(!obj)
		return;

	jDestroyAllObjects(obj->next);
	jDestroyObject(obj);
}
void jDestroyObject(Object * obj){
	if(!obj)
		return;

	jDestroyAllPairs(obj->pair);
	free(obj);
}
void jDestroyAllPairs(Pair * p){
	if(!p)
		return;

	jDestroyAllPairs(p->next);
	jDestroyPair(p);
}
void jDestroyPair(Pair * p){
	if(!p)
		return;

	jDestroyAllElements(p->element);
	free(p->key);
	free(p);
}
void jDestroyAllElements(Element * element){
	if(!element)
		return;

	jDestroyAllElements(element->next);
	jDestroyElement(element);
}
void jDestroyElement(Element * element){
	if(!element)
		return;

	if(element->type == TYPE_Array)
		jDestroyAllElements((Element *)element->value);
	else
		free(element->value);
	free(element);
}
void jString(Object * obj, char ** string){
	printf("jString()\n");

	if(obj->next){
		num_tabs = 1;
		strcat_realloc(string, "[\n");

		while(obj->next){
			jStringObject(obj, string);
			obj = obj->next;
			strcat_realloc(string, ",\n");
		}

		jStringObject(obj, string);
		strcat_realloc(string, "\n]");
	}else{
		num_tabs = 0;
		jStringObject(obj, string);
	}
}
void jStringObject(Object * obj, char ** string){
	printf("jStringObject()\n");
	jStringAddTabs(string);
	strcat_realloc(string, "{\n");

	num_tabs++;
	jStringPair(obj->pair, string);

	num_tabs--;
	jStringAddTabs(string);
	strcat_realloc(string, "}");
}
void jStringPair(Pair * p, char ** string){
	printf("jStringPair()\n");
	while(p->next){
		jStringAddTabs(string);
		strcat_realloc(string, "\"");
		strcat_realloc(string, p->key);
		strcat_realloc(string, "\": ");
		jStringElement(p->element, string);
		p = p->next;
		strcat_realloc(string,  ",\n");
	}

	jStringAddTabs(string);
	strcat_realloc(string, "\"");
	strcat_realloc(string, p->key);
	strcat_realloc(string, "\": ");
	jStringElement(p->element, string);
	strcat_realloc(string,  "\n");
}
void jStringElement(Element * element, char ** string){
	printf("jStringElement\n");
	char temp[32] = {0};
	switch(element->type){
		case TYPE_Null:
			strcat_realloc(string, "null");
			break;
		case TYPE_True:
			strcat_realloc(string, "true");
			break;
		case TYPE_False:
			strcat_realloc(string, "false");
			break;
		case TYPE_Long:
			snprintf(temp, 31, "%ld", *(long *)element->value);
			strcat_realloc(string, temp);
			break;
		case TYPE_Double:
			snprintf(temp, 31, "%f", *(double *)element->value);
			strcat_realloc(string, temp);
			break;
		case TYPE_String:
			strcat_realloc(string, "\"");
			strcat_realloc(string, (char *)element->value);
			strcat_realloc(string, "\"");
			break;
		case TYPE_Element:
			break;
		case TYPE_Object:
			num_tabs++;
			jStringObject((Object *)element->value, string);
			num_tabs--;
			break;
		case TYPE_Array:
			num_tabs++;
			strcat_realloc(string, "[\n");

			element = (Element *)element->value;
			while(element->next){
				jStringAddTabs(string);
				jStringElement(element, string);
				element = element->next;
				strcat_realloc(string, ",\n");
			}
			jStringAddTabs(string);
			jStringElement(element, string);
			strcat_realloc(string, "\n");

			num_tabs--;
			jStringAddTabs(string);
			strcat_realloc(string, "]");
			break;
	}
}
void jStringAddTabs(char ** string){
	for(int i = 0; i < num_tabs; i++){
		strcat_realloc(string, "\t");
	}
}
char jHasProperty(Object * obj, char * prop){
	Pair * p = obj->pair;
	while(p){
		if(!strcmp(p->key, prop))
			return TRUE;
	}
	return FALSE;
}
int jGetValue(Pair * p, char * k, Element ** ret){
	while(p){
		if(!strcmp(p->key, k)){
			*ret = p->element;
			return 0;
		}
		p = p->next;
	}
	return -1;
}
int jGetAsLong(Element * element, long  * l){
	if(element->type != TYPE_Long)
		return -1;

	*l = *(long *)element->value;
	return 0;
}
int jGetAsDouble(Element * element, double  * d){
	if(element->type != TYPE_Double)
		return -1;

	*d = *(double *)element->value;
	return 0;
}
int jGetAsString(Element * element, char ** s){
	if(!element)
		return -1;

	if(element->type != TYPE_String)
		return -1;

	*s = (char *)element->value;
	return 0;
}
int jGetAsBool(Element * element, char * s){
	switch(element->type){
		case TYPE_False:
			*s = 0;
			break;
		case TYPE_True:
			*s = 1;
			break;
		default:
			return -1;
	}
	return 0;
}
int jGetAsObject(Element * element, Object ** obj){
	if(element->type != TYPE_Object)
		return -1;

	*obj = (Object *)element->value;
	return 0;
}
int jGetElementFromArray(Element * element, Element ** result, int n){
	if(!element)
		return -1;

	if(element->type != TYPE_Array)
		return -1;

	element = (Element *)element->value;
	int i = 0;

	while(element){
		if(i++ == n){
			*result = element;
			return 0;
		}
		element = element->next;
	}
	return -1;
}
int jAddPair(Pair ** p, char * key, Element * element){
	while(!p || !key || !element)
		return -1;

	Pair ** curr = p;
	while(curr){
		if(!strcmp((*curr)->key, key)){
			jDestroyAllElements((*curr)->element);
			(*curr)->element = element;
			return 0;
		}
		curr = &(*curr)->next;
	}
	*curr = malloc(sizeof(Pair));
	memset(*curr, 0, sizeof(Pair));
	(*curr)->key = key;
	(*curr)->element = element;
	return 0;
}
int jAddElement(Element ** element, Element * ne, int n){
	if(!(*element) || !ne)
		return -1;

	if((*element)->type != TYPE_Array)
		return -1;

	Element ** curr = (Element **)&(*element)->value;
	int i = 0;

	while(curr){
		if(i++ == n){
			Element * temp = *curr;
			*curr = ne;
			(*curr)->next = temp;
		}
		curr = &(*curr)->next;
	}
	*curr = ne;
	return 0;
}
int jRemovePair(Pair ** p, char * k){
	if(!*p)
		return -1;

	Pair ** curr = p;
	while(curr){
		if(!strcmp((*curr)->key, k)){
			Pair * temp = *curr;
			*curr = temp->next;
			jDestroyPair(temp);
			return 0;
		}
		curr = &(*curr)->next;
	}
	return -1;
}
int jRemoveElement(Element ** element, int n){
	if(!element)
		return -1;

	Element ** curr = element;
	int i = 0;

	while(curr){
		if(i++ == n){
			Element * temp = *curr;
			*curr = (*curr)->next;
			jDestroyElement(temp);
			return 0;
		}
		curr = &(*curr)->next;
	}
	return -1;
}
int jLongArrayToList(Element ** element, long * arr, size_t n){
	*element = malloc(sizeof(Element));
	memset(*element, 0, sizeof(Element));
	(*element)->type = TYPE_Array;
	
	Element ** curr = (*element)->value;
	for(int i = 0; i < n; i++){
		*curr = malloc(sizeof(Element));
		memset(*curr, 0, sizeof(Element));
		(*curr)->type = TYPE_Long;
		(*curr)->value = malloc(sizeof(long));
		*(long *)(*curr)->value = arr[i];
		curr = &(*curr)->next;
	}
}
int jDoubleArrayToList(Element ** element, double * arr, size_t n){
	*element = malloc(sizeof(Element));
	memset(*element, 0, sizeof(Element));
	(*element)->type = TYPE_Array;
	
	Element ** curr = (*element)->value;

	for(int i = 0; i < n; i++){
		*curr = malloc(sizeof(Element));
		memset(*curr, 0, sizeof(Element));
		(*curr)->type = TYPE_Double;
		(*curr)->value = malloc(sizeof(double));
		*(double *)(*curr)->value = arr[i];
		curr = &(*curr)->next;
	}
}
int strcat_realloc(char ** left, char * right){
	int lenl;
	int lenr;
	int newlen;

	if(!right)
		return 0;

	if(*left)
		lenl = strlen(*left);
	else 
		lenl = 0;

	lenr = strlen(right);
	newlen = lenl + lenr + 1;
	
	*left = realloc(*left, newlen);
	if(!*left)
		return -1;

	if(memcpy(*left + lenl, right, lenr + 1) < 0)
		return -1;

	return 0;
}
