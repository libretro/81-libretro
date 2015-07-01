/* EightyOne  - A Windows ZX80/81/clone emulator.
 * Copyright (C) 2003-2006 Michael D Wynne
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * AccDRaw_.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#include <cddraw.h>
#include <stdio.h>
#pragma hdrstop

#include "AccDraw_.h"
#include "main_.h"
#include "zx81.h"
#include "zx81config.h"
#include "Fullscreen.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#define HTOL 405
#define VTOL 290
#define HMIN 10
#define VMIN 350

int HSYNC_TOLLERANCE=HTOL;
int VSYNC_TOLLERANCE=VTOL;
int HSYNC_MINLEN=HMIN;
int VSYNC_MINLEN=VMIN;

#define BlWinT 56
#define BlWinB (BlWinT+192)
#define BlWinL 74
#define BlWinR (BlWinL+256)

#define SmWinT 52
#define SmWinB (SmWinT+200)
#define SmWinL 70
#define SmWinR (SmWinL+264)

#define NoWinT  32
#define NoWinB  (NoWinT+240)
#define NoWinL  42
#define NoWinR  (NoWinL+320)

#define LaWinT 0
#define LaWinB (LaWinT+300)
#define LaWinL 0
#define LaWinR (LaWinL+400)

#define FuWinT 0
#define FuWinB (FuWinT+312)
#define FuWinL 0
#define FuWinR (FuWinL+413)

/*
#define Plot(x,c) { if (Paletteised) dest[RasterX+(x)]= (c); \
                    else *(DWORD *)(dest+RasterX+(x)) = Colours[(c)]; }
*/

#define Plot(x,c) { *(DWORD *)(dest+RasterX+(x)) = Colours[(c)]; }

int WinR=NoWinR;
int WinL=NoWinL;
int WinT=NoWinT;
int WinB=NoWinB;

int RasterX=0, RasterY=0;
int TVW, TVH, TVP;
int BPP, ScanLen;
int Paletteised, Scale;

int sync_len, sync_valid;
long noise;
BYTE scanline[800*50];
int scanline_len=0;

DWORD Palette[256], Colours[256], LetterBoxColour;

LPDIRECTDRAW7 m_pDD=NULL;
LPDIRECTDRAWSURFACE7 m_pddsFrontBuffer=NULL;
LPDIRECTDRAWSURFACE7 m_pddsFrame=NULL;
LPDIRECTDRAWCLIPPER pcClipper=NULL;
HWND hWnd;
LPDIRECTDRAWSURFACE7 DDFrame;
DDSURFACEDESC2 DDFrameSurface;

Graphics::TBitmap *GDIFrame;

BYTE *dest=NULL, *buffer=NULL;

TRect BorderTop, BorderBottom, BorderLeft, BorderRight;
TRect rcsource, rcdest;

void add_blank(int tstates, BYTE colour)
{
        while(tstates-- > 0)
        {
                scanline[scanline_len++]=colour;
                scanline[scanline_len++]=colour;
        }
}

// -----------------------------------------------------------------------------
// DirectDraw Functions
// -----------------------------------------------------------------------------

bool DDError(bool result, AnsiString Message)
{
        if (result)
                MessageBoxA(NULL,
                "Count not initialise DirectDraw.\nPlease ensure DirectX 7 or greater is installed",
                Message.c_str(),
                MB_OK);

        return(result);

}

void DDEnd(void)
{
        if (m_pDD)
        {
                if (m_pddsFrontBuffer != NULL)
                {
                        if (m_pddsFrame) m_pddsFrame->Release();
                        m_pddsFrame = NULL;

                        if (pcClipper) pcClipper->Release();
                        pcClipper = NULL;

                        if (m_pddsFrontBuffer) m_pddsFrontBuffer->Release();
                        m_pddsFrontBuffer = NULL;
                }
                m_pDD->Release();
                m_pDD = NULL;
        }
}

int DDInit(void)
{
        HRESULT hRet;

        hWnd = Form1->Handle;

        DDEnd();
        hRet = DirectDrawCreateEx(NULL, (VOID**)&m_pDD, IID_IDirectDraw7, NULL);
        if(DDError(hRet != DD_OK,"DDCreateEX")) return(false);

        if (Form1->FullScreen)
        {
                hRet = m_pDD->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
                hRet = m_pDD->SetDisplayMode(FScreen.Width, FScreen.Height, FScreen.Bpp,NULL,NULL);

        }
        else    hRet = m_pDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
        if(DDError(hRet != DD_OK,"DDSetCoop")) return(false);

        HRESULT hr;

        DDSURFACEDESC2 ddsd;
        ZeroMemory( &ddsd, sizeof( ddsd ) );
        ddsd.dwSize = sizeof( ddsd );
        ddsd.dwFlags = DDSD_CAPS;
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

        if(DDError(hr = m_pDD->CreateSurface(&ddsd, &m_pddsFrontBuffer, NULL)
                ,"CreateFrontSurface")) return(false);

        // Create the backbuffer surface
        ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
        ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
        ddsd.dwWidth = 1024;
        ddsd.dwHeight = 768;

        hr = m_pDD->CreateSurface(&ddsd, &m_pddsFrame, NULL);
        if (hr)
        {
                ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
                ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
                ddsd.dwWidth = 460;
                ddsd.dwHeight = 400;
                hr = m_pDD->CreateSurface(&ddsd, &m_pddsFrame, NULL);
                if(DDError(hr,"CreateBackSurface")) return(false);
                tv.DisableAdvanced=1;
        }

        if (!Form1->FullScreen)
        {
                if(DDError(hr = m_pDD->CreateClipper(0, &pcClipper, NULL)
                        ,"CreateClipper")) return(false);

                if(DDError(hr = pcClipper->SetHWnd(0, hWnd)
                        ,"SetClipperHwnd"))
                {
                        pcClipper->Release();
                        return(false);
                }


                if(DDError(hr = m_pddsFrontBuffer->SetClipper(pcClipper)
                        ,"SetClipperSurface"))
                {
                        pcClipper->Release();
                        return(false);
                }
        }

        return(true);
}

int DDGetMaskInfo (DWORD Bitmask, int* lpShift)
{
    int Precision, Shift;

    Precision = Shift = 0;
    //count the zeros on right hand side
    while (!(Bitmask & 0x01L))
    {
        Bitmask >>= 1;
        Shift++;
    }

    //count the ones on right hand side
    while (Bitmask & 0x01L)
    {
        Bitmask >>= 1;
        Precision++;
    }
    *lpShift = Shift;
    return Precision;
}

void DDDrawBorder()
{
        DDBLTFX ddbltfx;

        if (FScreen.Stretch) return;

        ddbltfx.dwSize=sizeof(ddbltfx);
	ddbltfx.dwFillColor = LetterBoxColour; //RGB(250, 25, 5);

        m_pddsFrontBuffer->Blt(&BorderTop, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
        m_pddsFrontBuffer->Blt(&BorderBottom, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
        m_pddsFrontBuffer->Blt(&BorderRight, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
        m_pddsFrontBuffer->Blt(&BorderLeft, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
}

void DDAccurateInit(int resize)
{
        DDPIXELFORMAT DDpf;
        DDSURFACEDESC2 ddsd;

        float OrigW, OrigH, ScaleW, ScaleH;

        OrigW=Form1->ClientWidth;
        OrigH=Form1->ClientHeight;
        if (Form1->StatusBar1->Visible) OrigH -= Form1->StatusBar1->Height;

        if (Form1->BaseWidth==0) Form1->BaseWidth= NoWinR-NoWinL;
        if (Form1->BaseHeight==0) Form1->BaseHeight= NoWinT-NoWinB;

        ScaleW = OrigW / Form1->BaseWidth;
        ScaleH = OrigH / Form1->BaseHeight;

        RasterX=0;
        RasterY=random(256);

        //fill the DDpf structure and get the BytesPerPixel
        ZeroMemory (&DDpf, sizeof(DDpf));
        DDpf.dwSize = sizeof(DDpf);
        m_pddsFrontBuffer->GetPixelFormat(&DDpf);
        BPP = DDpf.dwRGBBitCount/8;

        Paletteised = (BPP==1) ? true:false;
        Scale= tv.AdvancedEffects?2:1;

        //ScanLen=460*BPP;
        ScanLen=(2+machine.tperscanline*2)*BPP;

        switch(zx81.bordersize)
        {
        case BORDERNONE:
                WinL=BlWinL; WinR=BlWinR; WinT=BlWinT; WinB=BlWinB;
                if (zx81.NTSC) { WinT-=24; WinB-=24; }
                break;
        case BORDERSMALL:
                WinL=SmWinL; WinR=SmWinR; WinT=SmWinT; WinB=SmWinB;
                if (zx81.NTSC) { WinT-=24; WinB-=24; }
                break;
        case BORDERNORMAL:
                WinL=NoWinL; WinR=NoWinR; WinT=NoWinT; WinB=NoWinB;
                if (zx81.NTSC) { WinT-=24; WinB-=24; }
                break;
        case BORDERLARGE:
                WinL=LaWinL; WinR=LaWinR; WinT=LaWinT; WinB=LaWinB;
                if (zx81.NTSC) { WinB-=24; }
                break;
        case BORDERFULL:
                WinL=FuWinL; WinR=FuWinR; WinT=FuWinT; WinB=FuWinB;
                if (zx81.NTSC) WinB-=51;
                break;
        }

        ZeroMemory( &ddsd, sizeof( ddsd ) );
        ddsd.dwSize = sizeof( ddsd );
        // Create the backbuffer surface
        ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
        ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;

        if (tv.AdvancedEffects)
        {
                WinL*=2; WinR*=2; WinT*=2; WinB*=2; ScanLen*=2;
                TVW=ddsd.dwWidth = 1024;
                TVH=ddsd.dwHeight = 768;
                HSYNC_TOLLERANCE=HTOL*2; HSYNC_MINLEN=10;
                VSYNC_TOLLERANCE=VTOL*2; VSYNC_MINLEN=350;
        }
        else
        {
                TVW=ddsd.dwWidth = 520;
                TVH=ddsd.dwHeight = 380;
                HSYNC_TOLLERANCE=HTOL; HSYNC_MINLEN=10;
                VSYNC_TOLLERANCE=VTOL; VSYNC_MINLEN=350;
        }

        m_pddsFrame->Release(); m_pddsFrame = NULL;
        m_pDD->CreateSurface(&ddsd, &m_pddsFrame, NULL);

        if (zx81.NTSC) VSYNC_TOLLERANCE-=60;

        if ((resize) && (!Form1->FullScreen))
        {
                Form1->BaseWidth=WinR-WinL;
                Form1->BaseHeight=WinB-WinT;

                OrigW = Form1->BaseWidth * ScaleW;
                OrigH = Form1->BaseHeight * ScaleH;
                if (Form1->StatusBar1->Visible) OrigH += Form1->StatusBar1->Height;

                Form1->ClientWidth = OrigW;
                Form1->ClientHeight = OrigH;
        }

        DDFrame=m_pddsFrame;

        ZeroMemory(&DDFrameSurface, sizeof(DDFrameSurface));
        DDFrameSurface.dwSize = sizeof(DDFrameSurface);
        DDFrame->Lock(NULL, &DDFrameSurface, DDLOCK_WAIT |  DDLOCK_NOSYSLOCK, NULL);
        dest=buffer= (BYTE*)DDFrameSurface.lpSurface;
        TVP=DDFrameSurface.lPitch;

        RecalcPalette();
        RecalcWinSize();
}

void DDAccurateUpdateDisplay(bool singlestep)
{
        static int framecounter=0;
        HRESULT hRet;
        RECT rDest;

        if (++framecounter > zx81.frameskip || singlestep)
                framecounter=0;
        else
                return;

        DDFrame->Unlock(NULL);

        POINT p = {0, 0};
        if(!Form1->FullScreen) p=Form1->ClientToScreen(p);

        rDest=rcdest;
        rDest.left += p.x;
        rDest.top += p.y;
        rDest.right += p.x;
        rDest.bottom += p.y;

        //if (Form1->FullScreen) DDDrawBorder();
        while(1)
        {
                hRet = m_pddsFrontBuffer->Blt(&rDest, DDFrame, &rcsource, DDBLT_WAIT, NULL);

                if (hRet == DD_OK) break;
                else
                if(hRet == DDERR_SURFACELOST)
                {
                        m_pddsFrontBuffer->Restore();
                        m_pddsFrame->Restore();
                }
                else if(hRet != DDERR_WASSTILLDRAWING) return;
        }


        DDFrame->Lock(NULL, &DDFrameSurface, DDLOCK_WAIT |  DDLOCK_NOSYSLOCK, NULL);
        dest=buffer= (BYTE*)DDFrameSurface.lpSurface;
}

// -----------------------------------------------------------------------------
// GDI Functions
// -----------------------------------------------------------------------------

void GDIAccurateInit(int resize)
{
        float OrigW, OrigH, ScaleW, ScaleH;

        if (GDIFrame)
        {
                delete GDIFrame;
                GDIFrame=NULL;
        }

        //if (Form1->RenderMode==RENDERGDI)
        //{
        //        FScreen.Width = GetSystemMetrics(SM_CXSCREEN);
        //        FScreen.Height = GetSystemMetrics(SM_CYSCREEN);
        //}


        OrigW=Form1->ClientWidth;
        OrigH=Form1->ClientHeight;
        if (Form1->StatusBar1->Visible) OrigH -= Form1->StatusBar1->Height;

        if (Form1->BaseWidth==0) Form1->BaseWidth= NoWinR-NoWinL;
        if (Form1->BaseHeight==0) Form1->BaseHeight= NoWinT-NoWinB;

        ScaleW = OrigW / Form1->BaseWidth;
        ScaleH = OrigH / Form1->BaseHeight;

        RasterX=0;
        RasterY=random(256);

        BPP = 2;

        Paletteised = (BPP==1) ? true:false;
        Scale= tv.AdvancedEffects?2:1;

        //ScanLen=460*BPP;
        ScanLen=(2+machine.tperscanline*2)*BPP;

        switch(zx81.bordersize)
        {
        case BORDERNONE:
                WinL=BlWinL; WinR=BlWinR; WinT=BlWinT; WinB=BlWinB;
                if (zx81.NTSC) { WinT-=24; WinB-=24; }
                break;
        case BORDERSMALL:
                WinL=SmWinL; WinR=SmWinR; WinT=SmWinT; WinB=SmWinB;
                if (zx81.NTSC) { WinT-=24; WinB-=24; }
                break;
        case BORDERNORMAL:
                WinL=NoWinL; WinR=NoWinR; WinT=NoWinT; WinB=NoWinB;
                if (zx81.NTSC) { WinT-=24; WinB-=24; }
                break;
        case BORDERLARGE:
                WinL=LaWinL; WinR=LaWinR; WinT=LaWinT; WinB=LaWinB;
                if (zx81.NTSC) { WinB-=24; }
                break;
        case BORDERFULL:
                WinL=FuWinL; WinR=FuWinR; WinT=FuWinT; WinB=FuWinB;
                if (zx81.NTSC) WinB-=51;
                break;
        }


        GDIFrame=new Graphics::TBitmap;

        if (tv.AdvancedEffects)
        {
                WinL*=2; WinR*=2; WinT*=2; WinB*=2; ScanLen*=2;
                TVW=1024;
                TVH=768;
                HSYNC_TOLLERANCE=HTOL*2; HSYNC_MINLEN=10;
                VSYNC_TOLLERANCE=VTOL*2; VSYNC_MINLEN=350;
        }
        else
        {
                TVW=520;
                TVH=380;
                HSYNC_TOLLERANCE=HTOL; HSYNC_MINLEN=10;
                VSYNC_TOLLERANCE=VTOL; VSYNC_MINLEN=350;
        }


        GDIFrame->Width=TVW;
        GDIFrame->Height=TVH;
        GDIFrame->PixelFormat=pf16bit;

        //m_pddsFrame->Release(); m_pddsFrame = NULL;
        //m_pDD->CreateSurface(&ddsd, &m_pddsFrame, NULL);

        if (zx81.NTSC) VSYNC_TOLLERANCE-=60;

        if ((resize) && (!Form1->FullScreen))
        {
                Form1->BaseWidth=WinR-WinL;
                Form1->BaseHeight=WinB-WinT;

                OrigW = Form1->BaseWidth * ScaleW;
                OrigH = Form1->BaseHeight * ScaleH;
                if (Form1->StatusBar1->Visible) OrigH += Form1->StatusBar1->Height;

                Form1->ClientWidth = OrigW;
                Form1->ClientHeight = OrigH;
        }

        //DDFrame=m_pddsFrame;

        //ZeroMemory(&DDFrameSurface, sizeof(DDFrameSurface));
        //DDFrameSurface.dwSize = sizeof(DDFrameSurface);
        //DDFrame->Lock(NULL, &DDFrameSurface, DDLOCK_WAIT |  DDLOCK_NOSYSLOCK, NULL);

        dest=buffer= (unsigned char *) GDIFrame->ScanLine[0];

        TVP = ((char *)GDIFrame->ScanLine[1]) - ((char *)GDIFrame->ScanLine[0]);

        RecalcPalette();
        RecalcWinSize();
}

void GDIAccurateUpdateDisplay(bool singlestep)
{
        static int framecounter=0;
        int ret;

        if (++framecounter > zx81.frameskip || singlestep)
                framecounter=0;
        else
                return;

        ret=StretchBlt(Form1->Canvas->Handle,
                        rcdest.Left, rcdest.Top,
                        (rcdest.Right-rcdest.Left),
                        (rcdest.Bottom-rcdest.Top),
                        GDIFrame->Canvas->Handle,
                        rcsource.Left, rcsource.Top,
                        (rcsource.Right-rcsource.Left),
                        (rcsource.Bottom-rcsource.Top),
                        SRCCOPY);
        if (!ret) ShowMessage(SysErrorMessage(GetLastError()));

        dest=buffer= (unsigned char *) GDIFrame->ScanLine[0];
}

void GDIDrawBorder()
{
        HBRUSH hBrush;

        if (FScreen.Stretch) return;

        if (FScreen.WhiteLetterbox)
                hBrush= CreateSolidBrush(RGB(Palette[7*16]&0xff,
                                                (Palette[7*16]>>8)&0xff,
                                                (Palette[7*16]>>16)&0xff));
        else
                hBrush= CreateSolidBrush(RGB(Palette[0]&0xff,
                                                (Palette[0]>>8)&0xff,
                                                (Palette[0]>>16)&0xff));



        FillRect(Form1->Canvas->Handle,&BorderTop, hBrush);
        FillRect(Form1->Canvas->Handle,&BorderBottom, hBrush);
        FillRect(Form1->Canvas->Handle,&BorderRight, hBrush);
        FillRect(Form1->Canvas->Handle,&BorderLeft, hBrush);

        DeleteObject(hBrush);
}

// -----------------------------------------------------------------------------
// Common Functions
// -----------------------------------------------------------------------------


int AccurateDraw()
{
        static int FrameNo=0;
        static int LastVSyncLen=0, Shade=0;
        register int i,c;

        if (!dest) return(0);

        for(i=0; i<scanline_len; i++)
        {
                c=scanline[i];

                Plot(FrameNo*TVP, c+Shade);

                if (tv.AdvancedEffects)
                {
                        if (!tv.Interlaced) Plot(TVP, c+8-Shade);

                        if (zx81.machine!=MACHINESPEC48)
                        {
                                RasterX +=BPP;
                                Plot(FrameNo*TVP, c+Shade);
                                if (!tv.Interlaced) Plot(TVP, c+8-Shade)
                        }

                }

                RasterX += BPP;

                if (RasterX > ScanLen)
                {
                        RasterX=0;
                        dest += TVP * Scale;
                        RasterY += Scale;
                        if (!tv.AdvancedEffects) Shade=8-Shade;

                        if (RasterY>=TVH)
                        {
                                //RasterX=0;
                                //RasterY=0;
                                //dest=buffer;
                                i=scanline_len+1;
                                sync_valid=true;
                        }
                }
        }
        if (sync_len<HSYNC_MINLEN) sync_valid=0;
        if (sync_valid)
        {
                if (RasterX>(HSYNC_TOLLERANCE*BPP))
                {
                        //scanlen[RasterY]=RasterX;
                        RasterX=0;
                        RasterY+= Scale;
                        if (!tv.AdvancedEffects) Shade=8-Shade;
                        dest += TVP* Scale;
                }
                if (RasterY>=TVH || (sync_len>VSYNC_MINLEN && RasterY>VSYNC_TOLLERANCE))
                {
                        CompleteFrame();
                        RasterX=RasterY=0;
                        dest=buffer;

                        if (tv.Interlaced)
                        {
                                FrameNo = 1-FrameNo;
                                Shade=FrameNo*8;

                                if (scanline_len >= ((LastVSyncLen*5)/4)) FrameNo=0;
                                LastVSyncLen=scanline_len;
                        }
                        else
                        {
                                FrameNo=0;
                                Shade=0;
                        }
                        AccurateUpdateDisplay(false);
                }

                sync_len=0;
                sync_valid=-1;
        }

        if (zx81.single_step)
        {
                int i;

                for(i=0;i<8;i++) *(DWORD *)(dest+RasterX+i*BPP) = Colours[15];
                AccurateUpdateDisplay(true);
        }
        return(0);
}


void RecalcWinSize(void)
{
        int sw,sh, dw,dh, scale;
        int bw,bh;

        rcsource.Right=WinR;
        rcsource.Bottom=WinB;
        rcsource.Left=WinL;
        rcsource.Top=WinT;

        //if (Form1->RenderMode==RENDERGDI)
        //{
        //        rcsource.Top +=32;
        //        rcsource.Bottom +=32;
        //}


        if (!Form1->FullScreen)
        {
                rcdest.Top=0; rcdest.Bottom=Form1->ClientHeight;
                rcdest.Left=0; rcdest.Right=Form1->ClientWidth;
                if (Form1->StatusBar1->Visible) rcdest.Bottom -= Form1->StatusBar1->Height;

        }
        else if (FScreen.Stretch)
        {
                rcdest.Top=0; rcdest.Bottom=FScreen.Height;
                rcdest.Left=0; rcdest.Right=FScreen.Width;
        }
        else
        {
                scale=1; sw=WinR-WinL; sh=WinB-WinT;

                dw=FScreen.Width; dh=FScreen.Height;

                while((sw*scale <= dw) && (sh*scale <= dh)) scale++;
                scale--;

                if (scale==-1)
                {
                        rcdest.Top=0; rcdest.Right=FScreen.Width;
                        rcdest.Left=0; rcdest.Bottom=FScreen.Height;
                }
                else
                {
                        bw = (dw - sw*scale)/2; bh = (dh - sh*scale)/2;

                        rcdest.Top=bh; rcdest.Bottom=bh + sh*scale;
                        rcdest.Left=bw; rcdest.Right=bw + sw*scale;

                        BorderTop.Top=0; BorderTop.Bottom=rcdest.Top;
                        BorderTop.Left=0; BorderTop.Right=dw;

                        BorderBottom.Top=dh - bh; BorderBottom.Bottom=dh;
                        BorderBottom.Left=0; BorderBottom.Right=dw;

                        BorderLeft.Top=bh; BorderLeft.Bottom=dh - bh;
                        BorderLeft.Left=0; BorderLeft.Right=bw;

                        BorderRight.Top=bh; BorderRight.Bottom=dh - bh;
                        BorderRight.Left=dw - bw; BorderRight.Right=dw;
                }
        }
}


void CompleteFrame(void)
{
        int x=RasterX, y=RasterY;
        BYTE *dest=buffer + y*TVP;

        while(y<=WinB)
        {
                while(x<=(WinR*BPP))
                {
                        if (BPP==1) dest[x]= 0;
                        else *(DWORD *)(dest+x) = Colours[0];

                        x += BPP;
                }
                x=0;
                y++;
                dest+=TVP;
        }
}

int RenderInit(void)
{
        if (Form1->RenderMode==RENDERDDRAW) return(DDInit());
        return(1);
}

void RenderEnd(void)
{
        if (GDIFrame)
        {
                delete GDIFrame;
                GDIFrame=NULL;
        }

        DDEnd();
}

void RenderDrawBorder()
{
        if (Form1->RenderMode==RENDERDDRAW) DDDrawBorder();
        else GDIDrawBorder();
}

void RecalcPalette(void)
{
        int rsz, gsz, bsz; 	//bitsize of field
        int rsh, gsh, bsh;	//0’s on left (the shift value)
        DWORD CompiledPixel;
        int i,r,g,b;

        if (Form1->RenderMode==RENDERDDRAW)
        {
                DDPIXELFORMAT DDpf;
                ZeroMemory (&DDpf, sizeof(DDpf));
                DDpf.dwSize = sizeof(DDpf);
                m_pddsFrontBuffer->GetPixelFormat(&DDpf);

                rsz = DDGetMaskInfo (DDpf.dwRBitMask, &rsh);
                gsz = DDGetMaskInfo (DDpf.dwGBitMask, &gsh);
                bsz = DDGetMaskInfo (DDpf.dwBBitMask, &bsh);
        }
        else
        {
                rsz=5;
                gsz=6;
                bsz=5;
                rsh=11;
                gsh=5;
                bsh=0;
        }

        for(i=0;i<256;i++)
        {
                r=Palette[i]&0xff;
                g=(Palette[i]>>8)&0xff;
                b=(Palette[i]>>16)&0xff;

                r >>= (8-rsz);	//keep only the MSB bits of component
                g >>= (8-gsz);
                b >>= (8-bsz);
                r <<= rsh;	//SHIFT THEM INTO PLACE
                g <<= gsh;
                b <<= bsh;

                CompiledPixel = (DWORD)(r | g | b);
                Colours[i]=CompiledPixel;
                if (i==0 && !FScreen.WhiteLetterbox) LetterBoxColour=CompiledPixel;
                if (i==(7*16) && FScreen.WhiteLetterbox) LetterBoxColour=CompiledPixel;

        }
}

void AccurateInit(int resize)
{
        dest=buffer=NULL;
        if (Form1->RenderMode==RENDERDDRAW) DDAccurateInit(resize);
        else GDIAccurateInit(resize);
}

void AccurateUpdateDisplay(bool singlestep)
{
        if (Form1->RenderMode==RENDERDDRAW) DDAccurateUpdateDisplay(singlestep);
        else GDIAccurateUpdateDisplay(singlestep);
}

static void GetPixelColour(int x, int y, unsigned char *r, unsigned char *g, unsigned char *b)
{
        char *p;
        DWORD c;
        int i;

        p=buffer + x*BPP + y*TVP;
        c=*(DWORD *)p;

        switch(BPP)
        {
        case 2: c &= 65535; break;
        case 3: c &= 16777215; break;
        case 1:
        case 4: break;
        }

        for(i=0;i<256;i++)
                if (Colours[i]==c)
                {
                        *r=Palette[i]&0xff;
                        *g=(Palette[i]>>8)&0xff;
                        *b=(Palette[i]>>16)&0xff;
                        return;
                }

        *r=0;
        *g=0;
        *b=0;
}

void SaveScreenShot(AnsiString Filename)
{
        FILE *f;
        int w,h,x,y,Pw;
        int pad, size;
        int i,j,k;
        unsigned char r,g,b;
        short int b16;
        int b32;

        w=WinR-WinL;
        h=WinB-WinT;
        x=WinL;
        y=WinT;

        Pw=w*3;
        pad=4-(Pw%4); if (pad==4) pad=0;

        size = ((Pw+pad)*h)+54;

        f=fopen(Filename.c_str(),"wb");

        fwrite("BM", 1, 2, f);
        b32=size; fwrite(&b32, 1, 4, f);
        b16=0; fwrite(&b16, 1, 2, f);
        b16=0; fwrite(&b16, 1, 2, f);
        b32=54; fwrite(&b32, 1, 4, f);
        b32=40; fwrite(&b32, 1, 4, f);
        b32=w; fwrite(&b32, 1, 4, f);
        b32=h; fwrite(&b32, 1, 4, f);
        b16=1; fwrite(&b16, 1, 2, f);
        b16=24; fwrite(&b16, 1, 2, f);
        b32=0; fwrite(&b32, 1, 4, f);
        b32=0; fwrite(&b32, 1, 4, f);
        b32=0; fwrite(&b32, 1, 4, f);
        b32=0; fwrite(&b32, 1, 4, f);
        b32=0; fwrite(&b32, 1, 4, f);
        b32=0; fwrite(&b32, 1, 4, f);

        for(i=h-1;i>=0;i--)
        {
                for(j=0;j<w;j++)
                {
                        GetPixelColour(x+j, y+i, &r, &g, &b);
                        fwrite(&b, 1,1, f);
                        fwrite(&g, 1,1, f);
                        fwrite(&r, 1,1, f);
                }
                for(k=0;k<pad;k++) fwrite("\0", 1,1, f);
        }

        fclose(f);
}

