#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>
#include <sys/mman.h>

#include "dsnvm.h"
#include "../Hotpot/hotpot/dsnvm-helper.h"
#include "../Hotpot/hotpot/dsnvm-common.h"

//Define parameters for the size of the dataset
#define NR_REGIONS 2 //no of regions in the dataset
#define NR_PAGES (NR_REGIONS * DR_PAGE_NR) //no of pages = no of regions * pages per region


//Define parameters for the transaction request

static void die(const char* fmt, ...) {
	va_list args;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	fputc('\n', stderr);
	exit (-1);
}

static void* virt_dsnvm;
static int fd_dsnvm;
static size_t mmap_len;

static void open_and_mmap_files() {
	mmap_len = NR_REGIONS * DR_PAGE_NR * DSNVM_PAGE_SIZE;

	printf("NR Pages: %d\n NR Regions: %d\n mmap_len: %u \n", NR_PAGES, NR_REGIONS, DR_PAGE_NR);

	//Open a dsnvm file
	fd_dsnvm = open("/mnt/hotpot/test", O_RDWR|O_CREAT);
	if (fd_dsnvm < 0) {
		die("Cannot open file (1):%s", strerror(fd_dsnvm));
	}

	//mmap DSNVM regions
	virt_dsnvm = mmap(NULL, mmap_len, PROT_WRITE, MAP_SHARED, fd_dsnvm, 0);
	if (virt_dsnvm == MAP_FAILED) {
		die("Cannot mmap (1): %s", strerror(errno));
	}
}


static void close_files() {
	if (fd_dsnvm > 0) close(fd_dsnvm);
}

int main(int argc, char **argv) {
	open_and_mmap_files();
	
	printf("Open and mmap successful\n");

	close_files();
}

