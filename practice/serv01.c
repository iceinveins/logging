/* include serv01 */
#include	"unp.c"

#define	MAXN	16384		/* max # bytes client can request */

void
web_child(int sockfd)
{
	int			ntowrite;
	ssize_t		nread;
	char		line[MAXLINE], result[MAXN];

	for ( ; ; ) {
		if ( (nread = read(sockfd, line, MAXLINE)) == 0)
			return;		/* connection closed by other end */

			/* 4line from client specifies #bytes to write back */
		// ntowrite = atol(line);
		printf("server got: %s\n", line);
		// if ((ntowrite <= 0) || (ntowrite > MAXN))
		// 	err_quit("client request for %d bytes", ntowrite);

		// write(sockfd, result, ntowrite);
	}
}
void
sig_int(int signo)
{
	exit(0);
}

void
sig_chld(int signo)
{
	return;		/* just interrupt door_call() */
}

int
main(int argc, char **argv)
{
	int					listenfd, connfd;
	pid_t				childpid;
	void				sig_chld(int), sig_int(int), web_child(int);
	socklen_t			clilen, addrlen;
	struct sockaddr		*cliaddr;

	if (argc == 2)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else
		err_quit("usage: serv01 <port>");
	cliaddr = malloc(addrlen);

	signal(SIGCHLD, sig_chld);
	signal(SIGINT, sig_int);

	for ( ; ; ) {
		clilen = addrlen;
		if ( (connfd = accept(listenfd, cliaddr, &clilen)) < 0) {
			if (errno == EINTR)
				continue;		/* back to for() */
			else
				err_sys("accept error");
		}

		if ( (childpid = fork()) == 0) {	/* child process */
			close(listenfd);	/* close listening socket */
			web_child(connfd);	/* process request */
			exit(0);
		}
		close(connfd);			/* parent closes connected socket */
	}
}
/* end serv01 */
