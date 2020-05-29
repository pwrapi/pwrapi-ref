#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <libgen.h>
#include <error.h>
#include <sys/wait.h>
#include <pwr.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/*
 * Exit codes copied from coreutils/system.h
 */
enum {
	EXIT_CANCELLED = 125,
	EXIT_CANNOT_INVOKE = 126,
	EXIT_ENOENT = 127
};

/*
 * This is the exit-code offset that /bin/sh returns for processes that
 * exited with a signal.
 */
#define SIGNALLED_OFFSET 128

static char *myname;						/* pgm name */

static volatile unsigned period;				/* secs */
static char *path;						/* log path */
static FILE *output;						/* log stream */

/*
 * Power API counter info
 */
struct CounterInfo {
	const char *name;					/* name */
	const PWR_AttrName id;					/* PWR name */
	const PWR_AttrDataType dataType;			/* data type */
	const char *unit;					/* unit */
	unsigned flags;
};

/*
 * "Counter" info flags.
 */
#define CIFACC	0x1						/* is acc? */

typedef struct LispObject LispObject;

/*
 * Most everything is centered around list processing.
 */
struct LispObject {
	enum LispTag { PWROBJ, COUNTER, REG } tag;		/* tag */
	unsigned refd :1;					/* refd? */
	LispObject *next;					/* in all obj */
	union {
		/*
		 * Atoms.
		 */
		struct {
			unsigned
				isIntern :1;
			union {
				struct {
					PWR_Obj pwrObj;
					int order;
					char name[1];
				} poi;
				struct Counter {
					const struct CounterInfo *ci;
					union {
						double asdbl;
						uint64_t asui64;
					} val;
					PWR_Time ts;
					int rc;
				} counter;
			} u;
		} a;
		/*
		 * List "register".
		 */
		struct {
			LispObject *address;
			LispObject *decrement;
		} r;
	} u;
};

static PWR_Cntxt cntxt;						/* PWR ctx */
static PWR_Obj self;						/* entry */

static LispObject* (*printAs)(FILE *, LispObject *);

/*
 * Table of name to power-object-type mappings.
 */
static const struct {
	const char *name;
	const PWR_ObjType objType;
} namPWRObjTypMap[] = {
	{ "platform",	PWR_OBJ_PLATFORM },
	{ "cabinet",	PWR_OBJ_CABINET },
	{ "chassis",	PWR_OBJ_CHASSIS },
	{ "board",	PWR_OBJ_BOARD },
	{ "node",	PWR_OBJ_NODE },
	{ "socket",	PWR_OBJ_SOCKET },
	{ "core",	PWR_OBJ_CORE },
	{ "plane",	PWR_OBJ_POWER_PLANE },
	{ "mem",	PWR_OBJ_MEM },
	{ "nic",	PWR_OBJ_NIC },

	{ "tx2soc",	PWR_OBJ_TX2_SOC },
	{ "tx2sram",	PWR_OBJ_TX2_SRAM },
	{ "tx2core",	PWR_OBJ_TX2_CORE },
	{ "tx2mem",	PWR_OBJ_TX2_MEM },

	{ NULL, }
};

/*
 * Table of name to counter-info mappings.
 */
static const struct CounterInfo namPWRAttrMap[] = {
	{ "p-state",
	  PWR_ATTR_PSTATE,
	  PWR_ATTR_DATA_UINT64,
	  "",
	  0
	}, {
	 "c-state",
	 PWR_ATTR_CSTATE,
	 PWR_ATTR_DATA_UINT64,
	 "",
	 0
	}, {
	 "s-state",
	 PWR_ATTR_SSTATE,
	 PWR_ATTR_DATA_UINT64,
	 "",
	 0
	}, {
	 "current",
	 PWR_ATTR_CURRENT,
	 PWR_ATTR_DATA_DOUBLE,
	 "A",
	 0
	}, {
	 "voltage",
	 PWR_ATTR_VOLTAGE,
	 PWR_ATTR_DATA_DOUBLE,
	 "V",
	 0
	}, {
	 "power",
	 PWR_ATTR_POWER,
	 PWR_ATTR_DATA_DOUBLE,
	 "W",
	 0
	}, {
	 "frequency",
	 PWR_ATTR_FREQ,
	 PWR_ATTR_DATA_DOUBLE,
	 "Hz",
	 0
	}, {
	 "energy",
	 PWR_ATTR_ENERGY,
	 PWR_ATTR_DATA_DOUBLE,
	 "J",
	 CIFACC
	}, {
	 "temperature",
	 PWR_ATTR_TEMP,
	 PWR_ATTR_DATA_DOUBLE,
	 "C",
	 0
	}, {
	 "ttime",
	 PWR_ATTR_THROTTLED_TIME,
	 PWR_ATTR_DATA_UINT64,
	 "",
	 CIFACC
	}, {
	 "tcount",
	 PWR_ATTR_THROTTLED_COUNT,
	 PWR_ATTR_DATA_UINT64,
	 "",
	 CIFACC
	},
	{ NULL, }
};

static void
__attribute__ ((noreturn))
usage(int status)
{

	(void )fprintf(stderr,
		       ("Usage: %s"
			" [OPTION]"
		        " COMMAND [ARG] ..."
			"\n"),
		       basename(myname));

	if (status)
		exit(EXIT_CANCELLED);

	(void )fputs((\
"Run COMMAND, then print power counter(s)\n"\
"\n"\
	), stdout);
	(void )fputs((\
"-h			Produce this message\n"\
"-p	period		Periodically sample counters, in seconds\n"\
"-o	path		Log periodic samples to file. '-' for stdout\n"\
"-t	target-class	Monitor a class of targets\n"\
"-n	target-name	Monitor a named target\n"\
"-a	attribute	Select a counter within current target-class\n"\
"-l	as-list		Periodic output in LISP-list format, one per line\n"\
"-j	as-JSON		Periodic output is JSON, one per line\n"\
"\n"\
"The target-class and attribute options are, typically, used in groups. As\n"\
"in, a group is specified by selecting a target-class, then one or more\n"\
"attributes are selected, optionally followed by another target-class and\n"\
"attributes, ad nauseum.\n"\
"\n"\
	), stdout);
	(void )fputs((\
"The following target-classes may be supported:"\
	), stdout);
	{
		unsigned linlen, which, n;
		const char *name;

		linlen = 80;
		for (which = 0; (name = namPWRObjTypMap[which].name); which++) {
			n = strlen(name);
			if (!(linlen + 2 + n < 80)) {
				(void )fputs("\n ", stdout);
				linlen = 1;
			} else {
				(void )fputs(", ", stdout);
				linlen += 2;
			}
			linlen += n;
			(void )fputs(name, stdout);
		}
		(void )fputs("\n", stdout);
	}

	(void )fputs((\
"\n"\
"The following attributes may be supported:"\
	), stdout);
	{
		unsigned linlen, n;
		const struct CounterInfo *ci;

		linlen = 80;
		ci = namPWRAttrMap;
		while (ci->name) {
			n = strlen(ci->name);
			if (!(linlen + 2 + n < 80)) {
				(void )fputs("\n ", stdout);
				linlen = 1;
			} else {
				(void )fputs(", ", stdout);
				linlen += 2;
			}
			linlen += n;
			(void )fputs(ci->name, stdout);
			ci++;
		}
		(void )fputs("\n", stdout);
	}

	(void )fputs((\
"\n"\
"Note: Some attribute values may require periodic sampling for proper\n"\
" maintenance; see the relevant power-api implementation documentation. In\n"\
" order to do so, specify an appropriate period.\n"\
	), stdout);
	exit(EXIT_SUCCESS);
}

/*
 * Why is this not included in the API?
 */
static const char *
PWR_StrError(int rc)
{
	const char *tbl[] = {
		"Out of Range",
		"No permission",
		"Not attempted",
		"Bad index",
		"Bad value",
		"Read only",
		"No meta",
		"No attribute",
		"Length",
		"Invalid",
		"Empty",
		"Not implemented",
		"Failure",
		"Success",
		"Not optimized",
		"No parent",
		"No children",
		"No object at index",
		"No group by name",
		"Truncated"
	};

	rc += 13;
	if (rc < 0 || (size_t )rc >= sizeof(tbl) / sizeof(const char *))
		return "Unknown error";
	return tbl[rc];
}

/*
 * Like error(), print a message, with qualifier if !PWR_RET_SUCCESS, and
 * exit if non-zero status.
 */
static void
xPWR_Error(int status, int rc, const char *format, ...)
{
	va_list ap;

	(void )fflush(stdout);

	(void )fprintf(stderr, "%s: ", myname);
	va_start(ap, format);
	(void )vfprintf(stderr, format, ap);
	va_end(ap);
	if (rc != PWR_RET_SUCCESS)
		(void )fprintf(stderr, ": %s", PWR_StrError(rc));
	(void )fputc('\n', stderr);
	(void )fflush(stderr);

	if (status)
		exit(status);
}

static void *
xrealloc(void *p, size_t nbytes)
{

	p = realloc(p, nbytes);
	if (!p)
		error(0, errno, "memory allocation failed");
	return p;
}

static void *
xmalloc(size_t nbytes)
{

	return xrealloc(NULL, nbytes);
}

static void *
mustAlloc(size_t nbytes)
{
	void *p;

	if (!(p = xrealloc(NULL, nbytes)))
		exit(EXIT_FAILURE);
	return p;
}

/*
 * Map/Search states.
 */
typedef enum {
	xPWR_MAP_SEARCH,
	xPWR_MAP_PRUNE,
	xPWR_MAP_DONE
} xPWR_MapState;

static LispObject *internPWRObj(PWR_Obj pwrObj);
static LispObject *print(FILE *stream, LispObject *x);

/*
 * From, and including, the given power-object in a power-api tree, perform
 * a in-order traversal, passing each node, a pointer to the current state,
 * and the given argument to the given callback.
 *
 * The callback can control traversal by resetting the state:
 *
 * xPWR_MAP_SEARCH; continue
 * xPWR_MAP_PRUNE; abandon processing of the current ply
 * xPWR_MAP_DONE; abandon processing
 *
 * If the callback returns other than PWR_RET_SUCCESS, processing is abandoned
 * and that code becomes the final return to the caller.
 */
static int
xPWR_InOrderMap(PWR_Obj at,
		 xPWR_MapState *statep,
		 int (*callback)(PWR_Obj, xPWR_MapState *, void *),
		 void *arg)
{
	int rc, i;
	PWR_Grp children;
	PWR_Obj child;

	rc = (*callback)(at, statep, arg);
	if (rc != PWR_RET_SUCCESS)
		return rc;
	if (*statep == xPWR_MAP_PRUNE) {
		*statep = xPWR_MAP_SEARCH;
		return PWR_RET_SUCCESS;
	}
	if (*statep != xPWR_MAP_SEARCH)
		return PWR_RET_SUCCESS;

	rc = PWR_ObjGetChildren(at, &children);
	if (rc != PWR_RET_SUCCESS) {
		if (rc == PWR_RET_WARN_NO_CHILDREN)
			return PWR_RET_SUCCESS;
		return rc;
	}
	for (i = 0; i < PWR_GrpGetNumObjs(children); i++) {
		if ((rc = PWR_GrpGetObjByIndx(children, i, &child)) ||
		    (rc = xPWR_InOrderMap(child, statep, callback, arg)))
		    	break;
	}
	return rc;
}

static int
isatom(LispObject *x)
{

	return !x || x->tag != REG;
}

static LispObject *
rplaca(LispObject *x, LispObject *y)
{

	if (isatom(x))
		error(1, 0, "first argument to rplaca is not a cons");
	x->u.r.address = y;
	return x;
}

static LispObject *
rplacd(LispObject *x, LispObject *y)
{

	if (isatom(x))
		error(1, 0, "first argument to rplacd is not a cons");
	x->u.r.decrement = y;
	return x;
}

static LispObject *
car(LispObject *x)
{

	if (!x)
		return NULL;
	assert(!isatom(x));
	return x->u.r.address;
}

static LispObject *
cdr(LispObject *x)
{

	if (!x)
		return NULL;
	assert(!isatom(x));
	return x->u.r.decrement;
}

/*
 * All extant LISP objects appear on this list.
 */
static LispObject *allLispObjects;

/*
 * LISP objects protected from GC
 */
static LispObject *protect;;

static LispObject *
registerLispObject(LispObject *x, enum LispTag tag)
{

	x->tag = tag;
	x->refd = 0;
	x->next = allLispObjects;
	allLispObjects = x;
	return x;
}

static LispObject *
cons(LispObject *x, LispObject *y)
{
	LispObject *result;

	result = registerLispObject(mustAlloc(sizeof(LispObject)), REG);
	(void )rplaca(result, x);
	(void )rplacd(result, y);
	return result;
}

/*
 * The "symbol" table for PWR objects.
 */
static LispObject *pwrObjTab;

/*
 * Return an "internalized" PWROBJ atom.
 */
static LispObject *
internPWRObj(PWR_Obj pwrObj)
{
	LispObject *u, *result;
	static int counter;


	/*
	 * Have this one already?
	 */
	for (u = pwrObjTab; !isatom(u); u = cdr(u)) {
		result = car(u);
		assert(result->tag == PWROBJ);
		if (result->u.a.u.poi.pwrObj == pwrObj)
			return result;
	}

	/*
	 * Need to create it.
	 */
	{
		size_t n;
		int rc;

		result = NULL;
		n = 30;
		for (;;) {
			result = xrealloc(result, sizeof(LispObject) + n);
			rc = PWR_ObjGetName(pwrObj, result->u.a.u.poi.name, n);
			if (rc == PWR_RET_SUCCESS) {
				n = strlen(result->u.a.u.poi.name);
				result =
				    xrealloc(result,
					     sizeof(LispObject) + n + 1);
				break;
			}
			if (rc != PWR_RET_WARN_TRUNC) {
				xPWR_Error(EXIT_FAILURE,
					   rc,
					   "cannot get object name");
				break;
			}
			n *= 2;
		}
	}
	result->u.a.isIntern = 1;
	result->u.a.u.poi.pwrObj = pwrObj;
	result->u.a.u.poi.order = counter++;
	pwrObjTab = cons(registerLispObject(result, PWROBJ), pwrObjTab);
	return result;
}

static LispObject *
newCOUNTER(const struct CounterInfo *ci)
{
	LispObject *result;

	result = mustAlloc(sizeof(LispObject));
	result->u.a.isIntern = 0;
	result->u.a.u.counter.ci = ci;
	(void )memset(&result->u.a.u.counter.val,
		      0,
		      sizeof(result->u.a.u.counter.val));
	result->u.a.u.counter.ts = 0;
	result->u.a.u.counter.rc = PWR_RET_SUCCESS;
	return registerLispObject(result, COUNTER);
}

static LispObject *
reverse(LispObject *sequence)
{
	LispObject *result;

	result = NULL;
	while (!isatom(sequence)) {
		result = cons(car(sequence), result);
		sequence = cdr(sequence);
	}
	if (sequence)
		result = cons(sequence, result);
	return result;
}

static void
map(LispObject *(*f)(LispObject *, void *), void *arg, LispObject *x)
{

	for (;;) {
		if (isatom(x)) {
			if (x)
				(void )(*f)(x, arg);
			break;
		}
		(void )(*f)(car(x), arg);
		x = cdr(x);
	}
}

static LispObject *
mapcar(LispObject *(*f)(LispObject *, void *), void *arg, LispObject *x)
{
	LispObject *result, *u;

	result = NULL;
	for (;;) {
		if (!x)
			break;
		u = isatom(x) ? x : car(x);
		result = cons((*f)(u, arg), result);
		if (isatom(x))
			break;
		x = cdr(x);
	}
	return result;
}

/*
 * Mark all LISP objects, reachable from that given, as referenced.
 */
static void
mark(LispObject *x)
{

	for (;;) {
		if (!x || x->refd)
			break;
		x->refd = 1;
		if (isatom(x))
			break;
		mark(car(x));
		mark(x = cdr(x));
	}
}

/*
 * Garbage collection.
 */
static void
gc(LispObject *extra)
{
	LispObject *next, *x, **tailp;

	/* clear */
	for (x = allLispObjects; x; x = x->next)
		x->refd = 0;

	mark(extra);
	mark(protect);
	mark(pwrObjTab);

	/* sweep */
	tailp = &allLispObjects;
	next = allLispObjects;
	while ((x = next)) {
		next = x->next;
		if (!x->refd) {
			free(x);
			*tailp = next;
		} else
			tailp = &x->next;
	}
}

/*
 * Print a LISP COUNTER atom.
 */
static void
prinCounter(FILE *stream, LispObject *x)
{
	struct Counter *cntr;

	assert(x && x->tag == COUNTER);
	cntr = &x->u.a.u.counter;
	(void )fprintf(stream, "<counter:%s ", cntr->ci->name);
	do {
		if (cntr->rc != PWR_RET_SUCCESS) {
			(void )fprintf(stream,
				       "rc:\"%s\"",
				       PWR_StrError(cntr->rc));
			break;
		}

		(void )fprintf(stream, "@%ld ", cntr->ts);
		switch (cntr->ci->dataType) {
		case PWR_ATTR_DATA_DOUBLE:
			(void )fprintf(stream, "%lf", cntr->val.asdbl);
			break;
		case PWR_ATTR_DATA_UINT64:
			(void )fprintf(stream, "%lu", cntr->val.asui64);
			break;
		default:
			error(EXIT_FAILURE,
			      0,
			      "unhandled PWR data type %d in prin",
			      cntr->ci->dataType);
		}
		(void )fprintf(stream, "%s", cntr->ci->unit);
	} while (0);
	(void )putc('>', stream);
}

static LispObject *
prin(FILE *stream, LispObject *x)
{

	if (!x) {
		(void )fprintf(stream, "()");
		return x;
	}
	if (isatom(x)) {
		switch (x->tag) {
		case PWROBJ:
			(void )fprintf(stream,
				       "<pwrObj:%s>",
				       x->u.a.u.poi.name);
			break;
		case COUNTER:
			prinCounter(stream, x);
			break;
		default:
			error(EXIT_FAILURE,
			      0,
			      "unhandled atom-tag %d in prin", x->tag);
		}
		return x;
	}
	{
		LispObject *m;

		(void )putc('(', stream);
		m = x;
		for (;;) {
			prin(stream, car(m));
			m = cdr(m);
			if (isatom(m)) {
				if (!m)
					break;
				(void )putc('.', stream);
				(void )prin(stream, m);
				break;
			}
			(void )putc(' ', stream);
		}
		(void )putc(')', stream);
	}
	return x;
}

static LispObject *
print(FILE *stream, LispObject *x)
{

	(void )prin(stream, x);
	(void )putc('\n', stream);
	return x;
}

/*
 * Print a LISP COUNTER atom.
 */
static void
prinCounterJSON(FILE *stream, LispObject *x)
{
	struct Counter *cntr;

	assert(x && x->tag == COUNTER);
	cntr = &x->u.a.u.counter;
	(void )fprintf(stream, " { \"%s\" : {", cntr->ci->name);
	do {
		if (cntr->rc != PWR_RET_SUCCESS) {
			(void )fprintf(stream,
				       " \"error\" : \"%s\"",
				       PWR_StrError(cntr->rc));
			break;
		}

		(void )fprintf(stream,
			       " \"time\" : %ld , \"value\" : ",
			       cntr->ts);
		switch (cntr->ci->dataType) {
		case PWR_ATTR_DATA_DOUBLE:
			(void )fprintf(stream, "%lf", cntr->val.asdbl);
			break;
		case PWR_ATTR_DATA_UINT64:
			(void )fprintf(stream, "%lu", cntr->val.asui64);
			break;
		default:
			error(EXIT_FAILURE,
			      0,
			      "unhandled PWR data type %d in prin",
			      cntr->ci->dataType);
		}
		if (!cntr->ci->unit)
			(void )fputc(' ', stream);
		else
			(void )fprintf(stream,
				       " , \"unit\" : \"%s\"",
				       cntr->ci->unit);
	} while (0);
	(void )fprintf(stream, " } }");
}

static LispObject *
prinJSON(FILE *stream, LispObject *x)
{
	LispObject *u, *v;

	if (isatom(x)) {
		assert(x);
		switch (x->tag) {
		case PWROBJ:
			(void )fprintf(stream,
				       "\"%s\"",
				       x->u.a.u.poi.name);
			break;
		case COUNTER:
			prinCounterJSON(stream, x);
			break;
		default:
			error(EXIT_FAILURE,
			      0,
			      "unhandled atom-tag %d in prinJSON", x->tag);
		}
		return x;
	}

	(void )fprintf(stream, " { ");
	for (u = x; !isatom(u); u = cdr(u)) {
		v = car(u);
		if (isatom(v))
			error(EXIT_FAILURE, 0, "bad a-list -- prinJSON");
		else {
			if (isatom(car(v))) {
				prinJSON(stream, car(v));
				(void )fprintf(stream, " : ");
				prinJSON(stream, cdr(v));
				if (!isatom(cdr(u)))
					(void )fprintf(stream, " , ");
			} else
				prinJSON(stream, v);
		}
	}
	(void )fprintf(stream, " } ");

	return x;
}

static LispObject *
printJSON(FILE *stream, LispObject *x)
{

	(void )prinJSON(stream, x);
	(void )putc('\n', stream);
	return x;
}

/*
 * Context record for use by the, below, gather routines.
 */
struct byObjType {
	const PWR_ObjType objType;
	LispObject *list;
};

/*
 * Gather callback.
 */
static int
doGather(PWR_Obj pwrObj, xPWR_MapState *statep, struct byObjType *ctx)
{
	int rc;
	PWR_ObjType objType;

	rc = PWR_ObjGetType(pwrObj, &objType);
	if (rc != PWR_RET_SUCCESS)
		return rc;
	if (objType != ctx->objType)
		return PWR_RET_SUCCESS;

	ctx->list = cons(internPWRObj(pwrObj), ctx->list);
	*statep = xPWR_MAP_PRUNE;

	return PWR_RET_SUCCESS;
}

/*
 * For each node found at or below the given power-object matching the
 * given power-object-type, add a PWROBJ to the given list.
 */
static int
gather(PWR_Obj at, const PWR_ObjType objType, LispObject **listp)
{
	xPWR_MapState state;
	struct byObjType context = { objType, NULL };
	int rc;

	state = xPWR_MAP_SEARCH;
	rc =
	    xPWR_InOrderMap(at,
			     &state,
			     (int (*)(PWR_Obj,
				      xPWR_MapState *,
				      void *))doGather,
			     &context);
	*listp = context.list;
	return rc;
}

struct byName {
	const char *name;
	LispObject *result;
};

static int
doSearch(PWR_Obj pwrObj, xPWR_MapState *statep, struct byName *ctx)
{
	LispObject *x;

	x = internPWRObj(pwrObj);
	if (strcmp(x->u.a.u.poi.name, ctx->name) == 0) {
		ctx->result = x;
		*statep = xPWR_MAP_PRUNE;
	}

	return PWR_RET_SUCCESS;
}

/*
 * Find, first, node with given name and it.
 */
static int
search(PWR_Obj at, const char *name, LispObject **resultp)
{
	xPWR_MapState state;
	struct byName context = { name, NULL };
	int rc;

	state = xPWR_MAP_SEARCH;
	rc =
	    xPWR_InOrderMap(at,
			     &state,
			     (int (*)(PWR_Obj,
				      xPWR_MapState *,
				      void *))doSearch,
			     &context);
	*resultp = context.result;
	return rc;
}

static int
setup()
{
	int rc;

	do {
		rc =
		    PWR_CntxtInit(PWR_CNTXT_DEFAULT,
				  PWR_ROLE_APP,
				  "App",
				  &cntxt);
		if (rc != PWR_RET_SUCCESS) {
			xPWR_Error(0, rc, "cannot initialize power context");
			break;
		}

		rc = PWR_CntxtGetEntryPoint(cntxt, &self);
		if (rc != PWR_RET_SUCCESS) {
			xPWR_Error(0, rc, "cannot get power tree entry point");
			break;
		}

		return PWR_RET_SUCCESS;
	} while (0);
	return rc;
}

static pid_t
background(char *const argv[])
{
	pid_t pid;

	pid = fork();
	if (pid < 0)
		error(EXIT_CANCELLED, errno, "cannot fork");
	else if (pid == 0) {
		int err;

		/* child */
		(void )execvp(argv[0], argv);
		err = errno;
		error(0, err, "cannot run %s", argv[0]);
		exit(err == ENOENT ? EXIT_ENOENT : EXIT_CANNOT_INVOKE);
	}
	return pid;
}

const struct CounterInfo *
attrLook(const char *name)
{
	const struct CounterInfo *ci;

	for (ci = namPWRAttrMap; ci->name; ci++)
		if (strcmp(name, ci->name) == 0)
			return ci;
	return NULL;
}

static int
readCounter(PWR_Obj pwrObj, struct Counter *cntr)
{

	return 
	    PWR_ObjAttrGetValue(pwrObj,
				cntr->ci->id,
				&cntr->val,
				&cntr->ts);
}

static LispObject *
parseOpts(int argc, char * const argv[])
{
	int i, rc;
	LispObject *init, *targets, *a;

	a = NULL;
	targets = init = cons(internPWRObj(self), NULL);
	while ((i = getopt(argc, argv, "+hp:o:t:n:a:lj")) != -1)
		switch (i) {
		case 'h':					/* help */
			usage(EXIT_SUCCESS);
			break;
		case 'p':					/* period */
			{
				unsigned long ul;
				char *end;

				ul = strtoul(optarg, &end, 10);
				if (*optarg == '\0' || *end != '\0')
					error(EXIT_CANCELLED,
					      0,
					      "cannot parse period");
				if (!ul || ul > UINT_MAX - 1)
					error(EXIT_CANCELLED,
					      0,
					      "period out of range");
				if (ul)
					period = ul;
				break;
			}
		case 'o':					/* log path */
			if (path)
				error(EXIT_CANCELLED,
				      0,
				      "profiling file path not unique");
			path = optarg;
			break;
		case 't':					/* tgt class */
			do {
				unsigned which;
				const char *name;

				for (which = 0;
				     (name = namPWRObjTypMap[which].name);
				     which++)
					if (strcmp(optarg, name) == 0)
						break;
				if (!name)
					error(EXIT_CANCELLED,
					      0,
					      "unknown power object %s",
					      optarg);
				targets = NULL;
				rc =
				    gather(self,
					   namPWRObjTypMap[which].objType,
					   &targets);
				if (rc != PWR_RET_SUCCESS)
					xPWR_Error(EXIT_CANCELLED,
						   rc,
						   "cannot gather for type %s",
						   optarg);
				if (!targets) {
					error(0,
					      0,
					      "No targets of type %s found",
					      optarg);
					break;
				}
			} while (0);
			break;
		case 'n':
			targets = NULL;
			rc = search(self, optarg, &targets);
			if (rc != PWR_RET_SUCCESS)
				xPWR_Error(EXIT_CANCELLED,
					   rc,
					   "cannot search for %s",
					   optarg);
			if (!targets) {
				error(0,
				      0,
				      "No target named %s found",
				      optarg);
				break;
			}
			targets = cons(targets, NULL);
			break;
		case 'a':					/* attr */
			if (!targets)
				error(EXIT_CANCELLED,
				      0,
				      "no target for %s counter",
				      optarg);
			{
				const struct CounterInfo *ci;
				LispObject *x;

				if (!(ci = attrLook(optarg)))
					error(EXIT_CANCELLED,
					      0,
					      "unknown counter, \"%s\"",
					      optarg);
				/*
				 * For each target, push a (PWROBJ, COUNTER)
				 * pair onto the given form.
				 */
				for (x = targets; !isatom(x); x = cdr(x))
					a =
					    cons(cons(car(x),
						      newCOUNTER(ci)),
						 a);
			}
			break;
		case 'l':
			printAs = print;
			break;
		case 'j':
			printAs = printJSON;
			break;
		default:
			usage(EXIT_CANCELLED);
		}

	/* No, or unusable, a-list from parsing the command-line options? */
	if (!a) {
		const char *attrNames[] =
		    { "energy", "power", NULL },
		    **attrNamep;
		struct Counter counter;

		if (targets != init)
			error(EXIT_CANCELLED,
			      0,
			      "No, usable, counters specified");

		/* try to find a usable default */
		for (attrNamep = attrNames; *attrNamep; attrNamep++)
			if ((counter.ci = attrLook(*attrNamep)) &&
			    readCounter(self, &counter) == PWR_RET_SUCCESS)
				break;
		if (!*attrNamep)
			error(EXIT_CANCELLED,
			      0,
			      "cannot find a, usable, default power attribute");
		a =
		    cons(cons(internPWRObj(self),
			      newCOUNTER(counter.ci)),
			 NULL);
	}

	/*
	 * Open log output file, if asked.
	 */
	do {
		int flags;
		int fd;

		if (!path)
			break;

		if (!period) {
			error(0, 0, "suppressing log creation");
			break;
		}

		flags = O_CREAT|O_CLOEXEC|O_WRONLY;
		if (path && path[0] == '-' && path[1] == '\0')
			path = "/dev/stdout";
		else {
			/* no clobber */
			flags |= O_EXCL;
		}
		fd = open(path, flags, 0666);
		if (fd < 0)
			error(EXIT_CANCELLED, errno, "cannot create %s", path);
		output = fdopen(fd, "w");
		if (!output)
			error(EXIT_CANCELLED,
			      errno,
			      "cannot convert %s to stream", path);
	} while (0);

	a = reverse(a);
	gc(a);
	return a;
}

/*
 * quicksort; a list
 *
 * NB: This "do it with LISP primitives" thing is getting a little onerous.
 */
static LispObject *
quicksort(LispObject *list, int (*cmp)(LispObject **, LispObject **))
{
	size_t len;
	LispObject *x, **arr, **slotp, *result, **tailp;

	len = 0;
	for (x = list; !isatom(x); x = cdr(x))
		len++;
	if (!len)
		return 0;

	arr = xmalloc(len * sizeof(LispObject *));
	for (x = list, slotp = arr; !isatom(x); x = cdr(x), slotp++)
		*slotp = car(x);

	qsort(arr,
	      len,
	      sizeof(LispObject *),
	      (int (*)(const void *, const void *))cmp);

	result = 0;
	tailp = &result;
	slotp = arr;
	while (len--) {
		*tailp = cons(*slotp++, 0);
		tailp = &(*tailp)->u.r.decrement;
	}

	free(arr);
	return result;
}

static int
cmpByKey(LispObject **a, LispObject **b)
{
	LispObject *u, *v;

	assert(!(isatom(*a) || isatom(*b)));
	u = car(*a);
	assert(u && u->tag == PWROBJ);
	v = car(*b);
	assert(v && u->tag == PWROBJ);
	if (u->u.a.u.poi.order < v->u.a.u.poi.order)
		return -1;
	if (u->u.a.u.poi.order > v->u.a.u.poi.order)
		return 1;
	if (u->u.a.u.poi.pwrObj < v->u.a.u.poi.pwrObj)
		return -1;
	if (u->u.a.u.poi.pwrObj > v->u.a.u.poi.pwrObj)
		return 1;
	return 0;
}

/*
 * Return a copy of a LISP object.
 */
static LispObject *
duplicate(LispObject *x)
{
	LispObject *result;

	result = NULL;
	if (!x)
		return NULL;
	if (isatom(x)) {
		if (x->u.a.isIntern)
			return x;
		result = xmalloc(sizeof(LispObject));
		if (!result)
			exit(EXIT_FAILURE);
		*result = *x;
	} else
		result = cons(duplicate(car(x)), duplicate(cdr(x)));
	return result;
}

static LispObject *
sample(LispObject *pair, void *arg __attribute__ ((unused)))
{
	struct Counter *cntr;

	assert(pair && !isatom(pair));
	assert(car(pair)->tag == PWROBJ && cdr(pair)->tag == COUNTER);

	cntr = &cdr(pair)->u.a.u.counter;
	if (cntr->rc != PWR_RET_SUCCESS) {
		/*
		 * If this counter has failed, skip it.
		 */
		return NULL;
	}
	cntr->rc = readCounter(car(pair)->u.a.u.poi.pwrObj, cntr);
	if (cntr->rc != PWR_RET_SUCCESS) {
		xPWR_Error(0,
			   cntr->rc,
			   "cannot get %s for \"%s\"",
			   cntr->ci->name,
			   car(pair)->u.a.u.poi.name);
		return NULL;
	}
	return pair;
}

static void
diffCtrHelper(struct Counter *minuend,
	      struct Counter *subtrahend,
	      struct Counter *result)
{

	assert(minuend && subtrahend);
	assert(minuend->ci->dataType == subtrahend->ci->dataType);

	if (minuend->rc != PWR_RET_SUCCESS)
		result->rc = minuend->rc;
	if (subtrahend->rc != PWR_RET_SUCCESS)
		result->rc = subtrahend->rc;
	if (result->rc != PWR_RET_SUCCESS)
		return;

	result->ci = minuend->ci;
	switch (minuend->ci->dataType) {
	case PWR_ATTR_DATA_DOUBLE:
		result->val.asdbl =
		    minuend->val.asdbl - subtrahend->val.asdbl;
		break;
	case PWR_ATTR_DATA_UINT64:
		result->val.asui64 =
		    minuend->val.asui64 - subtrahend->val.asui64;
		break;
	default:
		error(EXIT_FAILURE,
		      0,
		      "unhandled data type %d in diffCtr",
		      minuend->ci->dataType);
	}
	result->ts = minuend->ts - subtrahend->ts;
}

static LispObject *
diffCtr(LispObject *minuend, LispObject *subtrahend, LispObject *result)
{

	assert(minuend && minuend->tag == COUNTER);
	assert(subtrahend && subtrahend->tag == COUNTER);
	assert(result && result->tag == COUNTER);

	diffCtrHelper(&minuend->u.a.u.counter,
		      &subtrahend->u.a.u.counter,
		      &result->u.a.u.counter);
	return result;
}

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static struct timespec
now()
{
	struct timespec buf;

	if (clock_gettime(CLOCK_REALTIME, &buf) != 0)
		error(EXIT_FAILURE, errno, "cannot read the clock");
	return buf;
}

static void
acquire_mutex(pthread_mutex_t *mutex)
{
	int err;

	if ((err = pthread_mutex_lock(mutex)))
		error(EXIT_FAILURE, err, "cannot acquire mutex");
}

static void
release_mutex(pthread_mutex_t *mutex)
{
	int err;

	if ((err = pthread_mutex_unlock(mutex)))
		error(EXIT_FAILURE, err, "cannot release mutex");
}

static void
profile(LispObject *targets)
{

	map(sample, NULL, targets);
	if (output)
		(void )(*printAs)(output, targets);
}

static void *
periodic(LispObject *a)
{
	struct timespec when, cur;

	profile(a);
	when = now();
	for (;;) {
		when.tv_sec += period;
		for (;;) {
			acquire_mutex(&mutex);
			(void )pthread_cond_timedwait(&cond, &mutex, &when);
			release_mutex(&mutex);
			if (!period) {
				profile(a);
				return NULL;
			}
			cur = now();
			if (cur.tv_sec < when.tv_sec ||
			    (cur.tv_sec == when.tv_sec &&
			     cur.tv_nsec < when.tv_nsec))
				continue;
			break;
		}

		profile(a);
	}
}

static void
summarize(LispObject *a)
{
	LispObject *x, *u;
	struct Counter *cntr;

	x = NULL;
	while (!isatom(a)) {
		u = car(a);
		assert(u && u->tag == REG &&
		       car(u) && car(u)->tag == PWROBJ &&
		       cdr(u) && cdr(u)->tag == COUNTER);
		a = cdr(a);

		cntr = &cdr(u)->u.a.u.counter;
		if (!(cntr->ci->flags & CIFACC))
			continue;
		if (cntr->rc != PWR_RET_SUCCESS)
			continue;
		if (car(u) != x) {
			x = car(u);
			(void )printf("%s\n", x->u.a.u.poi.name);
		}
		(void )printf("\t%s ", cntr->ci->name);
		switch (cntr->ci->dataType) {
		case PWR_ATTR_DATA_DOUBLE:
			(void )printf("%lf", cntr->val.asdbl);
			break;
		case PWR_ATTR_DATA_UINT64:
			(void )printf("%lu", cntr->val.asui64);
			break;
		default:
			error(EXIT_FAILURE,
			      0,
			      ("unhandled PWR data type %d"
			       "in summarize"),
			      cntr->ci->dataType);
		}
		(void )printf("%s\n", cntr->ci->unit);
	}
}

int
main(int argc, char *const argv[])
{
	LispObject *a, *scratch;
	struct sigaction intr_saved, quit_saved;
	pthread_t thread;
	pid_t pid;
	int wstatus, status;

	myname = argv[0];

	if (setup() != PWR_RET_SUCCESS)
		exit(EXIT_CANCELLED);

	printAs = printJSON;
	a = parseOpts(argc, argv);
	a = quicksort(a, cmpByKey);
	protect = cons(a, protect);
	gc(NULL);

	/* take first sample */
	{
		LispObject *x;

		x = mapcar(sample, NULL, a);
		while (!isatom(x)) {
			if (car(x))
				break;
			x = cdr(x);
		}
		gc(NULL);
		if (!x)
			error(EXIT_CANCELLED,
			      0,
			      "No, usable, counters specified");
	}

	if (!(argc - optind))
		usage(EXIT_CANCELLED);

	/* run the given command, in the background, and get it's exit status */
	pid = background(&argv[optind]);

	{
		struct sigaction intr_action, quit_action;

		/* Ignore interrupt and quit signals, if possible. */
		intr_action.sa_handler = SIG_IGN;
		(void )sigaction(SIGINT, &intr_action, &intr_saved);
		quit_action.sa_handler = SIG_IGN;
		(void )sigaction(SIGQUIT, &quit_action, &quit_saved);
	}

	if (period) {
		int err;

		scratch = duplicate(a);
		err =
		    pthread_create(&thread,
				   NULL,
				   (void *(*)(void *))periodic,
				   scratch);
		if (err)
			error(EXIT_CANCELLED, err, "cannot create thread");
	}


	status = EXIT_FAILURE;
	if (waitpid(pid, &wstatus, 0) < 0) {
		error(0, errno, "cannot wait for child");
	} else if (WIFEXITED(wstatus))
		status = WEXITSTATUS(wstatus);
	else if (WIFSIGNALED(wstatus))
		status = WTERMSIG(wstatus) + SIGNALLED_OFFSET;
	else if (WIFSTOPPED(wstatus))
		status = WSTOPSIG(wstatus) + SIGNALLED_OFFSET;
	else {
		error(0,
		      0,
		      "unknown status (%d) from %s",
		      wstatus,
		      argv[0]);
	}

	/* Restore interrupt and quit signals to their original state. */
	(void )sigaction(SIGINT, &intr_saved, NULL);
	(void )sigaction(SIGQUIT, &quit_saved, NULL);

	if (period) {
		void *dummy;
		int err;

		acquire_mutex(&mutex);
		period = 0;
		err = pthread_cond_signal(&cond);
		release_mutex(&mutex);
		if (err)
			error(EXIT_FAILURE,
			      err,
			      "cannot signal profiling thread");

		if ((err = pthread_join(thread, &dummy)))
			error(EXIT_FAILURE,
			      err,
			      "cannot join profiling thread");
	}

	if (output) {
		if (fclose(output) != 0)
			error(0, errno, "%s", path);
		output = NULL;
	}

	/*
	 * Sample, again, and display summation.
	 */
	gc(NULL);
	{
		LispObject *scratch, *x, *y, *u, *v;

		map(sample, NULL, scratch = duplicate(a));
		for (x = scratch, y = a;
		     !(isatom(x) || isatom(y));
		     x = cdr(x), y = cdr(y)) {
			u = car(x); v = car(y);
			assert(!(isatom(u)) || isatom(y));
			if (!cdr(u) ||
			    cdr(u)->tag != COUNTER ||
			    !(cdr(u)->u.a.u.counter.ci->flags & CIFACC))
				continue;
			diffCtr(cdr(u), cdr(v), cdr(v));
		}
		summarize(a);
	}

#ifndef NDEBUG
	/* cleanup */
	protect = NULL;
	pwrObjTab = NULL;
	gc(NULL);
	assert(!allLispObjects);
#endif

	return status;
}
