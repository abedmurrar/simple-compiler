#include "compiler.h"

int main(int argc, char const *argv[])
{
    char * a = null;
    setString(&a,trim("      abed    "));
    printf("%d\n",'\0');
    printf("%s\n",a);
    return 0;
}
