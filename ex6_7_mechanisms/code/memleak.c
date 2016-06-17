/* create a memory leak */
#include <sys/resource.h>
#define MEMLIMIT (1024 * 1024 * 30)


//  echoBuffer = malloc (RCVBUFSIZE);
// inside if as well    echoBuffer = malloc (RCVBUFSIZE);


int main()
{
  struct rlimit rlim;

  /* set resource limits */
  rlim.rlim_cur = MEMLIMIT;
  rlim.rlim_max = MEMLIMIT;
  if (0 != setrlimit (RLIMIT_AS, &rlim))
    printErrorAndQuit ("setrlimit failed");
