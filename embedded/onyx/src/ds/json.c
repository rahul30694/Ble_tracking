#include "json.h"
#include "cstring.h"
#include "serial.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define isBrackets(c) ((c == '[') || (c == ']') || (c == '{') || (c == '}'))
#define isquote(c) ((c == '\'') || (c == '\"'))
#define isTokValidChar(c) (isalnum(c) || isBrackets(c) || isQuotes(c) || (c == '_'))
#define isUnderScore(c) (c == '_')
#define isWhiteSpace(c) ((c == '\n') || (c == '\r') || (c == '\t') || (c == ' '))


struct jasonParser_s{
	char *jsStr;
	int jsOffset;

	int numJSPair;
	bool parsed;
};

typedef struct {
	int start;
	int end;

	char type; //'i' => invalid, 's' => string, 'n' => primitive, 'o' => object, 'a' => array
}jsonTok_t;

typedef struct {
	jsonTok_t key;
	jsonTok_t value;
} jsonPair_t;

#define MAX_JSON_PAIR 200

struct jasonParser_s jsParser;
jsonPair_t jsPair[MAX_JSON_PAIR];

int json_init(char *jsonStr)
{
	if (!jsonStr)
		return -1;

	jsParser.parsed = false;
	jsParser.jsStr = jsonStr;

	jsParser.numJSPair = 0;
	jsParser.jsOffset = 0;
	return 0;
}

jsonPair_t *jsPair_alloc()
{
	if (jsParser.numJSPair >= MAX_JSON_PAIR)
		return NULL;

	return &(jsPair[jsParser.numJSPair++]);
}

jsonTok_t *keyTok_alloc(jsonPair_t *pair)
{
	if (!pair)
		return NULL;

	pair->key.start = pair->key.end = -1;
	pair->key.type = 'i';
	return &(pair->key);
}

jsonTok_t *valTok_alloc(jsonPair_t *pair)
{
	if (!pair)
			return NULL;

	pair->value.start = pair->value.end = -1;
	pair->value.type = 'i';
	return &(pair->value);
}

char getTokType(jsonTok_t *tok)
{
	char *js = jsParser.jsStr;
	char type = 'i';

	if (isquote(js[tok->start])) { //Remove Quotation!!
		if (js[tok->start] != js[tok->end])
			return type;
		tok->start++; tok->end--;
	}
	//Escape White Spaces!!
	while (isWhiteSpace(js[tok->start]) && tok->start <= tok->end)
		tok->start++;
	while (isWhiteSpace(js[tok->end]) && tok->start <= tok->end)
		tok->end--;
	if (tok->start >= tok->end)
		return type;

	int pos = tok->start;
	if (isalpha(js[pos]) || isUnderScore(js[pos])) { //Check for Strings
		type = 's';
		for (; pos <= tok->end; pos++) {
			if (!isalnum(js[pos]) && !isUnderScore(js[pos])) {
				type = 'i';
				break;
			}
		}
	} else if (isdigit(js[pos])) {
		bool decimal = false;
		type = 'n';
		for (; pos <= tok->end; pos++) {
			if (isdigit(js[pos]))
				continue;
			if (js[pos] == '.' && !decimal) {
				decimal = true;
				continue;
			}
			type = 'i';
			break;
		}
	}
	return type;
}

int pairValidation(jsonPair_t *pair)
{
	jsonTok_t *key = &pair->key;
	jsonTok_t *val = &pair->value;

	if ((key->type = getTokType(key)) == 'i')
		return 1;
	if (key->type != 's')
		return 1;

	if ((val->type = getTokType(val)) == 'i')
		return 1;
	return 0;
}

int jsonParseArray()
{
	return -1;
}

int jsonParseObject()
{
	jsonPair_t *pair;
	jsonTok_t *tok;

	char c;
	char keyOrValue = 'u'; //'k' => key, 'v' => value, 'u' => unknown token!
	bool validTok = false;
//	tokType =>'s' => string, 'p' => primitive, 'a' => array, 'o' => object, 'i' => invalid

	do {
		switch (c = jsParser.jsStr[jsParser.jsOffset]) {
			case '{':
				pair = jsPair_alloc();
				if (!pair)
					goto ERROR;
				tok = keyTok_alloc(pair);
				keyOrValue = 'k';
				validTok = false;
				break;

			case ',': //Should be here after processing 'value' tok
				pair = jsPair_alloc();
				if (!pair)
					goto ERROR;
				if (keyOrValue != 'v')
					goto ERROR;
				if (!validTok)
					goto ERROR;
				tok = keyTok_alloc(pair);
				keyOrValue = 'k';
				validTok = false;
				break;
			case ':': //Should be here after processing 'key' token
				if (keyOrValue != 'k')
					goto ERROR;
				if (!validTok)
					goto ERROR;
				tok = valTok_alloc(pair);
				keyOrValue = 'v';
				validTok = false;
				break;

			case '}':
				if (keyOrValue != 'v')
					goto ERROR;
				if (!validTok)
					goto ERROR;
				goto EXIT;
				break;

			case '\n': case '\r': case '\t': case ' ':
				break;

			default: //Alphabets!!
				if (!tok)
					goto ERROR;
				if (!validTok)
					tok->start = jsParser.jsOffset;
				tok->end = jsParser.jsOffset;
				validTok = true;
				break;
		}
		jsParser.jsOffset++;
	}  while (1);

	int i;
	EXIT:
	for (i = 0; i < jsParser.numJSPair; i++)
		if (pairValidation(jsPair + i))
			goto ERROR;

	jsParser.parsed = true;
	return 0;

	ERROR:
	jsParser.parsed = false;
	jsParser.jsOffset = 0;
	jsParser.numJSPair = 0;
	return -1;
}

int jsonParse()
{
	return jsonParseObject();
}

int getValAtJSKey(char *key, char *val)
{
	if (!jsParser.parsed)
		return -1;

	size_t keyLen = strlen(key);
	char *js = jsParser.jsStr;
	int i;
	for (i = 0; i < jsParser.numJSPair; i++) {
		size_t jsKeyLen = jsPair[i].key.end - jsPair[i].key.start + 1;
		if (keyLen != jsKeyLen)
			continue;
		if (strncmp(key, &js[jsPair[i].key.start], jsKeyLen))
			continue;
		size_t jsValLen = jsPair[i].value.end - jsPair[i].value.start + 1;
		strlcpy(val, &js[jsPair[i].value.start], jsValLen+1);
		return 0;
	}
	return 1;
}
int getJSKeyList(char *key_list)
{
	if (!jsParser.parsed)
		return -1;

	char *js = jsParser.jsStr;
	int i;
	key_list[0] = '\0';
	for (i = 0; i < jsParser.numJSPair; i++) {
		strncat(key_list, &js[jsPair[i].key.start], jsPair[i].key.end - jsPair[i].key.start + 1);
		if (i < (jsParser.numJSPair - 1))
			strcat(key_list, ", ");
	}
	return 0;
}

int printParsedJS()
{
	if (!jsParser.parsed)
			return -1;

	char *jsStr = jsParser.jsStr;
	int i;
	serialPrint("{ ");
	for (i = 0; i < jsParser.numJSPair; i++) {
		serialPrint("%.*s", jsPair[i].key.end - jsPair[i].key.start + 1, &jsStr[jsPair[i].key.start]);
		serialPrint("(%c): ", jsPair[i].key.type);
		serialPrint("%.*s", jsPair[i].value.end - jsPair[i].value.start + 1, &jsStr[jsPair[i].value.start]);
		serialPrint("(%c)", jsPair[i].value.type);
		if (i < (jsParser.numJSPair - 1))
			serialPrint(", ");
	}
	serialPrintln(" }");
	return 0;
}

int getParsedJS(char *json)
{
	if (!jsParser.parsed)
			return -1;

	char *jsStr = jsParser.jsStr;
	int i;
	json[0] = '\0';
	char typeStr[10];
	for (i = 0; i < jsParser.numJSPair; i++) {
		strncat(json, &jsStr[jsPair[i].key.start], jsPair[i].key.end - jsPair[i].key.start + 1);
		snprintf(typeStr, sizeof(typeStr), "(%c): ", jsPair[i].key.type);
		strncat(json, typeStr, sizeof(typeStr));
		strncat(json, &jsStr[jsPair[i].value.start], jsPair[i].value.end - jsPair[i].value.start + 1);
		snprintf(typeStr, sizeof(typeStr), "(%c)", jsPair[i].value.type);
		strncat(json, typeStr, sizeof(typeStr));
		if (i < (jsParser.numJSPair - 1))
			strcat(json, ", ");
	}
	return 0;
}

//#define __MAIN__

#ifdef __MAIN__

int main()
{
	char js[100] = "\t{\"\nabhi\":\t2.9, \"\t__anu\":\"best\", \" papa\":\"angry\", \"\t  _mommy\":\"swddddddddeet\"}";
	json_init(js);
	serialPrintln("Jason String - %s\n", js);
	jsonParse();
	char token[100];
	printParsedJS();
	serialPrintln("Json - %s", token);
	char val[10] = "\0";
	getValAtJSKey("__anu", val);
	serialPrintln("val - %s\n", val);

	char tok[1024];
	getJSKeyList(tok);
	serialPrintln("Key list is - %s", tok);

	return 0;
}
#endif
