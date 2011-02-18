/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include <stdio.h>
#include "fstorage_str.h"

bool
str_charin(char c, const char *chars)
{
        unsigned int chars_len;
        
        if (    (chars != NULL) && 
                ((chars_len = strlen(chars)) > 0)) {
                for (unsigned int i = 0; i < chars_len; i++) {
                        if (chars[i] == c)
                                return (true);
                }
        }
        
        return (false);
}

int
str_rstrip(const char *str, const char *chars, char *resultp)
{
        unsigned int str_len, i, n;
        
        if ((str == NULL) || ((str_len = strlen(str)) <= 0))
                return (ERR_STR_LENGTH);
        
        i = str_len - 1;
        n = 0;
        while (i > 0) {
                if (!str_charin(str[i], chars)) 
			break;
                n++; i--;
        }
	
        strncpy(resultp, str, strlen(str) - n + 1);
        resultp[strlen(str) - n] = '\0';
	
        return (ERR_OK);
}

int
str_lstrip(const char *str, const char *chars, char *resultp)
{
        unsigned int str_len, i, n;
        const char *ptr;
        
        if ((str == NULL) || ((str_len = strlen(str)) <= 0))
                return (ERR_STR_LENGTH);
                
        i = n = 0;
        ptr = str;
        while (i < str_len) {
                if (!str_charin(str[i], chars)) break;
                n++; i++;
        }
        
        ptr += n;
	
        strncpy(resultp, ptr, strlen(ptr) + 1);
	resultp[strlen(ptr)] = '\0';
        
        return (ERR_OK);
}

int
str_strip(const char *str, const char *chars, char *resultp)
{
        int err;
        char tmp[2048];
	
        if ((err = str_rstrip(str, chars, (char *)&tmp)) != ERR_OK)
                return (err);
        
        if ((err = str_lstrip(tmp, chars, resultp)) != ERR_OK)
                return (err);
                
        return (ERR_OK);
}

int
str_split(const char *str, const char *pattern, unsigned int *cnt, char ***strs)
{
        unsigned int i, len=0, str_len=0, pattern_len=0;
        char *tmp, **tab;
        const char *ptr;
        bool ok = true;
        
        if ((str == NULL) || ((str_len = strlen(str)) == 0) || (pattern == NULL) || ((pattern_len = strlen(pattern)) == 0))
                return (ERR_STR_LENGTH);
        
	if ((tab = malloc(1024 * sizeof(char *))) == NULL)
		return (ERR_MEMORY);
	
	memset(tab, 0, 1024);
	
        ptr = str;
        *cnt = 0;
        i = 0;
        while (ok) {
		if (ptr == NULL || strlen(ptr) == 0)
			break;
		
                ok = (tmp = strstr(ptr, pattern)) != NULL;
		
                len = ok ? (strlen(ptr) - strlen(tmp)) : strlen(ptr);
		
              
		tab[i] = (char *)malloc(len + 1);
	
		memset(tab[i], 0, len + 1);
		strncpy(tab[i], ptr, len);
		tab[i][len] = '\0';
		
		ptr += len + pattern_len;
		
                i++;
        }
        
	tab[i+1] = NULL;
        *cnt = i;
        *strs = tab;
        
        return ((i > 0) ? ERR_OK : ERR_NO_RESULT);
}

int
str2upper(const char *str, char *resultp)
{
        unsigned int str_len;
        
        if ((str == NULL) || ((str_len = strlen(str)) == 0))
                return (ERR_STR_LENGTH);
        
        for (unsigned int i = 0; i < str_len; i++)
                resultp[i] = toupper(str[i]);
	
        resultp[str_len] = '\0';
	
        return (ERR_OK);
}

int
str2lower(const char *str, char *resultp)
{
        unsigned int str_len;
        
        if ((str == NULL) || ((str_len = strlen(str)) == 0))
                return (ERR_STR_LENGTH);
        
        for (unsigned int i = 0; i < str_len; i++)
                resultp[i] = tolower(str[i]);
        
	resultp[str_len] = '\0';
	
        return (ERR_OK);
}

int
str2int(const char *str, int *result)
{
	
        if (str == NULL || strlen(str) == 0)
                return (ERR_STR_LENGTH);
                
        return ((sscanf(str, "%d", result) == 1) ? ERR_OK : ERR_TYPE);
}

int
str2uint64_t(const char *str, uint64_t *result)
{
	
	if (str == NULL || strlen(str) == 0)
                return (ERR_STR_LENGTH);
                
        return ((sscanf(str, "%lu", result) == 1) ? ERR_OK : ERR_TYPE);
}

int
str2bool(const char *str, bool *result)
{
        char tmp[256];
        int err;
        
        if ((err = str_strip(str, " \n\r\t", (char *)&tmp) != ERR_OK))
                return (err);
        
        if (    strcmp(tmp, "true") == ERR_OK ||
                strcmp(tmp, "yes") == ERR_OK ||
                strcmp(tmp, "TRUE") == ERR_OK ||
                strcmp(tmp, "YES") == ERR_OK ||
                strcmp(tmp, "1") == ERR_OK) {
                *result = true;
                err = ERR_OK;
        }       
        else if (strcmp(tmp, "false") == ERR_OK ||
                strcmp(tmp, "no") == ERR_OK ||
                strcmp(tmp, "FALSE") == ERR_OK ||
                strcmp(tmp, "NO") == ERR_OK ||
                strcmp(tmp, "0") == ERR_OK) { 
                *result = false;
                err = ERR_OK;
        }
        else {
                err = ERR_TYPE;
        }
        
        return (err);
}

void
str_free(char *str)
{
        
        if (str == NULL)
                return;
                
        free(str);
}

void
strs_free(char **strs)
{
        
        if (strs == NULL)
                return;
        
        for (unsigned int i = 0; strs[i] != NULL; i++) {
		if (strs[i] != NULL) {
			str_free(strs[i]);
		}
	}
	
	free(strs);
}
