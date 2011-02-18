/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include <math.h>
#include <string.h>

#include "fstorage_ns.h"
#include "fstorage_str.h"

/**
 * Koduje liczbe dziesietna do zadanego systemu liczbowego.
 * @param num: liczba dziesietna
 * @param base: system liczbowy (2-binarny, 3,4,5, ... ,62)
 * @param ciphertext: zakodowana liczba, pointer to string
 * @return:
 *		ERR_* as integer
 */
int
fs_ns_encode(uint64_t num, uint8_t base, char *ciphertextp)
{
	unsigned int i;
	uint64_t x, L;
	char tmp[256], buff[256];
	
	memset(&tmp, 0, 256);
	memset(&buff, 0, 256);
	
	L = num;
	i = 0;
	
	while (L > 0) {
		
		x = L % base;
		
		if (x < 10)
			tmp[i] = (char)(x + (int)'0');
		else if (x < 36)
			tmp[i] = (char)((x-10) + (int)'A');
		else 
			tmp[i] = (char)((x-35) + (int)'a');
		
		if (!tmp[i])
			tmp[i] = '0';
		
		L = (L - (L % base)) / base;
		
		i++;
	}
	
	for (i = 0; i < strlen(tmp); i++)
		buff[i] = tmp[strlen(tmp) - i - 1];
	
	memcpy(ciphertextp, buff, strlen(buff) + 1);
	ciphertextp[strlen(buff)] = '\0';
	
	return ERR_OK;
}

/**
 * Dekoduje liczbe z zadanego systemu liczbowego do dziesietnego.
 * @param ciphertext: zakodowana liczba
 * @param base: system liczbowy (2-binarny, 3,4,5, ... ,62)
 * @param num: liczba dziesietna, pointer to cardinal
 * @return:
 *		ERR_* as integer
 */
int
fs_ns_decode(const char *ciphertext, uint8_t base, uint64_t *num)
{
	unsigned int i, n;
	uint64_t L;
	
	if (ciphertext == NULL || ciphertext == '\0')
		return ERR_PARAM;
	
	n = strlen(ciphertext);
	L = 0;
	
	for (i = 0; i < strlen(ciphertext); i++) {
		
		if (str_charin(ciphertext[i], "0123456789"))
			L += pow(base,n-1) * ((int)ciphertext[i] - (int)'0');
		else if (str_charin(ciphertext[i], "ABCDEFGHIJKLMNOPRSTQUWVXYZ"))
			L += pow(base,n-1) * ((int)ciphertext[i] - (int)'A' + 10);
		else
			L += pow(base,n-1) * ((int)ciphertext[i] - (int)'a' + 35);
		n--;
	}	
	
	*num = L;
	
	return ERR_OK;
}
