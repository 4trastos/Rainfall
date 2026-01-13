#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#define _GNU_SOURCE

int main (int argc, char **argv)
{
    int     number;
    gid_t   gid;
    uid_t   uid;
    char    *argmts[2];

    number = atoi(argv[1]);
    if (number != 423)
        fwrite("No !\n", 1, 5, stderr);
    else
    {
        argmts[0] = strdup("/bin/sh");
        argmts[1] = NULL;

        gid = getegid();
        uid = geteuid();
        setresgid(gid, gid, gid);
        setresuid(uid, uid, uid);

        execv("/bin/sh", argmts);
    }
    return (0);
}