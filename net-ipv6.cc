/*
 * net-ipv6.cc -- IPv6 Network  (based upon ip-net.cc)
 */

/*-
 * Copyright (c) 1993-1994 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the University of
 *      California, Berkeley and the Network Research Group at
 *      Lawrence Berkeley Laboratory.
 * 4. Neither the name of the University nor of the Laboratory may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

static const char rcsid[] =
    "@(#) $Header$";

#ifdef HAVE_IPV6

#include <stdio.h>
#include <errno.h>
#include <string.h>
#ifdef WIN32
#include <io.h>
#define close closesocket
#else
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#endif
#if defined(sun) && defined(__svr4__)
#include <sys/systeminfo.h>
#endif

#include "config.h"
#include "net.h"
#include "Tcl.h"

#include "inet6.h"
#include "net-addr.h"

struct in6_addr const vic_in6addr_any = IN6ADDR_ANY_INIT;

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN (46)
#endif

#ifndef IS_SAME_IN6_ADDR
#define IS_SAME_IN6_ADDR(a1,a2) IN6_ARE_ADDR_EQUAL(a1,a2)
#endif

class IP6Address : public Address {
  public:
	IP6Address() { text_ = new char[INET6_ADDRSTRLEN]; };
	virtual int operator=(const char*);
	int operator=(const struct in6_addr& addr);

	virtual Address* copy() const;
	virtual size_t length() const { return sizeof(addr_);}
	virtual operator const void*() const { return &addr_;}

	operator struct in6_addr() const { return addr_; }
	operator const struct in6_addr&() const { return addr_; }

  protected:
	struct in6_addr addr_;
};


static class IP6AddressType : public AddressType {
public:
  virtual Address* resolve(const char* name) {
  struct in6_addr addr;
    IP6Address * result = 0;
    if (inet6_LookupHostAddr(&addr, name) >= 0) {
      result = new IP6Address;
      *result = addr;
    } else {
      result = 0;
    }
    return (result);
  }
} ip6_address_type;


class IP6Network : public Network {
  public:
	IP6Network() : Network(*(new IP6Address), *(new IP6Address)) {;}
	virtual int command(int argc, const char*const* argv);
	virtual void reset();
  protected:
	virtual int dorecv(u_char* buf, int len, Address& from, int fd);
	int open(const char * host, int port, int ttl);
	int close();
	int localname(sockaddr_in6*);
	int openssock(Address & addr, u_short port, int ttl);
	int openrsock(Address & addr, u_short port, Address & local);
	time_t last_reset_;
	unsigned int flowLabel_;
};

static class IP6NetworkMatcher : public Matcher {
    public:
	IP6NetworkMatcher() : Matcher("network") {}
	TclObject* match(const char* id) {
		if (strcasecmp(id, "ip6") == 0)
			return (new IP6Network);
		else
			return (0);
	}
} nm_ip6;

int IP6Address::operator=(const char* text)  {
  inet6_LookupHostAddr(&addr_, text);
  return ((inet_ntop(AF_INET6, &addr_, text_, INET6_ADDRSTRLEN) != 0) ?
	  (0) : (1));
}

Address * IP6Address::copy() const {
  IP6Address * result = new IP6Address;
  *result = addr_;
  return (result);
}

int IP6Address::operator=(const struct in6_addr& addr) {
  memcpy(&addr_, &addr, sizeof(addr));
  return ((inet_ntop(AF_INET6, &addr_, text_, INET6_ADDRSTRLEN) != 0) ?
	  (0) : (1));
}


int IP6Network::command(int argc, const char*const* argv)
{
	Tcl& tcl = Tcl::instance();
	if (argc == 2) {
		if (strcmp(argv[1], "close") == 0) {
			close();
			return (TCL_OK);
		}
		char* cp = tcl.result();
		if (strcmp(argv[1], "addr") == 0) {
/* __IPV6 use Address */
			strcpy(cp, addr_);
			return (TCL_OK);
		}
		if (strcmp(argv[1], "interface") == 0) {
/* __IPV6 use Address */
			strcpy(cp, local_);
			return (TCL_OK);
		}
		if (strcmp(argv[1], "port") == 0) {
			sprintf(cp, "%d", ntohs(port_));
			return (TCL_OK);
		}
		if (strcmp(argv[1], "localport") == 0) {
			sprintf(cp, "%d", ntohs(lport_));
			return (TCL_OK);
		}
		if (strcmp(argv[1], "ttl") == 0) {
			sprintf(cp, "%d", ttl_);
			return (TCL_OK);
		}
/* __IPV6 use IN6_IS_ADDR */
		if (strcmp(argv[1], "ismulticast") == 0) {
			const in6_addr & addr = (IP6Address&)addr_;
			tcl.result(IN6_IS_ADDR_MULTICAST(&addr)? "1" : "0");
			return (TCL_OK);
/* __IPV6 user IPV6_MULTICAST_LOOP */
		}
	} else if (argc == 3) {
		if (strcmp(argv[1], "loopback") == 0) {
			char c = atoi(argv[2]);
			if (setsockopt(ssock_, IPPROTO_IP, 
				       IPV6_MULTICAST_LOOP, &c, 1) < 0) {
				/*
				 * If we cannot turn off loopback (Like on the
				 * Microsoft TCP/IP stack), then declare this
				 * option broken so that our packets can be
				 * filtered on the recv path.
				 */
				if (c == 0)
					noloopback_broken_ = 1;
			}
			return (TCL_OK);
		}
	} else if (argc == 5) {
		if (strcmp(argv[1], "open") == 0) {
/* __IPV6 use v6 lookup */
			const char * host = argv[2];
			int port = htons(atoi(argv[3]));
			int ttl = atoi(argv[4]);
			flowLabel_ = htonl(atoi(tcl.attr("flowLabel")));
			if (open(host, port, ttl) < 0)
				tcl.result("0");
			else
				tcl.result("1");
			return (TCL_OK);
		}
	}
	return (Network::command(argc, argv));
}

int IP6Network::open(const char * host, int port, int ttl)
{
	addr_ = host;
	port_ = port;
	ttl_ = ttl;

	ssock_ = openssock(addr_, port, ttl);
	if (ssock_ < 0)
		return (-1);
	/*
	 * Connecting the send socket also bound the local address.
	 * On a multihomed host we need to bind the receive socket
	 * to the same local address the kernel has chosen to send on.
	 */
	struct sockaddr_in6 local;
	if (localname(&local) < 0) {
		return (-1);
	}
	(IP6Address&)local_ = local.sin6_addr;
	rsock_ = openrsock(addr_, port, local_);
	if (rsock_ < 0) {
		(void)::close(ssock_);
		return (-1);
	}
	lport_ = local.sin6_port;
	last_reset_ = 0;
	return (0);
}

int IP6Network::close()
{
	if (ssock_ >= 0) {
		::close(ssock_);
		::close(rsock_);
		ssock_ = rsock_ = -1;
	}
	return (0);
}

int IP6Network::localname(sockaddr_in6* p) {

  memset((char *)p, 0, sizeof(*p));
  p->sin6_family = AF_INET6;
  int len = sizeof(*p), result = 0;

  if ((result = getsockname(ssock_, (struct sockaddr *)p, &len)) < 0) {
    perror("getsockname");
    p->sin6_addr = vic_in6addr_any;
    p->sin6_port = 0;
  }

#ifdef DAS_IPV6
  if (IS_SAME_IN6_ADDR(p->sin6_addr, vic_in6addr_any)) {
#else 
  if (IN6_ARE_ADDR_EQUAL(&(p->sin6_addr), &vic_in6addr_any)) {
#endif
    result = inet6_LookupLocalAddr(&p->sin6_addr);
  }

  return (result);

}

void IP6Network::reset()
{
	time_t t = time(0);
	int d = int(t - last_reset_);
	if (d > 3) {
		last_reset_ = t;
		(void)::close(ssock_);
		ssock_ = openssock(addr_, port_, ttl_);
	}
}

int IP6Network::openrsock(Address & addr, u_short port, Address & local)
{
	int fd;
	struct sockaddr_in6 sin;

	fd = socket(AF_INET6, SOCK_DGRAM, 0);
	if (fd < 0) {
		perror("socket");
		exit(1);
	}
	nonblock(fd);
	int on = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on,
			sizeof(on)) < 0) {
		perror("SO_REUSEADDR");
	}
#ifdef SO_REUSEPORT
	on = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (char *)&on,
		       sizeof(on)) < 0) {
		perror("SO_REUSEPORT");
		exit(1);
	}
#endif
	memset((char *)&sin, 0, sizeof(sin));
	sin.sin6_family = AF_INET6;
	sin.sin6_addr = (IP6Address&)addr;
	sin.sin6_port = port;
#ifdef IPV6_ADD_MEMBERSHIP
	if (IN6_IS_ADDR_MULTICAST(&sin.sin6_addr)) {
		/*
		 * Try to bind the multicast address as the socket
		 * dest address.  On many systems this won't work
		 * so fall back to a destination of INADDR_ANY if
		 * the first bind fails.
		 */
/* __IPV6 memcopy address */
		if (bind(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
			sin.sin6_addr = vic_in6addr_any;
			if (bind(fd, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
				perror("bind");
				exit(1);
			}
		}
		/* 
		 * XXX This is bogus multicast setup that really
		 * shouldn't have to be done (group membership should be
		 * implicit in the IP class D address, route should contain
		 * ttl & no loopback flag, etc.).  Steve Deering has promised
		 * to fix this for the 4.4bsd release.  We're all waiting
		 * with bated breath.
		 */
#if defined(__sun__) || defined(WIN32)
                struct ipv6_mreq mr;
#endif
#if defined(__FreeBSD__)
                struct oipv6_mreq mr;
#endif

/* __IPV6 memcopy address */
#ifdef DAS_IPV6
		mr.i6mr_interface = 1;
		mr.i6mr_multiaddr = (IP6Address&)addr;
#else
		mr.ipv6mr_interface = 0;
		mr.ipv6mr_multiaddr = (IP6Address&)addr;
#endif


#ifdef DAS_IPV6
		if (setsockopt(fd, IPPROTO_IP, IPV6_ADD_MEMBERSHIP, 
			       (char *)&mr, sizeof(mr)) < 0) {
			perror("IPV6_ADD_MEMBERSHIP");
#else
		if (setsockopt(fd, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, 
			       (char *)&mr, sizeof(mr)) < 0) {
			perror("IPV6_ADD_MEMBERSHIP");
#endif
			exit(1);
		}
	} else
#endif
	{
		/*
		 * bind the local host's address to this socket.  If that
		 * fails, another vic probably has the addresses bound so
		 * just exit.
		 */
/* __IPV6 memcopy address */
		sin.sin6_addr = (IP6Address&)local;
		if (bind(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
			perror("bind");
			exit(1);
		}
		/*
		 * Despite several attempts on our part to get this fixed,
		 * Microsoft Windows isn't complient with the Internet Host
		 * Requirements standard (RFC-1122) and won't let us include
		 * the source address in the receive socket demux state.
		 * (The consequence of this is that all conversations have
		 * to be assigned a unique local port so the vat 'side
		 * conversation' (middle click on site name) function is
		 * essentially useless under windows.)
		 */
#ifndef WIN32
		/*
		 * (try to) connect the foreign host's address to this socket.
		 */
		sin.sin6_port = 0;
/* __IPV6 memcopy address */
		sin.sin6_addr = (IP6Address&)addr;
		connect(fd, (struct sockaddr *)&sin, sizeof(sin));
#endif
	}

	/*
	 * XXX don't need this for the session socket.
	 */	
	int bufsize = 80 * 1024;
	if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&bufsize,
			sizeof(bufsize)) < 0) {
		bufsize = 32 * 1024;
		if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&bufsize,
				sizeof(bufsize)) < 0)
			perror("SO_RCVBUF");
	}
	return (fd);
}	

int IP6Network::openssock(Address & addr, u_short port, int ttl) 
{
	int fd;
	struct sockaddr_in6 sin;


	fd = socket(AF_INET6, SOCK_DGRAM, 0);
	if (fd < 0) {
		perror("socket");
		exit(1);
	}
	nonblock(fd);

	memset((char *)&sin, 0, sizeof(sin));
	sin.sin6_family = AF_INET6;
	sin.sin6_port = 0;
	sin.sin6_flowinfo = flowLabel_;
/* __IPV6 memcopy address */
	sin.sin6_addr = vic_in6addr_any;
	if (bind(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		perror("bind");
		exit(1);
	}

	memset((char *)&sin, 0, sizeof(sin));
	sin.sin6_family = AF_INET6;
/* __IPV6 memcopy address */
	sin.sin6_addr = (IP6Address&)addr;
	sin.sin6_port = port;
	sin.sin6_flowinfo = flowLabel_;
	if (connect(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		perror("connect");
		exit(1);
	}
	if (IN6_IS_ADDR_MULTICAST(&sin.sin6_addr)) {
#ifdef IPV6_ADD_MEMBERSHIP
		char c;

		/* turn off loopback */
		c = 0;
#ifdef DAS_IPV6
		if (setsockopt(fd, IPPROTO_IP, IPV6_MULTICAST_LOOP, &c, 1) < 0) {
#else
		if (setsockopt(fd, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &c, 1) < 0) {
#endif
			/*
			 * If we cannot turn off loopback (Like on the
			 * Microsoft TCP/IP stack), then declare this
			 * option broken so that our packets can be
			 * filtered on the recv path.
			 */
			if (c == 0)
				noloopback_broken_ = 1;
		}
		/* set the multicast TTL */
//#if defined(SOLARIS7_IPV6) || defined(WIN32) && !defined(DAS_IPV6)
		u_int t;
//#else
//		INT32_T t;
//#endif
		t = (ttl > 255) ? 255 : (ttl < 0) ? 0 : ttl;

#ifdef DAS_IPV6
		if (setsockopt(fd, IPPROTO_IP, IPV6_MULTICAST_HOPS,
			       (const char*)&t, sizeof(t)) < 0) {
#else
		if (setsockopt(fd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS,
			       (const char*)&t, sizeof(t)) < 0) {
#endif
			perror("IPV6_MULTICAST_HOPS");
			exit(1);
		}
#else
		fprintf(stderr, "\
not compiled with support for IP multicast\n\
you must specify a unicast destination\n");
		exit(1);
#endif
	}
	/*
	 * XXX don't need this for the session socket.
	 */
	int bufsize = 80 * 1024;
	if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *)&bufsize,
		       sizeof(bufsize)) < 0) {
		bufsize = 48 * 1024;
		if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *)&bufsize,
			       sizeof(bufsize)) < 0)
			perror("SO_SNDBUF");
	}
	return (fd);
}

int IP6Network::dorecv(u_char* buf, int len, Address& from, int fd)
{
	sockaddr_in6 sfrom;
	int fromlen = sizeof(sfrom);
	int cc = ::recvfrom(fd, (char*)buf, len, 0,
			    (sockaddr*)&sfrom, &fromlen);
	if (cc < 0) {
		if (errno != EWOULDBLOCK)
			perror("recvfrom");
		return (-1);
	}
	(IP6Address&)from = sfrom.sin6_addr;

	if (noloopback_broken_ && from == local_ && sfrom.sin6_port == lport_)
		    return (0);

	return (cc);
}

#endif /* HAVE_IPV6 */