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
    FILE *codeFile;
    char *buffer = (char*)malloc(sizeof(BUFFER_SIZE));
    boolean newJobSection = true;
    boolean constantsSection = false;
    boolean variablesSection = false;
    boolean statementsSection = false;

    tokenList* fileTokens = null;


    /**
     * LEXICAL ANALYSIS
     */

    register int id=1,i;
    for(i=0;i<KEYWORDS_NUMBER;i++)
    {
        insertToken(id++,KEYWORD_NAMES[i],KEYWORD_REPRESENTATION[i]);
    }
    codeFile = fopen("input.txt", "r");
    if(codeFile == null)
    {
        exit(1);
    }

    /**
     * Check each line if matches any regex
     * if not, then line has syntax error
     */
    while(fgets(buffer,1000,codeFile))
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
            if(check_match(HEADER_NEWJOB,trimmedBuffer))
            {
                
                char* tokena = strtok(trimmedBuffer," ");
                // tokena = trim(strtok(NULL,";"));
                setString(&tokena,trim(strtok(NULL,";")));
                token* _temp_token = findToken("newjob",tokens);
                if(_temp_token != null)
                {
                    insertTokenIntoList(_temp_token->id,_temp_token->name,_temp_token->value,&fileTokens);
                }
                _temp_token = findToken(";",tokens);
                if(_temp_token!=null)
                {
                    insertTokenIntoList(_temp_token->id,_temp_token->name,_temp_token->value,&fileTokens);
                }
                insertTokenIntoList(id++,"NEWJOB_NAME",tokena,&fileTokens);
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
        if(constantsSection)
        {
            if(check_match(BLK_CONSTANTS_REGEX,trimmedBuffer))
            {
                continue;
            }
            else if(check_match(STMT_ASSIGNMENT_REGEX,trimmedBuffer))
            {
                char* tokena = strtok(trimmedBuffer,"=");
                char* tokenb = trim(strtok(NULL,";"));
                token* _temp_token = findToken(tokena,fileTokens);
                if(_temp_token == null)
                {
                    insertTokenIntoList(id++,"CONSTANT",tokena,&fileTokens);
                    insertConstant(tokena, tokenb);
                }
                continue;
            }
            else if(check_match(BLK_VARIABLES_REGEX,trimmedBuffer))
            {
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
        if(variablesSection)
        {
            if(check_match(VRB_PRIMITIVE_INST,trimmedBuffer))
            {
                char* tokena = NULL;
                setString(&tokena,trim(strtok(NULL,";")));
                continue;
            }
            else if(check_match(VRB_ARRAY_INST,trimmedBuffer))
            {
                continue;
            }
            else if(check_match(BLK_BEGIN_BLOCK_REGEX,trimmedBuffer))
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

    printTokensFromList(fileTokens);
    printf("\n");

	return 0;
}

