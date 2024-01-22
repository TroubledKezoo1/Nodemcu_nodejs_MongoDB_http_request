#include <unistd.h>
#include <stdio.h>
int ft_strlen(char *src)
{
    int i;

    i = 0;
    while (src[i] != '\0')
    {
        i++;
    }
    return(i);
}

int main() {
    char *src = "yunus iremi cok seviyor";

    int i;

    i = 0;
    while (src[i] != '\0');
    {
        write(1,&src[i],1);
        i++;
    }
    return 0;
}
