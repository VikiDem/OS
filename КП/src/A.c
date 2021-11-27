#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include "getl.h"

int id1, id2;
int pAC[2];
int pAB[2];
int pCA[2];
int pCB[2];

int main(){
	pipe(pAC);
	pipe(pAB);
	pipe(pCA);
    pipe(pCB);

    
	id1 = fork();
    if (id1 == -1)
	{
		perror("Fork trouble!\n");
		return 1;
	}
    else if(id1 == 0)
    {
        char dAB[3] = "";
        sprintf(dAB, "%d", pAB[0]);
        char dCB[3] = "";
        sprintf(dCB, "%d", pCB[0]);

        close(pAC[0]);
        close(pAC[1]);
        close(pCA[0]);
        close(pCA[1]);
        close(pAB[1]);
        close(pCB[1]);

        char fileName[] = "./B";
        execl(fileName, fileName, dAB, dCB, NULL);
    }
	else
	{		
		id2 = fork();
    	if (id2 == -1)
		{
		    perror("fork2");
		    return 1;
		}
		else if(id2 == 0)
		{
            char dAC[3] = "";
            sprintf(dAC, "%d", pAC[0]);
            char dCA[3] = "";
            sprintf(dCA, "%d", pCA[1]);
            char dCB[3] = "";
            sprintf(dCB, "%d", pCB[1]);

            close(pAC[1]);
            close(pCA[0]);
            close(pAB[0]);
            close(pAB[1]);
            close(pCB[0]);

            char name[] = "./C";
			execl(name, name, dAC, dCA, dCB, NULL);
		}
		else
		{
			char* line = NULL;
			int size;
			while((size = get_line(&line, 0)) != 0)
			{
	            write(pAC[1], &size, sizeof(int));
	            write(pAC[1], line, size*sizeof(char));

                int ok;
                read(pCA[0], &ok, sizeof(int));

                write(pAB[1], &size, sizeof(int));
			}
			free(line);

            kill(id1, 10);
            kill(id2, 10);
		}	
	}


    close(pAC[0]);
    close(pAC[1]);
    close(pCA[0]);
    close(pCA[1]);
    close(pAB[0]);
    close(pAB[1]);
    close(pCB[0]);
    close(pCB[1]);

    
}
