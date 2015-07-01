object Printer: TPrinter
  Left = 594
  Top = 127
  Anchors = [akLeft, akTop, akBottom]
  BorderIcons = [biSystemMenu]
  BorderStyle = bsToolWindow
  Caption = 'ZX Printer'
  ClientHeight = 453
  ClientWidth = 271
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
  object DisplayImage: TImage
    Left = 0
    Top = 0
    Width = 254
    Height = 265
    Anchors = [akLeft, akTop, akRight]
  end
  object ScrollBar: TScrollBar
    Left = 255
    Top = 0
    Width = 16
    Height = 265
    Anchors = [akTop, akRight]
    Kind = sbVertical
    PageSize = 0
    TabOrder = 1
    OnChange = ScrollBarChange
  end
  object SaveImage: TButton
    Left = 8
    Top = 272
    Width = 75
    Height = 25
    Caption = 'Save Image'
    TabOrder = 2
    OnClick = SaveImageClick
  end
  object ClearImage: TButton
    Left = 100
    Top = 272
    Width = 75
    Height = 25
    Caption = 'Clear Image'
    TabOrder = 0
    OnClick = ClearImageClick
  end
  object OptionsBtn: TButton
    Left = 192
    Top = 272
    Width = 75
    Height = 25
    Caption = 'Options'
    TabOrder = 3
    OnClick = OptionsBtnClick
  end
  object Panel1: TPanel
    Left = 8
    Top = 304
    Width = 257
    Height = 145
    TabOrder = 4
    object BitRateTxt: TLabel
      Left = 24
      Top = 8
      Width = 73
      Height = 13
      Caption = 'Carriage Speed'
    end
    object LineRateTxt: TLabel
      Left = 24
      Top = 64
      Width = 89
      Height = 13
      Caption = 'Paper Feed Speed'
    end
    object Label1: TLabel
      Left = 16
      Top = 124
      Width = 26
      Height = 13
      Caption = 'Style:'
    end
    object BitRate: TTrackBar
      Left = 8
      Top = 24
      Width = 241
      Height = 33
      Max = 100
      Orientation = trHorizontal
      Frequency = 5
      Position = 0
      SelEnd = 0
      SelStart = 0
      TabOrder = 0
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = BitRateChange
    end
    object LineRate: TTrackBar
      Left = 8
      Top = 80
      Width = 241
      Height = 33
      Max = 100
      Orientation = trHorizontal
      Frequency = 5
      Position = 0
      SelEnd = 0
      SelStart = 0
      TabOrder = 1
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = LineRateChange
    end
    object Style: TComboBox
      Left = 48
      Top = 120
      Width = 97
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 2
      OnChange = StyleChange
      Items.Strings = (
        'ZX Printer'
        'Alphacom 32'
        'Plain Paper')
    end
  end
  object SaveDialog: TSaveDialog
    DefaultExt = 'bmp'
    Filter = 'Windows Bitmap (BMP)|*.bmp'
    Left = 8
    Top = 8
  end
end
