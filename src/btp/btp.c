/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include "fstorage_btp.h"


int
btp_init(int fd, btp_t **btpp)
{
        if ((*btpp = malloc(sizeof(btp_t))) == NULL)
                return (ERR_MEMORY);
        
        if (fd <= 0)
		return (ERR_SOCKET);
                
	(*btpp)->fd = fd;
        
        return (ERR_OK);
}

void
btp_fini(btp_t *btp)
{
		
        if (btp == NULL)
                return;
	
	if (btp->fd > 0)
		close(btp->fd);
	
        free(btp);
}

int
btp_snd(btp_t *btp, btp_pkg_t *pkg)
{
	int n;
	
        n = write(btp->fd, pkg, btp_pkg_size(pkg));
	
	//fprintf(stderr, "[BTP_SND] Sending PKG, opcode = %d\n", btp_pkg_get_opcode(pkg));
	//fprintf(stderr, "[BTP_SND] Sent %d bytes\n", n);
	
        return (n == (int)btp_pkg_size(pkg) ? ERR_OK : ERR_CONNECT);
}

int
btp_rcv(btp_t *btp, btp_pkg_t **pkgp)
{
	unsigned int bytes;
	uint32_t size;
	
	*pkgp = malloc(BTP_PKG_SIZE_MIN);
	memset(*pkgp, 0, BTP_PKG_SIZE_MIN);
	
	bytes = read(btp->fd, *pkgp, BTP_PKG_SIZE_MIN);
	
	//fprintf(stderr, "[BTP_RCV] Received %d bytes\n", bytes);
	
	if (bytes == 0)
		return (ERR_SOCKET);
	
	if ((bytes > 0) && (bytes < BTP_PKG_SIZE_MIN))
		return (ERR_CORRUPTED);
	
	size = btp_pkg_size(*pkgp);

	if (size > BTP_PKG_SIZE_MAX)
		return (ERR_SIZE);
	
	*pkgp = realloc(*pkgp, size);
	
	if ((bytes = size - BTP_PKG_SIZE_MIN) > 0)
		bytes = read(btp->fd, (*pkgp + BTP_PKG_SIZE_MIN) , bytes);
		
	if (bytes != (size - BTP_PKG_SIZE_MIN))
		return (ERR_CORRUPTED);
	
	return (ERR_OK);
}

int
btp_snd_file(btp_t *btp, const char *filepath)
{
	int err, n;
	char buff[BTP_FILE_PART_SIZE];
	FILE *fd;
	size_t filesize, partsize, bytes;
	
	if (!(fd = fopen(filepath, "rb")))
		return (ERR_FILESYSTEM);
	
	/* Getting file size */
	fseek(fd, 0, 2);
	filesize = ftell(fd);
	fseek(fd, 0, 0);
	
	/* Writing file to socket */
	bytes = 0;
	partsize = BTP_FILE_PART_SIZE;
	err = ERR_OK;
	while (bytes < filesize) {
		
		if ((filesize - bytes) <  BTP_FILE_PART_SIZE)
			partsize = filesize - bytes;
		
		partsize = fread(&buff, 1, partsize, fd);
		bytes += partsize;
		n = write(btp->fd, buff, partsize);
		if (n != (int)partsize) {
			err = ERR_CONNECT;
			break;
		}
	}
	
	fclose(fd);
	
	return (err);
}

int
btp_rcv_file(btp_t *btp, const char *filepath, size_t filesize)
{
	int err, n;
	char buff[BTP_FILE_PART_SIZE + 1];
	FILE *fd;
	size_t partsize, bytes;
	
	if (!(fd = fopen(filepath, "wb")))
		return (ERR_FILESYSTEM);
	
	/* Reading file from socket */
	bytes = 0;
	partsize = BTP_FILE_PART_SIZE;
	err = ERR_OK;
	while (bytes < filesize) {
		if ((filesize - bytes) <  BTP_FILE_PART_SIZE)
			partsize = filesize - bytes;
		
		if ((partsize = read(btp->fd, &buff, partsize)) == 0) {
			fclose(fd);
			return (ERR_CONNECT);
		}
		
		bytes += partsize;
		
		n = fwrite(&buff, 1, partsize, fd);
		
		if (n != (int)partsize) {
			err = ERR_FILESYSTEM;
			break;
		}
		
		fflush(fd);
	}
	
	fclose(fd);
	
	return (ERR_OK);
}
