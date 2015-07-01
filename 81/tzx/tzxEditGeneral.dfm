object EditGeneralForm: TEditGeneralForm
  Left = 574
  Top = 53
  Width = 632
  Height = 427
  BorderIcons = []
  Caption = 'Edit General Data Block'
  Color = clBtnFace
  Constraints.MaxWidth = 632
  Constraints.MinHeight = 377
  Constraints.MinWidth = 632
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label4: TLabel
    Left = 144
    Top = 8
    Width = 116
    Height = 13
    Caption = 'Pilot/Sync Symbol Table'
  end
  object Label5: TLabel
    Left = 144
    Top = 104
    Width = 90
    Height = 13
    Caption = 'Data Symbol Table'
  end
  object Label7: TLabel
    Left = 8
    Top = 371
    Width = 30
    Height = 13
    Anchors = [akRight, akBottom]
    Caption = 'Pause'
  end
  object Label8: TLabel
    Left = 8
    Top = 208
    Width = 23
    Height = 13
    Caption = 'Data'
  end
  object Label9: TLabel
    Left = 312
    Top = 371
    Width = 68
    Height = 13
    Anchors = [akLeft, akBottom]
    Caption = 'Character Set:'
  end
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 75
    Height = 13
    Caption = 'Pilot/Sync Data'
  end
  object SymDefP: TMemo
    Left = 144
    Top = 24
    Width = 473
    Height = 73
    Lines.Strings = (
      'SyncPulses')
    ReadOnly = True
    ScrollBars = ssBoth
    TabOrder = 0
  end
  object SymDefD: TMemo
    Left = 144
    Top = 120
    Width = 473
    Height = 81
    Lines.Strings = (
      'Alphabet')
    ReadOnly = True
    ScrollBars = ssBoth
    TabOrder = 1
  end
  object Pause: TEdit
    Left = 48
    Top = 367
    Width = 145
    Height = 21
    Anchors = [akRight, akBottom]
    TabOrder = 2
    Text = 'Pause'
  end
  object OK: TButton
    Left = 544
    Top = 362
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'OK'
    Default = True
    TabOrder = 3
    OnClick = OKClick
  end
  object Data: TMemo
    Left = 8
    Top = 224
    Width = 609
    Height = 131
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
    TabOrder = 4
  end
  object CharSet: TComboBox
    Left = 392
    Top = 367
    Width = 89
    Height = 21
    Style = csDropDownList
    Anchors = [akLeft, akBottom]
    ItemHeight = 13
    TabOrder = 5
    OnChange = CharSetChange
    Items.Strings = (
      'ZX81'
      'ASCII')
  end
  object PRLE: TMemo
    Left = 8
    Top = 24
    Width = 129
    Height = 177
    Lines.Strings = (
      'PRLE')
    ScrollBars = ssVertical
    TabOrder = 6
  end
end
