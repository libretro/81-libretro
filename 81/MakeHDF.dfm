object CreateHDF: TCreateHDF
  Left = 344
  Top = 84
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Create HDF File'
  ClientHeight = 151
  ClientWidth = 305
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 12
    Width = 50
    Height = 13
    Caption = 'File Name:'
  end
  object Label2: TLabel
    Left = 12
    Top = 44
    Width = 45
    Height = 13
    Alignment = taRightJustify
    Caption = 'Cylinders:'
  end
  object Label3: TLabel
    Left = 23
    Top = 68
    Width = 34
    Height = 13
    Alignment = taRightJustify
    Caption = 'Heads:'
  end
  object Label4: TLabel
    Left = 18
    Top = 92
    Width = 39
    Height = 13
    Alignment = taRightJustify
    Caption = 'Sectors:'
  end
  object Label5: TLabel
    Left = 180
    Top = 68
    Width = 68
    Height = 13
    Alignment = taRightJustify
    Caption = 'Effective Size:'
  end
  object Label6: TLabel
    Left = 168
    Top = 92
    Width = 80
    Height = 13
    Alignment = taRightJustify
    Caption = 'HDF Image Size:'
  end
  object FName: TEdit
    Left = 64
    Top = 8
    Width = 233
    Height = 21
    ReadOnly = True
    TabOrder = 0
    Text = 'FName'
  end
  object CylText: TEdit
    Left = 64
    Top = 40
    Width = 57
    Height = 21
    TabOrder = 1
    Text = '16'
    OnChange = CylTextChange
  end
  object HeadText: TEdit
    Left = 64
    Top = 64
    Width = 57
    Height = 21
    TabOrder = 2
    Text = '16'
    OnChange = HeadTextChange
  end
  object SecText: TEdit
    Left = 64
    Top = 88
    Width = 57
    Height = 21
    TabOrder = 3
    Text = '16'
    OnChange = SecTextChange
  end
  object CylUpDown: TUpDown
    Left = 122
    Top = 40
    Width = 39
    Height = 21
    Min = -32768
    Max = 32767
    Orientation = udHorizontal
    Position = -31745
    TabOrder = 4
    Wrap = False
    OnClick = CylUpDownClick
  end
  object HeadUpDown: TUpDown
    Left = 122
    Top = 64
    Width = 39
    Height = 21
    Min = 1
    Max = 15
    Orientation = udHorizontal
    Position = 8
    TabOrder = 5
    Wrap = False
    OnClick = HeadUpDownClick
  end
  object SecUpDown: TUpDown
    Left = 122
    Top = 88
    Width = 39
    Height = 21
    Min = 1
    Max = 255
    Orientation = udHorizontal
    Position = 128
    TabOrder = 6
    Wrap = False
    OnClick = SecUpDownClick
  end
  object EightBit: TCheckBox
    Left = 188
    Top = 40
    Width = 81
    Height = 17
    Alignment = taLeftJustify
    BiDiMode = bdLeftToRight
    Caption = '8 Bit Image'
    ParentBiDiMode = False
    TabOrder = 7
    OnClick = EightBitClick
  end
  object EffSize: TEdit
    Left = 256
    Top = 64
    Width = 41
    Height = 21
    ReadOnly = True
    TabOrder = 8
    Text = '0'
  end
  object HDFSize: TEdit
    Left = 256
    Top = 88
    Width = 41
    Height = 21
    ReadOnly = True
    TabOrder = 9
    Text = '0'
  end
  object OK: TButton
    Left = 224
    Top = 120
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 10
    OnClick = OKClick
  end
  object Progress: TProgressBar
    Left = 8
    Top = 124
    Width = 209
    Height = 17
    Min = 0
    Max = 983025
    TabOrder = 11
  end
end
