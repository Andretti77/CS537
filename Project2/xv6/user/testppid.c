#include "types.h"
#include "stat.h"
#include "user.h"
int
main(void)
{
  int pid, ppid, orig_pid;

  printf(1, "ppid test\n");
  
  orig_pid = getpid();

  pid = fork();

  while(1){

   if(pid == 0){
	  
     ppid = getppid();
     printf(1, "pid = %d\n ppid = %d\n",orig_pid, ppid);

     if(ppid == -1){
	   printf(1, "no parent process\n");
	   exit();
     }else if(ppid== orig_pid){

	   printf(1, "success\n");
	   exit();
     }else{
	   printf(1, "failure\n");
	   exit();
     }

   }else{
	wait();
	exit();
   }

  }	  

  
  exit();
}
