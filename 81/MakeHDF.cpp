//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "MakeHDF.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TCreateHDF *CreateHDF;
//---------------------------------------------------------------------------
__fastcall TCreateHDF::TCreateHDF(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TCreateHDF::CylUpDownClick(TObject *Sender,
      TUDBtnType Button)
{
        ValueChanged();
}
//---------------------------------------------------------------------------

AnsiString TCreateHDF::ConvertValue(long Val)
{
        AnsiString output;
        if (Val>=2097152)
        {
                Val/=2097152;
                output=Val;
                output=output+"Gb";
                return(output);
        }

        if (Val>=2048)
        {
                Val/=2048;
                output=Val;
                output=output+"Mb";
                return(output);
        }

        if (Val>=2)
        {
                Val/=2;
                output=Val;
                output=output+"kb";
                return(output);
        }

        output=Val*512;
        output=output+"b";
        return(output);
}

void TCreateHDF::ValueChanged(void)
{
        long size;

        CylText->Text = CylUpDown->Position+32769;
        HeadText->Text = HeadUpDown->Position;
        SecText->Text = SecUpDown->Position;

        size = (CylUpDown->Position+32769) * HeadUpDown->Position * SecUpDown->Position;
        //size *= 512;

        EffSize->Text=ConvertValue(size);

        if (EightBit->Checked) HDFSize->Text = ConvertValue(size/2);
        else HDFSize->Text = ConvertValue(size);
}

void __fastcall TCreateHDF::HeadUpDownClick(TObject *Sender,
      TUDBtnType Button)
{
        ValueChanged();        
}
//---------------------------------------------------------------------------

void __fastcall TCreateHDF::SecUpDownClick(TObject *Sender,
      TUDBtnType Button)
{
        ValueChanged();
}
//---------------------------------------------------------------------------

void __fastcall TCreateHDF::FormShow(TObject *Sender)
{
        Progress->Visible=false;
        ValueChanged();
        FName->Text=FileName;
        EnableAll();
}
//---------------------------------------------------------------------------

void __fastcall TCreateHDF::EightBitClick(TObject *Sender)
{
        ValueChanged();
}
//---------------------------------------------------------------------------

void __fastcall TCreateHDF::CylTextChange(TObject *Sender)
{
        int i;
        i=atoi(CylText->Text.c_str());

        if (((i-32769)>=CylUpDown->Min) && ((i-32769)<=CylUpDown->Max))
        {
                CylText->Color = clWindow;
                CylUpDown->Position=i-32769;
                ValueChanged();
        }
        else    CylText->Color = clRed;
}
//---------------------------------------------------------------------------

void __fastcall TCreateHDF::HeadTextChange(TObject *Sender)
{
        int i;
        i=atoi(HeadText->Text.c_str());
        if ((i>=HeadUpDown->Min) && (i<=HeadUpDown->Max))
        {
                HeadText->Color = clWindow;
                HeadUpDown->Position=i;
                ValueChanged();
        }
        else    HeadText->Color = clRed;

}
//---------------------------------------------------------------------------

void __fastcall TCreateHDF::SecTextChange(TObject *Sender)
{
        int i;
        i=atoi(SecText->Text.c_str());
        if ((i>=SecUpDown->Min) && (i<=SecUpDown->Max))
        {
                SecText->Color = clWindow;
                SecUpDown->Position=i;
                ValueChanged();
        }
        else    SecText->Color = clRed;
}
//---------------------------------------------------------------------------
void TCreateHDF::WriteWord(unsigned char *base, int WordNo, int Value)
{
        base[WordNo*2]=Value&255;
        base[WordNo*2 +1] = (Value>>8)&255;
}

void __fastcall TCreateHDF::OKClick(TObject *Sender)
{
        FILE *f;
        unsigned char *track, head[512+0x16];
        int i,j, tracklen;

        int cylinders, heads, sectors;
        int secsize, tracks;
        int totalsectors;

        char ModelName[]="iEhgytnO eDH F                          ";

        DisableAll();

        sectors = SecUpDown->Position;
        heads = HeadUpDown->Position;
        cylinders = CylUpDown->Position+32769;
        secsize=EightBit->Checked ? 256:512;
        tracks=cylinders*heads;
        totalsectors=tracks*sectors;
        tracklen=sectors * secsize;

        track=(unsigned char *) malloc(tracklen);
        memset(track, 0, tracklen);
        memset(head, 0, 512+16);

        strcpy(head,"RS-IDE");
        head[0x06]=0x1a;
        head[0x07]=0x11;
        head[0x08]=EightBit->Checked ? 1:0;
        head[0x09]=0x16;
        head[0x0a]=0x02;

        WriteWord(head+0x16,  0, 64);
        WriteWord(head+0x16,  1, cylinders);
        WriteWord(head+0x16,  3, heads);
        WriteWord(head+0x16,  6, sectors);
        WriteWord(head+0x16, 49, 512);
        WriteWord(head+0x16, 54, cylinders);
        WriteWord(head+0x16, 55, heads);
        WriteWord(head+0x16, 56, sectors);
        WriteWord(head+0x16, 57, totalsectors&65535);
        WriteWord(head+0x16, 58, (totalsectors>>16)&65535);
        WriteWord(head+0x16, 60, totalsectors&65535);
        WriteWord(head+0x16, 61, (totalsectors>>16)&65535);

        //memcpy(head+0x16+54, ModelName, 40);
        for(i=0;i<40;i++) head[0x16+54+i]=ModelName[i];

        Progress->Max=tracks;
        Progress->Visible=true;

        f=fopen(FileName.c_str(), "wb");
        if (!f) Close();
        fwrite(head, 512+16,1,f);

        for(i=0;i<tracks;i++)
        {
                fwrite(track, tracklen, 1, f);
                Progress->Position=i;
                Application->ProcessMessages();
        }

        fclose(f);
        free(track);

        Close();
}
//---------------------------------------------------------------------------
void TCreateHDF::EnableAll(void)
{
        Label1->Enabled=true;
        Label2->Enabled=true;
        Label3->Enabled=true;
        Label4->Enabled=true;
        Label5->Enabled=true;
        Label6->Enabled=true;
        FName->Enabled=true;
        CylText->Enabled=true;
        HeadText->Enabled=true;
        SecText->Enabled=true;
        CylUpDown->Enabled=true;
        HeadUpDown->Enabled=true;
        SecUpDown->Enabled=true;
        EightBit->Enabled=true;
        EffSize->Enabled=true;
        HDFSize->Enabled=true;
        OK->Enabled=true;
}

void TCreateHDF::DisableAll(void)
{
        Label1->Enabled=false;
        Label2->Enabled=false;
        Label3->Enabled=false;
        Label4->Enabled=false;
        Label5->Enabled=false;
        Label6->Enabled=false;
        FName->Enabled=false;
        CylText->Enabled=false;
        HeadText->Enabled=false;
        SecText->Enabled=false;
        CylUpDown->Enabled=false;
        HeadUpDown->Enabled=false;
        SecUpDown->Enabled=false;
        EightBit->Enabled=false;
        EffSize->Enabled=false;
        HDFSize->Enabled=false;
        OK->Enabled=false;
}
