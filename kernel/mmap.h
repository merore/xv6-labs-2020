struct mmap {
  uint64 addr;
  uint64 begin;
  uint64 end;
  int prot;
  int flags;
  int offset;
  struct file *file;
};
