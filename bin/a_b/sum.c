#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "../http_request.h"

static char* check(char* arg);

int main()
{
    char *buf, *p;
    char arg1[33], arg2[33], body[128], header[128];
    int n1 = 0, n2 = 0;
    
    if((buf = getenv("cal_a_b")) != NULL)
    {
        p = strchr(buf, '&');
        *p = '\0';
        strcpy(arg1, buf);
        strcpy(arg2, p+1);
        n1 = atoi(check(arg1));
        n2 = atoi(check(arg2));
        sprintf(body, "%sThe answer is: %d + %d = %d\r\n<p>", body, n1, n2, n1 + n2);
    }
    else sprintf(body, "Welcome to add.com.");
    sprintf(header, H200NONKEEP, strlen(body));

    printf("%s", header);
    printf("%s", body);
    fflush(stdout);
    return 0;
}

char* check(char* arg)
{
    char *p;
    if((p = strchr(arg, '=')))
        strncpy(arg, p+1, 30);
    return arg;
}

