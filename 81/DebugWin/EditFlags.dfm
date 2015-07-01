object EditFlag: TEditFlag
  Left = 461
  Top = 127
  BorderIcons = [biHelp]
  BorderStyle = bsToolWindow
  Caption = 'EditFlag'
  ClientHeight = 82
  ClientWidth = 140
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
    Left = 10
    Top = 8
    Width = 8
    Height = 16
    Caption = 'S'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
  end
  object Label2: TLabel
    Left = 26
    Top = 8
    Width = 8
    Height = 16
    Caption = 'Z'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
  end
  object Label4: TLabel
    Left = 58
    Top = 8
    Width = 8
    Height = 16
    Caption = 'H'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
  end
  object Label6: TLabel
    Left = 90
    Top = 8
    Width = 8
    Height = 16
    Caption = 'V'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
  end
  object Label7: TLabel
    Left = 106
    Top = 8
    Width = 8
    Height = 16
    Caption = 'N'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
  end
  object F7: TCheckBox
    Left = 8
    Top = 24
    Width = 17
    Height = 17
    TabOrder = 0
  end
  object F6: TCheckBox
    Left = 24
    Top = 24
    Width = 17
    Height = 17
    TabOrder = 1
  end
  object F5: TCheckBox
    Left = 40
    Top = 24
    Width = 17
    Height = 17
    TabOrder = 2
  end
  object F4: TCheckBox
    Left = 56
    Top = 24
    Width = 17
    Height = 17
    TabOrder = 3
  end
  object F3: TCheckBox
    Left = 72
    Top = 24
    Width = 17
    Height = 17
    TabOrder = 4
  end
  object F2: TCheckBox
    Left = 88
    Top = 24
    Width = 17
    Height = 17
    TabOrder = 5
  end
  object F1: TCheckBox
    Left = 104
    Top = 24
    Width = 17
    Height = 17
    TabOrder = 6
  end
  object F0: TCheckBox
    Left = 120
    Top = 24
    Width = 17
    Height = 17
    TabOrder = 7
  end
  object OK: TButton
    Left = 8
    Top = 48
    Width = 57
    Height = 25
    Caption = 'OK'
    TabOrder = 8
    OnClick = OKClick
  end
  object Cancel: TButton
    Left = 72
    Top = 48
    Width = 61
    Height = 25
    Caption = 'Cancel'
    TabOrder = 9
    OnClick = CancelClick
  end
end
