#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include "keyValueStore.h"

#include "dsnvm.h"
#include "../Hotpot/hotpot/dsnvm-helper.h"
#include "../Hotpot/hotpot/dsnvm-common.h"
#include "genericApi.h"

//Define parameters for the size of the dataset
#define NR_REGIONS 32 //no of regions in the dataset
#define NR_PAGES (NR_REGIONS * DR_PAGE_NR) //no of pages = no of regions * pages per region

//Define parameters for the transaction request
#define NR_REPLICA 1
#define XACT_AREA_SIZE 1
#define MAX_NR_RETRY 100
#define TABLE_SIZE 67
#define MAX_UNCOMMITED_AREAS 5

Node *hashTable[TABLE_SIZE];
KeyType kType;
static void* virt_dsnvm;
static int fd_dsnvm;
static size_t mmap_len;
void* curAddr;
void *maxAddr;
 
static void open_and_mmap_files() {
	mmap_len = NR_REGIONS * DR_PAGE_NR * DSNVM_PAGE_SIZE;

	printf("NR Pages: %d\nNR Regions: %d\nmmap_len: %lu \n", NR_PAGES, NR_REGIONS, mmap_len);

	//Open a dsnvm file
	fd_dsnvm = open("/mnt/hotpot/keyValueStore", O_RDWR|O_CREAT);
	if (fd_dsnvm < 0) {
		die("Cannot open file (1):%s", strerror(fd_dsnvm));
	}

	//mmap DSNVM regions
	virt_dsnvm = mmap(NULL, mmap_len, PROT_WRITE, MAP_SHARED, fd_dsnvm, 0);

	if (virt_dsnvm == MAP_FAILED) {
		die("Cannot mmap (1): %s", strerror(errno));
	}
	
	maxAddr = (char*)virt_dsnvm + mmap_len - 1;
	curAddr = virt_dsnvm;
}

static struct dsnvm_xact_header *xact_header;
static struct dsnvm_addr_len *xact_areas;
static void *xact_struct;

static void init_xact() {
	size_t malloc_size;

	malloc_size = sizeof(struct dsnvm_xact_header);
	malloc_size += MAX_UNCOMMITED_AREAS*sizeof(struct dsnvm_addr_len);
	xact_struct = malloc(malloc_size);
	if (xact_struct == NULL) die("OOM");
	xact_header = (struct dsnvm_xact_header*)xact_struct;
	xact_header->rep_degree = NR_REPLICA;
	xact_header->xact_id = -1;
	xact_areas = (struct dsnvm_addr_len*)((char*)xact_struct + sizeof(struct dsnvm_xact_header));
}

static void commit_xact() {
	int ret = 0;
	unsigned long round = 0;

    retry:
	ret = msync(xact_struct, MAX_UNCOMMITED_AREAS, DSNVM_COMMIT_XACT_FLAG);
	
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

static void append_xact_commit_area(Node *node) {
	static int noOfAreas = 0;	
	
	xact_areas[noOfAreas].addr = (unsigned long)node;
	xact_areas[noOfAreas].len = node->totSize;
	noOfAreas++;

	if (noOfAreas == MAX_UNCOMMITED_AREAS) {
		commit_xact();
	}
}
/*static void init_xact_area() {
	int i = 0;
	unsigned long base;

	base = (unsigned long)virt_dsnvm;
	
	for (i = 0; i < NR_XACT_AREAS; i++) {
		xact_areas[i].addr = base + i*XACT_AREA_SIZE;
		xact_areas[i].len = XACT_AREA_SIZE;
	}
}*/

static void close_files() {
	if (fd_dsnvm > 0) close(fd_dsnvm);
}

static void createOrLoadStore() {
	int i = 0;
	for (i = 0; i < TABLE_SIZE; i++) {
		hashTable[i] = (Node*)curAddr;
		curAddr += sizeof(Node);
	}
}

int main(int argc, char **argv) {
	open_and_mmap_files();	
	printf("Open and mmap successful\n");

	init_xact();
	createOrLoadStore();

	printf("\nEnter key Type: 1.Int 2.String\n");
	scanf("%d", &kType);
	if (kType != 1 && kType != 2) die("Invalid Key Type");

	int choice = 0, item  = 0, i = 0;
	char value[256] = {'\0'};
	char strKey[256] = {'\0'};
	int intKey = -1;
	Key key;
	while (1) {
		printf("\n1.Add item \n2.Get Item \n3.Scan \n4.Update Item \n5.Exit\nEnter your choice:");
		scanf("\n%d", &choice);
		switch (choice) {
			case 1: printf("\nInput item key:\n");
				if (kType == INT) {
				    scanf("%d", &intKey);
				    key.intKey = intKey;				   	    		} else {
				    scanf("%s", strKey);
			 	    key.strKey = strKey;
				}
				printf("\nInput item value:");
				scanf("%s", value);
				putItem(key, value);
				break; 
			case 2: if (kType == INT) {
                                    scanf("%d", &intKey);
                                    key.intKey = intKey;                        				} else {
                                    scanf("%s", strKey);
                                    key.strKey = strKey;
                                }
			 	Value v = getItem(key);	
				break;
			case 3: scan();
				break;
			case 4: break;
			case 5: die("exiting");
			default: printf("\nInvalid choice!! please reenter!\n");
		}
	}
	close_files();
}
