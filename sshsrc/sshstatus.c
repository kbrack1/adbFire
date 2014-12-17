#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {


    int i;
    int strsize = 0;
    for (i=1; i<argc; i++) {
        strsize += strlen(argv[i]);
        if (argc > i+1)
            strsize++;
    }



    char *args;
    args = malloc(strsize);
    args[0] = '\0';

    for (i=1; i<argc; i++) {
        strcat(args, argv[i]);
        if (argc > i+1)
            strcat(args, " ");
    }


if (strstr(args, "-v") != NULL)

{
printf("sshstatus v1.0 by jocala@jocala.com\n");
return 0;
}




FILE *lsofFile_p = popen("pidof dropbear", "r");

  if (!lsofFile_p)
  {
    return -1;
  }

  char buffer[1024];
  char *line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
  pclose(lsofFile_p);



if (strlen(buffer) > 0)
{
printf ("%s\n","ssh running");	
}

else

{
printf ("%s\n","ssh not running");	
}

return 0;
}

