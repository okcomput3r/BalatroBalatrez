#include <Utils/logs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

FILE *log;

/***
 *
 * innitLogs() creates the dump files and reddirects the standard 
 * output so every print goes to the log.out file
 * 
 * @return int -> returns -1 if the files couldn't be open
 * 
*/
int innitLogs()
{
    log = freopen("out.txt", "w", stdout);

    if(log == NULL) { return -1; }

    return 0;
}

/***
 *
 *  Update the log content by closing it and reopening in append
 *  mode
 *
*/

void updateLogs()
{
    fclose(log);
    log = freopen("out.txt", "a", stdout);
}

/**
 *  Closes the logs
 *
 *
 *
 *
 */

void terminateLogs()
{
    fclose(log);
}