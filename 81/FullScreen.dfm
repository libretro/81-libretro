object FSSettings: TFSSettings
  Left = 272
  Top = 324
  BorderIcons = []
  BorderStyle = bsSingle
  Caption = 'Display Settings'
  ClientHeight = 238
  ClientWidth = 207
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel
    Left = 8
    Top = 8
    Width = 47
    Height = 13
    Caption = 'Renderer:'
  end
  object OK: TButton
    Left = 128
    Top = 208
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 0
    OnClick = OKClick
  end
  object GroupBox1: TGroupBox
    Left = 8
    Top = 64
    Width = 193
    Height = 129
    Caption = 'Full Screen Settings'
    TabOrder = 1
    object Label1: TLabel
      Left = 16
      Top = 24
      Width = 53
      Height = 13
      Caption = 'Resolution:'
    end
    object ModeList: TComboBox
      Left = 16
      Top = 40
      Width = 161
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 0
      OnChange = ModeListChange
    end
    object Letterbox: TRadioButton
      Left = 16
      Top = 88
      Width = 97
      Height = 17
      Caption = 'Letterbox'
      TabOrder = 1
      OnClick = ModeListChange
    end
    object Stretch: TRadioButton
      Left = 16
      Top = 72
      Width = 97
      Height = 17
      Caption = 'Stretch Image'
      Checked = True
      TabOrder = 2
      TabStop = True
      OnClick = ModeListChange
    end
    object White: TCheckBox
      Left = 16
      Top = 104
      Width = 97
      Height = 17
      Caption = 'White Letterbox'
      TabOrder = 3
      OnClick = ModeListChange
    end
  end
  object GDIBtn: TRadioButton
    Left = 24
    Top = 32
    Width = 65
    Height = 17
    Caption = 'GDI'
    TabOrder = 2
    OnClick = GDIBtnClick
  end
  object DDrawBtn: TRadioButton
    Left = 104
    Top = 32
    Width = 89
    Height = 17
    Caption = 'DirectDraw'
    Checked = True
    TabOrder = 3
    TabStop = True
    OnClick = GDIBtnClick
  end
end
