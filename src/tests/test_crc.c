/*-
 * (c) 2009 by Author
 *
 * Author: Lukasz Podkalicki <lukasz@podkalicki.com>
 */

#include "misc.h"
#include "fstorage.h"
#include "fstorage_crc.h"
#include "fstorage_ns.h"

/*
# 1
key = kluczyk :) bla bla bla dsdasdasdasdasd asdads
hash = 2549483569
cipher = 3Gi9ro # 60
L = 2549483569
D = 49

# 2
key = x_1
hash = 3378422107
cipher = 4KfpZ7
L = 3378422107
D = 91

# 3
key = x_2
hash = 1347940577
cipher = 1j0RvH
L = 1347940577
D = 225

*/
int
main(void)
{
	const char *key = "plumek";
	uint64_t hash, L;
	uint8_t D;
	
	char buff[256], buff2[256];
	
	fprintf(stderr, "\nkey = %s\n", key);
	
	hash = fs_crc(key, strlen(key));
	fprintf(stderr, "hash = %llu\n", hash);
	
	fs_ns_encode(hash, 60, (char *)&buff);
	fprintf(stderr, "cipher = %s\n", buff);
	
	fs_hash(key, (char *)&buff2);
	fprintf(stderr, "fs_hash = %s\n", buff2);
	
	fs_ns_decode((char *)buff, 60, &L);
	fprintf(stderr, "L = %llu\n", L);
	
	D = L % 256;
	fprintf(stderr, "D = %u\n", D);
	
	EXPECT_INIT(0);
	
	return (ERR_OK);
}
