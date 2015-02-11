#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2)
      { 
       printf("Usage: remount rw|ro -- mounts /system\n");
       return 0;
      }

char str[200];

    int i;
    int strsize = 0;
    for (i=1; i<argc; i++) {
        strsize += strlen(argv[i]);
        if (argc > i+1)
            strsize++;
    }



    char *password;
    password = malloc(strsize);
    password[0] = '\0';

    for (i=1; i<argc; i++) {
        strcat(password, argv[i]);
        if (argc > i+1)
            strcat(password, " ");
    }


if (strstr(password, "-v") != NULL)
{
printf("remount v1.0 by jocala@jocala.com\n");
return 0;
}

if (strstr(password, "rw") != NULL)
{
system("su -c mount -o remount,rw /system");
printf("/system is r/w\n");
return 0;
}

if (strstr(password, "ro") != NULL)
{
system("su -c mount -o remount,ro /system");
printf("/system is r/o\n");
return 0;
}

return 0;

}

