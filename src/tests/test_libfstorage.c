/*-
 * (c) 2009 by Author
 *
 * Author: Lukasz Podkalicki <lukasz@podkalicki.com>
 */

#include "misc.h"
#include "fstorage.h"

int
main(void)
{
	fs_ctx_t *ctx;
	size_t size;
	
	EXPECT_INIT(1);

	fprintf(stderr, "[ fstorage_init ] zainicjowanie context'u (fs_ctx_t)\n");
	EXPECT_OK(fstorage_init("fstp://fs1@127.0.0.1:8001", 5, NULL, &ctx));
	
	
	fprintf(stderr, "[ fstorage_file_send ] wysłanie pliku , key = plum,  file = ./test_crc.c\n");
	EXPECT_OK(fstorage_file_send(ctx, "plum", "./test_crc.c"));
	
	fprintf(stderr, "[ fstorage_file_recv ] odbieranie pliku , key = plum, file = ./test_crc.tmp1\n");
	EXPECT_OK(fstorage_file_recv(ctx, "plum", "./test_crc.tmp", &size));
	
	fprintf(stderr, "[ fstorage_file_replace ] podmiana pliku , key = plum, file = ./test_libfstorage.c\n");
	EXPECT_OK(fstorage_file_replace(ctx, "plum", "./test_libfstorage.c"));
	
	fprintf(stderr, "[ fstorage_file_recv ] odbieranie pliku , key = plum, file = ./test_crc.tmp2\n");
	EXPECT_OK(fstorage_file_recv(ctx, "plum", "./test_crc.tmp2", &size));
	
	fprintf(stderr, "[ fstorage_file_remove ] odbieranie pliku , key = plum, file = ./test_crc.tmp2\n");
	EXPECT_OK(fstorage_file_remove(ctx, "plum"));
	
	fprintf(stderr, "[ fstorage_file_recv ] próba odbieranie pliku , key = plum, file = ./test_crc.tmp3\n");
	EXPECT_OK(fstorage_file_recv(ctx, "plum", "./test_crc.tmp3", &size));
	
	fstorage_fini(ctx); /* void */

	return (ERR_OK);
}
