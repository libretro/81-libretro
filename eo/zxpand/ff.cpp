
/*----------------------------------------------------------------------------/
/  FatFs - FAT file system module  R0.07e                    (C)ChaN, 2009
/-----------------------------------------------------------------------------
/ Completely hacked to use a local subfolder for EightyOne
/----------------------------------------------------------------------------*/

#include <stdint.h>
#include "ff.h" /* FatFs configurations and declarations */
#include "diskio.h"

#include "zxpand_host.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h> /* PATH_MAX */
#include <stdio.h>
#include "zxpand_host.h"
#include <fcntl.h>
#include "zxpand_host.h"

extern void loadFileSymbolsProxy(const char *);

struct stat stats;

extern int dirOpen(const char *path);
extern void dirReadNext(void *fileinfo);

// rootpath, curpath; always slash-terminated.
//
const char *zxpandSDCardFolderRoot = NULL;
char curPath[32768] = {0};
char tempPath[32768] = {0};

void AddSlash(char *p)
{
   size_t n = strlen(p);
   if (p[n - 1] != '/' && p[n - 1] != '\\')
   {
      p[n] = '/';
      p[n + 1] = 0;
   }
}

extern void dirGetExeLocation(char *buffer, int bufSize);

void SetRoot(const char *root)
{
   dirGetExeLocation(tempPath, 32768);

   AddSlash(tempPath);
   strcat(tempPath, root);
   AddSlash(tempPath);

   char *t = tempPath;
   for (; *t; ++t)
   {
      if (*t == '\\')
      {
         *t = '/';
      }
   }

   zxpandSDCardFolderRoot = strdup(tempPath);
   strcpy(curPath, zxpandSDCardFolderRoot);
}

// builds an absolute path to an actual folder in the filing system
// given a 'zxpand sd card root' relative path
const char* createFullPath(const char *path)
{
   char buf[32768];

   if (path == NULL)
   {
      return NULL;
   }

   if (*path == '/')
   {
      strcpy(buf, zxpandSDCardFolderRoot);
      ++path;
   }
   else
   {
      strcpy(buf, curPath);
   }
   strcat(buf, path);

   // canonicalize
   if (_fullpath(tempPath, buf, 32768) == NULL)
      return NULL;

   // Replace backslashes with forward slashes so the
   // rest of the code behaves correctly.
   for (unsigned int idx = 0; idx < strlen(tempPath); ++idx)
   {
      if (tempPath[idx] == '\\')
         tempPath[idx] = '/';
   }

   // if input path is a folder then add a slash
   struct stat lstats;
   if (-1 != stat(tempPath, &lstats))
   {
        if (lstats.st_mode & _S_IFDIR) {
                AddSlash(tempPath);
        }
   }

   // specified relative path took us outside of the card root directory
   if (strlen(tempPath) < strlen(zxpandSDCardFolderRoot))
      return NULL;

   return tempPath;
}

void mem_cpy(void *dst, const void *src, int cnt)
{
   char *d = (char *)dst;
   const char *s = (const char *)src;
   while (cnt--)
      *d++ = *s++;
}

char chk_chr(const char *str, char chr)
{
   while (*str && *str != chr)
      str++;
   return *str;
}

FRESULT cvtERRNO(void)
{
   switch (errno)
   {
   case EACCES: // Tried to open read-only file for writing, file's sharing mode does not allow specified operations, or given path is directory.
      return FR_DENIED;

   case EEXIST: // _O_CREAT and _O_EXCL flags specified, but filename already exists.
      return FR_EXIST;

   case EINVAL: // Invalid oflag or pmode argument.
      return FR_INVALID_OBJECT;

   case ENOENT: // File or path not found.
      return FR_NO_PATH;

   default:
      return FR_INT_ERR;
   }
}

void SetHandle(FIL *fp, int handle)
{
   fp->fs = (FATFS *)(intptr_t)handle;
}

int GetHandle(FIL *fp)
{
   return (int)(intptr_t)fp->fs;
}

/*-----------------------------------------------------------------------*/
/* Helper                                                                */
/*-----------------------------------------------------------------------*/

void get_fileinfo_special(struct _FILINFO_ *fi)
{
   fi->fsize = stats.st_size;
   if (!(stats.st_mode & _S_IWRITE))
   {
      fi->fattrib = AM_RDO;
   }
   else
   {
      fi->fattrib = 0;
   }
}

/*-----------------------------------------------------------------------*/
/* Mount a logical filesystem                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_mount(
    BYTE vol, /* Logical drive number to be mounted/unmounted */
    FATFS *fs /* Pointer to new file system object (NULL for unmount)*/
)
{
   return FR_OK;
}

/*-----------------------------------------------------------------------*/
/* Open or Create a File                                                 */
/*-----------------------------------------------------------------------*/

FRESULT f_open(
    FIL *fp,           /* Pointer to the blank file object */
    const XCHAR *path, /* Pointer to the file name */
    BYTE mode          /* Access mode and file open mode flags */
)
{
   if (GetHandle(fp) != 0)
   {
      _close(GetHandle(fp));
      SetHandle(fp, 0);
   }

   int flags = _O_BINARY;
   if (mode & FA_WRITE)
   {
      flags |= _O_CREAT | _O_TRUNC | _O_RDWR;
      if (mode & FA_CREATE_NEW)
      {
         // fail if exists
         flags |= _O_EXCL;
      }
   }

   // use stat(name,...) instead of fstat(handle,...)

   const char *fullNme = createFullPath(path);
   if (NULL == fullNme) return FR_NO_PATH;

   int handle = open(fullNme, flags, _S_IREAD | _S_IWRITE);
   if (-1 == handle)
   {
      return cvtERRNO();
   }

   if (-1 == stat(fullNme, &stats))
   {
      _close(handle);
      return cvtERRNO();
   }

   fp->fsize = stats.st_size;
   fp->flag = 0;

   loadFileSymbolsProxy(fullNme);

   SetHandle(fp, handle);
   return FR_OK;
}

FRESULT f_lseek(FIL *fp, DWORD pos)
{
   if (lseek(GetHandle(fp), pos, SEEK_SET) == -1)
   {
      return cvtERRNO();
   }
   return FR_OK;
}

/*-----------------------------------------------------------------------*/
/* Read File                                                             */
/*-----------------------------------------------------------------------*/

FRESULT f_read(
    FIL *fp,    /* Pointer to the file object */
    void *buff, /* Pointer to data buffer */
    UINT btr,   /* Number of bytes to read */
    UINT *br    /* Pointer to number of bytes read */
)
{
   int n = _read(GetHandle(fp), buff, btr);
   if (n < 0)
   {
      return cvtERRNO();
   }

   *br = (UINT)n;
   return FR_OK;
}

/*-----------------------------------------------------------------------*/
/* Write File                                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_write(
    FIL *fp,          /* Pointer to the file object */
    const void *buff, /* Pointer to the data to be written */
    UINT btw,         /* Number of bytes to write */
    UINT *bw          /* Pointer to number of bytes written */
)
{
   int n = _write(GetHandle(fp), buff, btw);
   if (n < 0)
   {
      return cvtERRNO();
   }

   *bw = (UINT)n;
   return FR_OK;
}

/*-----------------------------------------------------------------------*/
/* Close File                                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_close(
    FIL *fp /* Pointer to the file object to be closed */
)
{
   _close(GetHandle(fp));
   SetHandle(fp, 0);
   return FR_OK;
}

/*-----------------------------------------------------------------------*/
/* Change Current Drive/Directory                                        */
/*-----------------------------------------------------------------------*/

FRESULT f_chdrive(
    BYTE drv /* Drive number */
)
{
   return FR_OK;
}

FRESULT f_chdir(
    const XCHAR *path /* Pointer to the directory path */
)
{
   // path is zxpand-sd-relative

   FRESULT result = FR_NO_PATH;

   const char *absPath = createFullPath(path);
   if (NULL != absPath)
   {
      // Check that it exists
      if (0 == _access(absPath, 0))
      {
         strcpy(curPath, absPath);
         result = FR_OK;
      }
   }

   return result;
}

/*-----------------------------------------------------------------------*/
/* Open Directory ready fr reading                                       */
/*-----------------------------------------------------------------------*/

FRESULT f_opendir(
    DIR *dj,          /* Pointer to directory object to create */
    const XCHAR *path /* Pointer to the directory path */
)
{
   return (FRESULT)dirOpen(createFullPath(path));
}

/*-----------------------------------------------------------------------*/
/* Read Directory Entry in Sequense                                      */
/*-----------------------------------------------------------------------*/

FRESULT f_readdir(
    DIR *dj,     /* Pointer to the open directory object */
    FILINFO *fno /* Pointer to file information to return */
)
{
   dirReadNext(fno);
   return FR_OK;
}

FRESULT f_mkdir(const XCHAR *dirname)
{
   FRESULT ret = FR_OK;

   const char* absPath = createFullPath(dirname);
   if (NULL == absPath) return FR_NO_PATH;

   if (zxp_mkdir(absPath) == -1)
   {
      ret = cvtERRNO();
   }

   return ret;
}

/*-----------------------------------------------------------------------*/
/* Delete a File or Directory                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_rename(
    const XCHAR *path1, /* Pointer to the file or directory path */
    const XCHAR *path2  /* Pointer to the file or directory path */
)
{
   FRESULT ret = FR_OK;

   char tmpp[32768];

   const char* absPath1 = createFullPath(path1);
   if (NULL == absPath1) return FR_NO_PATH;

   strcpy(tmpp, absPath1);

   const char* absPath2 = createFullPath(path2);
   if (NULL == absPath2) return FR_NO_PATH;

   // a concatenation of the mmc dir & the requested path
   if (rename(tmpp, absPath2))
   {
      ret = cvtERRNO();
   }

   return ret;
}

/*-----------------------------------------------------------------------*/
/* Delete a File or Directory                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_unlink(
    const XCHAR *path /* Pointer to the file or directory path */
)
{
   FRESULT ret = FR_OK;
   const char *target = createFullPath(path);
   if (NULL == target) return FR_NO_PATH;

   if (-1 == stat(target, &stats))
   {
      return cvtERRNO();
   }

   if (stats.st_mode & S_IFDIR)
   {
      if (_rmdir(target) == -1)
      {
         ret = FR_DENIED;
      }
   }
   else
   {
      // a concatenation of the mmc dir & the requested path

      if (remove(target) == -1)
      {
         ret = cvtERRNO();
      }
   }

   return ret;
}
