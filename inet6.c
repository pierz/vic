/*
 * inet6.cc -- netdb functions for IPv6 addresses
 */

/*
 * Copyright (c) 1991 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Lawrence Berkeley Laboratory,
 * Berkeley, CA.  The name of the University may not be used to
 * endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

static const char rcsid[] =
    "@(#) $Header$";

#ifdef HAVE_IPV6

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef WIN32
#include <winsock.h>
#else
#include <sys/param.h>
#include <netdb.h>
#include <sys/socket.h>
#endif
#include <stdio.h>

#include "config.h"

#include "inet6.h"

int 
inet6_LookupHostAddr(struct in6_addr *addr, const char* s) {
  if (inet_pton(AF_INET6, s, addr->s6_addr) != 1) {
    struct hostent *hp;
#ifdef SOLARIS7_IPV6
    int error_num;
    hp = getipnodebyname(s, AF_INET6, AI_DEFAULT, &error_num);
    if (error_num!=0) return (-1);
#else
    hp = gethostbyname2(s, AF_INET6);
    if (hp == 0)  return (-1);
#endif
    memcpy(addr->s6_addr, *(hp->h_addr_list), hp->h_length);
  }
  return (0);
}


int 
inet6_LookupLocalAddr(struct in6_addr *addr) {
  static int once = 0;
  struct in6_addr local_addr;

  if (!once) {
    char name[MAXHOSTNAMELEN];
    gethostname(name, sizeof(name));
    if (inet6_LookupHostAddr(&local_addr, name) != 0)
      return (-1);
    once++;
  }
  memcpy(addr->s6_addr, local_addr.s6_addr, sizeof(local_addr.s6_addr));
  return (0);
}


int 
inet6_InetNtoa(char *address, struct in6_addr *addr) {
  address = (char *)malloc(INET6_ADDRSTRLEN);
  return ((inet_ntop(AF_INET6, addr, address, INET6_ADDRSTRLEN) != NULL) 
	  ? (-1)
	  : (0));
}

int
inet6_LookupHostName(char* name, struct in6_addr *addr) {
  char *p;
  struct hostent* hp;

  if (IN6_IS_ADDR_MULTICAST(addr))  return (inet6_InetNtoa(name, addr));

  hp = gethostbyaddr(addr->s6_addr, sizeof(addr->s6_addr), AF_INET6);

  if (hp == 0) return inet6_InetNtoa(name, addr);

  name = (char *)malloc(strlen(hp->h_name) + 1);
  strcpy(name, hp->h_name);
  return (0);
}



#endif /* HAVE_IPV6 */
