object ZX97Dialog: TZX97Dialog
  Left = 333
  Top = 82
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = 'ZX97 Configuration'
  ClientHeight = 112
  ClientWidth = 336
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 37
    Height = 13
    Caption = 'Protect:'
  end
  object Label2: TLabel
    Left = 176
    Top = 28
    Width = 66
    Height = 13
    Caption = 'Printer port to:'
  end
  object Label3: TLabel
    Left = 176
    Top = 50
    Width = 45
    Height = 13
    Caption = 'Filename:'
  end
  object Protect08: TCheckBox
    Left = 56
    Top = 8
    Width = 105
    Height = 17
    Caption = '$0000 - $1FFF'
    Checked = True
    State = cbChecked
    TabOrder = 0
    OnClick = Protect08Click
  end
  object ProtectAB: TCheckBox
    Left = 56
    Top = 24
    Width = 105
    Height = 17
    Caption = '$A000 - $BFFF'
    TabOrder = 1
    OnClick = ProtectABClick
  end
  object ProtectB0: TCheckBox
    Left = 56
    Top = 40
    Width = 105
    Height = 17
    Caption = 'Bank 0 (ROM)'
    Checked = True
    State = cbChecked
    TabOrder = 2
    OnClick = ProtectB0Click
  end
  object ProtectB115: TCheckBox
    Left = 56
    Top = 56
    Width = 105
    Height = 17
    Caption = 'Bank 1-15 (RAM)'
    TabOrder = 3
    OnClick = ProtectB115Click
  end
  object SaveRAM: TCheckBox
    Left = 176
    Top = 8
    Width = 153
    Height = 17
    Alignment = taLeftJustify
    BiDiMode = bdLeftToRight
    Caption = 'Save RAM on power down'
    Checked = True
    ParentBiDiMode = False
    State = cbChecked
    TabOrder = 4
  end
  object PrinterOutput: TComboBox
    Left = 256
    Top = 24
    Width = 73
    Height = 21
    Style = csDropDownList
    DropDownCount = 12
    Enabled = False
    ItemHeight = 13
    TabOrder = 5
    OnChange = PrinterOutputChange
    Items.Strings = (
      'File...'
      'LPT1:'
      'LPT2:'
      'LPT3:'
      'LPT4:'
      'COM1:'
      'COM2:'
      'COM3:'
      'COM4:'
      'COM5:'
      'COM6:'
      'COM7:'
      'COM8:')
  end
  object FileName: TEdit
    Left = 224
    Top = 48
    Width = 81
    Height = 21
    Enabled = False
    TabOrder = 6
  end
  object FileNameSelector: TButton
    Left = 304
    Top = 48
    Width = 25
    Height = 21
    Caption = '...'
    Enabled = False
    TabOrder = 7
    OnClick = FileNameSelectorClick
  end
  object OK: TButton
    Left = 256
    Top = 80
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 8
    OnClick = OKClick
  end
  object SwapRAMROM: TCheckBox
    Left = 56
    Top = 88
    Width = 113
    Height = 17
    Caption = 'Swap ROM/RAM'
    TabOrder = 9
    OnClick = SwapRAMROMClick
  end
  object SaveDialog1: TSaveDialog
    Left = 8
    Top = 24
  end
end
