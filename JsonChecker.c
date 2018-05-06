#include "JsonChecker.h"

void advanceptr(){
	int cw = u_strmbw(str);

	if(cw < 0){
		error = -1;
		return;
	}
	else
		str += cw;
	
	// str++;
}
int json_init(){
	int o = 0;
	if(*str == '['){
		while(*str){
			advanceptr();
			o = object();
			if(o < 0)
				return -1;

			if(*str != ',')
				break;
		}

		if(*str != ']'){
			return -1;
		}

		return 0;
	}
	else if(*str == '{'){
		return object();
	}

	return -1;
}
int object(){
	printf("object()\n");
	if(*str != '{')
		return -1;

	advanceptr();
	if(members() < 0)
		return -1;

	if(*str != '}')
		return -1;

	advanceptr();

	return 0;
}
int members(){
	printf("members()\n");
	if(*str == '"'){
		int n = pair();
		// printf("pair(): %d\n", n);
		// if(pair() < 0)
		if(n < 0)
			return -1;

	}

	if(*str == ','){
		advanceptr();

		if(*str != '"')
			return -1;

		return members();
	}

	return 0;
}
int pair(){
	printf("pair()\n");
	if(string() <= 0)
		return -1;
	
	if(*str != ':')
		return -1;

	advanceptr();

	int n = value();
	// printf("n: %d\n", n);
	// if(value() <= 0)
	if(n < 0)
		return -1;

	return 0;
}
int string(){
	printf("string()\n");
	int n = 0;

	if(*str != '"')
		return -1;

	advanceptr();

	while(*str){
		// printf("%c ", *str);
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
	// printf("\n");

	if(*str != '"')
		return -1;

	advanceptr();

	return n;
}
int value(){
	printf("value()\n");
	if(*str == '"')
		return string();
	else if((*str >= '0' && *str <= '9') || *str == '-')
		return number();
	else if(*str == '{')
		return object();
	else if(*str == '[')
		return array();
	else if(*str == 't')
		return value_true();
	else if(*str == 'f')
		return value_false();
	else if(*str == 'n')
		return value_null();
	advanceptr();
	return 0;
}
int number(){
	printf("number()\n");
	int n = 0;
	
	n = value_int();
	if(n < 0)
		return -1;

	if(*str == '.'){
		advanceptr();
		int d = digits();
		// printf("d = %d\n", d);
		if(d <= 0)
			return -1;
	}

	if(*str == 'e' || *str == 'E'){
		advanceptr();
		if(*str != '+' || *str != '-')
			advanceptr();

		if(digits() <= 0 ){

		}
	}
}
int value_int(){
	printf("value_int()\n");
	if(*str == '0'){
		advanceptr();
		return 1;
	}else if(*str >= '1' && *str <= '9'){
		advanceptr();
		return 1 + digits();
	}else if(*str == '-'){
		advanceptr();
		return 1 + value_int();
	}

	return -1;
}
int digits(){
	printf("digits()\n");
	if(*str >= '0' && *str <= '9'){
		advanceptr();

		if(*str >= '0' && *str <= '9')
			return 1 + digits();

		return 1;
	}

	return 0;
}
int array(){
	printf("array()\n");
	int e = 0;

	if(*str != '[')
		return -1;
	advanceptr();

	e = elements();
	if(e < 0)
		return -1;
	
	if(*str != ']')
		return -1;
	advanceptr();

	// printf("num elements: %d\n", e);
	return e;
}
int elements(){
	printf("elements()\n");
	// printf("elements str: %c\n", *str);
	if(*str == '"' || *str == 't' || *str == 'f' || *str == 'n' || (*str >= '0' && *str <= '9') || *str == '-' || *str == '{' || *str == '['){
		value();

		if(*str == ','){
			advanceptr();
			if(!(*str == '"' || *str == 't' || *str == 'f' || *str == 'n' || (*str >= '0' && *str <= '9') || *str == '-' || *str == '{' || *str == '['))
				return -1;

			return elements();
		}

		return 0;
	}

	return 0;
}
int isHex(char c){
	if((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') || (c >= '0' && c <= '9'))
		return TRUE;
	return FALSE;
}
int value_true(){
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

	return 4;
}
int value_false(){
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

	return 5;
}
int value_null(){
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

	return 4;
}
int main(void){
	// char test[] = "{\"Hello hahah 1515 \\\" hahah \\u0eE1\":\"sup\",\"lol\":\"anotherone\",\"ff\":false}";
	// char test[] = "{\"name\":false,\"id\":\"superman\"}";
	char empty[] = "{}";
	char oneStringPair[] = "{\"name\":\"daniel\"}";
	char emptyKey[] = "{\"\":\"daniel\"}";
	char emptyValue[] = "{\"name\":\"\"}";
	char oneTruePair[] = "{\"name\":true}";
	char oneFalsePair[] = "{\"name\":false}";
	char oneNullPair[] = "{\"name\":null}";
	char oneValuelessPair[] = "{\"name\":}";
	char oneObjectPair[] = "{\"person\":{\"name\":\"daniel\",\"admin\":true}}";
	char oneIntPair[] = "{\"age\":-0.58e-5}";
	char emptyArray[] = "{\"ages\":[]}";
	char oneIntArray[] = "{\"ages\":[1,2,4]}";
	char oneMultiIntArray[] = "{\"ages\":[[1,2,4],[[125]],[55,2525]]}";
	char arrayOfObjects[] = "[{\"name\":\"daniel\"},{\"name\":\"alex\"}]";
	char arrayOfObjectsNoKey[] = "[{\"\":\"daniel\"},{\"name\":\"alex\"}]";
	char arrayOfObjectsNoValue[] = "[{\"name\":,{\"name\":\"alex\"}]";
	char arrayOfObjectsTrailingComma[] = "[{\"name\":\"daniel\"},]";
	char arrayOfObjectsMultiDArray[] = "[{\"name\":[[1,2,4],[[1,2],[3,4]],[1,2,3]]}]";
	char arrayOfEmptyObjects[] = "[{},{},{}]";
	char unicodeKey[] = "{\"name\":\"乳\"}";
	char unicodeValue[] = "{\"乳\":\"name\"}";

	// str = empty;
	// printf("empty: %d\n", json_init());

	// str = oneStringPair;
	// printf("oneStringPair: %d\n", json_init());

	// str = emptyKey;
	// printf("emptyKey: %d\n", json_init());

	// str = emptyValue;
	// printf("emptyValue: %d\n", json_init());

	// str = oneTruePair;
	// printf("oneTruePair: %d\n", json_init());

	// str = oneFalsePair;
	// printf("oneFalsePair: %d\n", json_init());

	// str = oneNullPair;
	// printf("oneNullPair: %d\n", json_init());

	// str = oneValuelessPair;
	// printf("oneValuelessPair: %d\n", json_init());

	// str = oneObjectPair;
	// printf("oneObjectPair: %d\n", json_init());

	// str = oneIntPair;
	// printf("oneIntPair: %d\n", json_init());

	// str = emptyArray;
	// printf("emptyArray: %d\n", json_init());

	// str = oneIntArray;
	// printf("oneIntArray: %d\n", json_init());

	// str = oneMultiIntArray;
	// printf("oneMultiIntArray: %d\n", json_init());

	// str = arrayOfObjects;
	// printf("arrayOfObjects: %d\n", json_init());

	// str = arrayOfObjectsNoKey;
	// printf("arrayOfObjectsNoKey: %d\n", json_init());

	// str = arrayOfObjectsNoValue;
	// printf("arrayOfObjectsNoValue: %d\n", json_init());

	// str = arrayOfObjectsTrailingComma;
	// printf("arrayOfObjectsTrailingComma: %d\n", json_init());

	// str = arrayOfObjectsMultiDArray;
	// printf("arrayOfObjectsMultiDArray: %d\n", json_init());

	// str = arrayOfEmptyObjects;
	// printf("arrayOfEmptyObjects: %d\n", json_init());

	// str = unicodeKey;
	// printf("unicodeKey: %d\n", json_init());

	// str = unicodeValue;
	// printf("unicodeValue: %d\n", json_init());

	return 0;
}
