#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2)
      { 
       printf("Usage: sshstart password\n");
       return 0;
      }

char *part1 = "su -c /data/jocala/ssh/dropbear -A -N root -U 0 -G 0 -C ";
char *part2 = " -r /data/jocala/ssh/dropbear_rsa_host_key";
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
printf("sshstart v1.0 by jocala@jocala.com\n");
return 0;
}

strcpy (str, part1);
strcat (str, password);
strcat (str, part2);

system(str);
printf ("ssh started with password: \"%s\".\n",password);

return 0;
}

