#ifndef _SP0256_H
#define _SP0256_H

struct PHONE
{
        char *position;
        int length;
};

#ifdef __cplusplus
extern "C" {
#endif

void SP0256_Init(void);
void SP0256_Write(unsigned char Data);
unsigned char SP0256_Busy(void);

#ifdef __cplusplus
}
#endif

#endif
