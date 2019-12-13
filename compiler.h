#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>
#include <ctype.h>

#define null NULL
#define MAX_STRING_SIZE 150


typedef char string[MAX_STRING_SIZE];

typedef struct token_t {
    int id;
    char *name;
    char *value;
} token;

typedef struct tokenList_t {
    token token_entry;
    struct tokenList_t *next;
} tokenList;

typedef struct var_t {
    char *name;
    char *type;
    void *value;
} var;

typedef struct varList_t {
    var variable;
    struct variableList_t *next;
} varList;

typedef struct const_t {
    char *name;
    void *value;
} constant;

typedef struct constList_t {
    constant _const;
    struct constList_t *next;
} constList;

typedef enum bool {
    false=0,
    true=1
} boolean;  

void insertToken(int id,string name, string value);
void insertTokenIntoList(int id,string name, string value, tokenList** list);
void printTokensFromList(tokenList* list);
void printKeywords();
int check_match(char* pattern, char* string);
token* findToken(string value, tokenList *list);
char *ltrim(char *s);
char *rtrim(char *s);
char *trim(char *s);

string digit = "[[:digit:]]";
string letter = "[[:alpha:]]";
string blank = "[[:blank:]]";
string at_lest_one = "{1,}";
string one_or_none = "{0,1}";
string may_exist_many_or_none = "{0,}";
string semicolon = ";";
string number;
string whitespace;
string alphabetics;
string whitespace_can_exist;
// snprintf((char *)whitespace, MAX_STRING_SIZE,"%d", 1);
string HEADER_NEWJOB = "^newjob[[:blank:]]{1,}[[:alpha:]]{1,};$";
string STMT_ASSIGNMENT_REGEX = "^[[:alpha:]]+[[:blank:]]{0,}=[[:blank:]]{0,}[[:digit:]]+(\\.{0,1}[[:digit:]]{1,})?;{1}$";
string STMT_INPUT_REGEX = "^input[[:blank:]]{1,}<<[[:alpha:]]+>>;";
string STMT_OUTPUT_REGEX = "^output[[:blank:]]{1,}<<[[:alpha:]]+>>;";
string BLK_CONSTANTS_REGEX = "^constants$";
string BLK_VARIABLES_REGEX = "^variables$";
string BLK_IF_THEN_REGEX = "^if[[:blank:]]{1,}.*[[:blank:]]{1,}then$";
string BLK_ELSE_REGEX = "^else$";
string BLK_END_IF = "^endif;";
string BLK_WHILE_DO_REGEX="^while[[:blank:]]{1,}.*[[:blank:]]{1,}do$";
string BLK_LOOP_START_REGEX = "^loop$";
string BLK_LOOP_UNTIL_REGEX = "^until .*;$";
string BLK_BEGIN_BLOCK_REGEX="^beginb$";
string BLK_END_BLOCK_REGEX="^endb;$";
string VRB_PRIMITIVE_INST = "^(integer|float|char)[[:blank:]]{1,}[[:alpha:]]{1,}([[:blank:]]{0,},[[:blank:]]{0,}[[:alpha:]]{1,}){0,}?;$";
string VRB_INT_INST = "^integer[[:blank:]]{1,}[[:alpha:]]{1,};$";
string VRB_ARRAY_INST = "^array\\[(integer|float|char)\\][[:blank:]]{1,}[[:alpha:]]{1,};$";
string CONDITION = "^<[[:blank:]]{0,}.*((!|=)=|<=?|>=?).*[[:blank:]]{0,}>$"; //aint done

/** Lists */
tokenList* tokens = null;
varList* variables = null;
constList* constants = null;

void initiate_regex_strings()
{
    snprintf(whitespace, MAX_STRING_SIZE, "%s%s",blank,at_lest_one);
    snprintf(whitespace_can_exist, MAX_STRING_SIZE, "%s%s",blank,may_exist_many_or_none);
    snprintf(alphabetics, MAX_STRING_SIZE, "%s%s",letter, at_lest_one);
    snprintf(number,MAX_STRING_SIZE,"%s%s",digit,at_lest_one);
    snprintf(HEADER_NEWJOB, MAX_STRING_SIZE,"^%s%s%s%s$","newjob",whitespace,alphabetics,semicolon);
    //"^newjob[[:blank:]]{1,}[[:alpha:]]{1,};$"
}

/** TOKENS */

void insertToken(int id,string name, string value)
{
    insertTokenIntoList(id,name,value,&tokens);
}

void insertTokenIntoList(int id,string name, string value, tokenList** list)
{
    tokenList* tln = (tokenList *)malloc(sizeof(tokenList));
    tln -> next = null;
    tln->token_entry.id=id;
    tln->token_entry.name=name;
    tln->token_entry.value=value;
    tokenList* temp = *list;
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

void insertVariable(string name, string type, char * value)
{
    varList* vln = (varList *)malloc(sizeof(varList));
    vln -> next = null;
    vln->variable.name=name;
    vln->variable.type=type;
    if(strcmp(type,"integer")==0)
    {
        vln->variable.value = malloc(sizeof(int));
        vln->variable.value = (int) atoi(value);
    }
    else if(strcmp(type,"float")==0)
    {
        float val =(float) atof(value);
        vln->variable.value =  malloc(sizeof(float));
        vln->variable.value = (float*) &val;
    }
    else if(strcmp(type,"char")==0)
    {
        vln->variable.value = malloc(sizeof(char));
        vln->variable.value = (char) *value;
    }
    else
    {
        fprintf(stderr,"Unknown datatype");
        exit(EXIT_FAILURE);
    }
    
    varList* temp = variables;
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

void insertConstant(string name, char * value)
{
    constList* cln = (constList *)malloc(sizeof(constList));
    cln->next = null;
    cln->_const.name=name;
    if(strstr(value,".")!=null)
    {
        float val =(float) atof(value);
        cln->_const.value = malloc(sizeof(float));
        cln->_const.value = (float*)&val;
    }
    else
    {
        cln->_const.value = malloc(sizeof(int));
        cln->_const.value = (int)atoi(value);
    }

    constList* temp = constants;
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

void printKeywords()
{
    printTokensFromList(tokens);
}

void printTokensFromList(tokenList* list)
{
    tokenList *temp = list;
    printf("%-30s %-20s %-20s\n","Token","Representation","ID");
    while(temp!=null)
    {
        printf("%-30s %-20s %-20d\n",temp->token_entry.name, temp->token_entry.value,temp->token_entry.id);
        temp=temp->next;
    }
}

token* findToken(string value, tokenList *list)
{
    tokenList *temp = list;
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
/** END TOKENS */
/** REGEX */
int check_match(char* pattern, char* string)
{
        regex_t regex;
        int reti;
        char msgbuf[100];
        boolean match = false;
        
        /* Compile regular expression */
        reti = regcomp(&regex, pattern, REG_EXTENDED);
        //^[a-zA-Z]+\s+?=\s+?[\d]$

        /* Execute regular expression */
        reti = regexec(&regex, string, 0, NULL, 0);
        if( !reti ){
                match = true;
        }

        /* Free compiled regular expression if you want to use the regex_t again */
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

    printf("%d\n",*back);
    if((*back) != '\0'){
        printf("it is not 0, it is %c = %d, and b+1 is %c = %d\n",*back,(int) *back,*(back+1),(int) *(back+1));
        // *(back+1) = '\0';
    }
    return s;
}

char *trim(char *s)
{
    return rtrim(ltrim(s)); 
}

void setString(char** dest, char* src)
{
    size_t len = strlen(src);
    *dest = (char*)malloc(sizeof((int)strlen(src)));
    strcpy(*dest,src);
}
/** END STRING */