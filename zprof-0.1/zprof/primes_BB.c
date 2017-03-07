/* Profiled version generated from primes.c by zprof 0.1 */
typedef struct {
  const unsigned hi;
  const char *const fName;
} _BBFNameType;

typedef struct {
  const unsigned *const counters;
  const _BBFNameType *const fNames;
  const unsigned *const lineNums;
  const unsigned nCounters;
  unsigned isInit;
} _BBProfType;

static _BBProfType _BBProf;
static unsigned _BBCntrs[];
static _BBFNameType _BBFNames[];
static unsigned _BBLineNums[];
static void _BBAtExit();
int atexit();

# 1 "primes.c"
# 1 "primes.c"
 

static int
prime(int n) 
{
  int i;
  int _BBInit= _BBProf.isInit || ((_BBProf.isInit= 1) && atexit(_BBAtExit)); int _BBCntrInc= _BBCntrs[0]++; for (i= 2; i < n; i++) {
    _BBCntrs[1]++; if (n % i == 0) {
      _BBCntrs[2]++; return 0;
    }
  }
  { _BBCntrs[3]++; return 1; } 
}

int 
main()
{
  const int n= 1000;
  int i;
  int _BBInit= _BBProf.isInit || ((_BBProf.isInit= 1) && atexit(_BBAtExit)); int _BBCntrInc= _BBCntrs[4]++; for (i= 2; i <= n; i++) {
    _BBCntrs[5]++; if (prime(i)) {
      _BBCntrs[6]++; printf("%d\n", i);
    }
  }
  { _BBCntrs[7]++; return 0; } 
}
static unsigned _BBCntrs[8];
static _BBFNameType _BBFNames[]= {
  { 8, "primes.c" }
};

static unsigned _BBLineNums[]= {
  7,  8,  9,  12,  20,  21,  22,  25 
};

static _BBProfType _BBProf= {
 &_BBCntrs[0], &_BBFNames[0], &_BBLineNums[0], 8, 0
};

extern void _bbProfOut(_BBProfType *);
static void _BBAtExit(void) { _bbProfOut(&_BBProf); }
