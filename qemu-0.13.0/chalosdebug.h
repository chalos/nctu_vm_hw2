#ifndef CHALOS_DEBUG
#define CHALOS_DEBUG

#define NO_DEBUG

#ifndef NO_DEBUG

#define DEBUG(s) printf(" %s\n", s)
#define DEBUG_STR(s) printf(" " #s ": %s\n", s)
#define DEBUG_INT(d) printf(" " #d ": %d\n", d)
#define DEBUG_UINT(d) printf(" " #d ": %u\n", d)
#define DEBUG_XINT(d) printf(" " #d ": %x\n", d)
#define DEBUG_LONG(d) printf(" " #d ": %ld\n", d)
#define DEBUG_ULONG(d) printf(" " #d ": %lu\n", d)
#define DEBUG_XLONG(d) printf(" " #d ": %lx\n", d)
#define DEBUG_PTR(d) printf(" " #d ": %p\n", d)
#define DEBUG_LINE_XLONG(l,d) printf("%d: %lx\n", l, d);

#define INF printf(">> In %s\n", __func__)
#define OUTF printf("<< End %s\n", __func__)

#define RET OUTF; return;
#define RETURN(x) { OUTF; return x; }

#define GENHELPER0(name) glue(gen_helper_,name)();
#define GENHELPER1(name, v1) glue(gen_helper_,name)(v1);
#define GENHELPER2(name, v1, v2) glue(gen_helper_,name)(v1,v2);
#define TARGET_TRACE gen_helper_print_line(tcg_const_tl(__LINE__));

#else

#define DEBUG(s) ;
#define DEBUG_STR(s) ;
#define DEBUG_INT(d) ;
#define DEBUG_UINT(d) ;
#define DEBUG_XINT(d) ;
#define DEBUG_LONG(d) ;
#define DEBUG_ULONG(d) ;
#define DEBUG_XLONG(d) ;
#define DEBUG_PTR(d) ;
#define DEBUG_LINE_XLONG(l,d) ;

#define INF ;
#define OUTF ;

#define RET return;
#define RETURN(x) return x;

#define GENHELPER0(name) ;
#define GENHELPER1(name, v1) ;
#define GENHELPER2(name, v1, v2) ;
#define TARGET_TRACE ;

#endif

#endif