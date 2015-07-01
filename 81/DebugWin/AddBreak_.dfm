object NewBreakForm: TNewBreakForm
  Left = 417
  Top = 141
  BorderStyle = bsDialog
  Caption = 'New Breakpoint'
  ClientHeight = 71
  ClientWidth = 180
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
    Top = 10
    Width = 41
    Height = 13
    Caption = 'Address:'
  end
  object Value: TEdit
    Left = 56
    Top = 8
    Width = 113
    Height = 21
    TabOrder = 0
    Text = 'Value'
  end
  object OKBtn: TButton
    Left = 8
    Top = 40
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    TabOrder = 1
    OnClick = OKBtnClick
  end
  object CancelBtn: TButton
    Left = 96
    Top = 40
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 2
    OnClick = CancelBtnClick
  end
end
