#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2)
      { 
       printf("Usage: sshinit password\n");
       printf("write new init.d/02sshd\n");
       return 0;
      }

char str[256];
char hashbang[17]; 
strcpy (hashbang,"#!/system/bin/sh");
char sshstart[256];
strcpy (sshstart,"/system/xbin/sshstart ");

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
printf("sshinit v1.0 by jocala@jocala.com\n");
return 0;
}

system("su -c mount -o remount,rw /system");
FILE *f = fopen("/system/etc/init.d/02sshd", "w");
if (f == NULL)
{
    printf("Error opening file!\n");
    exit(1);
}

strcat (sshstart, password);
fprintf(f, "%s\n", hashbang);
fprintf(f, "%s\n", sshstart);
fclose(f);
system("su -c chmod 0755 /system/etc/init.d/02sshd");
system("su -c mount -o remount,ro /system");
printf("/system/etc/init.d/02sshd written\n");


return 0;

}





