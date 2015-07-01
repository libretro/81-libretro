object EditPauseForm: TEditPauseForm
  Left = 200
  Top = 123
  BorderIcons = []
  BorderStyle = bsDialog
  Caption = 'Edit Pause Block'
  ClientHeight = 64
  ClientWidth = 138
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
    Left = 16
    Top = 8
    Width = 33
    Height = 13
    Caption = 'Pause:'
  end
  object Label2: TLabel
    Left = 112
    Top = 8
    Width = 13
    Height = 13
    Caption = 'ms'
  end
  object Edit: TEdit
    Left = 56
    Top = 6
    Width = 49
    Height = 21
    TabOrder = 0
    Text = '0'
  end
  object OK: TButton
    Left = 40
    Top = 32
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    TabOrder = 1
    OnClick = OKClick
  end
end
