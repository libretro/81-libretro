object IF1: TIF1
  Left = 386
  Top = 111
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Interface 1'
  ClientHeight = 168
  ClientWidth = 424
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object GroupSerial: TGroupBox
    Left = 8
    Top = 8
    Width = 217
    Height = 121
    Caption = 'Serial Port'
    TabOrder = 0
    object Label1: TLabel
      Left = 6
      Top = 20
      Width = 59
      Height = 13
      Alignment = taRightJustify
      Caption = 'Connect To:'
    end
    object PortPanel: TPanel
      Left = 4
      Top = 40
      Width = 209
      Height = 77
      BevelOuter = bvNone
      TabOrder = 2
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
          'Auto'
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
    object TCPPanel: TPanel
      Left = 4
      Top = 40
      Width = 209
      Height = 77
      BevelOuter = bvNone
      TabOrder = 3
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
    object FilePanel: TPanel
      Left = 4
      Top = 40
      Width = 209
      Height = 73
      BevelOuter = bvNone
      TabOrder = 1
      object Label2: TLabel
        Left = 15
        Top = 8
        Width = 46
        Height = 13
        Alignment = taRightJustify
        Caption = 'Input File:'
      end
      object Label3: TLabel
        Left = 7
        Top = 32
        Width = 54
        Height = 13
        Alignment = taRightJustify
        Caption = 'Output File:'
      end
      object InputFileEdit: TEdit
        Left = 68
        Top = 4
        Width = 113
        Height = 21
        TabOrder = 0
        Text = 'c:\if1_in.txt'
      end
      object InputFileBrowse: TButton
        Left = 186
        Top = 4
        Width = 17
        Height = 21
        Caption = '...'
        TabOrder = 1
        OnClick = InputFileBrowseClick
      end
      object OutputFileEdit: TEdit
        Left = 68
        Top = 28
        Width = 113
        Height = 21
        TabOrder = 2
        Text = 'c:\if1_out.txt'
      end
      object OutputFileButton: TButton
        Left = 186
        Top = 28
        Width = 17
        Height = 21
        Caption = '...'
        TabOrder = 3
        OnClick = OutputFileButtonClick
      end
    end
    object ComPortList: TComboBox
      Left = 72
      Top = 16
      Width = 137
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 0
      OnChange = ComPortListChange
    end
  end
  object OK: TButton
    Left = 342
    Top = 136
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 1
    OnClick = OKClick
  end
  object GroupNetwork: TGroupBox
    Left = 232
    Top = 8
    Width = 185
    Height = 73
    Caption = 'Network'
    TabOrder = 2
  end
  object GroupDrives: TGroupBox
    Left = 232
    Top = 88
    Width = 185
    Height = 41
    Caption = 'Microdrives'
    TabOrder = 3
    object Label4: TLabel
      Left = 8
      Top = 16
      Width = 85
      Height = 13
      Caption = 'Number of Drives:'
    end
    object NoMicroDrives: TComboBox
      Left = 98
      Top = 12
      Width = 81
      Height = 21
      Style = csDropDownList
      DropDownCount = 9
      ItemHeight = 13
      TabOrder = 0
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8')
    end
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
    OnRxChar = ComPortRxChar
    OnTxEmpty = ComPortTxEmpty
    Left = 8
    Top = 136
  end
  object ClientSocket: TClientSocket
    Active = False
    ClientType = ctNonBlocking
    Port = 0
    OnError = ClientSocketError
    Left = 40
    Top = 136
  end
  object OpenDialog: TOpenDialog
    Filter = 'All Files|*.*'
    Options = [ofHideReadOnly, ofFileMustExist, ofEnableSizing]
    Left = 80
    Top = 136
  end
  object SaveDialog: TSaveDialog
    Filter = 'All Files|*.*'
    Left = 112
    Top = 136
  end
end
