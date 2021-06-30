#include "csapp.h"

int main(int argc, char **argv) {
  struct addrinfo *p, *listp, hints;
  char buf[MAXLINE];
  char buf2[MAXLINE]; // bufor dla dodatkowego argumentu
  int rc, flags;

  if (argc != 2 && argc != 3)
    app_error("usage: %s <domain name>\n", argv[0]);
  
  /* Get a list of addrinfo records */
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC; // przyjmuje zarówno IPv4, jak i IPv6
  hints.ai_socktype = SOCK_DGRAM; // SOCK_DGRAM aby obsługiwać tftp
  /* Connections only */
  char* service = NULL;
  if(argc == 3) service = argv[2];

  if ((rc = getaddrinfo(argv[1], service, &hints, &listp)) != 0)
    gai_error(rc, "getaddrinfo");

  /* Walk the list and display each IP address */
  flags = NI_NUMERICHOST | NI_NUMERICSERV; /* Display address string instead of domain name */
  for (p = listp; p; p = p->ai_next) {
    Getnameinfo(p->ai_addr, p->ai_addrlen, buf, MAXLINE, buf2, MAXLINE, flags);
    if(p->ai_family == PF_INET) 
      printf("%s", buf); //IPv4 
    else 
      printf("[%s]", buf); //IPv6
    if(service != NULL)
      printf(":%s", buf2);        
    printf("\n");
  }

  /* Clean up */
  freeaddrinfo(listp);

  return EXIT_SUCCESS;
}
