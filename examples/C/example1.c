/**
 * Simple example using libfstorage library.
 * @author: Lukasz Marcin Podkalicki <lukasz\@fstorage.pl>
 *
 * Compilation:
 *	gcc ./example1.c -o example1 -lfstorage
 * Runing:
 *	./example1
 */


#include <stdio.h>
#include <fstorage.h>


int
main (void)
{
	int err;
	fs_ctx_t *ctx;

	int fstorage_init(const char *fsuri, unsigned int debug, const char *logfile, fs_ctx_t **ctxp);

	/** Initialize fstorage context */
	if ((err = fstorage_init("fstp://127.0.0.1:8001", 0, NULL, &ctx)) != 0) {
		printf ("Error: %s\n", fstorage_strerror(err));
		return (err);
	}

	/** Removing any file with key "example in C" - if exist */
	if ((err = fstorage_file_remove(ctx, "example in C")) != 0) {
		printf ("Error: %s\n", fstorage_strerror(err));
	}

	/** Sending file ./example1.c with key="example in C" to fstoraged server */
	if ((err = fstorage_file_send(ctx, "example in C", "./example1.c")) != 0) {
		printf ("Error: %s\n", fstorage_strerror(err));
		return (err);
	}
	
	/** Finalize fstorage context */
	fstorage_fini(ctx);
	
	return (0); 
}
