object SerialConfig: TSerialConfig
  Left = 724
  Top = 115
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = 'TS2050 Configuration'
  ClientHeight = 186
  ClientWidth = 243
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
    Left = 42
    Top = 12
    Width = 55
    Height = 13
    Alignment = taRightJustify
    Caption = 'Connect to:'
  end
  object SerialPanel: TPanel
    Left = 16
    Top = 32
    Width = 217
    Height = 89
    BevelOuter = bvNone
    TabOrder = 3
    object Label2: TLabel
      Left = 13
      Top = 12
      Width = 68
      Height = 13
      Alignment = taRightJustify
      Caption = '/1 Baud Rate:'
    end
    object Label3: TLabel
      Left = 8
      Top = 36
      Width = 74
      Height = 13
      Alignment = taRightJustify
      Caption = '/16 Baud Rate:'
    end
    object Label4: TLabel
      Left = 8
      Top = 60
      Width = 74
      Height = 13
      Caption = '/64 Baud Rate:'
    end
    object X1Baud: TComboBox
      Left = 88
      Top = 8
      Width = 121
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 0
      Items.Strings = (
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
        '57600'
        '115200')
    end
    object X16Baud: TComboBox
      Left = 88
      Top = 32
      Width = 121
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 1
      Items.Strings = (
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
        '57600'
        '115200')
    end
    object X64Baud: TComboBox
      Left = 88
      Top = 56
      Width = 121
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 2
      Items.Strings = (
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
        '57600'
        '115200')
    end
  end
  object FilePanel: TPanel
    Left = 8
    Top = 32
    Width = 225
    Height = 81
    BevelOuter = bvNone
    TabOrder = 4
    object Label5: TLabel
      Left = 8
      Top = 12
      Width = 45
      Height = 13
      Caption = 'Filename:'
    end
    object FileNameBox: TEdit
      Left = 8
      Top = 28
      Width = 209
      Height = 21
      TabOrder = 0
      Text = 'FileNameBox'
    end
    object Browse: TButton
      Left = 142
      Top = 52
      Width = 75
      Height = 17
      Caption = 'Browse'
      TabOrder = 1
      OnClick = BrowseClick
    end
  end
  object ComPortList: TComboBox
    Left = 104
    Top = 8
    Width = 121
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 0
    OnChange = ComPortListChange
  end
  object Enabled: TCheckBox
    Left = 16
    Top = 160
    Width = 77
    Height = 17
    Alignment = taLeftJustify
    Caption = 'Connected:'
    TabOrder = 1
  end
  object OK: TButton
    Left = 152
    Top = 156
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 2
    OnClick = OKClick
  end
  object TCPPanel: TPanel
    Left = 16
    Top = 32
    Width = 209
    Height = 121
    BevelOuter = bvNone
    TabOrder = 5
    object Label6: TLabel
      Left = 8
      Top = 52
      Width = 38
      Height = 13
      Caption = 'Address'
      Enabled = False
    end
    object Label7: TLabel
      Left = 152
      Top = 52
      Width = 19
      Height = 13
      Caption = 'Port'
      Enabled = False
    end
    object TCPOutboundAddress: TEdit
      Left = 8
      Top = 28
      Width = 137
      Height = 21
      TabOrder = 0
    end
    object TCPInboundPort: TEdit
      Left = 8
      Top = 92
      Width = 201
      Height = 21
      Enabled = False
      TabOrder = 1
    end
    object ConnectTo: TRadioButton
      Left = 8
      Top = 8
      Width = 113
      Height = 17
      Caption = 'Connect to:'
      Checked = True
      TabOrder = 2
      TabStop = True
    end
    object ListenOn: TRadioButton
      Left = 8
      Top = 72
      Width = 113
      Height = 17
      Caption = 'Listen on port:'
      Enabled = False
      TabOrder = 3
    end
    object TCPOutboundPort: TEdit
      Left = 152
      Top = 28
      Width = 57
      Height = 21
      TabOrder = 4
    end
  end
  object ComPort1: TComPort
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
    OnRxChar = ComPort1RxChar
    OnTxEmpty = ComPort1TxEmpty
    OnBreak = ComPort1Break
    OnCTSChange = ComPort1CTSChange
    OnDSRChange = ComPort1DSRChange
    OnError = ComPort1Error
  end
  object BrowseDialog: TSaveDialog
    Filter = 'All Files (*.*)|*.*'
    Left = 216
  end
  object ClientSocket: TClientSocket
    Active = False
    ClientType = ctNonBlocking
    Port = 0
    OnError = ClientSocketError
    Top = 24
  end
end
