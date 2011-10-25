#include <stdio.h>
#include <unistd.h>

int main(int argc)
{
    char currentDir[1024];
    char input[200];
    
    while(1) {
        printf("%s >", getcwd(currentDir, sizeof(currentDir)));
        fgets(input, sizeof(input), stdin);
        printf("%s", input);
    }
    
    return 0;
}