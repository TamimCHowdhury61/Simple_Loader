#include "loader.h"
#define MAP_ANONYMOUS 0x20

Elf32_Ehdr *ehdr = NULL;
Elf32_Phdr *phdr = NULL;
int fd = -1;

void loader_cleanup() {
  if (ehdr) {
    free(ehdr);
    ehdr = NULL;
  }
  if (phdr) {
    free(phdr);
    phdr = NULL;
  }
  if (fd != -1) {
    close(fd);
    fd = -1;
  }
}

void load_and_run_elf(char **exe) {
  fd = open(exe[0], O_RDONLY);
  if (fd == -1) {
    perror("Error opening ELF file");
    return;
  }

  ehdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));
  if (read(fd, ehdr, sizeof(Elf32_Ehdr)) != sizeof(Elf32_Ehdr)) {
    perror("Error reading ELF header");
    return;
  }

  // Validate ELF header (magic number and other sanity checks)
  if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0 || ehdr->e_type != ET_EXEC) {
    perror("Not a valid ELF executable");
    return;
  }

  phdr = (Elf32_Phdr *)malloc(ehdr->e_phentsize * ehdr->e_phnum);
  lseek(fd, ehdr->e_phoff, SEEK_SET);
  if (read(fd, phdr, ehdr->e_phentsize * ehdr->e_phnum) != (ssize_t)(ehdr->e_phentsize * ehdr->e_phnum)) {
    perror("Error reading program headers");
    return;
  }

  int loaded = 0;
  for (int i = 0; i < ehdr->e_phnum; i++) {
    if (phdr[i].p_type == PT_LOAD) {
      void *sgmt_addr = mmap((void *)(intptr_t)phdr[i].p_vaddr, phdr[i].p_memsz,
                             PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      if (sgmt_addr == MAP_FAILED) {
        perror("Error mapping memory segment");
        return;
      }

      lseek(fd, phdr[i].p_offset, SEEK_SET);
      if (read(fd, sgmt_addr, phdr[i].p_filesz) != (ssize_t)phdr[i].p_filesz) {
        perror("Error reading segment data");
        return;
      }
      loaded = 1;
    }
  }

  if (!loaded) {
    perror("No loadable segment found");
    return;
  }

  int (*_start)() = (int (*)())(intptr_t)(ehdr->e_entry);
  int result = _start();
  printf("User _start return value = %d\n", result);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: %s <ELF Executable>\n", argv[0]);
    exit(1);
  }

  load_and_run_elf(argv + 1);
  loader_cleanup();
  return 0;
}
