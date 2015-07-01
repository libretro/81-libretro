object EditDataForm: TEditDataForm
  Left = 305
  Top = 172
  Width = 632
  Height = 322
  BorderIcons = []
  Caption = 'Edit Data Block'
  Color = clBtnFace
  Constraints.MaxWidth = 632
  Constraints.MinHeight = 322
  Constraints.MinWidth = 632
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel
    Left = 8
    Top = 8
    Width = 85
    Height = 13
    Caption = 'Pilot Pulse Length'
  end
  object Label3: TLabel
    Left = 8
    Top = 56
    Width = 85
    Height = 13
    Caption = 'No. of Pilot pulses'
  end
  object Label6: TLabel
    Left = 480
    Top = 8
    Width = 94
    Height = 13
    Caption = 'Final Byte Used Bits'
  end
  object Label7: TLabel
    Left = 480
    Top = 56
    Width = 30
    Height = 13
    Caption = 'Pause'
  end
  object Label8: TLabel
    Left = 8
    Top = 104
    Width = 23
    Height = 13
    Caption = 'Data'
  end
  object Label4: TLabel
    Left = 160
    Top = 8
    Width = 98
    Height = 13
    Caption = 'Sync Pulse 1 Length'
  end
  object Label5: TLabel
    Left = 160
    Top = 56
    Width = 98
    Height = 13
    Caption = 'Sync Pulse 2 Length'
  end
  object Label1: TLabel
    Left = 320
    Top = 8
    Width = 57
    Height = 13
    Caption = 'Bit 0 Length'
  end
  object Label9: TLabel
    Left = 320
    Top = 56
    Width = 57
    Height = 13
    Caption = 'Bit 1 Length'
  end
  object Label10: TLabel
    Left = 8
    Top = 263
    Width = 68
    Height = 13
    Anchors = [akLeft, akBottom]
    Caption = 'Character Set:'
  end
  object PPL: TEdit
    Left = 8
    Top = 24
    Width = 137
    Height = 21
    TabOrder = 0
    Text = 'PPL'
  end
  object NoPP: TEdit
    Left = 8
    Top = 72
    Width = 137
    Height = 21
    TabOrder = 1
    Text = 'NoPP'
  end
  object UsedBits: TEdit
    Left = 480
    Top = 24
    Width = 137
    Height = 21
    TabOrder = 2
    Text = 'UsedBits'
  end
  object Pause: TEdit
    Left = 480
    Top = 72
    Width = 137
    Height = 21
    TabOrder = 3
    Text = 'Pause'
  end
  object OK: TButton
    Left = 544
    Top = 256
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'OK'
    Default = True
    TabOrder = 4
    OnClick = OKClick
  end
  object Data: TMemo
    Left = 8
    Top = 120
    Width = 609
    Height = 129
    Anchors = [akLeft, akTop, akRight, akBottom]
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Fixedsys'
    Font.Style = []
    Lines.Strings = (
      'Data')
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 5
  end
  object Sync1: TEdit
    Left = 160
    Top = 24
    Width = 145
    Height = 21
    TabOrder = 6
    Text = 'Sync1'
  end
  object Sync2: TEdit
    Left = 160
    Top = 72
    Width = 145
    Height = 21
    TabOrder = 7
    Text = 'Sync2'
  end
  object Bit0: TEdit
    Left = 320
    Top = 24
    Width = 145
    Height = 21
    TabOrder = 8
    Text = 'Bit0'
  end
  object Bit1: TEdit
    Left = 320
    Top = 72
    Width = 145
    Height = 21
    TabOrder = 9
    Text = 'Bit1'
  end
  object CharSet: TComboBox
    Left = 88
    Top = 259
    Width = 89
    Height = 21
    Style = csDropDownList
    Anchors = [akLeft, akBottom]
    ItemHeight = 13
    TabOrder = 10
    OnChange = CharSetChange
    Items.Strings = (
      'ZX81'
      'ASCII')
  end
end
