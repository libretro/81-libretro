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
 * TZXMan.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "TZXMan.h"
#include "tzxfile.h"
#include "main_.h"
#include "utils.h"
#include "sound.h"
#include "Wavload_.h"
#include "zx81config.h"
#include "ZipFile_.h"

#define edt1 0x480

TTZXFile TZXFile;

extern int AutoLoadCount;
int FlashLoadable;

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "OffBtn"
#pragma resource "*.dfm"
TTZX *TZX;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void TTZX::ClockTick(int TStates, bool ZX81)
{
        static bool Previous=true;

        if (ZX81 && WavLoad->IgnoreZX81) return;
        if (RecTimeOut)
        {
                RecTimeOut -= TStates;
                if (RecTimeOut < 884)
                {
                        RecTimeOut=RecTimeOut;
                }
                if (RecTimeOut<=0) while(RecState!=REC_STOP) RecStopCheck();
        }

        if (TZXFile.Playing!=Previous)
        {
                Previous=TZXFile.Playing;
                if (Previous)
                {
                        TZX->PlayBtn->Down=true;
                        TZX->StopBtn->Down=false;
                }
                else
                {
                        TZX->PlayBtn->Down=false;
                        TZX->StopBtn->Down=true;
                }
                TZX->UpdateButtons();
        }

        if (!TZXFile.Playing) return;

        if (SoundOn->Down) sound_beeper(TZXFile.GetEarState());

        if (TZXFile.ClockTick(TStates))
        {
                int a;
                a=TZXFile.CurBlock+1;
                if (a>=TZX->Table->RowCount) a=TZX->Table->RowCount-1;
                TZX->Table->Row=a;
                TZXFile.CurBlock=a-1;
        }
}

//---------------------------------------------------------------------------

void TTZX::AddBlock(char *buffer, int len)
{
        if (!buffer)    TZXFile.AddPauseBlock(len/3250);
        else    TZXFile.AddGeneralBlock(buffer,len);
        UpdateTable(false);
}

void TTZX::UpdateTable(bool NewFile)
{
        int i,j;
        AnsiString Name, Type, Length;
        int Indent;

        Table->RowCount=TZXFile.Blocks+2;

        for(i=0;i<TZXFile.Blocks;i++)
        {
                Type=TZXFile.GetBlockType(i);
                Name=TZXFile.GetBlockName(i);
                Length=TZXFile.GetBlockLength(i);
                Indent=TZXFile.GetGroup(i)*8;

                Name = AnsiString::StringOfChar(' ', Indent) + Name;
                //for(j=0;j<Indent;j++) Name = " " + Name;

                //Table->Cells[0][i+1]=Type;
                Table->Cells[0][i+1]=Name;
                Table->Cells[1][i+1]=Length;
        }

        Table->Rows[TZXFile.Blocks+1]->CommaText=",";
        TZXFile.CurBlock=Table->Row-1;

        if (NewFile)
        {
                Table->Row=1;
                TZXFile.CurBlock=Table->Row-1;
        }

        FormResize(NULL);
        UpdateButtons();
}
//---------------------------------------------------------------------------
void TTZX::UpdateScrollBar(void)
{
        ScrollBar1->Min=1;
        ScrollBar1->Max=Table->RowCount - Table->VisibleRowCount ;
        ScrollBar1->Position = Table->TopRow;

        if (Table->RowCount < (Table->Height /
                                (Table->DefaultRowHeight+Table->GridLineWidth)))
        {
                ScrollBar1->Visible=false;
                Table->Width = Bevel1->Width - 8;
        }
        else
        {
                ScrollBar1->Visible=true;
                Table->Width = Bevel1->Width - 8 - ScrollBar1->Width;
        }

        ScrollBar1->LargeChange = Table->VisibleRowCount;
}

void TTZX::UpdateButtons(void)
{
        RewEnd->Enabled=true;
        Rewind->Enabled=true;
        FForward->Enabled=true;
        FFEnd->Enabled=true;
        if (Table->Row == 1)
        {
                RewEnd->Enabled=false;
                Rewind->Enabled=false;
        }

        if (Table->Row == Table->RowCount-1)
        {
                FForward->Enabled=false;
                FFEnd->Enabled=false;
        }
}
//---------------------------------------------------------------------------
__fastcall TTZX::TTZX(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile *ini;

        ini = new TIniFile(zx81.inipath);
        LoadSettings(ini);
        delete ini;

        DragAcceptFiles(Handle, true);
        Table->Rows[0]->CommaText="Description,Size";
        FormResize(NULL);
}
//---------------------------------------------------------------------------
void TTZX::LoadFile(AnsiString Filename, bool Insert)
{
        AnsiString Extension;

        TZXFile.Stop(false);

        Extension=FileNameGetExt(Filename);
        if (Extension == ".ZIP")
        {
                Filename=ZipFile->ExpandZIP(Filename, OpenDialog->Filter);
                if (Filename=="") return;
                Extension = FileNameGetExt(Filename);
        }


        if (!TZXFile.LoadFile(Filename, Insert)) return;

        //UpdateTable();
        //Table->Row=1;
        //TZXFile.CurBlock=Table->Row-1;

        if (!Insert)
        {
                TZXFile.AutoStart=AutoStartBtn->Down;
                if (AutoLoadBtn->Down) AutoLoadCount=1;

                Caption = FileNameGetFname(Filename)+FileNameGetExt(Filename)+" - Tape";
                Form1->InTZXManagerClick(NULL);
                Form1->OutTZXManagerClick(NULL);
        }
}
//---------------------------------------------------------------------------
void __fastcall TTZX::Open1Click(TObject *Sender)
{
        int i, insert=false;

        TZXFile.Stop(false);
        if (OpenDialog->Execute())
                for(i=0;i < OpenDialog->Files->Count; i++)
                {
                        LoadFile(OpenDialog->Files->Strings[i] , insert);
                        insert=true;
                        TZXFile.CurBlock=TZXFile.Blocks;
                }
        UpdateTable(true);
        //Table->Row=1;
        //TZXFile.CurBlock=Table->Row-1;
}
//---------------------------------------------------------------------------

void __fastcall TTZX::FormResize(TObject *Sender)
{
        Table->ColWidths[1]=50;
        Table->ColWidths[0]=Table->Width-Table->ColWidths[1];
        UpdateScrollBar();
}
//---------------------------------------------------------------------------

void __fastcall TTZX::TableSelectCell(TObject *Sender, int ACol, int ARow,
      bool &CanSelect)
{
        int line;
        //TZXFile.Stop();
        TZXFile.CurBlock=Table->Row-1;

        line=Table->Row;
/*        if (line<1 || line>TZXFile.Blocks)
        {
                EditBlock2->Enabled=false;
                InsertBlock2->Enabled=false;
                ExtractBlock2->Enabled=false;
                MoveBlockUp2->Enabled=false;
                MoveBlockDown2->Enabled=false;
                DeleteBlock2->Enabled=false;
        }
        else
        {
                if (TZXFile.IsEditable(line-1)) EditBlock2->Enabled=true;
                else EditBlock2->Enabled=false;

                //EditBlock1->Enabled=true;
                //InsertBlock1->Enabled=true;
                //ExtractBlock1->Enabled=true;
                MoveBlockUp2->Enabled=true;
                MoveBlockDown2->Enabled=true;
                DeleteBlock2->Enabled=true;
        }
*/
        if (line==TZXFile.Blocks+1)
        {
                Table->Row=line;
                //InsertBlock2->Enabled=true;
        }

        //if (line==TZXFile.Blocks) MoveBlockDown2->Enabled=false;
        //if (line==1) MoveBlockUp2->Enabled=false;
        //if (Table->Cells[0][line] == "25") ExtractBlock2->Enabled=true;
        //else ExtractBlock2->Enabled=false;
        UpdateScrollBar();
        UpdateButtons();
}
//---------------------------------------------------------------------------
void TTZX::MergeBlocks(void)
{
        TZXFile.MergeBlocks();
}

//---------------------------------------------------------------------------
void TTZX::LoadSettings(TIniFile *ini)
{
        Top = ini->ReadInteger("TZXMAN","Top",Top);
        Left = ini->ReadInteger("TZXMAN","Left",Left);
        Height = ini->ReadInteger("TZXMAN","Height",Height);
        Width = ini->ReadInteger("TZXMAN","Width",Width);

        OpenDialog->FileName = ini->ReadString("TZXMAN","Filename",OpenDialog->FileName);
        OpenDialog->InitialDir = ini->ReadString("TZXMAN","Dir",OpenDialog->InitialDir);

        FlashLoadBtn->Down = ini->ReadBool("TZXMAN","FlashLoad",FlashLoadBtn->Down);
        AutoStartBtn->Down = ini->ReadBool("TZXMAN","AutoStart",AutoStartBtn->Down);
        AutoLoadBtn->Down = ini->ReadBool("TZXMAN","AutoLoad",AutoLoadBtn->Down);
        SoundOn->Down = ini->ReadBool("TZXMAN","Sound",SoundOn->Down);
        TZXFile.AutoStart=AutoStartBtn->Down;
        TZXFile.FlashLoad=FlashLoadBtn->Down;
        if (Form1->TZXMan->Checked) Show();

        //TZXFile.NewTZX();
        NewTZXClick(NULL);
        UpdateTable(false);
}

void TTZX::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("TZXMAN","Top",Top);
        ini->WriteInteger("TZXMAN","Left",Left);
        ini->WriteInteger("TZXMAN","Height",Height);
        ini->WriteInteger("TZXMAN","Width",Width);

        ini->WriteString("TZXMAN","Filename",OpenDialog->FileName);
        ini->WriteString("TZXMAN","Dir",OpenDialog->InitialDir);
        ini->WriteBool("TZXMAN","FlashLoad",FlashLoadBtn->Down);
        ini->WriteBool("TZXMAN","AutoStart",AutoStartBtn->Down);
        ini->WriteBool("TZXMAN","AutoLoad",AutoLoadBtn->Down);
        ini->WriteBool("TZXMAN","Sound",SoundOn->Down);
}
//---------------------------------------------------------------------------


void __fastcall TTZX::NewTZXClick(TObject *Sender)
{
        TZXFile.NewTZX();
        TZXFile.AutoStart=AutoStartBtn->Down;
        UpdateTable(false);
        Table->Row=2;
        Caption="Untitled - Tape";
}
//---------------------------------------------------------------------------

void __fastcall TTZX::SaveAs1Click(TObject *Sender)
{
        int zx81, spec, other;
        int canT81, canTAP, canP;
        AnsiString Filter;
        AnsiString FileName;

        canT81=canTAP=canP=true;

        TZXFile.CountBlocks(&zx81, &spec, &other);

        if (other) { canTAP=canT81=canP=false; }
        if (spec) { canT81=canP=false; }
        if (zx81) { canTAP=false; }
        if (zx81!=1) {canP=false; }
        if (!zx81 && !spec && !other) { canTAP=canT81=canP=false; }

        Filter="TZX File (*.tzx)|*.tzx";
        if (canTAP) Filter += "|TAP File (*.tap)|*.tap";
        if (canT81) Filter += "|T81 File (*.t81)|*.t81";
        if (canP) Filter += "|P File (*.p)|*.p|O File (*.o)|*.o";

        SaveDialog->Filter=Filter;

        FileName=TZXFile.FileName;

        if (FileName != "")
        {
                AnsiString Ext;

                Ext=GetExt(FileName);
                FileName=RemoveExt(FileName);

                if (Ext==".TAP" && !canTAP) Ext=".TZX";
                if (Ext==".T81" && !canT81) Ext=".TZX";
                if (Ext==".P" && !canP) Ext=".TZX";
                if (Ext==".O" && !canP) Ext=".TZX";

                SaveDialog->FilterIndex=1;
                if (Ext==".TAP") SaveDialog->FilterIndex=2;
                if (Ext==".T81") SaveDialog->FilterIndex=2;
                if (Ext==".P") SaveDialog->FilterIndex=3;
                if (Ext==".O") SaveDialog->FilterIndex=3;

                FileName += Ext;
        }

        SaveDialog->FileName=FileName;
        if (!SaveDialog->Execute()) return;
        TZXFile.SaveFile(SaveDialog->FileName);
}
//---------------------------------------------------------------------------

void __fastcall TTZX::FlashLoad1Click(TObject *Sender)
{
        FlashLoadBtn->Down = !FlashLoadBtn->Down;
        TZXFile.FlashLoad=FlashLoadBtn->Down;
}
//---------------------------------------------------------------------------

void __fastcall TTZX::FormClose(TObject *Sender, TCloseAction &Action)
{
        Form1->TZXMan->Checked=false;        
}
//---------------------------------------------------------------------------

void __fastcall TTZX::TableContextPopup(TObject *Sender, TPoint &MousePos,
      bool &Handled)
{
        int line;
        Mx=Left+MousePos.x;
        My=32+Top+MousePos.y;

        line=Table->TopRow + MousePos.y / (Table->DefaultRowHeight+Table->GridLineWidth) - 1;
        if (line<1 || line>TZXFile.Blocks)
        {
                EditBlock1->Enabled=false;
                InsertBlock1->Enabled=false;
                ExtractBlock1->Enabled=false;
                MoveBlockUp1->Enabled=false;
                MoveBlockDown1->Enabled=false;
                DeleteBlock1->Enabled=false;
        }
        else
        {
                int type;
                Table->Row=line;

                if (TZXFile.IsEditable(line-1)) EditBlock1->Enabled=true;
                else EditBlock1->Enabled=false;

                MoveBlockUp1->Enabled=true;
                MoveBlockDown1->Enabled=true;
                DeleteBlock1->Enabled=true;

                type=TZXFile.Tape[line-1].BlockID;
                switch(type)
                {
                case TZX_BLOCK_ROM:
                case TZX_BLOCK_TURBO:
                case TZX_BLOCK_TONE:
                case TZX_BLOCK_PULSE:
                case TZX_BLOCK_DATA:
                case TZX_BLOCK_DREC:
                case TZX_BLOCK_CSW:
                case TZX_BLOCK_GENERAL:
                        ConvertBlocktoWave1->Enabled=true;
                        break;

                default:
                        ConvertBlocktoWave1->Enabled=false;
                        break;
                }
        }

        if (line==TZXFile.Blocks+1)
        {
                Table->Row=line;
                InsertBlock1->Enabled=true;
        }

        if (line==TZXFile.Blocks) MoveBlockDown1->Enabled=false;
        if (line==1) MoveBlockUp1->Enabled=false;
        if (Table->Cells[0][line] == "25") ExtractBlock1->Enabled=true;
        else ExtractBlock1->Enabled=false;

}
//---------------------------------------------------------------------------

void __fastcall TTZX::MoveBlockUp1Click(TObject *Sender)
{
        TZXFile.SwapBlocks(Table->Row-1, Table->Row-2);
        UpdateTable(false);
}
//---------------------------------------------------------------------------

void __fastcall TTZX::MoveBlockDown1Click(TObject *Sender)
{
        TZXFile.SwapBlocks(Table->Row, Table->Row-1);
        UpdateTable(false);
}
//---------------------------------------------------------------------------

void __fastcall TTZX::DeleteBlock1Click(TObject *Sender)
{
        TZXFile.DeleteBlock(Table->Row-1);
        UpdateTable(false);
}
//---------------------------------------------------------------------------

void __fastcall TTZX::EditBlock1Click(TObject *Sender)
{
        Mx = TZX->Left + Table->Left + (Table->Width/2);
        My = TZX->Top + Table->Top + (
                Table->Row - Table->TopRow)*(Table->DefaultRowHeight*Table->GridLineWidth);

        TZXFile.EditBlock(Table->Row-1, Mx, My);
        UpdateTable(false);
}
//---------------------------------------------------------------------------

void __fastcall TTZX::AutoStartStopClick(TObject *Sender)
{
        AutoStartBtn->Down = !AutoStartBtn->Down;
        TZXFile.AutoStart = AutoStartBtn->Down;

        if (!AutoStartBtn->Down && AutoLoadBtn->Down)
                AutoLoadonInsert1Click(NULL);
}
//---------------------------------------------------------------------------


void __fastcall TTZX::HardwareInformation1Click(TObject *Sender)
{
        TZXFile.MoveBlock(TZXFile.AddHWTypeBlock(0,0x0c),Table->Row-1);
        TZXFile.EditBlock(Table->Row-1, Mx, My);
        UpdateTable(false);

}
//---------------------------------------------------------------------------

void __fastcall TTZX::ArchiveInfo1Click(TObject *Sender)
{
        TZXFile.MoveBlock(TZXFile.AddArchiveBlock("Title"),Table->Row-1);
        TZXFile.EditBlock(Table->Row-1, Mx, My);
        UpdateTable(false);

}
//---------------------------------------------------------------------------

void __fastcall TTZX::TextDescription1Click(TObject *Sender)
{
        TZXFile.MoveBlock(TZXFile.AddTextBlock("Message"),Table->Row-1);
        TZXFile.EditBlock(Table->Row-1, Mx, My);
        UpdateTable(false);
}
//---------------------------------------------------------------------------

void __fastcall TTZX::Pause1Click(TObject *Sender)
{
        TZXFile.MoveBlock(TZXFile.AddPauseBlock(0),Table->Row-1);
        TZXFile.EditBlock(Table->Row-1, Mx, My);
        UpdateTable(false);

}
//---------------------------------------------------------------------------

void TTZX::RecordByte(unsigned char c)
{
        if (RecState==REC_STOP)
        {
                RecPtr=RecBuf;
                RecState=REC_RECORDING;
        }

        if (RecState==REC_RECORDING || RecState==REC_ENDING)
        {
                *RecPtr=c;
                RecPtr++;
                RecState=REC_RECORDING;
                RecTimeOut=1000;
        }
}

void TTZX::RecStopCheck(void)
{
        int BlockLen;

        if (RecState==REC_STOP) return;

        if (RecState==REC_RECORDING)
        {
                RecState=REC_ENDING;
                return;
        }

        RecState=REC_STOP;
        RecTimeOut=0;

        BlockLen=RecPtr-RecBuf;
        if (BlockLen<2) return;

        if (zx81.machine==MACHINESPEC48 || zx81.machine==MACHINEACE)
        {
                TZXFile.MoveBlock(TZXFile.AddROMBlock(RecBuf, BlockLen),Table->Row-1);
                TZXFile.Tape[TZXFile.CurBlock].Pause=3000;
        }
        else
        {
                TZXFile.MoveBlock(TZXFile.AddGeneralBlock(RecBuf, BlockLen),Table->Row-1);
                TZXFile.Tape[TZXFile.CurBlock].Pause=5000;
        }

        UpdateTable(false);
        Table->Row ++;
}


void __fastcall TTZX::Data1Click(TObject *Sender)
{
        int i;
        int row;

        TZXFile.Stop(false);
        OpenDialog->Title="Insert";

        if (OpenDialog->Execute())
                for(i=0;i < OpenDialog->Files->Count; i++)
                {
                        LoadFile(OpenDialog->Files->Strings[i] , true);
                        if (TZXFile.CurBlock<(TZXFile.CurBlock+1))
                                TZXFile.CurBlock++;
                }
        OpenDialog->Title="Open";
        TZXFile.Stop(false);
        row=TZXFile.CurBlock;
        UpdateTable(false);
        TZXFile.CurBlock=row;
        Table->Row = row+1;
}
//---------------------------------------------------------------------------


void __fastcall TTZX::ExtractBlock1Click(TObject *Sender)
{
        unsigned char *Data, *p;
        int DataLen;
        int FNameLen;
        FILE *f;

        Data=TZXFile.Tape[Table->Row-1].Data.Data;
        DataLen=TZXFile.Tape[Table->Row-1].Head.General.DataLen;

        FNameLen=0;
        p=Data;

        while( !((*p)&128) && FNameLen<64){ p++; FNameLen++; } p++;

        if (FNameLen>0 && FNameLen<=32)
        {
                ExtractDialog1->FilterIndex=1;
                ExtractDialog1->FileName=ConvertZX81ASCII(Data);
        }
        else
        {
                ExtractDialog1->FilterIndex=2;
                ExtractDialog1->FileName="code";
                p=Data;
        }

        if (!ExtractDialog1->Execute()) return;

        f=fopen(ExtractDialog1->FileName.c_str(), "wb");
        if (!f) return;

        fwrite(p, 1, DataLen - (p-Data), f);
        fclose(f);
}
//---------------------------------------------------------------------------

void __fastcall TTZX::Save1Click(TObject *Sender)
{
        if (TZXFile.FileName=="") SaveAs1Click(NULL);
        else TZXFile.SaveFile(SaveDialog->FileName);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall TTZX::AutoLoadonInsert1Click(TObject *Sender)
{
        AutoLoadBtn->Down = !AutoLoadBtn->Down;
        if (!AutoStartBtn->Down && AutoLoadBtn->Down) AutoStartStopClick(NULL);
}
//---------------------------------------------------------------------------


void __fastcall TTZX::ScrollBar1Change(TObject *Sender)
{
        //ScrollBar1->Max=Table->RowCount - Table->VisibleRowCount ;
        Table->TopRow = ScrollBar1->Position;
}
//---------------------------------------------------------------------------

void __fastcall TTZX::RewEndClick(TObject *Sender)
{
        TZXFile.Stop(false);

        Table->Row = 1;
        TZXFile.CurBlock=Table->Row-1;
        UpdateScrollBar();
        UpdateButtons();
}
//---------------------------------------------------------------------------


void __fastcall TTZX::RewindClick(TObject *Sender)
{
        TZXFile.Stop(false);

        if (Table->Row>1)
        {
                Table->Row --;
                TZXFile.CurBlock=Table->Row-1;
        }

        UpdateScrollBar();
        UpdateButtons();
}
//---------------------------------------------------------------------------


void __fastcall TTZX::StopBtnClick(TObject *Sender)
{
        int a;

        while(RecState!=REC_STOP) RecStopCheck();

        if (StopBtn->Down) return;
        TZXFile.Stop(false);
        a=TZXFile.CurBlock;
        Table->Row = a+1;
        TZXFile.CurBlock=a;
        PlayBtn->Down=false;
}
//---------------------------------------------------------------------------

void __fastcall TTZX::PlayBtnClick(TObject *Sender)
{
        if (TZXFile.Tape[TZXFile.CurBlock].BlockID == TZX_BLOCK_PAUSE)
        {
                Table->Row ++; TZXFile.CurBlock ++;
        }
        if (!TZXFile.Start()) return;

        if (PlayBtn->Down) return;
        PlayBtn->Down=true;
        StopBtn->Down=false;

}
//---------------------------------------------------------------------------

void __fastcall TTZX::FForwardClick(TObject *Sender)
{
        TZXFile.Stop(false);

        if (Table->Row < Table->RowCount-1)
        {
                Table->Row ++;
                TZXFile.CurBlock=Table->Row-1;
        }
        UpdateScrollBar();
        UpdateButtons();
}
//---------------------------------------------------------------------------

void __fastcall TTZX::FFEndClick(TObject *Sender)
{
        TZXFile.Stop(false);

        Table->Row = Table->RowCount-1;
        TZXFile.CurBlock=Table->Row-1;
        UpdateScrollBar();
        UpdateButtons();
}
//---------------------------------------------------------------------------



void __fastcall TTZX::SaveDialogTypeChange(TObject *Sender)
{
        int i,p;
        AnsiString filter, newext;
        AnsiString Fname;

        THandle *h;
        TSaveDialog *d;

        filter=SaveDialog->Filter;
        p=1;
        i=SaveDialog->FilterIndex;
        i=(i*2)-1;

        while(i)
        {
                if (filter[p]=='|') i--;
                p++;
        }
        newext = "";
        p++;

        while(filter[p]!='|')
        {
                newext += filter[p];
                p++;
                if (p>filter.Length()) break;
        }

        Fname = RemovePath(RemoveExt(SaveDialog->FileName) + newext);

        d=(TSaveDialog *)Sender;
        h=(THandle *)GetParent(d->Handle);
        SendMessage(h, CDM_SETCONTROLTEXT, edt1, (long)(Fname.c_str()));
}

//---------------------------------------------------------------------------

void __fastcall TTZX::SoundOnClick(TObject *Sender)
{
        SoundOn->Down = !SoundOn->Down;
}
//---------------------------------------------------------------------------



void __fastcall TTZX::FormCreate(TObject *Sender)
{
        if (Panel1->Top > (ClientHeight-Panel1->Height))
        {
                Panel1->Top = ClientHeight-(Panel1->Height + 5);
                Panel1->Left = (ClientWidth/2) - (Panel1->Width/2);
        }

        if (Table->Width > (ClientWidth-Table->Left*2)) Table->Width=ClientWidth-Table->Left*2;
        if (Bevel1->Width > (ClientWidth-Bevel1->Left*2)) Bevel1->Width=ClientWidth-Bevel1->Left*2;

        if ((Bevel1->Height+Bevel1->Top) > (Panel1->Top-5))
                Bevel1->Height = (Panel1->Top-5)-Bevel1->Top;

        if ((Table->Height+Table->Top) > (Panel1->Top-9))
                Table->Height = (Panel1->Top-9)-Table->Top;

        if (ScrollBar1->Left > (Panel1->Top-9))
        {
                ScrollBar1->Left = (Table->Left+Table->Width)-ScrollBar1->Width;
                ScrollBar1->Height = Table->Height;
        }

        if (Panel2->Width > ClientWidth) Panel2->Width = ClientWidth - (Panel2->Left*2);
        //if ((AutoLoadBtn->Left+AutoLoadBtn->Width) > (ClientWidth-10))
        //{
        //        AutoLoadBtn->Left = ClientWidth - (AutoLoadBtn->Width+10);
        //        AutoStartBtn->Left = AutoLoadBtn->Left - AutoStartBtn->Width;
        //        FlashLoadBtn->Left = AutoStartBtn->Left - FlashLoadBtn->Width;
        //        SoundOn->Left = FlashLoadBtn->Left - SoundOn->Width;
        //}
}
//---------------------------------------------------------------------------


void __fastcall TTZX::ConvertTapetoWave1Click(TObject *Sender)
{
        int i;
        bool FlashLoad, AutoStart, AutoLoad;

        FlashLoad=FlashLoadBtn->Down;
        AutoStart=AutoStartBtn->Down;
        AutoLoad=AutoLoadBtn->Down;

        if (FlashLoad) FlashLoad1Click(NULL);
        if (AutoStart) AutoStartStopClick(NULL);

        StopBtnClick(NULL);
        RewEndClick(NULL);
        WavLoad->NewClick(NULL);
        WavLoad->IgnoreZX81=true;

        WavLoad->RecordBtnClick(NULL);
        PlayBtnClick(NULL);

        i=0;
        while(TZXFile.CurBlock < TZXFile.Blocks)
        {
                ClockTick(16, false);
                WavLoad->ClockTick(16, 0, TZXFile.GetEarState());
                if (++i==10) { i=0; Application->ProcessMessages(); }
        }

        StopBtnClick(NULL);
        i=3500000;
        while(i>=0)
        {
                ClockTick(8, false);
                WavLoad->ClockTick(8, 0, TZXFile.GetEarState());
                Application->ProcessMessages();
                i-=8;
        }

        WavLoad->StopBtnClick(NULL);
        WavLoad->IgnoreZX81=false;
        if (FlashLoad) FlashLoad1Click(NULL);
        if (AutoStart) AutoStartStopClick(NULL);
        if (AutoLoad) AutoLoadonInsert1Click(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TTZX::ConvertBlocktoWave1Click(TObject *Sender)
{
        int i,Block;
        bool FlashLoad, AutoStart, AutoLoad;

        FlashLoad=FlashLoadBtn->Down;
        AutoStart=AutoStartBtn->Down;
        AutoLoad=AutoLoadBtn->Down;

        StopBtnClick(NULL);
        //RewEndClick(NULL);
        WavLoad->NewClick(NULL);
        WavLoad->IgnoreZX81=true;

        WavLoad->RecordBtnClick(NULL);
        PlayBtnClick(NULL);

        Block=Table->Row;
        i=0;
        while(Table->Row == Block)
        {
                ClockTick(16, false);
                WavLoad->ClockTick(16, 0, TZXFile.GetEarState());

                if (++i==10) { Application->ProcessMessages(); i=0; }
        }

        StopBtnClick(NULL);
        i==3500000;
        while(i>=0)
        {
                ClockTick(8, false);
                WavLoad->ClockTick(8, 0, TZXFile.GetEarState());
                Application->ProcessMessages();
                i-=8;
        }
        WavLoad->StopBtnClick(NULL);
        WavLoad->IgnoreZX81=false;
        if (FlashLoad) FlashLoad1Click(NULL);
        if (AutoStart) AutoStartStopClick(NULL);
        if (AutoLoad) AutoLoadonInsert1Click(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TTZX::GroupStart1Click(TObject *Sender)
{
        TZXFile.MoveBlock(TZXFile.AddGroupStartBlock("Group Start"),Table->Row-1);
        TZXFile.EditBlock(Table->Row-1, Mx, My);
        TZXFile.GroupCount();
        UpdateTable(false);
}
//---------------------------------------------------------------------------

void __fastcall TTZX::GroupEnd1Click(TObject *Sender)
{
        TZXFile.MoveBlock(TZXFile.AddGroupEndBlock(),Table->Row-1);
        TZXFile.EditBlock(Table->Row-1, Mx, My);
        TZXFile.GroupCount();
        UpdateTable(false);
}
//---------------------------------------------------------------------------

