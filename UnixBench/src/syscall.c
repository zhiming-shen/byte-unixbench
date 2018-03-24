/*******************************************************************************
 *  The BYTE UNIX Benchmarks - Release 3
 *          Module: syscall.c   SID: 3.3 5/15/91 19:30:21
 *
 *******************************************************************************
 * Bug reports, patches, comments, suggestions should be sent to:
 *
 *	Ben Smith, Rick Grehan or Tom Yager at BYTE Magazine
 *	ben@bytepb.byte.com   rick_g@bytepb.byte.com   tyager@bytepb.byte.com
 *
 *******************************************************************************
 *  Modification Log:
 *  $Header: syscall.c,v 3.4 87/06/22 14:32:54 kjmcdonell Beta $
 *  August 29, 1990 - Modified timing routines
 *  October 22, 1997 - code cleanup to remove ANSI C compiler warnings
 *                     Andy Kahn <kahn@zk3.dec.com>
 *
 ******************************************************************************/
/*
 *  syscall  -- sit in a loop calling the system
 *
 */
char SCCSid[] = "@(#) @(#)syscall.c:3.3 -- 5/15/91 19:30:21";

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "timeit.c"
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>

unsigned long iter;

#define O_TMPFILE __O_TMPFILE
#define FNAME_SIZE 20

char fname[FNAME_SIZE];

void report()
{
	fprintf(stderr,"COUNT|%ld|1|lps\n", iter);
	unlink(fname);
	exit(0);
}

int main(argc, argv)
int	argc;
char	*argv[];
{
        char   *test;
	int	duration;
	int file_fd;
	
	int pid;
	pid = getpid();
	sprintf(fname, "/tmp/%d", pid);
		
	if (argc < 2) {
		fprintf(stderr,"Usage: %s duration [ test ]\n", argv[0]);
                fprintf(stderr,"test is one of:\n");
                fprintf(stderr,"  \"mix\" (default), \"close\", \"getpid\", \"exec\"\n");
		exit(1);
	}
        if (argc > 2)
            test = argv[2];
        else
            test = "mix";

	duration = atoi(argv[1]);

	iter = 0;
	wake_me(duration, report);

	if((file_fd = creat(fname, 0600)) == -1) {
            perror("syscall: creat");
            exit(1);
	}
	
	if(file_fd <= 0)
	  report();

        switch (test[0]) {
        case 'm':
	   while (1) {	     
		close(dup(file_fd));
		getpid();
		getuid();
		umask(022);
		iter++;		
	   }
	   /* NOTREACHED */
        case 'c':
           while (1) {
                close(dup(0));
                iter++;
           }
           /* NOTREACHED */
        case 'g':
           while (1) {
                getpid();
                iter++;
           }
           /* NOTREACHED */
        case 'e':
           while (1) {
                pid_t pid = fork();
                if (pid < 0) {
                    fprintf(stderr,"%s: fork failed\n", argv[0]);
                    exit(1);
                } else if (pid == 0) {
                    execl("/bin/true", "/bin/true", (char *) 0);
                    fprintf(stderr,"%s: exec /bin/true failed\n", argv[0]);
                    exit(1);
                } else {
                    if (waitpid(pid, NULL, 0) < 0) {
                        fprintf(stderr,"%s: waitpid failed\n", argv[0]);
                        exit(1);
                    }
                }
                iter++;
           }
           /* NOTREACHED */
        }

        exit(9);
}

