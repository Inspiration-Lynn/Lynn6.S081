//ϵͳ���õ�����ʵ��
//add the real implementation of your method here

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
//lab2: sysinfo
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void)
{
  int n;   //trace argument eg:2(b10) 32(b10000)
  if(argint(0, &n) < 0)
    return -1;
  myproc()->mask = n;
  return 0;
}

uint64
sys_sysinfo(void)
{
  struct sysinfo systemInfo;
  uint64 addr;    // user pointer to struct sysinfo
  struct proc *p = myproc();

  if(argaddr(0, &addr) < 0)   // user mode pass argument: struct sysinfo*(to receive return value from kernel)
    return -1;
  systemInfo.freemem = freemem_cnt();    // amount of free memory (bytes)
  systemInfo.nproc = process_cnt();      // number of process

  // copy a struct sysinfo back to user space
  // Copy len bytes from src to virtual address dstva in a given page table.
  // Return 0 on success, -1 on error.
  // int copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len)
  if(copyout(p->pagetable, addr, (char *)&systemInfo, sizeof(systemInfo)) < 0)
    return -1;
  return 0;
}