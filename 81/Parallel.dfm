object ParallelPort: TParallelPort
  Left = 400
  Top = 101
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Printer Port'
  ClientHeight = 152
  ClientWidth = 230
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
    Left = 14
    Top = 20
    Width = 59
    Height = 13
    Alignment = taRightJustify
    Caption = 'Connect To:'
  end
  object ComPortList: TComboBox
    Left = 80
    Top = 16
    Width = 137
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 0
    OnChange = ComPortListChange
  end
  object FilePanel: TPanel
    Left = 12
    Top = 40
    Width = 209
    Height = 73
    BevelOuter = bvNone
    TabOrder = 1
    object Label3: TLabel
      Left = 7
      Top = 32
      Width = 54
      Height = 13
      Alignment = taRightJustify
      Caption = 'Output File:'
    end
    object OutputFileEdit: TEdit
      Left = 68
      Top = 28
      Width = 113
      Height = 21
      TabOrder = 0
      Text = 'c:\parallel.txt'
    end
    object OutputFileButton: TButton
      Left = 186
      Top = 28
      Width = 17
      Height = 21
      Caption = '...'
      TabOrder = 1
    end
  end
  object TCPPanel: TPanel
    Left = 12
    Top = 40
    Width = 209
    Height = 77
    BevelOuter = bvNone
    TabOrder = 2
    object Label9: TLabel
      Left = 4
      Top = 12
      Width = 56
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'Address:'
    end
    object Label10: TLabel
      Left = 4
      Top = 44
      Width = 56
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'Port:'
    end
    object TCPAddress: TEdit
      Left = 68
      Top = 8
      Width = 133
      Height = 21
      TabOrder = 0
    end
    object TCPPort: TEdit
      Left = 68
      Top = 40
      Width = 45
      Height = 21
      TabOrder = 1
    end
  end
  object PortPanel: TPanel
    Left = 12
    Top = 40
    Width = 209
    Height = 77
    BevelOuter = bvNone
    TabOrder = 3
    object Label5: TLabel
      Left = 4
      Top = 8
      Width = 56
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'Baud Rate:'
    end
    object Label6: TLabel
      Left = 4
      Top = 32
      Width = 56
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'Data Bits:'
    end
    object Label7: TLabel
      Left = 112
      Top = 32
      Width = 45
      Height = 13
      Caption = 'Stop Bits:'
    end
    object Label8: TLabel
      Left = 4
      Top = 56
      Width = 56
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'Parity:'
    end
    object BaudRate: TComboBox
      Left = 68
      Top = 4
      Width = 137
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 0
      Items.Strings = (
        '50'
        '110'
        '300'
        '600'
        '1200'
        '2400'
        '4800'
        '9600'
        '14400'
        '19200'
        '38400'
        '56000'
        '115200')
    end
    object DataBits: TComboBox
      Left = 68
      Top = 28
      Width = 41
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 1
      Items.Strings = (
        '5'
        '6'
        '7'
        '8')
    end
    object StopBits: TComboBox
      Left = 164
      Top = 28
      Width = 41
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 2
      Items.Strings = (
        '1'
        '1.5'
        '2')
    end
    object Parity: TComboBox
      Left = 68
      Top = 52
      Width = 137
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 3
      Items.Strings = (
        'None'
        'Odd'
        'Even'
        'Mark'
        'Space')
    end
  end
  object OK: TButton
    Left = 144
    Top = 120
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 4
    OnClick = OKClick
  end
  object ComPort: TComPort
    BaudRate = br9600
    Port = 'COM1'
    Parity.Bits = prNone
    StopBits = sbOneStopBit
    DataBits = dbEight
    Events = [evRxChar, evTxEmpty, evRxFlag, evRing, evBreak, evCTS, evDSR, evError, evRLSD, evRx80Full]
    FlowControl.OutCTSFlow = False
    FlowControl.OutDSRFlow = False
    FlowControl.ControlDTR = dtrDisable
    FlowControl.ControlRTS = rtsDisable
    FlowControl.XonXoffOut = False
    FlowControl.XonXoffIn = False
    Top = 120
  end
  object ClientSocket: TClientSocket
    Active = False
    ClientType = ctNonBlocking
    Port = 0
    Left = 32
    Top = 120
  end
  object SaveDialog: TSaveDialog
    Filter = 'All Files|*.*'
    Left = 64
    Top = 120
  end
end
