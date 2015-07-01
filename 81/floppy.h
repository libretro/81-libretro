#include "765.h"

#ifdef __cplusplus
extern "C" {
#endif

extern FDC_PTR p3_fdc;
extern FDRV_PTR p3_drive_a;
extern FDRV_PTR p3_drive_b;
extern FDRV_PTR p3_drive_null;

void floppy_init(void);
void floppy_eject(int drive);
void floppy_setimage(int drive, char *filename);
void floppy_ClockTick(int ts);
void floppy_shutdown(void);

BYTE floppy_get_state(void);

void floppy_set_motor(BYTE Data);
void floppy_write_cmdreg(BYTE Data);
void floppy_write_trackreg(BYTE Data);
void floppy_write_secreg(BYTE Data);
void floppy_write_datareg(BYTE Data);
BYTE floppy_read_datareg(void);
BYTE floppy_read_statusreg(void);
BYTE floppy_read_trackreg(void);
BYTE floppy_read_secreg(void);

BYTE OpusD6821Access(BYTE reg, BYTE Data, BYTE Dir);

#ifdef __cplusplus
}
#endif
