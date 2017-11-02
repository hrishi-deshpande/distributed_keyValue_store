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
#define NR_REPLICA 1
#define XACT_AREA_SIZE 1
#define MAX_NR_RETRY 100

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

int* element_count = NULL;
int *arr = NULL;

int NUM_ELEMENTS = 0;
int NR_XACT_AREAS = 0;

static void open_and_mmap_files() {
	mmap_len = NR_REGIONS * DR_PAGE_NR * DSNVM_PAGE_SIZE;

	NUM_ELEMENTS = mmap_len/4 - 1;
	NR_XACT_AREAS = NUM_ELEMENTS + 1;

	printf("NR Pages: %d\nNR Regions: %d\nmmap_len: %lu \n", NR_PAGES, NR_REGIONS, mmap_len);

	printf("\nNo of array elements allowed: %d\n", NUM_ELEMENTS);
	//Open a dsnvm file
	fd_dsnvm = open("/mnt/hotpot/btest", O_RDWR|O_CREAT);
	if (fd_dsnvm < 0) {
		die("Cannot open file (1):%s", strerror(fd_dsnvm));
	}

	//mmap DSNVM regions
	virt_dsnvm = mmap(NULL, mmap_len, PROT_WRITE, MAP_SHARED, fd_dsnvm, 0);
	if (virt_dsnvm == MAP_FAILED) {
		die("Cannot mmap (1): %s", strerror(errno));
	}
}

static struct dsnvm_xact_header *xact_header;
static struct dsnvm_addr_len *xact_areas;
static void *xact_struct;

static void init_xact() {
	size_t malloc_size;

	malloc_size = sizeof(struct dsnvm_xact_header);
	malloc_size += NR_XACT_AREAS*sizeof(struct dsnvm_addr_len);
	
	xact_struct = malloc(malloc_size);
	if (xact_struct == NULL) die("OOM");
	xact_header = xact_struct;
	xact_header->rep_degree = NR_REPLICA;
	xact_header->xact_id = -1;
	xact_areas = xact_struct + sizeof(struct dsnvm_xact_header);
}

static void init_xact_area() {
	int i = 0;

	unsigned long base;

	base = (unsigned long)virt_dsnvm;
	
	for (i = 0; i < NR_XACT_AREAS; i++) {
		xact_areas[i].addr = base + i*XACT_AREA_SIZE;
		xact_areas[i].len = XACT_AREA_SIZE;
	}
}

static void commit_xact() {
	int ret = 0;
	unsigned long round = 0;

    retry:
	ret = msync(xact_struct, *element_count, DSNVM_COMMIT_XACT_FLAG);
	
	if (ret == DSNVM_RETRY) {
		round++;
		if (round > MAX_NR_RETRY) {
			die("Too many commit rounds (%lu)", round);
		}
		goto retry;
	} else if (unlikely(ret < 0)) {
		die("Failed to commit transaction ret_value: %d", ret);
	}
}

static void close_files() {
	if (fd_dsnvm > 0) close(fd_dsnvm);
}

int main(int argc, char **argv) {
	open_and_mmap_files();	
	printf("Open and mmap successful\n");

	init_xact();
	init_xact_area();

	element_count = virt_dsnvm;

	printf("Element count: %d\n", *element_count);
	arr = virt_dsnvm + sizeof(int);
	
	int choice = 0, item  = 0, i = 0;
	while (1) {
		printf("\n1. Add item \n2. Print All\n3. getArraySize()\n4. Exit\nEnter your choice:");
		scanf("\n%d", &choice);
		switch (choice) {
			case 1: printf("\nInput item value:");
				scanf("%d", &item);
				if (*element_count == NUM_ELEMENTS) {
					die("Out of memory");
				}
				arr[(*element_count)++] = item; 
				if (*element_count % 5 == 0) commit_xact();
				break; 
			case 2: for (i = 0; i < *element_count; i++) {
					printf("%d ", arr[i]);
				}
				printf("\n");
				break;
			case 3: printf("No of elements: %d\n", *element_count);
				break;
			case 4: die("exiting");
			default: printf("\nInvalid choice!! please reenter!\n");
		}
	}
	close_files();
}

