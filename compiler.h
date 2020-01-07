/* INCLUDE HEADERS */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>
#include <ctype.h>
#include <stdbool.h>
/* END INCLUDE HEADERS */

/* GLOBAL CONSTANTS */
#define null NULL
#define MAX_STRING_SIZE 500
/* END GLOBAL CONSTANTS */

/* TYPE DEFINITIONS */
typedef char string[MAX_STRING_SIZE];

typedef enum type_t {
    INTEGER_T = 1,
    FLOAT_T,
    CHARACTER_T
} type;

typedef struct token_t {
    int id;
    char *name;
    char *value;
} token;

typedef struct array_value_t {
    int index;
    void *value;
    struct array_value_t* next;
} array_values_list, array_value, array_value_node;

typedef struct var_t {
    bool is_array;
    char *name;
    type _type;
    union
    {
        char characterValue;
        int integerValue;
        float floatValue;
    } value;
    
} var, constant;

typedef struct tokenList_t {
    token token_entry;
    struct tokenList_t *next;
} token_list, token_list_node;

typedef struct varList_t {
    var variable;
    struct varList_t *next;
} var_list, var_list_node;

typedef struct constList_t {
    constant _const;
    struct constList_t *next;
} const_list, const_list_node;
/* END TYPE DEFINITIONS */

/* FUNCTIONS HEADER */
void initiate_regex_strings();
void insert_token(int ,string, string);
token* find_token(string, token_list *);
bool is_keyword(string);
bool is_reserved_token(string, token_list *);
void insert_token_into_list(int, string, string, token_list**);
void insert_variable(string, string, bool);
var* find_variable(string);
bool is_int(string);
bool is_character(string);
bool is_float(string);
void set_variable_value(string, string, int);
void insert_constant(string, char *);
void print_keywords();
void print_tokens_from_list(token_list*);
bool matches(const char*, char*);
bool starts_with(const char*, char*);
bool ends_with(const char*, char*);
char *ltrim(char *);
char *rtrim(char *);
char *trim(char *);
void set_string(char**, char*);
/* END FUNCTIONS HEADER */

/* REGEX EBNF */
const string DIGIT = "[[:digit:]]";
const string LETTER = "[[:alpha:]]";
const string BLANK = "[[:blank:]]";
const string AT_LEAST_ONCE = "{1,}";
const string ONCE_OR_NONE = "{0,1}";
const string MAY_EXIST = "{0,}";
const string SEMICOLON = ";";
const string COMMA = ",";
const string DOT = "\\.";
const string EQUALS = "=";
const string SINGLE_QUOTE = "'";
const string OR = "|";
const string SQUARE_BRACKET_OPEN="\\[";
const string SQUARE_BRACKET_CLOSE="\\]";
const string SMALLER_THAN = "<";
const string LARGER_THAN = ">";
const string DOUBLE_SMALLER_THAN = "<<";
const string DOUBLE_LARGER_THAN = ">>";
const string EQUALS_EQUALS = "==";
const string LESS_THAN_OR_EQUALS = "<=";
const string LARGER_THAN_OR_EQUALS = ">=";
const string NOT_EQUALS = "!=";
const string START_SET = "(";
const string END_SET = ")";
const string STARTS_WITH = "^";
const string ENDS_WITH = "$";
string NUMBER;
string FLOATING_NUMBER;
string WHITESPACE;
string CHARS;
string WHITESPACE_MAY_EXIST;
/* KEYWORDS */
const string INTEGER_KEYWORD = "integer";
const string FLOAT_KEYWORD = "float";
const string CHAR_KEYWORD = "char";
const string INPUT_KEYWORD = "input";
const string OUTPUT_KEYWORD = "output";
const string NEWJOB_KEYWORD = "newjob";
const string ARRAY_KEYWORD = "array";

/* END REGEX EBNF */

/* BLOCK REGEX */
const string BLK_CONSTANTS_REGEX = "^constants$";
const string BLK_VARIABLES_REGEX = "^variables$";
const string BLK_ELSE_REGEX = "^else$";
const string BLK_END_IF = "^endif;";
const string BLK_LOOP_START_REGEX = "^loop$";
const string BLK_BEGIN_BLOCK_REGEX="^beginb$";
const string BLK_END_BLOCK_REGEX="^endb;$";
/* END BLOCK REGEX */
string HEADER_NEWJOB;
string STMT_ASSIGNMENT_REGEX;
string ENDS_WITH_NUMBER;
string ENDS_WITH_FLOATING_NUMBER;
string STMT_INPUT_REGEX;
string STMT_OUTPUT_REGEX;
string VRB_PRIMITIVE_INST;
string STARTS_WITH_INTEGER;
string STARTS_WITH_FLOAT;
string STARTS_WITH_CHAR;
string VRB_ARRAY_INST;
string VRB_ARRAY_INTEGER;
string VRB_ARRAY_FLOAT;
string VRB_ARRAY_CHAR;
string BLK_IF_CONDITION_THEN_REGEX = "^if[[:blank:]]{1,}.*[[:blank:]]{1,}then$";
string BLK_WHILE_CONDITION_DO_REGEX="^while[[:blank:]]{1,}.*[[:blank:]]{1,}do$";
string BLK_LOOP_UNTIL_CONDITION_REGEX = "^until .*;$";



string CONDITION = "^<[[:blank:]]{0,}([[:alpha:]]{1,}|[[:digit:]]+)[[:blank:]]{0,}(!=|==|<=?|>=?)[[:blank:]]{0,}([[:alpha:]]{1,}|[[:digit:]]+)[[:blank:]]{0,}>$"; // done

/** Lists */
token_list* tokens = null;
var_list* variables = null;
const_list* constants = null;

void initiate_regex_strings()
{
    // WHITESPACE : sequential spaces or tabs (must exist at least once)
    snprintf(WHITESPACE, MAX_STRING_SIZE, "%s%s",BLANK,AT_LEAST_ONCE);
    // WHITESPACE_MAY_EXIST:  optional sequential spaces or tabs (may exist once or many or none)
    snprintf(WHITESPACE_MAY_EXIST, MAX_STRING_SIZE, "%s%s",BLANK,MAY_EXIST);
    /** CHARS : sequence of characters (at least one character)
     * Ex: Abed, murrar, bIrZeIt, COMpileR, word
    */
    snprintf(CHARS, MAX_STRING_SIZE, "%s%s",LETTER, AT_LEAST_ONCE);
    /** NUMBER : sequence of digits (at least one digit)
     * Ex: 1, 65, 123565, 3249835987123, 0
    */
    snprintf(NUMBER,MAX_STRING_SIZE,"%s%s",DIGIT,AT_LEAST_ONCE);
    /** 
     * FLOATING_NUMBER : sequence of digits (at least one digit)
        followed by a dot (must exist) 
        then follow by another sequence of digits (at least one digit)
        Ex: 1.0, 0.0, 21.1235, 9999.123, 10.5
    */
    snprintf(FLOATING_NUMBER, 2*strlen(NUMBER)+strlen(DOT)+1, "%s%s%s",NUMBER,DOT,NUMBER);
    /**
     * HEADER_NEWJOB : starts with static word "newjob"
     *                 followed by WHITESPACE
     *                 then followed by CHARS (name)
     *                 then ended with a semicolon
     * Ex: newjob helloworld;
    */
    snprintf(HEADER_NEWJOB,
     strlen(STARTS_WITH)+
     strlen(NEWJOB_KEYWORD)+
     strlen(WHITESPACE)+
     strlen(CHARS)+
     strlen(SEMICOLON)+
     strlen(ENDS_WITH)+
     1,
     "%s%s%s%s%s%s",
     STARTS_WITH,
     NEWJOB_KEYWORD,
     WHITESPACE,
     CHARS,
     SEMICOLON,
     ENDS_WITH
     );
    /**
     * STMT_ASSIGNMENT_REGEX : starts with CHARS (must exist)
     *                         followed by WHITESPACE_MAY_EXIST
     *                         then followed EQUALS sign (=)
     *                         then followed by another WHITESPACE_MAY_EXIST
     *                         then followed by a NUMBER OR FLOATING_NUMBER
     * Ex: x = 12.4;
     *     myvar=           55;
     *     foo                  =                2.0;
     *     bar=3;
    */
    snprintf(STMT_ASSIGNMENT_REGEX, MAX_STRING_SIZE, "%s%s%s%s%s%s%s%s%s%s%s%s",STARTS_WITH,CHARS,WHITESPACE_MAY_EXIST,EQUALS,WHITESPACE_MAY_EXIST,START_SET,NUMBER,OR,FLOATING_NUMBER,END_SET,SEMICOLON,ENDS_WITH);
    /**
     * ENDS_WITH_NUMBER : ends with a NUMBER followed by a SEMICOLON disregarding what it starts with
     * Ex: 3;
    */
    snprintf(ENDS_WITH_NUMBER, MAX_STRING_SIZE,"%s%s%s",NUMBER,SEMICOLON,ENDS_WITH);
    /**
     * ENDS_WITH_FLOATING_NUMBER : ends with a FLOATING_NUMBER followed by a SEMICOLON disregarding what it starts with
     * Ex: 213.23;
    */
    snprintf(ENDS_WITH_FLOATING_NUMBER, MAX_STRING_SIZE, "%s%s%s", FLOATING_NUMBER, SEMICOLON,ENDS_WITH); // ends with a floating number
    /**
     * STMT_INPUT_REGEX: starts with input keywords and followed by << then a variable name then >> and ended with a semicolon
     * Ex: input <<foo>>;
     */
    snprintf(STMT_INPUT_REGEX, MAX_STRING_SIZE, "%s%s%s%s%s%s%s%s",STARTS_WITH,INPUT_KEYWORD,WHITESPACE,DOUBLE_SMALLER_THAN,CHARS,DOUBLE_LARGER_THAN,SEMICOLON,ENDS_WITH);
    /**
     * STMT_OUTPUT_REGEX: starts with output keywords and followed by << then a variable name then >> and ended with a semicolon
     * Ex: output <<x>>;
     */
    snprintf(STMT_OUTPUT_REGEX, MAX_STRING_SIZE, "%s%s%s%s%s%s%s%s",STARTS_WITH,OUTPUT_KEYWORD,WHITESPACE,DOUBLE_SMALLER_THAN,CHARS,DOUBLE_LARGER_THAN, SEMICOLON,ENDS_WITH);
    //string VRB_PRIMITIVE_INST = "([[:alpha:]]{1,}){0,};$";
    snprintf(VRB_PRIMITIVE_INST,MAX_STRING_SIZE,"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
    STARTS_WITH,
    START_SET,
    INTEGER_KEYWORD,
    OR,
    FLOAT_KEYWORD,
    OR,
    CHAR_KEYWORD,
    END_SET,
    WHITESPACE,
    CHARS,
    START_SET,
    WHITESPACE_MAY_EXIST,
    COMMA,
    WHITESPACE_MAY_EXIST,
    CHARS,
    END_SET,
    MAY_EXIST,
    SEMICOLON,
    ENDS_WITH
    );
    /**
     * string STARTS_WITH_INTEGER = "^integer";
string STARTS_WITH_FLOAT = "^float";
string STARTS_WITH_CHAR = "^char";
     * 
     */ 

    snprintf(STARTS_WITH_INTEGER, MAX_STRING_SIZE, "%s%s",STARTS_WITH,INTEGER_KEYWORD);
    snprintf(STARTS_WITH_FLOAT, MAX_STRING_SIZE, "%s%s",STARTS_WITH,FLOAT_KEYWORD);
    snprintf(STARTS_WITH_CHAR, MAX_STRING_SIZE, "%s%s",STARTS_WITH,CHAR_KEYWORD);

    /**
     * 
     * string VRB_ARRAY_INST = "^array\\[(integer|float|char)\\][[:blank:]]{1,}[[:alpha:]]{1,};$";
string VRB_ARRAY_INTEGER ="^array\\[integer\\]";
string VRB_ARRAY_FLOAT ="^array\\[float\\]";
string VRB_ARRAY_CHAR ="^array\\[char\\]";
     */ 

    snprintf(VRB_ARRAY_INST, 
    strlen(STARTS_WITH)+
    strlen(ARRAY_KEYWORD)+
    strlen(SQUARE_BRACKET_OPEN)+
    strlen(START_SET)+
    strlen(INTEGER_KEYWORD)+
    strlen(OR)+
    strlen(FLOAT_KEYWORD)+
    strlen(OR)+
    strlen(CHAR_KEYWORD)+
    strlen(END_SET)+
    strlen(SQUARE_BRACKET_CLOSE)+
    strlen(WHITESPACE)+
    strlen(CHARS)+
    strlen(SEMICOLON)+
    strlen(ENDS_WITH)+
    1
,"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
    STARTS_WITH,
    ARRAY_KEYWORD,
    SQUARE_BRACKET_OPEN,
    START_SET,
    INTEGER_KEYWORD,
    OR,
    FLOAT_KEYWORD,
    OR,
    CHAR_KEYWORD,
    END_SET,
    SQUARE_BRACKET_CLOSE,
    WHITESPACE,
    CHARS,
    SEMICOLON,
    ENDS_WITH
    );


    snprintf(VRB_ARRAY_INTEGER,MAX_STRING_SIZE,"%s%s%s%s%s",
    STARTS_WITH,
    ARRAY_KEYWORD,
    SQUARE_BRACKET_OPEN,
    INTEGER_KEYWORD,
    SQUARE_BRACKET_CLOSE
    );
    snprintf(VRB_ARRAY_FLOAT,MAX_STRING_SIZE,"%s%s%s%s%s",
    STARTS_WITH,
    ARRAY_KEYWORD,
    SQUARE_BRACKET_OPEN,
    FLOAT_KEYWORD,
    SQUARE_BRACKET_CLOSE
    );
    snprintf(VRB_ARRAY_CHAR,MAX_STRING_SIZE,"%s%s%s%s%s",STARTS_WITH,ARRAY_KEYWORD,SQUARE_BRACKET_OPEN,CHAR_KEYWORD,SQUARE_BRACKET_CLOSE);


}

/** TOKENS */
void insert_token(int id,string name, string value)
{
    insert_token_into_list(id,name,value,&tokens);
}

token* find_token(string value, token_list *list)
{
    token_list *temp = list;
    token* found = null;

    while(temp!=null)
    {
        if(strcmp(temp->token_entry.value,value)==0)
        {
            found=&temp->token_entry;
            break;
        }
        temp=temp->next;
    }
    // kawto = &found;
    return found;
}

bool is_keyword(string word)
{
    if(find_token(word,tokens) != NULL){
        return true;
    }
    return false;
}

bool is_reserved_token(string word, token_list *list)
{
    if(is_keyword(word) || find_token(word,list) != null){
        return true;
    }
    return false;
}

void insert_token_into_list(int id,string name, string value, token_list** list)
{
    // create node
    token_list_node* tln = (token_list_node *)malloc(sizeof(token_list_node));
    // assign values
    tln -> next = null;
    tln -> token_entry.id=id;
    tln -> token_entry.name=name;
    tln -> token_entry.value=value;
    // append to list to end
    token_list* temp = *list;
    if(temp == null)
    {
        *list=tln;
    }
    else
    {
        while (temp->next != null)
        {
            temp=temp->next;
        }
        temp -> next = tln;
    }
}

void insert_variable(string name, string type, bool is_array)
{
    // create node
    var_list_node* vln = (var_list_node *)malloc(sizeof(var_list_node));
    // assign values
    vln -> next = null;
    vln->variable.name=name;
    // vln->variable.type=type;
    vln->variable.is_array = is_array;
    // vln->variable.value=NULL;

    if(is_int(type)){
        vln->variable._type=INTEGER_T;
    }
    else if(is_float(type)){
        vln->variable._type=FLOAT_T;;
    }
    else if(is_character(type)){
        vln->variable._type=CHARACTER_T;
    }
    else{
        fprintf(stderr,"Unknown type : %s",type);
        exit(EXIT_FAILURE);
    }
    
    // assign to list
    var_list* temp = variables;
    if(temp == null)
    {
        variables=vln;
    }
    else
    {
        while (temp->next != null)
        {
            temp=temp->next;
        }
        temp -> next = vln;
    }
}

var* find_variable(string name)
{
    var_list* temp = variables;
    while(temp != null){
        if(strcmp(temp->variable.name, name) == 0)
        {
            return &temp->variable;
        }
    }
    return null;
}

bool is_int(string type)
{
    if(strcmp(type,"integer") == 0)
    {
        return true;
    }
    return false;
}

bool is_character(string type)
{
    if(strcmp(type, "char") == 0)
    {
        return true;
    }
    return false;
}

bool is_float(string type)
{
    if(strcmp(type,"float") == 0)
    {
        return true;
    }
    return false;
}

void set_variable_value(string name, string value, int index)
{
    var* variable = find_variable(name);
    if(variable != null){
        if(!variable->is_array)
        {
            switch (variable->_type)
            {
                case INTEGER_T:
                    variable->value.integerValue = atoi(value);
                    break;
                case FLOAT_T:
                    variable->value.floatValue = atof(value);
                    break;
                case CHARACTER_T:
                    variable->value.characterValue = *value;
                    break;
            }
        }
    }
}

void insert_constant(string name, char * value)
{
    // create node
    const_list_node* cln = (const_list_node *)malloc(sizeof(const_list_node));
    // assign values
    cln->next = null;
    cln->_const.name=name;
    if(matches(FLOATING_NUMBER, value))
    {
        cln->_const.type = "float";
        cln->_const.value.floatValue = atof(value);
    }
    else if(matches(NUMBER, value))
    {
        cln->_const.value.integerValue = atoi(value);
        cln->_const.type = "integer";
    }

    // append to list
    const_list* temp = constants;
    if(temp == null)
    {
        constants=cln;
    }
    else
    {
        while (temp->next != null)
        {
            temp=temp->next;
        }
        temp -> next = cln;
    }
}

void print_keywords()
{
    print_tokens_from_list(tokens);
}

void print_tokens_from_list(token_list* list)
{
    token_list *temp = list;
    printf("%-30s %-20s %-20s\n","Token","Representation","ID");
    while(temp!=null)
    {
        printf("%-30s %-20s %-20d\n",temp->token_entry.name, temp->token_entry.value,temp->token_entry.id);
        temp=temp->next;
    }
}


/** END TOKENS */
/** REGEX */
bool matches(const char* pattern, char* string)
{
        regex_t regex;
        int reti;
        char msgbuf[100];
        bool match = false;
        
        /* Compile regular expression */
        reti = regcomp(&regex, pattern, REG_EXTENDED);

        /* Execute regular expression */
        reti = regexec(&regex, string, 0, NULL, 0);
        if( !reti ){
            match = true;
        }

        /* Free compiled regular expression if you want to use the regex_t again */
	    regfree(&regex);
        return match;
}

bool starts_with(const char* pattern, char* string)
{
        regex_t regex;
        int reti;
        char _p[100];
        bool match = false;

        snprintf(_p, 100, "^%s",string);
        
        /* Compile regular expression */
        reti = regcomp(&regex, _p, REG_EXTENDED);

        /* Execute regular expression */
        reti = regexec(&regex, string, 0, NULL, 0);
        if( !reti ){
                match = true;
        }

        /* Free compiled regular expression to use the regex_t again */
	    regfree(&regex);
        return match;
}

bool ends_with(const char* pattern, char* string)
{
        regex_t regex;
        int reti;
        char _p[100];
        bool match = false;

        snprintf(_p, 100, "%s$",string);
        
        /* Compile regular expression */
        reti = regcomp(&regex, _p, REG_EXTENDED);

        /* Execute regular expression */
        reti = regexec(&regex, string, 0, NULL, 0);
        if( !reti ){
                match = true;
        }

        /* Free compiled regular expression to use the regex_t again */
	    regfree(&regex);
        return match;
}
/** END REGEX */

/** STRING */
char *ltrim(char *s)
{
    while(isspace((unsigned char)*s)) s++;
    return s;
}

char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0'; 
    return s;
}

char *trim(char *s)
{
    return rtrim(ltrim(s)); 
}

void set_string(char** dest, char* src)
{
    size_t len = strlen(src);
    *dest = (char*)malloc(sizeof((int)strlen(src)*sizeof(char)));
    strcpy(*dest,src);
}
/** END STRING */
