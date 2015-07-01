object MemSave: TMemSave
  Left = 503
  Top = 100
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = 'MemSave'
  ClientHeight = 126
  ClientWidth = 199
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
    Top = 48
    Width = 41
    Height = 13
    Caption = 'Address:'
  end
  object Label2: TLabel
    Left = 104
    Top = 48
    Width = 23
    Height = 13
    Caption = 'Size:'
  end
  object Label3: TLabel
    Left = 8
    Top = 8
    Width = 45
    Height = 13
    Caption = 'Filename:'
  end
  object Address: TEdit
    Left = 8
    Top = 64
    Width = 89
    Height = 21
    TabOrder = 0
    Text = '0'
  end
  object Size: TEdit
    Left = 104
    Top = 64
    Width = 89
    Height = 21
    TabOrder = 1
    Text = '0'
  end
  object Save: TButton
    Left = 64
    Top = 96
    Width = 75
    Height = 25
    Caption = 'Save File...'
    TabOrder = 2
    OnClick = SaveClick
  end
  object Fname: TEdit
    Left = 8
    Top = 24
    Width = 153
    Height = 21
    TabOrder = 3
  end
  object Browse: TButton
    Left = 168
    Top = 24
    Width = 27
    Height = 21
    Caption = '...'
    TabOrder = 4
    OnClick = BrowseClick
  end
  object SaveDialog: TSaveDialog
    DefaultExt = '*.bin'
    Filter = 
      'Binary Files|*.bin|ROM Files|*.rom|Sprectrum Screen|*.scr|All Fi' +
      'les|*.*'
    Title = 'Save Memory Block...'
    Left = 8
    Top = 96
  end
  object OpenDialog: TOpenDialog
    Filter = 'All Files|*.*|Spectrum Screen|*.scr'
    Left = 168
    Top = 96
  end
end
