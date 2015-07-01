object Speed: TSpeed
  Left = 268
  Top = 165
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = 'Speed'
  ClientHeight = 112
  ClientWidth = 217
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object AccuratePanel: TPanel
    Left = 0
    Top = 0
    Width = 209
    Height = 73
    BevelOuter = bvNone
    TabOrder = 1
    object Label9: TLabel
      Left = 8
      Top = 4
      Width = 34
      Height = 13
      Caption = 'Speed:'
    end
    object Label10: TLabel
      Left = 100
      Top = 48
      Width = 17
      Height = 13
      Caption = '16x'
    end
    object Label11: TLabel
      Left = 16
      Top = 48
      Width = 11
      Height = 13
      Caption = '1x'
    end
    object Label12: TLabel
      Left = 184
      Top = 48
      Width = 17
      Height = 13
      Caption = '32x'
    end
    object AccurateSpeed: TTrackBar
      Left = 8
      Top = 16
      Width = 201
      Height = 25
      Max = 100
      Orientation = trHorizontal
      Frequency = 17
      Position = 0
      SelEnd = 0
      SelStart = 0
      TabOrder = 0
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = Recalc
    end
  end
  object Button1: TButton
    Left = 136
    Top = 80
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    TabOrder = 0
    OnClick = Button1Click
  end
  object EnableFrameSkip: TCheckBox
    Left = 8
    Top = 84
    Width = 113
    Height = 17
    Caption = 'Enable Frame Skip'
    TabOrder = 2
    OnClick = EnableFrameSkipClick
  end
end
