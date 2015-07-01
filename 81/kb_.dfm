object Kb: TKb
  Left = 508
  Top = 139
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = 'Keyboard'
  ClientHeight = 111
  ClientWidth = 288
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
    Top = 18
    Width = 58
    Height = 13
    Caption = 'Cursor keys:'
  end
  object Label2: TLabel
    Left = 8
    Top = 56
    Width = 93
    Height = 13
    Caption = 'CTRL Key Maps to:'
  end
  object CursorMode: TComboBox
    Left = 72
    Top = 16
    Width = 113
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 0
    OnChange = CursorModeChange
    Items.Strings = (
      '5, 6, 7, 8'
      '5, 6, 7, 8 (Shifted)'
      'C,V,B,N'
      'C,V,B,N (Shifted)'
      'Custom')
  end
  object OK: TButton
    Left = 200
    Top = 80
    Width = 81
    Height = 25
    Caption = 'OK'
    Default = True
    TabOrder = 7
    OnClick = OKClick
  end
  object CustomUp: TEdit
    Left = 240
    Top = 8
    Width = 17
    Height = 21
    Enabled = False
    TabOrder = 3
    Text = '7'
  end
  object CustomDown: TEdit
    Left = 240
    Top = 32
    Width = 17
    Height = 21
    Enabled = False
    TabOrder = 4
    Text = '6'
  end
  object CustomLeft: TEdit
    Left = 216
    Top = 18
    Width = 17
    Height = 21
    Enabled = False
    TabOrder = 1
    Text = '5'
  end
  object CustomRight: TEdit
    Left = 264
    Top = 18
    Width = 17
    Height = 21
    Enabled = False
    TabOrder = 2
    Text = '8'
  end
  object RadioButton1: TRadioButton
    Left = 112
    Top = 56
    Width = 73
    Height = 17
    BiDiMode = bdLeftToRight
    Caption = 'Function'
    Checked = True
    ParentBiDiMode = False
    TabOrder = 5
    TabStop = True
  end
  object RadioButton2: TRadioButton
    Left = 112
    Top = 72
    Width = 73
    Height = 17
    Caption = '0 (Zero)'
    TabOrder = 6
  end
  object RadioButton3: TRadioButton
    Left = 112
    Top = 88
    Width = 81
    Height = 17
    Caption = 'Symbol Shift'
    Enabled = False
    TabOrder = 8
    Visible = False
  end
end
