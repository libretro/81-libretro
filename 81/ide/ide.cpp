#include <stdio.h>
#include <string.h>
#include "ide.h"

#include <funcs.h>

#define WriteWord(Base, WordNo, Value) { (Base)[(WordNo)*2]=(Value)&255; \
                                         (Base)[(WordNo)*2 +1] = ((Value)>>8)&255; }

ATA_CHANNEL ATA_Channel;
PHYSDRIVE PhysDrives[64];

static int PhysDriveID(unsigned char *buffer, int buflen, int *cyls, int *heads, int *secs);

void DetectPhysDrives(void)
{
        int i, drive;
        unsigned long size, size1, size2;
        char path[256];
        unsigned char buffer[512];
        HANDLE hDevice;               // handle to the drive to be examined
        BOOL bResult;                 // results flag
        DWORD junk;                   // discard results
        DISK_GEOMETRY pdg;
        PARTITION_INFORMATION pi;
        int heads,cyls,secs;
        unsigned long rubbish;
        int ReadOnly;

        for(i=0;i<64;i++) PhysDrives[i].Drive=-1;

        i=0;
        for(drive=0;drive<255;drive++)
        {
                sprintf(path,"\\\\.\\PhysicalDrive%d",drive);

                memset(buffer, 0 , 512);
                ReadOnly=0;
                hDevice = CreateFile(path,  // drive to open
                    GENERIC_READ|GENERIC_WRITE,                // no access to the drive
                    FILE_SHARE_READ | // share mode
                    FILE_SHARE_WRITE,
                    NULL,             // default security attributes
                    OPEN_EXISTING,    // disposition
                    0,                // file attributes
                    NULL);            // do not copy file attributes

                if (hDevice != INVALID_HANDLE_VALUE) // cannot open the drive
                {
                        bResult = DeviceIoControl(hDevice,  // device to be queried
                                                IOCTL_DISK_GET_DRIVE_GEOMETRY,  // operation to perform
                                                NULL, 0, // no input buffer
                                                &pdg, sizeof(pdg),     // output buffer
                                                &junk,                 // # bytes returned
                                                (LPOVERLAPPED) NULL);  // synchronous I/O


                        size1=0;
                        heads=8;
                        secs=32;
                        cyls=0;

                        if (bResult)
                        {
                                heads=pdg.TracksPerCylinder;
                                cyls=pdg.Cylinders.QuadPart;
                                secs=pdg.SectorsPerTrack;
                                size1=(pdg.Cylinders.QuadPart
                                                * (ULONG)pdg.TracksPerCylinder
                                                * (ULONG)pdg.SectorsPerTrack);
                        }

                        SetFilePointer(hDevice, 0, NULL, FILE_BEGIN);
                        ReadFile(hDevice, buffer, 512, &rubbish, NULL );
                        ReadOnly=PhysDriveID(buffer, 512, &cyls, &heads, &secs);

                        bResult = DeviceIoControl(hDevice,
                                        IOCTL_DISK_GET_PARTITION_INFO
                                        , NULL, 0
                                        , &pi, sizeof(pi)
                                        , &junk
                                        , NULL);

                        size2=0;
                        if (bResult) size2=(*(__int64*)&(pi.PartitionLength))/512;

                        if (cyls==0 && size2!=0) cyls=size2 / (heads*secs);

                        size=(size2>size1)? size2:size1;

                        if (secs>cyls)
                        {
                                heads=8;
                                secs=32;
                                cyls=size/(heads*secs);

                                while(cyls>65535)
                                {
                                        if ((++secs)==255) heads++;
                                        cyls=size/(heads*secs);
                                }
                        }

                        if (size)
                        {
                                PhysDrives[i].Type = (pdg.MediaType!=FixedMedia);
                                PhysDrives[i].Drive=drive;
                                PhysDrives[i].Heads=heads;
                                PhysDrives[i].Cylinders=cyls;
                                PhysDrives[i].Sectors=secs;
                                PhysDrives[i].Size = size;
                                PhysDrives[i].ReadOnly=ReadOnly;
                                strcpy(PhysDrives[i].Path, path);

                                i++;
                        }
                        CloseHandle(hDevice);
                }
        }
}

static int PhysDriveID(unsigned char *buffer, int buflen, int *cyls, int *heads, int *secs)
{
        int i;
        int RO=0;

        for(i=0;i<16;i++) if (buffer[i]==0) buffer[i]=32;

        if (strstr((char *)buffer, "PLUSIDEDOS"))
        {
                *cyls =  buffer[32] | (buffer[33]<<8);
                *heads = buffer[34];
                *secs = buffer[35];
        }

        if (buflen==512)
        {
                for(i=0;i<256;i++) buffer[i]=buffer[i*2];
                RO=PhysDriveID(buffer, 256, cyls, heads, secs);
        }
        if (buffer[buflen-2]==0x55 && buffer[buflen-1]==0xaa) RO=1;

        return(RO);
}

static int ATA_ReadHDF(void)
{
        ATA_DRIVE *Drv=&ATA_Channel.drive[ATA_Channel.cur_drive];
        byte *buffer, packed_buf[512];
        long sector_position;
        unsigned long rubbish;
        int i;

        //Drv=&ATA_Channel.drive[ATA_Channel.cur_drive];

        sector_position = Drv->data+(Drv->sector_size*ATA_Channel.sector_no);

        if (Drv->AccessMode==ACCESS_HDF)
        {
                if (fseek(Drv->f, sector_position, SEEK_SET)) return(1);

                if (Drv->sector_size==256) buffer=packed_buf;
                else buffer=ATA_Channel.buffer;

                if (fread(buffer, 1, Drv->sector_size, Drv->f ) != Drv->sector_size)
                        return(1);

                if (Drv->sector_size==512) return(0);

                for( i = 0; i < 256; i++ )
                {
                        ATA_Channel.buffer[i*2] = packed_buf[i];
                        ATA_Channel.buffer[i*2+1] = 0xff;
                }
                return(0);
        }
        else
        {
                if (SetFilePointer(Drv->h, sector_position, NULL, FILE_BEGIN)==INVALID_SET_FILE_POINTER)
                        return(1);
                ReadFile(Drv->h, ATA_Channel.buffer, Drv->sector_size, &rubbish, NULL );
                if (rubbish != Drv->sector_size)
                        return(1);
                return(0);
        }
}

static int ATA_WriteHDF(void)
{
        ATA_DRIVE *Drv;
        byte *buffer, packed_buf[512];
        long sector_position;
        unsigned long rubbish;
        int i;

        Drv=&ATA_Channel.drive[ATA_Channel.cur_drive];

        if (Drv->read_only) return(1);

        sector_position = Drv->data+(Drv->sector_size*ATA_Channel.sector_no);

        if (Drv->AccessMode==ACCESS_HDF)
        {
                if (fseek(Drv->f, sector_position, SEEK_SET)) return(1);

                if (Drv->sector_size==256)
                {
                        for( i = 0; i < 256; i++ )
                                packed_buf[i]=ATA_Channel.buffer[i*2];
                        buffer=packed_buf;
                }
                else    buffer=ATA_Channel.buffer;

                if (fwrite(buffer, 1, Drv->sector_size, Drv->f ) != Drv->sector_size)
                        return(1);
        }
        else
        {
                if (SetFilePointer(Drv->h, sector_position, NULL, FILE_BEGIN)==INVALID_SET_FILE_POINTER)
                        return(1);
                WriteFile(Drv->h, ATA_Channel.buffer, Drv->sector_size, &rubbish, NULL );
                if (rubbish != Drv->sector_size) return(1);
                return(0);
        }

        return 0;
}



static void ATA_IncSectorNo(void)
{
        ATA_DRIVE *Drv;
        int SectorNo;

        Drv=&ATA_Channel.drive[ATA_Channel.cur_drive];

        if (ATA_Channel.head & ATA_HEAD_LBA)
        {
                SectorNo = (ATA_Channel.head & ATA_HEAD_HEAD << 24)
                                + (ATA_Channel.cylinder_high << 16)
                                + (ATA_Channel.cylinder_low << 8)
                                + (ATA_Channel.sector);

                SectorNo += 1;

                ATA_Channel.head = (ATA_Channel.head & ~ATA_HEAD_HEAD)
                                        | ((SectorNo >> 24) & ATA_HEAD_HEAD);

                ATA_Channel.cylinder_high = (SectorNo>>16) & 255;
                ATA_Channel.cylinder_low = (SectorNo>>8) & 255;
                ATA_Channel.sector = SectorNo & 255;
        }
        else
        {
                int h;

                if (++ATA_Channel.sector > Drv->sectors)
                {
                        ATA_Channel.sector=1;

                        h= (ATA_Channel.head & ATA_HEAD_HEAD)+1;
                        ATA_Channel.head= (h & ATA_HEAD_HEAD) | (ATA_Channel.head & ~ATA_HEAD_HEAD);


                        if (h >= Drv->heads)
                        {
                                ATA_Channel.head=ATA_Channel.head & ~ATA_HEAD_HEAD;
                                if (++ATA_Channel.cylinder_low == 0)
                                {
                                        ATA_Channel.cylinder_low=0;
                                        ATA_Channel.cylinder_high++;
                                }
                        }
                }
        }
}

static int ATA_CalculateSectorNo(void)
{
        ATA_DRIVE *Drv;
        unsigned int SectorNo;

        Drv=&ATA_Channel.drive[ATA_Channel.cur_drive];

        //if( ATA_Channel.sector_count != 1 )
        //{
        //        ATA_Channel.status |= ATA_ERR;
        //        ATA_Channel.error = ATA_ERR_ABRT;
        //        return(1);
        //}

        if (ATA_Channel.head & ATA_HEAD_LBA)
        {
                SectorNo = (ATA_Channel.head & ATA_HEAD_HEAD << 24)
                                + (ATA_Channel.cylinder_high << 16)
                                + (ATA_Channel.cylinder_low << 8)
                                + (ATA_Channel.sector);
        }
        else
        {
                int cylinder, head, sector;

                cylinder = (ATA_Channel.cylinder_high << 8)
                                | (ATA_Channel.cylinder_low);

                head = ATA_Channel.head & ATA_HEAD_HEAD;
                sector = ATA_Channel.sector-1;

                if (cylinder >= Drv->cylinders
                        || head >= Drv->heads
                        || sector < 0
                        || sector >= Drv->sectors)
                {
                        SectorNo = -1;
                }
                else
                {
                        SectorNo =
	                        (((cylinder*Drv->heads)+head)*Drv->sectors)
                                        +sector;
                }
        }

        if (SectorNo<0 || SectorNo>=Drv->size)
        {
                Drv->status |= ATA_ERR;
                Drv->error = ATA_ERR_ABRT | ATA_ERR_IDNF;
                //ATA_Channel.status |= ATA_ERR;
                //ATA_Channel.error = ATA_ERR_ABRT | ATA_ERR_IDNF;
                return(1);
        }

        ATA_Channel.sector_no = SectorNo;

        return(0);
}


static void ATA_ReadSector(void)
{
        ATA_DRIVE *Drv=&ATA_Channel.drive[ATA_Channel.cur_drive];

        if (ATA_CalculateSectorNo()) return;

        if(ATA_ReadHDF())
        {
                Drv->status |= ATA_ERR;
                Drv->error = ATA_ERR_ABRT | ATA_ERR_UNC;
                //ATA_Channel.status |= ATA_ERR;
                //ATA_Channel.error = ATA_ERR_ABRT | ATA_ERR_UNC;
        }
        else
        {
                ATA_Channel.action = ATA_PIO_IN;
                Drv->status |= ATA_DRQ;
                //ATA_Channel.status |= ATA_DRQ;
                ATA_Channel.data_counter = 0;
        }
}

static void ATA_WriteSector(void)
{
        ATA_DRIVE *Drv=&ATA_Channel.drive[ATA_Channel.cur_drive];

        if (ATA_CalculateSectorNo()) return;

        ATA_Channel.action = ATA_PIO_OUT;
        Drv->status |= ATA_DRQ;
        //ATA_Channel.status |= ATA_DRQ;
        ATA_Channel.data_counter = 0;
}

static void ATA_IDDrive(void)
{
        ATA_DRIVE *Drv=&ATA_Channel.drive[ATA_Channel.cur_drive];
        int i;

        //for(i=0;i<512;i++) ATA_Channel.buffer[i] = 0;
        for(i=0;i<512;i++)
                ATA_Channel.buffer[i] =
                        ATA_Channel.drive[ATA_Channel.cur_drive].drive_id[i];

        ATA_Channel.action = ATA_PIO_IN;
        Drv->status |= ATA_DRQ;
        //ATA_Channel.status |= ATA_DRQ;
        ATA_Channel.data_counter = 0;
}

static void ATA_WriteData(int Data)
{
        ATA_DRIVE *Drv=&ATA_Channel.drive[ATA_Channel.cur_drive];
        int byte1, byte2;

        if (ATA_Channel.action != ATA_PIO_OUT) return;

        switch(ATA_Channel.mode)
        {
        case ATA_MODE_8BIT:
                byte1 = Data&0xff;
                byte2 = (Data>>8)&0xff;

                if (ATA_Channel.DataHighByte!=-1)
                {
                        byte2=ATA_Channel.DataHighByte;
                        ATA_Channel.DataHighByte=-1;
                }
                ATA_Channel.buffer[ATA_Channel.data_counter++]=byte1;
                ATA_Channel.buffer[ATA_Channel.data_counter++]=byte2;
                break;

        case ATA_MODE_16BIT:
                ATA_Channel.buffer[ATA_Channel.data_counter++]=Data;
                break;
        case ATA_MODE_16BIT_SWAPPED:
        case ATA_MODE_16BIT_WRSWAP:
                ATA_Channel.buffer[ATA_Channel.data_counter^1]=Data;
                ATA_Channel.data_counter++;
                break;
        }


        if( ATA_Channel.data_counter >= 512 )
        {
                ATA_Channel.action = ATA_READY;
                Drv->status &= ~ATA_DRQ;
                //ATA_Channel.status &= ~ATA_DRQ;

                if ( ATA_WriteHDF() )
                {
                        Drv->status |= ATA_ERR;
                        Drv->error = ATA_ERR_ABRT | ATA_ERR_UNC;
                        //ATA_Channel.status |= ATA_ERR;
                        //ATA_Channel.error = ATA_ERR_ABRT | ATA_ERR_UNC;
                }

                if (ATA_Channel.multi_sector!=1)
                {
                        ATA_Channel.multi_sector--;
                        ATA_IncSectorNo();
                        ATA_WriteSector();
                }
        }
}

static int ATA_ReadData(void)
{
        ATA_DRIVE *Drv=&ATA_Channel.drive[ATA_Channel.cur_drive];
        int byte1, byte2;
        int Data;

        if (ATA_Channel.action != ATA_PIO_IN) return(0xff);

        switch(ATA_Channel.mode)
        {
        case ATA_MODE_8BIT:
                byte1=ATA_Channel.buffer[ATA_Channel.data_counter++];
                byte2=ATA_Channel.buffer[ATA_Channel.data_counter++];

                Data=byte1 | (byte2<<8);
                ATA_Channel.DataHighByte=byte2;
                break;

        case ATA_MODE_16BIT:
        case ATA_MODE_16BIT_WRSWAP:
                Data=ATA_Channel.buffer[ATA_Channel.data_counter++];
                break;

        case ATA_MODE_16BIT_SWAPPED:
                Data=ATA_Channel.buffer[ATA_Channel.data_counter^1];
                ATA_Channel.data_counter++;
                break;
        }

        if (ATA_Channel.data_counter>=512)
        {
                ATA_Channel.action = ATA_READY;
                Drv->status &= ~ATA_DRQ;
                // ATA_Channel.status &= ~ATA_DRQ;

                if (ATA_Channel.multi_sector!=1)
                {
                        ATA_Channel.multi_sector--;
                        ATA_IncSectorNo();
                        ATA_ReadSector();
                }
        }

        return(Data);
}

static void ATA_Execute(int Data)
{
        ATA_DRIVE *Drv=&ATA_Channel.drive[ATA_Channel.cur_drive];
        //int Logical_Sectors=0, Logical_Heads=0;

        if (!(ATA_Channel.drive[ATA_Channel.cur_drive].AccessMode)) return;
        //if (ATA_Channel.action!=ATA_READY) return;

        Drv->error = ATA_ERR_OK;
        Drv->status &= ~ATA_ERR;
        Drv->status &= ~ATA_BUSY;
        Drv->status |= ATA_DRDY;

        //ATA_Channel.error = ATA_ERR_OK;
        //ATA_Channel.status &= ~ATA_ERR;
        //ATA_Channel.status &= ~ATA_BUSY;
        //ATA_Channel.status |= ATA_DRDY;

        switch(Data)
        {
        case READ_SECTOR:
        case READ_SECTOR_NV:
                        ATA_Channel.multi_sector=ATA_Channel.sector_count;
                        ATA_ReadSector(); break;

        case WRITE_SECTOR:
        case WRITE_SECTOR_NV:
                        ATA_Channel.multi_sector=ATA_Channel.sector_count;
                        ATA_WriteSector(); break;

        case ID_DRIVE:
        case ID_DRIVE_PACKET:
                        ATA_IDDrive(); break;

        case DRIVE_DIAGNOSTICS: ATA_Reset(); break;

        case SET_FEATURE:
                        if (ATA_Channel.feature&1) ATA_SetMode(ATA_MODE_16BIT);
                        else ATA_SetMode(ATA_MODE_8BIT);

        case INIT_PARAMS:
                        int c,h,s;

                        h=(ATA_Channel.head&15)+1;
                        s=ATA_Channel.sector_count;
                        c=Drv->size / (h*s);
                        if (c>65535) c=65535;

                        ATA_SetCHS(ATA_Channel.cur_drive, Drv->cylinders , Drv->heads, Drv->sectors);
                        //Logical_Sectors=ATA_Channel.sector_count;
                        //Logical_Heads=ATA_Channel.head&15;
                        Drv->status = ATA_DRDY;
                        break;

        //case 0x10:
        //        break;

        default:
                Drv->status |= ATA_ERR;
                Drv->error = ATA_ERR_ABRT;
                //ATA_Channel.status |= ATA_ERR;
                //ATA_Channel.error = ATA_ERR_ABRT;
        }
}

/*
   Public functions
*/

void ATA_Init()
{
        ATA_Channel.drive[0].f=NULL;
        ATA_Channel.drive[1].f=NULL;
        ATA_Channel.drive[0].h=NULL;
        ATA_Channel.drive[1].h=NULL;
        ATA_Channel.drive[0].filename[0]='\0';
        ATA_Channel.drive[1].filename[0]='\0';
        ATA_Channel.mode=ATA_MODE_16BIT;
        ATA_Channel.drive[0].read_only=0;
        ATA_Channel.drive[1].read_only=0;

}

void ATA_SetMode(int mode)
{
        ATA_Channel.mode=mode;
}

char *ATA_GetHDF(int drive)
{
        if (ATA_Channel.drive[drive].AccessMode)
                return(ATA_Channel.drive[drive].filename);
        else return(NULL);
}

int ATA_GetReadOnly(int drive)
{
        return(ATA_Channel.drive[drive].read_only);
}

void ATA_SetReadOnly(int drive, int read_only)
{
        ATA_Channel.drive[drive].read_only=read_only;
}

void ATA_GetCHS(int Drive, int *c, int *h, int *s, unsigned long *size)
{
        if (ATA_Channel.drive[Drive].AccessMode)
        {
                *c=ATA_Channel.drive[Drive].cylinders;
                *h=ATA_Channel.drive[Drive].heads;
                *s=ATA_Channel.drive[Drive].sectors;
                *size=ATA_Channel.drive[Drive].size;
        }
        else
        {
                *c=0;
                *h=0;
                *s=0;
                *size=0;
        }
}

void ATA_SetCHS(int Drive, int c, int h, int s)
{
        if (ATA_Channel.drive[Drive].AccessMode)
        {
                ATA_Channel.drive[Drive].cylinders=c;
                ATA_Channel.drive[Drive].heads=h;
                ATA_Channel.drive[Drive].sectors=s;
                //WriteWord(ATA_Channel.drive[Drive].drive_id,  1, c);
                //WriteWord(ATA_Channel.drive[Drive].drive_id,  3, h);
                //WriteWord(ATA_Channel.drive[Drive].drive_id,  6, s);
                WriteWord(ATA_Channel.drive[Drive].drive_id, 54, c);
                WriteWord(ATA_Channel.drive[Drive].drive_id, 55, h);
                WriteWord(ATA_Channel.drive[Drive].drive_id, 56, s);
        }
}


int ATA_LoadHDF(int drive, char *FileName)
{
        FILE *f;
        int len, idlen,i;
        int sectors=0;
        ATA_DRIVE *Drv = &ATA_Channel.drive[drive];


        ATA_EjectHDF(drive);
        if (!FileName) return(0);

        if (FileName[0]=='\\' && FileName[1]=='\\')
        {
                int i=0;
                char ModelName[]="iEhgtynO ehPsyiDks                      ";

                while(PhysDrives[i].Drive!=-1)
                {
                        if (!strcmp(FileName,PhysDrives[i].Path))
                        {
                                Drv->h = CreateFile(FileName,  // drive to open
                                                GENERIC_READ|GENERIC_WRITE,
                                                FILE_SHARE_READ | // share mode
                                                FILE_SHARE_WRITE,
                                                NULL,             // default security attributes
                                                OPEN_EXISTING,    // disposition
                                                0,                // file attributes
                                                NULL);            // do not copy file attributes

                                if (Drv->h == INVALID_HANDLE_VALUE) // cannot open the drive
                                {
                                        ATA_EjectHDF(drive);
                                        return(0);
                                }

                                Drv->cylinders = PhysDrives[i].Cylinders;
                                Drv->heads = PhysDrives[i].Heads;
                                Drv->sectors = PhysDrives[i].Sectors;
                                Drv->size = PhysDrives[i].Size;
                                strcpy(Drv->filename, FileName);
                                Drv->data = 0;
                                Drv->sector_size = 512;
                                Drv->AccessMode=ACCESS_PHY;
                                Drv->read_only=PhysDrives[i].ReadOnly;

                                sectors=PhysDrives[i].Size;
                                memset(Drv->drive_id, 0, 512);

                                WriteWord(Drv->drive_id,  0, 64);
                                WriteWord(Drv->drive_id,  1, Drv->cylinders);
                                WriteWord(Drv->drive_id,  3, Drv->heads);
                                WriteWord(Drv->drive_id,  6, Drv->sectors);
                                WriteWord(Drv->drive_id, 49, 512);
                                WriteWord(Drv->drive_id, 54, Drv->cylinders);
                                WriteWord(Drv->drive_id, 55, Drv->heads);
                                WriteWord(Drv->drive_id, 56, Drv->sectors);
                                //WriteWord(Drv->drive_id, 57, sectors&65535);
                                //WriteWord(Drv->drive_id, 58, (sectors>>16)&65535);
                                //WriteWord(Drv->drive_id, 60, sectors&65535);
                                //WriteWord(Drv->drive_id, 61, (sectors>>16)&65535);

                                //memcpy(head+0x16+54, ModelName, 40);
                                for(i=0;i<40;i++) Drv->drive_id[54+i]=ModelName[i];
                                return(1);
                        }
                        i++;
                }

                return(0);
        }

        f=fopen(FileName,"rb+");
        if (!f) return(1);

        len=fread( &(Drv->hdf), 1, sizeof(HDF_HEADER), f );

        if (len!=sizeof(HDF_HEADER))
        {
                fclose(f);
                return(1);
        }

        if (strncmp((char*)Drv->hdf.sig, "RS-IDE", 6) || (Drv->hdf.id != 0x1a))
        {
                fclose(f);
                return(1);
        }

        Drv->data = (Drv->hdf.data_high << 8) | Drv->hdf.data_low;
        Drv->sector_size = Drv->hdf.flags & 0x01 ? 256 : 512;

        idlen=(Drv->data)-sizeof(HDF_HEADER);
        if (idlen>512) idlen=512;

        fread(Drv->drive_id, idlen, 1, f);

        Drv->cylinders = (Drv->drive_id[ 3]<<8) | Drv->drive_id[2];
        Drv->heads = (Drv->drive_id[7]<<8) | Drv->drive_id[6];
        Drv->sectors = (Drv->drive_id[13]<<8) | Drv->drive_id[12];
        Drv->size = Drv->cylinders * Drv->heads * Drv->sectors;

        if (idlen<128)
        {
                WriteWord(Drv->drive_id,  0, 64);
                WriteWord(Drv->drive_id,  1, Drv->cylinders);
                WriteWord(Drv->drive_id,  3, Drv->heads);
                WriteWord(Drv->drive_id,  6, Drv->sectors);
                WriteWord(Drv->drive_id, 49, 512);
                WriteWord(Drv->drive_id, 54, Drv->cylinders);
                WriteWord(Drv->drive_id, 55, Drv->heads);
                WriteWord(Drv->drive_id, 56, Drv->sectors);

                sectors = Drv->cylinders*Drv->heads*Drv->sectors;

                WriteWord(Drv->drive_id, 57, sectors&65535);
                WriteWord(Drv->drive_id, 58, (sectors>>16)&65535);
                WriteWord(Drv->drive_id, 60, sectors&65535);
                WriteWord(Drv->drive_id, 61, (sectors>>16)&65535);
        }

        Drv->f = f;
        strcpy(Drv->filename, FileName);

        Drv->AccessMode=ACCESS_HDF;

        ATA_Reset();
        return(0);
}

void ATA_EjectHDF(int drive)
{
        if (ATA_Channel.drive[drive].AccessMode)
        {
                if (ATA_Channel.drive[drive].f) fclose(ATA_Channel.drive[drive].f);
                if (ATA_Channel.drive[drive].h) CloseHandle(ATA_Channel.drive[drive].h);
                ATA_Channel.drive[drive].f=NULL;
                ATA_Channel.drive[drive].h=NULL;
                ATA_Channel.drive[drive].filename[0]='\0';
                ATA_Channel.drive[drive].AccessMode=ACCESS_NONE;
        }
}

void ATA_Reset(void)
{
        ATA_Channel.action = ATA_READY;
        ATA_Channel.cur_drive = 0;

        if (ATA_Channel.drive[0].AccessMode || ATA_Channel.drive[1].AccessMode)
        {
                ATA_Channel.sector_count = 0x01;
                ATA_Channel.sector = 0x01;
                ATA_Channel.cylinder_low = 0x00;
                ATA_Channel.cylinder_high = 0x00;
                ATA_Channel.head = 0x00;
                //ATA_Channel.error = 0x01;
                //ATA_Channel.status = 0x50;
                ATA_Channel.feature = 0xff;
                ATA_Channel.DataHighByte=-1;
                ATA_Channel.drive[0].error=0x01;
                ATA_Channel.drive[1].error=0x01;
                ATA_Channel.drive[0].status=ATA_Channel.drive[0].AccessMode ? 0x50:0x00;
                ATA_Channel.drive[1].status=ATA_Channel.drive[1].AccessMode ? 0x50:0x00;
        }
        else
        {
                ATA_Channel.sector_count = 0xff;
                ATA_Channel.sector = 0xff;
                ATA_Channel.cylinder_low = 0xff;
                ATA_Channel.cylinder_high = 0xff;
                ATA_Channel.head = 0xff;
                ATA_Channel.feature = 0xff;
                ATA_Channel.DataHighByte=-1;
                ATA_Channel.drive[0].error=0xff;
                ATA_Channel.drive[1].error=0xff;
                ATA_Channel.drive[0].status=0xff;
                ATA_Channel.drive[1].status=0xff;
                //ATA_Channel.error = 0xff;
                //ATA_Channel.status = 0xff;
        }
}

int ATA_ReadRegister(int Register)
{
        ATA_DRIVE *Drv=&ATA_Channel.drive[ATA_Channel.cur_drive];

        switch(Register)
        {
        case ATA_REG_DATA:      return(ATA_ReadData());
        case ATA_REG_ERROR:     return(Drv->error);
        case ATA_REG_SEC_CNT:   return(ATA_Channel.sector_count);
        case ATA_REG_SEC:       return(ATA_Channel.sector);
        case ATA_REG_CYL_LOW:   return(ATA_Channel.cylinder_low);
        case ATA_REG_CYL_HIGH:  return(ATA_Channel.cylinder_high);
        case ATA_REG_HEAD:      return(ATA_Channel.head);
        case ATA_REG_STATUS:
                //if (ATA_Channel.drive[ATA_Channel.cur_drive].AccessMode)
                        return(Drv->status);
                //else
                        //return(0);
        case ATA_REG_HIGH:      return(ATA_Channel.DataHighByte);
        default:                return(0xff);
        }
}

void ATA_WriteRegister(int Register, int Data)
{
        switch(Register)
        {
        case ATA_REG_CMD:       ATA_Execute(Data); break;
        case ATA_REG_DATA:      ATA_WriteData(Data); break;
        case ATA_REG_HIGH:      ATA_Channel.DataHighByte=Data; break;
        case ATA_REG_FEATURE:   ATA_Channel.feature=Data;
                                ATA_Execute(SET_FEATURE); break;
        case ATA_REG_SEC_CNT:   ATA_Channel.sector_count=Data; break;
        case ATA_REG_SEC:       ATA_Channel.sector=Data; break;
        case ATA_REG_CYL_LOW:   ATA_Channel.cylinder_low=Data; break;
        case ATA_REG_CYL_HIGH:  ATA_Channel.cylinder_high=Data; break;
        case ATA_REG_HEAD:
                                ATA_Channel.head=Data;
                                ATA_Channel.cur_drive =
                                        Data&ATA_HEAD_DEVICE ? 1 : 0;
        default:                break;
        }
}



