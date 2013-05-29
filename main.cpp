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
 * 3. Neither the names of the copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

static const char rcsid[] =
"@(#) $Header$ (LBL)";


#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>

#ifdef WIN32
//#include <winsock.h>
#include <process.h>
#include <time.h>
#else
#include <sys/param.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/file.h>
#include <sys/utsname.h>
#endif

#ifdef sgi
#include <getopt.h>
#include <math.h>
#endif

#include "inet.h"
#include "vic_tcl.h"
#include "transmitter.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

extern "C" {
#include <tk.h>
#ifdef USE_SHM
#ifdef sgi
#define XShmAttach __XShmAttach__
#endif
#include <X11/extensions/XShm.h>
#ifdef _AIX
	extern Bool XShmQueryExtension (Display *dpy);
#endif
#ifdef sgi
#undef XShmAttach
	int XShmAttach(Display*, XShmSegmentInfo*);
#endif
#include <sys/ipc.h>
#include <sys/shm.h>
#if defined(sun) && !defined(__svr4__)
	int shmget(key_t, int, int);
	int shmctl(int shmid, int cmd, struct shmid_ds *);
#endif
#endif
}

#if defined(sun) && !defined(__svr4__)
extern "C" int gettimeofday(struct timeval*, struct timezone*);
extern "C" int nice(int incr);
#endif
#if defined(sun) || defined(__osf__)
extern "C" int gethostname(char* name, int len);
#endif


/*XXX*/
#define PROTOTYPES 1 
#include "global.h"
#include "md5.h"

#ifdef USE_PROFILING
#include "profiler.h"
#endif

#ifdef USE_DDRAW
#include "vw.h"
#endif

#if defined(sun) && defined(__svr4__)
#include <sys/utsname.h>
#define gethostname(name, len) { \
	struct utsname _uts_; \
	\
	if (uname(&_uts_) < 0) { \
		perror("uname"); \
		exit(1); \
	}\
	\
	strcpy((name), _uts_.nodename); \
}
#endif

static void
usage(const char *szOffending)
{
	char win_usage[] = "\
    \nVIC is a multicast (or unicast) video tool. It is best to start it\n\
    using a multicast directory tool, like sdr or multikit. If desired VIC\n\
    can be launched from the command line using:\n\n\
    vic <address>/<port>\n\n\
    where <address> is machine name, or a multicast IP address, and <port> is\n\
    the connection identifier (an even number between 1024-65536).\n\n\
    For more details see:\n\
    http://www-mice.cs.ucl.ac.uk/multimedia/software/vic/faq.html\n\n\
Options: vic [-HPs] [-A nv|ivs|rtp] [-B maxbps] [-C conf]\n\
	\t[-c ed|gray|od|quantize] [-D device] [-d display]\n\
	\t[-f bvc|cellb|h261|jpeg|nv|mpeg4|h264] [-F maxfps] [-i ifAddr ] [-I channel]\n\
	\t[-K key ] [-L flowLabel (ip6 only)] [-l (creates log file)]\n\
	\t[-M colormap] [-m mtu] [-N session] [-n atm|ip|ip6|rtip]\n\
	\t[-o clipfile] [-Q (queries and lists input devices)] [-t ttl]\n\
	\t[-U interval] [-u script] [-v version] [-V visual]\n\
	\t[-x ifIndex (ip6 only)] [-X resource=value] [-j numlayers] dest/port[/fmt/ttl]\n";

	if (szOffending == NULL) {
		szOffending = win_usage;
	}

#ifdef WIN32
	MessageBox(NULL, szOffending, "VIC Usage", MB_ICONINFORMATION | MB_OK);
#else
	printf(win_usage);
	if (szOffending) {
		printf(szOffending);
	}
#endif
	exit(1);
}

static class UsageCommand : public TclObject {
	public:
		UsageCommand() : TclObject("usage") {}
		int command(int argc, const char*const* argv) {
			UNUSED(argc);
			UNUSED(argv);
			usage(NULL);
			/*NOTREACHED*/
			return (0);
		}
} cmd_usage;

#ifndef SIGARGS
#if defined(__SUNPRO_CC) || defined(Linux) || defined(__FreeBSD__) || defined(WIN32)
#define SIGARGS int arg
#else
#define SIGARGS ... 
#endif
#endif

extern void adios();

	static SIGRET
ciao(SIGARGS)
{
	adios();
}

static class AdiosCommand : public TclObject {
	public:
		AdiosCommand() : TclObject("adios_rtcp") {}
		int command(int argc, const char*const* argv) {
			UNUSED(argc);
			UNUSED(argv);
			adios();
			/*NOTREACHED*/
			return (0);
		}
} cmd_adios;

static class HaveFontCommand : public TclObject {
	public:
		HaveFontCommand() : TclObject("havefont") {}
		int command(int argc, const char*const* argv) {
			Tcl& t = Tcl::instance();
			if (argc != 2)
				t.result("0");
			else {
				Tk_Window tk = t.tkmain();
				Tk_Uid uid = Tk_GetUid((char*)argv[1]);
				Tk_Font p = Tk_GetFont(t.interp(), tk, uid);
				t.result(p != 0 ? "1" : "0");
			}
			return (TCL_OK);
		}
} cmd_havefont;

static class GetHostNameCommand : public TclObject {
	public:
		GetHostNameCommand() : TclObject("gethostname") {}
		int command(int argc, const char*const* argv) {
			UNUSED(argc);
			UNUSED(argv);
			Tcl& tcl = Tcl::instance();
			char* bp = tcl.buffer();
			tcl.result(bp);
			gethostname(bp, MAXHOSTNAMELEN);
			return (TCL_OK);
		}
} cmd_gethostname;

extern "C" char version[];

static class VersionCommand : public TclObject {
	public:
		VersionCommand() : TclObject("version") {}
		int command(int argc, const char*const* argv) {
			UNUSED(argc);
			UNUSED(argv);
			Tcl::instance().result(version);
			return (TCL_OK);
		}
} cmd_version;

#ifndef USE_SHM
int use_shm = 0;
#else
int use_shm = 1;

#if defined(sgi) && !defined(IRIX6_2)
Bool XShmQueryExtension(Display* dpy)
{
	int mj, mn;
	Bool sp;
	return XShmQueryVersion(dpy, &mj, &mn, &sp);
}
#endif

	static int
noXShm(ClientData, XErrorEvent*)
{
	/*
	 * can get called twice, because on some systems the
	 * XShmDetach after a failed XShmAttach is an error
	 * (i.e., a second error), while on others (bsdi), the
	 * detach is necessary so we cannot omit it
	 */
	if (use_shm)
		fprintf(stderr, "vic: warning: not using shared memory\n");
	use_shm = 0;
	return (0);
}

/*
 * XXX this is a hack to see if we can used shared memory.
 * if the X server says we can, and we're on the same
 * host as the X server, then we are golden.
 */
	static void
checkXShm(Tk_Window tk, const char*)
{
	Display* dpy = Tk_Display(tk);

	if (XShmQueryExtension(dpy) == 0) {
		fprintf(stderr,
				"vic: warning: server doesn't support shared memory\n");
		use_shm = 0;
		return;
	}
	XShmSegmentInfo si;
	if ((si.shmid = shmget(IPC_PRIVATE, 512, IPC_CREAT|0777)) < 0) {
		if (use_shm)
			fprintf(stderr,
					"vic: warning: no shared memory available\n");
		use_shm = 0;
		return;
	}
	si.readOnly = 1;
	XSync(dpy, 0);
	Tk_ErrorHandler handler = Tk_CreateErrorHandler(dpy, -1, -1, -1,
			noXShm, 0);
	XShmAttach(dpy, &si);
	XSync(dpy, 0);
	XShmDetach(dpy, &si);
	Tk_DeleteErrorHandler(handler);
	(void)shmctl(si.shmid, IPC_RMID, 0);
}
#endif

#ifdef USE_DDRAW
int use_ddraw = 1;
#else
int use_ddraw = 0;
#endif

extern "C" char *optarg;
extern "C" int optind;

#ifndef WIN32
extern "C" int opterr;
#endif

int cc_type__;
int ecn_mode__;

const char*
parse_assignment(char* cp)
{
	cp = strchr(cp, '=');
	if (cp != 0) {
		*cp = 0;
		return (cp + 1);
	} else
		return ("true");
}

#ifdef __hpux
#include <sys/socket.h>
gethostid()
{
	int id;
	char hostname[256];		/* 255 is max legal DNS name */
	size_t hostname_size = 256;
	struct hostent *hostp;
	struct in_addr addru;		/* union for conversion */

	(void) gethostname(hostname, hostname_size);
	hostname[hostname_size] = '\0'; /* make sure it is null-terminated */

	hostp = gethostbyname(hostname);
	if(hostp == NULL)
		/* our own name was not found!  punt. */
		id = 0;
	else {
		/* return first address of host */
		memcpy(&(addru.s_addr), hostp->h_addr_list[0], 4);
		id = addru.s_addr;
	}

	return id;
}
#endif

#ifdef __svr4__
#include <sys/systeminfo.h>
#define gethostid xgethostid
	u_int32_t
gethostid()
{
	char wrk[32];
	if (sysinfo(SI_HW_SERIAL, wrk, sizeof(wrk)) > 0)
		return (atoi(wrk));
	return (0);
}
#endif

/*
 * From the RTP spec.
 */
	u_int32_t
heuristic_random()
{
	struct {
		struct  timeval tv;
		clock_t cpu;
		pid_t   pid;
		u_long  hid;
		uid_t   uid;
		gid_t   gid;
		struct  utsname name;
	} s;

	memset(&s, 0, sizeof(s)); /* Avoid uninitialized padding */ //SV-XXX: Debian
	gettimeofday(&s.tv, 0);
	uname(&s.name);
	s.cpu  = clock();
	s.pid  = getpid();
	s.hid  = gethostid();
	s.uid  = getuid();
	s.gid  = getgid();

	MD5_CTX context;
	MD5Init(&context);
	MD5Update(&context, (u_char*)&s, sizeof(s));
	u_int32_t out[4];
	MD5Final((u_char *)out, &context);
	return (out[0] ^ out[1] ^ out[2] ^ out[3]);
}

#ifdef WIN32
#if (TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION == 0)
int
SimplePutsCmd(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
#else
int
SimplePutsCmd(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv)
#endif
{
	int i, newline;

	i = 1;
	newline = 1;
	if ((argc >= 2) && (strcmp(argv[1], "-nonewline") == 0)) {
		newline = 0;
		i++;
	}
	printf("%s", argv[i]);
	if (newline) printf("\n");

	return TCL_OK;
}
#endif

void print_capabilities_xml(Tcl& tcl)
{
#ifdef WIN32
		tcl.CreateCommand("puts", SimplePutsCmd, NULL);
#endif

	tcl.evalc("print_capabilities_xml");
	exit(0);
}

void loadbitmaps(Tcl_Interp* tcl)
{
	static char rev[] = {
		0x30, 0x78, 0x7c, 0x7e, 0x7f, 0x7f, 0x7e, 0x7c, 0x78, 0x30
	};
	Tk_DefineBitmap(tcl, Tk_GetUid("rev"), rev, 7, 10);
	static char fwd[] = {
		0x06, 0x0f, 0x1f, 0x3f, 0x7f, 0x7f, 0x3f, 0x1f, 0x0f, 0x06
	};
	Tk_DefineBitmap(tcl, Tk_GetUid("fwd"), fwd, 7, 10);
}


#if (TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION == 0)
extern "C" int Tk_StripchartCmd(ClientData, Tcl_Interp*, int ac, char** av);
#else
extern "C" int Tk_StripchartCmd(ClientData, Tcl_Interp*, int ac, const char** av);
#endif

#ifdef WIN32
#if (TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION == 0)
extern "C" int WinPutsCmd(ClientData, Tcl_Interp*, int ac, char** av);
extern "C" int WinGetUserName(ClientData, Tcl_Interp*, int ac, char** av);
extern "C" int WinReg(ClientData, Tcl_Interp *, int, char **);
#else
extern "C" int WinPutsCmd(ClientData, Tcl_Interp*, int ac, const char** av);
extern "C" int WinGetUserName(ClientData, Tcl_Interp*, int ac, const char** av);
extern "C" int WinReg(ClientData, Tcl_Interp *, int, const char **);
#endif
#endif

extern "C" {
	int
		TkPlatformInit(Tcl_Interp *interp)
		{
			Tcl_SetVar(interp, "tk_library", ".", TCL_GLOBAL_ONLY);
#if !defined(WIN32) && !defined(MAC_OSX_TK)
			extern void TkCreateXEventSource(void);
			TkCreateXEventSource();
#endif
			return (TCL_OK);
		}

	void *TkSetPlatformInit(int (*func)(Tcl_Interp *));

}


int main(int argc, const char** argv)
{
#ifdef WIN32
	MSG msg;
#endif

#ifdef __FreeBSD__
	srandomdev(); //SV-XXX: FreeBSD
#else
	srandom(heuristic_random());
#endif

#ifdef SIGHUP
	signal(SIGHUP, (sig_t)ciao); //SV-XXX: Debian
#endif
#ifdef WIN32
	signal(SIGINT, ciao);
	signal(SIGTERM, ciao);
#else
	signal(SIGINT, (sig_t)ciao); //SV-XXX: Debian
	signal(SIGTERM, (sig_t)ciao); //SV-XXX: Debian
#endif
#ifdef __FreeBSD__
	signal(SIGSYS, (sig_t)noXShm);
#endif

#ifdef WIN32_NOT
	TkSetPlatformInit(TkPlatformInit);
#endif

#ifndef WIN32
	opterr = 1;
#endif

	// Option list; If letter is followed by ':' then it takes an argument
	const char* options = 
		"A:B:C:c:D:d:e:E:f:F:HI:i:j:K:lL:M:m:N:n:o:Pq:QrsST:t:U:u:vV:w:x:X:y";
	/* process display and window (-use) options before initialising tcl/tk */
	char buf[256], tmp[256];
	const char *display=0, *use=0;
	int op;
	//UNUSED Display *testDisplay;

	while ((op = getopt(argc, (char**)argv, (char*)options)) != -1) {
		if (op == 'd') {
			display = optarg;
		}
		if (op =='w') {
			use= optarg;
		}
		else if (op == '?')
			usage(NULL);
	}
#ifdef USE_ZVFS
	Tcl_FindExecutable((char*)argv[0]);
#endif
	Tcl::init("vic");
	Tcl& tcl = Tcl::instance();

#ifdef WIN32
	//if (display == NULL)
	//	display = "localhost:0";
#endif
#ifdef sgi
	if (display == NULL) {
		display=getenv("DISPLAY");
		/* check to see if we can open the display
		   if we can't and it's set to just :0.0
		   then set display to localhost:0.0
		*/
		if (XOpenDisplay(display)==NULL) {
			if ((display != NULL) && (strcmp(display,":0.0") == 0)) {
				strcpy(buf,"-name vic -display ");
				gethostname(&buf[19],sizeof(buf)-19);
				strcat(buf,":0");
			} else sprintf (buf, "-name vic");
		} else sprintf (buf, "-name vic");
	} else sprintf (buf, "-name vic -dispay %s", display);
#else
	sprintf(buf,display?
		    "-name vic -display %s" :
		    "-name vic",
		  display);
#endif
	sprintf(tmp,use?" -use %s":"",use);
	strncat(buf,tmp,strlen(tmp));
	Tcl_SetVar(tcl.interp(), "argv", buf, TCL_GLOBAL_ONLY);

	/* initialise tcl/tk but ignore errors under windows. */
	Tk_Window tk = 0;

	if(Tcl_Init(tcl.interp()) != TCL_OK){
		fprintf(stderr, "vic: %s\n", tcl.result());
	}

	if(Tk_Init(tcl.interp()) != TCL_OK){
		fprintf(stderr, "vic: %s\n", tcl.result());
	}

	tk = Tk_MainWindow(tcl.interp());
	if (tk == 0) {
		fprintf(stderr, "vic: %s\n", tcl.result());
		exit(1);
	}

	tcl.tkmain(tk);

	loadbitmaps(tcl.interp());
	tcl.CreateCommand("stripchart", Tk_StripchartCmd, (ClientData)tk);
#ifdef WIN32
	tcl.CreateCommand("puts", WinPutsCmd, (ClientData)tk);
	tcl.CreateCommand("getusername", WinGetUserName, (ClientData)tk);
	tcl.CreateCommand("registry", WinReg, (ClientData)tk);
#endif
	EmbeddedTcl::init();
	tcl.evalc("init_resources");
#ifdef USE_PROFILING
	initCounters();
	startCounter(0);
#endif

	optind=1;
	while ((op = getopt(argc, (char**)argv, (char*)options)) != -1) {
		switch (op) {

		default:
			usage(NULL);

		case 'A':
			tcl.add_option("sessionType", optarg);
			if (strcasecmp(optarg, "nv") == 0)
				tcl.add_default("defaultFormat", "nv");
			else if (strcasecmp(optarg, "ivs") == 0)
				tcl.add_default("defaultFormat", "h.261");
			break;

		case 'B':
			tcl.add_option("maxbw", optarg);
			break;

		case 'b':
			tcl.add_option("netBufferSize", optarg);
			break;

		case 'C':
			tcl.add_option("conferenceName", optarg);
			break;

		case 'c':
			tcl.add_option("dither", optarg);
			break;

		case 'D':
			tcl.add_option("device", optarg);
			break;

		case 'd':
			break;

		case 'e': 
			if (!strcasecmp(optarg,"RBCC")) cc_type__=RBCC;
			else if (!strcasecmp(optarg,"WBCC")) cc_type__=WBCC;
			else cc_type__=NOCC;
			break;
		case 'E': 
			if (!strcasecmp(optarg,"LOSS")) ecn_mode__=ECN_AS_LOSS;
			else if (!strcmp(optarg,"ACT")) ecn_mode__=ECN_ACTION;
			else ecn_mode__=ECN_AS_LOSS;
			break;

		case 'f':
			tcl.add_option("defaultFormat", optarg);
			break;

		case 'F':
			tcl.add_option("maxfps", optarg);
			break;

		case 'H':
			tcl.add_option("useHardwareDecode", "true");
			break;

		case 'i':
			tcl.add_option("ifAddr", optarg);
			break;

		case 'I':
			tcl.add_option("confBusChannel", optarg);
			break;

		case 'j':
			tcl.add_option("numLayers", optarg);
			break;

		case 'K':
			/*XXX probably do not want this in X server*/
			tcl.add_option("sessionKey", optarg);
			break;

		case 'l':
			tcl.add_option("logFrameRate", "true");
			// Uses auto generated filename of the form:
			// {UNIX seconds}{host IP}{username}
			break;

		case 'L':
			tcl.add_option("flowLabel", optarg);
			break;

		case 'M':
			tcl.add_option("colorFile", optarg);
			break;

		case 'm':
			tcl.add_option("mtu", optarg);
			break;

		case 'N':
			tcl.add_option("rtpName", optarg);
			break;

		case 'n':
			tcl.add_option("network", optarg);
			break;

		case 'o':
			tcl.add_option("outfile", optarg);
			break;

		case 'P':
			tcl.add_option("privateColormap", "true");
			break;

		case 'q':
			tcl.add_option("jpegQfactor", optarg);
			break;

		case 'Q':
			print_capabilities_xml(tcl);
			break;

		case 'r':
			tcl.add_option("relateInterface","true");
			break;

		case 's':
			use_shm = 0;
			break;

		case 'S':
			tcl.add_option("lipSync", "true");
			break;

		case 't':
			tcl.add_option("defaultTTL", optarg);
			break;

		case 'T':
			tcl.add_option("softJPEGthresh", optarg);
			break;

		case 'u':
			tcl.add_option("startupScript", optarg);
			break;

		case 'U':
			tcl.add_option("stampInterval", optarg);
			break;

		case 'v':
            		printf("VIC %s\n", version);
			exit(0);
			break;

		case 'V':
			tcl.add_option("visual", optarg);
			break;

		/* window for application embedding - same as use */
		case 'w':
			break;

		case 'x':
			tcl.add_option("ifIndex", optarg);
			break;

		case 'X':
			{
				const char* value = parse_assignment(optarg);
				tcl.add_option(optarg, value);
			}
			break;

		case 'y':
			{
				tcl.add_option("bw_control", "true");
			}
			break;
		}
	}
#ifdef USE_SHM
	if (use_shm)
		checkXShm(tk, display);
#endif

	const char* dst;
	if (optind < argc && argc > 1) {
		dst = argv[optind];
		if (argc - optind > 1) {
			fprintf(stderr,
				"vic: extra arguments (starting with `%s')\n",
				argv[optind + 1]);
			exit(1);
		}
	} else if ((dst = tcl.attr("defaultHostSpec")) == 0) {
		usage("vic: destination address required\n");
	}
	tcl.add_option("defaultHostSpec", dst);
#ifdef notdef
	const char* outfile = tcl.attr("outfile");
	if (outfile != 0) {
		int fd = open(outfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
		if (fd < 0) {
			fprintf(stderr, "vic: ");
			perror(outfile);
			exit(1);
		}
		Framer::dump(fd);
	}
#endif
	tcl.evalc("vic_main");

#ifdef USE_DDRAW
	const char *useDDraw = tcl.attr("use_ddraw");
	if (useDDraw != NULL)
	{
	    if (strcmp(useDDraw, "true") == 0)
		use_ddraw = 1;
	    else
		use_ddraw = 0;
	}
	if (use_ddraw)
	    DDrawVideoImage::initDirectDraw();
#endif

	/*
	 * Initialize the stuff that may be dependent on DDraw stuff
	 * after we initialize direct draw.
	 */
	tcl.evalc("init_ag");

	/*
	 * re-nice the vic process before we start processing video
	 * so that video processing won't screw up the local audio.
	 */
	int pri = atoi(tcl.attr("priority"));
	if (pri > 0)
		nice(pri);

	/*
	 * Randomize start sequence number and media timestamp
	 * per in case we turn on encryption.(XXX should re-do
	 * this when changing keys).
	 */
	/*XXX*/
	Transmitter::seqno(random());

#ifdef DEBUG_X_SYNCRONOUSLY
	XSynchronize(Tk_Display(tk), True);
#endif

	/* win32 needs the following to get the initial window painted */
	tcl.evalc("update idletasks");
#ifdef noyet
	while (tk_NumMainWindows > 0) {
		/*
		 * Tk doesn't give priority to the X server so file handlers
		 * (i.e., packets from the network) can starve the window
		 * system (technically, tk lets one X event through per file
		 * event, but a file event can trigger more than one X event
		 * so the X server loses to a high-rate video source).
		 * The solution is to let the X server go first by only
		 * processing file handlers when their are no pending
		 * X events.
		 */
		if (Tk_DoOneEvent(TK_X_EVENTS|TK_IDLE_EVENTS|
				  TK_DONT_WAIT) == 0)
				Tk_DoOneEvent(0);
	}
#else
#ifdef WIN32
	while(1) {
		Tcl_DoOneEvent(0);
		if(PeekMessage(&msg, NULL, WM_QUIT,WM_QUIT, PM_NOREMOVE)) {
			if (msg.message == WM_QUIT)
			{
				break;
			}
		}
	}
#else
	Tk_MainLoop();
#endif
#endif
	adios();
	return (0);
}
