#ifndef ZXPAND_EMU_H
#define ZXPAND_EMU_H
class zxpand_t;

class ZXpand
{
private:
        zxpand_t* _zxpand;

public:
        ZXpand();
        ~ZXpand();

        void Update(int millis);
        bool GetConfig(int& configData);
        void IO_Write(int addr, int data);
        int  IO_Read(int addr);
        int  IO_ReadStatus();

        static void SetSDCardPath();
};

enum
{
   CFG_BIT_LOW,
   CFG_BIT_DISABLED
};


//
// How to use the DLL
//
// Last update SirMorrisBig 20/05/2011
//

// * Load the DLL
//    HMODULE dll = LoadLibrary( TEXT("ZXpand_emu.dll") );
//
// * Create a set of pointers to the DLL functions
//    ZXP_CREATEFN  zxp_create  = (ZXP_CREATEFN)  GetProcAddress( dll, "ZXpand_Create" );
//    ZXP_DESTROYFN zxp_destroy = (ZXP_DESTROYFN) GetProcAddress( dll, "ZXpand_Destroy" );
//    ZXP_UPDATEFN  zxp_update  = (ZXP_UPDATEFN)  GetProcAddress( dll, "ZXpand_Update" );
//    ZXP_GETCFGFN  zxp_getcfg  = (ZXP_GETCFGFN)  GetProcAddress( dll, "ZXpand_GetConfig" );
//    ZXP_IOWRFN    zxp_iowr    = (ZXP_IOWRFN)    GetProcAddress( dll, "ZXpand_IO_Write" );
//    ZXP_IORDFN    zxp_iord    = (ZXP_IORDFN)    GetProcAddress( dll, "ZXpand_IO_Read" );
//
// * Create an instance of a ZXpand object
//    void* instance = zxp_create("card.bin");
//    if (instance == NULL) { [there has been an error] }
//
// * Initialise the interface state - ignore return on 1st call. The config variable should not be altered by your code.
//    int config;
//    zxp_getcfg( instance, config );
//
// * Periodically call update on the object
//    zxp_update( instance, [number of milliseconds elapsed since the last update call] );
//
// * When an IO read occurs on port 7
//    data = zxp_ioread( instance, [address register bits 8..15] );
//
// * When an IO write occurs on port 7
//    zxp_iowrite( instance, [address register bits 8..15], data );
//
// * After an IO write you can check if the interface's config has changed
//    bool changed = zxp_getcfg( instance, config );
//    if (changed)
//    {
//       if (config & (1<<ZXPAND_CFG_LOW)) { [there is RAM at 8K, 16-40K] }
//       else { [there is RAM at 16-48K] }
//       if (config & (1<<ZXPAND_CFG_DISABLED)) { [interface overlay ROM should be disabled] }
//       else { [interface overlay ROM is present] }
//    }
//
// * When emulation stops or the machine is reset or before a new instance is needed (EG changing card image)
//    zxp_destroy( instance );
//    instance = NULL;

#endif
