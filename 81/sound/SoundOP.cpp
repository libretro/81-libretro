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
 * SoundOP.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SoundOP.h"
#include "main_.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSoundOutput *SoundOutput;
//---------------------------------------------------------------------------

void TSoundOutput::UpdateImage(unsigned char *data, int len)
{
        long x,y;
        static int skip=0;

        if (++skip <3) return;
        skip=0;
        if (!SoundOutput->Visible) return;

        Img->Pen->Color = clWhite;
        Img->FillRect(rect);

        Img->Pen->Color = clRed;
        Img->MoveTo(0,Image1->Height/2);
        Img->LineTo(Image1->Width,Image1->Height/2);

        Img->Pen->Color = clBlack;
        Img->MoveTo(0, data[0]/2);
        for (x=0; x<Image1->Width; x++)
        {
                //Img->MoveTo(x,64);
                Img->LineTo(x, data[x]/2);
        }
}
//---------------------------------------------------------------------------
__fastcall TSoundOutput::TSoundOutput(TComponent* Owner)
        : TForm(Owner)
{
        Img=Image1->Canvas;
        rect.Top=0; rect.Left=0;
        rect.Right=Image1->Width; rect.Bottom=Image1->Height;
        //ClearImage();
}
//---------------------------------------------------------------------------

cdecl void SoundOP_Update(unsigned char *data, int len)
{
        SoundOutput->UpdateImage(data,len);
}
//---------------------------------------------------------------------------

void __fastcall TSoundOutput::FormClose(TObject *Sender,
      TCloseAction &Action)
{
        Form1->SoundOutput1->Checked=false;
}
//---------------------------------------------------------------------------


