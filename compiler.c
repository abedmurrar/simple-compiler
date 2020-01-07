// ABED AL RAHMAN MURRAR 1140155
// i wish i had more time to finish it
#include "compiler.h"
#define DEST_SIZE 40
#define KEYWORDS_NUMBER 35
#define BUFFER_SIZE  1000


string KEYWORD_NAMES[KEYWORDS_NUMBER]= {
    "NEWJOB",
    "CONSTANTS",
    "VARIABLES",
    "ARRAY",
    "BEGINB",
    "ENDB",
    "EXIT",
    "INPUT",
    "OUTPUT",
    "IF",
    "THEN",
    "ELSE",
    "ENDIF",
    "WHILE",
    "DO",
    "LOOP",
    "UNTIL",
    "INTEGER",
    "FLOAT",
    "CHAR",
    "CHECK_EQUALS",
    "NOT_EQUALS",
    "LESS_THAN",
    "LESS_OR_EQUAL_THAN",
    "GREATER_THAN",
    "GREATER_OR_EQUAL_THAN",
    "ADD",
    "MINUS",
    "DIVIDE",
    "MULTIPLY",
    "MODULUS",
    "SQUARE_BRACKET_OPEN",
    "SQUARE_BRACKET_CLOSE",
    "SEMICOLON",
    "EQUALS"
};

string KEYWORD_REPRESENTATION[KEYWORDS_NUMBER]= {
    "newjob",
    "constants",
    "variables",
    "array",
    "beginb",
    "endb",
    "exit",
    "input",
    "output",
    "if",
    "then",
    "else",
    "endif",
    "while",
    "do",
    "loop",
    "until",
    "integer",
    "float",
    "char",
    "==",
    "!=",
    "<",
    "<=",
    ">",
    ">=",
    "+",
    "-",
    "/",
    "*",
    "%%",
    "[",
    "]",
    ";",
    "="
};


int main()
{
    bool newJobSection = true;
    bool constantsSection = false;
    bool variablesSection = false;
    bool statementsSection = false;

    token_list* fileTokens = null;


    /**
     * LEXICAL ANALYSIS
     */

    register int id=1,i;
    for(i=0;i<KEYWORDS_NUMBER;i++)
    {
        insert_token(id++,KEYWORD_NAMES[i],KEYWORD_REPRESENTATION[i]);
    }
    FILE *codeFile = null;

    codeFile = fopen("input.txt", "r");
    if(codeFile == null)
    {
        exit(1);
    }
    initiate_regex_strings();

    /**
     * Check each line if matches any regex
     * if not, then line has syntax error
     */
    char *buffer = (char *) malloc(BUFFER_SIZE*sizeof(char));
    while(fgets(buffer,BUFFER_SIZE,codeFile)!=null)
    {
        // buffer=trim(buffer);
        char* trimmedBuffer = (char*)malloc(1000);
        strcpy(trimmedBuffer,trim(buffer));
        //  trimmedBuffer = trim(buffer);
        
        if(!strlen(trimmedBuffer))
        {
            // if line is empty
            continue;
        }

        if(newJobSection)
        {
            if(matches(HEADER_NEWJOB,trimmedBuffer))
            {
                
                char* tokena = strtok(trimmedBuffer," ");
                // tokena = trim(strtok(NULL,";"));
                set_string(&tokena,trim(strtok(NULL,";")));
                token* _temp_token = find_token("newjob",tokens);
                if(_temp_token != null)
                {
                    insert_token_into_list(_temp_token->id,_temp_token->name,_temp_token->value,&fileTokens);
                }
                _temp_token = find_token(";",tokens);
                if(_temp_token!=null)
                {
                    insert_token_into_list(_temp_token->id,_temp_token->name,_temp_token->value,&fileTokens);
                }
                if(!is_reserved_token(tokena,fileTokens)){
                    insert_token_into_list(id++,"NEWJOB_NAME",tokena,&fileTokens);
                }
                else
                {
                    fprintf(stderr,"Error in newjob section, %s is an already reserved keyword",tokena);
                    exit(EXIT_FAILURE);
                }
                
                newJobSection=false;
                constantsSection=true;
                continue;
            }
            else
            {
                fprintf(stderr,"Error in newjob line.: %s\n",trimmedBuffer);
                exit(EXIT_FAILURE);
            }
        }
        else if(constantsSection)
        {
            if(matches(BLK_CONSTANTS_REGEX,trimmedBuffer))
            {
                token* _temp_token = find_token(trimmedBuffer,tokens);
                if(_temp_token != null)
                {
                    insert_token_into_list(_temp_token->id,_temp_token->name,_temp_token->value,&fileTokens);
                }
                continue;
            }
            else if(matches(STMT_ASSIGNMENT_REGEX,trimmedBuffer))
            {
                char* tokena = strtok(trimmedBuffer,"=");
                char* tokenb;
                set_string(&tokenb, trim(strtok(NULL,";")));
                if(!is_reserved_token(tokenb,fileTokens))
                {
                    insert_token_into_list(id++,"CONSTANT",tokena,&fileTokens);
                    insert_constant(tokena, tokenb);
                }
                else
                {
                    fprintf(stderr,"Error in constants, %s is an already reserved keyword",tokena);
                    exit(EXIT_FAILURE);
                }
                continue;
            }
            else if(matches(BLK_VARIABLES_REGEX,trimmedBuffer))
            {
                token* _temp_token = find_token(trimmedBuffer,tokens);
                if(_temp_token != null)
                {
                    insert_token_into_list(_temp_token->id,_temp_token->name,_temp_token->value,&fileTokens);
                }
                constantsSection=false;
                variablesSection=true;
                continue;
            }
            else
            {
                fprintf(stderr,"Error in constants section.: %s\n",trimmedBuffer);
                exit(EXIT_FAILURE);
            }
        }
        else if(variablesSection)
        {
            continue;
            if(matches(VRB_PRIMITIVE_INST,trimmedBuffer))
            {
                char* tokena = NULL, *tokenb = NULL, *token_temp = NULL;
                char* type;
                set_string(&tokena,trim(strtok(trimmedBuffer," ")));
                if(matches(STARTS_WITH_INTEGER, tokena))
                {
                    type="integer";
                }
                else if (matches(STARTS_WITH_FLOAT, tokena))
                {
                    type="float";
                }
                else if (matches(STARTS_WITH_CHAR,tokena))
                {
                    type="char";
                }
                while((token_temp=strtok(NULL, ",;")) != null){
                    set_string(&tokenb, trim(token_temp));
                    if(!is_reserved_token(tokenb, fileTokens)){
                        insert_token_into_list(id++,"VAR", tokenb, &fileTokens);
                        insert_variable(tokenb,type,false);
                    }
                    else
                    {
                        fprintf(stderr,"Error in variables section, %s is an already reserved keyword",tokena);
                        exit(EXIT_FAILURE);
                    }
                }
                continue;
            }
            else if(matches(VRB_ARRAY_INST,trimmedBuffer))
            {
                char* tokena = NULL, *tokenb = NULL, *token_temp = NULL;
                char* type;
                set_string(&tokena,trim(strtok(trimmedBuffer," ")));
                // if(matches(INT))
                continue;
            }
            else if(matches(BLK_BEGIN_BLOCK_REGEX,trimmedBuffer))
            {
                variablesSection=false; 
                statementsSection=true;
                continue;
            }
            else
            {
                fprintf(stderr,"Error in variables section.: %s\n",trimmedBuffer);
                exit(EXIT_FAILURE);
            }
        }
    }
    fclose(codeFile);    
    print_tokens_from_list(fileTokens);
    printf("\n");

	return 0;
}

