object EditArchiveInfo: TEditArchiveInfo
  Left = 462
  Top = 135
  BorderIcons = []
  BorderStyle = bsDialog
  Caption = 'Edit Archive Information'
  ClientHeight = 328
  ClientWidth = 346
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 82
    Top = 10
    Width = 23
    Height = 13
    Alignment = taRightJustify
    Caption = 'Title:'
  end
  object Label2: TLabel
    Left = 59
    Top = 34
    Width = 46
    Height = 13
    Alignment = taRightJustify
    Caption = 'Publisher:'
  end
  object Label3: TLabel
    Left = 71
    Top = 58
    Width = 34
    Height = 13
    Alignment = taRightJustify
    Caption = 'Author:'
  end
  object Label4: TLabel
    Left = 13
    Top = 82
    Width = 92
    Height = 13
    Alignment = taRightJustify
    Caption = 'Year of Publication:'
  end
  object Label5: TLabel
    Left = 54
    Top = 106
    Width = 51
    Height = 13
    Alignment = taRightJustify
    Caption = 'Language:'
  end
  object Label6: TLabel
    Left = 78
    Top = 130
    Width = 27
    Height = 13
    Alignment = taRightJustify
    Caption = 'Type:'
  end
  object Label7: TLabel
    Left = 78
    Top = 156
    Width = 27
    Height = 13
    Alignment = taRightJustify
    Caption = 'Price:'
  end
  object Label8: TLabel
    Left = 12
    Top = 178
    Width = 93
    Height = 13
    Alignment = taRightJustify
    Caption = 'Protection Scheme:'
  end
  object Label9: TLabel
    Left = 75
    Top = 204
    Width = 30
    Height = 13
    Alignment = taRightJustify
    Caption = 'Origin:'
  end
  object Label10: TLabel
    Left = 53
    Top = 240
    Width = 52
    Height = 13
    Alignment = taRightJustify
    Caption = 'Comments:'
  end
  object Origin: TEdit
    Left = 112
    Top = 200
    Width = 225
    Height = 21
    TabOrder = 8
  end
  object Protection: TEdit
    Left = 112
    Top = 176
    Width = 225
    Height = 21
    TabOrder = 7
  end
  object Price: TEdit
    Left = 112
    Top = 152
    Width = 225
    Height = 21
    TabOrder = 6
  end
  object Type: TEdit
    Left = 112
    Top = 128
    Width = 225
    Height = 21
    TabOrder = 5
  end
  object Language: TEdit
    Left = 112
    Top = 104
    Width = 225
    Height = 21
    TabOrder = 4
  end
  object Year: TEdit
    Left = 112
    Top = 80
    Width = 225
    Height = 21
    TabOrder = 3
  end
  object Author: TEdit
    Left = 112
    Top = 56
    Width = 225
    Height = 21
    TabOrder = 2
  end
  object Publisher: TEdit
    Left = 112
    Top = 32
    Width = 225
    Height = 21
    TabOrder = 1
  end
  object Title: TEdit
    Left = 112
    Top = 8
    Width = 225
    Height = 21
    TabOrder = 0
  end
  object OK: TButton
    Left = 136
    Top = 296
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    TabOrder = 10
    OnClick = OKClick
  end
  object Comments: TMemo
    Left = 112
    Top = 224
    Width = 225
    Height = 65
    TabOrder = 9
  end
end
