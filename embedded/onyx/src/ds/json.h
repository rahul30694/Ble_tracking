#ifndef _JSON_H_
#define _JSON_H_

int json_init(char *jsonStr);
int jsonParse();

int getValAtJSKey(char *key, char *val);
int getJSKeyList(char *key_list);

int printParsedJS();
int getParsedJS(char *json);




#endif /* HELPER_C_JSON_H_ */
