#include <stdio.h>
#if ((!(defined(__PS3__) && !defined(__PSL1GHT))) && !defined(__APPLE__))
#include <malloc.h>
#endif
#include <string.h>

#include "sp0256.h"
#include "zx81config.h"

#include <funcs.h>

struct PHONE *Phones=NULL;

void SP0256_Init(void)
{
        FILE *f;
        int a,i,len;
        char FileName[256];
        int offset;

        if (Phones) free(Phones);
        Phones=NULL;

        strcpy(FileName,zx81.cwd);
        if (FileName[strlen(FileName)-1]=='\\') FileName[strlen(FileName)-1]='\0';
        strcat(FileName,"\\rom\\sp0256.bin");

        f=fopen(FileName,"rb");
        if (!f) return;

        fseek(f, 0, SEEK_END);
        len=ftell(f);
        fseek(f, 0, SEEK_SET);

        if (!len)
        {
                fclose(f);
                return;
        }

        Phones=malloc(len);
        if (!Phones)
        {
                fclose(f);
                return;
        }

        fread(Phones, 1, len, f);
        fclose(f);

        offset=64*sizeof(struct PHONE);

        for(i=0;i<64;i++)
        {
                a =(int)Phones[i].position;
                a += offset;
                Phones[i].position = (((char *)Phones) + a);
                a=0;
        }
}

static int Last=0;

void SP0256_Write(unsigned char Data)
{
        if (!Phones) return;
        if (!Data) return;
        if (Data==Last) return;

        Last=Data;

        PlaySound(NULL, NULL, SND_PURGE);
        PlaySound(Phones[Data&63].position, NULL, SND_MEMORY | SND_ASYNC);
}

unsigned char SP0256_Busy(void)
{
        if (!Phones) return(0);
        return(!PlaySound(Phones[0].position, NULL, SND_MEMORY | SND_ASYNC | SND_NOSTOP));
}

