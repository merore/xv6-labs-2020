#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "fcntl.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "proc.h"
#include "fs.h"
#include "file.h"
#include "mmap.h"

struct {
  struct spinlock lock;
  struct mmap mmap[NMMAP];
} mtable;

struct mmap*
mmapalloc(void)
{
  struct mmap *m;
  acquire(&mtable.lock);
  for(m = mtable.mmap; m < mtable.mmap + NMMAP; m++) {
    if(m->addr == 0) {
      m->addr = 1;
      release(&mtable.lock);
      return m;
    }
  }
  release(&mtable.lock);
  return 0;
}

struct mmap*
mmapdup(struct mmap *m)
{
  struct mmap* nm;
  if((nm = mmapalloc()) == 0)
    return 0;
  filedup(m->file);
  *nm = *m;
  return nm;
}

// 根据地址，返回一个正在映射中的 mmap
struct mmap*
getmmap(uint64 addr)
{
  struct mmap **m;
  struct proc *p = myproc();
  for(m = p->mmap; m < p->mmap + NOMMAP; m++)
    if(*m)
      if(addr >= (*m)->begin && addr < (*m)->end)
        return *m;

  return 0;
}

void
mmapclose(struct mmap *m, uint64 begin, uint64 end)
{
  struct proc *p = myproc();

  if(m->flags & MAP_SHARED) {
    begin_op();
    writei(m->file->ip, 1, begin, begin-m->addr, end-begin);
    end_op();
  }
  uvmunmap(p->pagetable, begin, (end-begin)/PGSIZE, 1);

  acquire(&mtable.lock);
  if(begin = m->begin)
    m->begin = end;
  if(end = m->end)
    m->end = begin;
  if(m->end < m->begin)
    m->addr = 0;
  release(&mtable.lock);

  // 这一段代码很奇怪，因为fileclose 可能陷入休眠，而在休眠期间，这个进程不能持有任何锁
  if(m->end < m->begin)
    fileclose(m->file);
}

int
mmaptrap(uint64 scause, uint64 addr)
{
  uint64 va, pa;
  int perm, off, i;
  struct mmap *m;
  struct proc *p = myproc();
  va = PGROUNDDOWN(addr);

  if((m = getmmap(addr)) == 0)
    return -1;
  // check scause has right prot
  if(scause == 13 && !(m->prot & PROT_READ))
    return -1;
  if(scause == 15 && !(m->prot & PROT_WRITE))
    return -1;
  if ((pa = (uint64)kalloc()) == 0)
    return -1;

  perm|=PTE_U;
  if(m->prot & PROT_WRITE)
    perm|=PTE_W;
  if(m->prot & PROT_READ)
    perm|=PTE_R;
  if(m->prot & PROT_EXEC)
    perm|=PTE_X;
  if(mappages(p->pagetable, va, PGSIZE, pa, perm))
    goto bad;

  off = (va - m->addr)/PGSIZE * PGSIZE;
  memset((void*)pa, 0, PGSIZE);
  readi(m->file->ip, 0, pa, off, PGSIZE);

  return 0;
bad:
  kfree((void *)pa);
  return -1;
}

