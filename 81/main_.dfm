object Form1: TForm1
  Left = 194
  Top = 112
  Width = 255
  Height = 215
  Caption = 'EightyOne'
  Color = clBtnFace
  Constraints.MinHeight = 96
  Constraints.MinWidth = 128
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnDeactivate = FormDeactivate
  OnKeyDown = FormKeyDown
  OnKeyPress = FormKeyPress
  OnKeyUp = FormKeyUp
  OnMouseDown = FormMouseDown
  OnMouseUp = FormMouseUp
  OnResize = FormResize
  PixelsPerInch = 96
  TextHeight = 13
  object StatusBar1: TStatusBar
    Left = 0
    Top = 146
    Width = 247
    Height = 19
    Panels = <
      item
        Text = 'EightyOne'
        Width = 100
      end
      item
        Alignment = taCenter
        Text = '50fps'
        Width = 75
      end
      item
        Style = psOwnerDraw
        Text = '  '
        Width = 25
      end
      item
        Width = 50
      end>
    SimplePanel = False
    OnDrawPanel = StatusBar1DrawPanel
  end
  object MainMenu1: TMainMenu
    Top = 8
    object RemoveCart1: TMenuItem
      Caption = 'File'
      object InsertTape1: TMenuItem
        Caption = 'Open Tape...'
        ShortCut = 8314
        OnClick = InsertTape1Click
      end
      object CloseTape1: TMenuItem
        Caption = 'Save Tape...'
        ShortCut = 8315
        OnClick = CloseTape1Click
      end
      object NewTape1: TMenuItem
        Caption = 'New Tape...'
        OnClick = NewTape1Click
      end
      object N6: TMenuItem
        Caption = '-'
      end
      object DiskDrives1: TMenuItem
        Caption = 'Drives...'
        OnClick = DiskDrives1Click
      end
      object N4: TMenuItem
        Caption = '-'
      end
      object LoadSnapshot1: TMenuItem
        Caption = 'Load Snapshot...'
        ShortCut = 122
        OnClick = LoadSnapshot1Click
      end
      object SaveSnapshot1: TMenuItem
        Caption = 'Save Snapshot...'
        ShortCut = 123
        OnClick = SaveSnapshot1Click
      end
      object N2: TMenuItem
        Caption = '-'
      end
      object InsertDockCart1: TMenuItem
        Caption = 'Insert Cart. into TS Dock'
        OnClick = InsertDockCart1Click
      end
      object RemoveDockCart1: TMenuItem
        Caption = 'Remove Cart. from Dock'
        OnClick = RemoveDockCart1Click
      end
      object DockSpacer: TMenuItem
        Caption = '-'
      end
      object LoadMemoryBlock1: TMenuItem
        Caption = 'Load Memory Block'
        OnClick = LoadMemoryBlock1Click
      end
      object SaveMemoryBlock1: TMenuItem
        Caption = 'Save Memory Block'
        OnClick = SaveMemoryBlock1Click
      end
      object SaveScreenshot1: TMenuItem
        Caption = 'Save Screenshot'
        OnClick = SaveScreenshot1Click
      end
      object N3: TMenuItem
        Caption = '-'
      end
      object Exit1: TMenuItem
        Caption = 'Exit'
        OnClick = Exit1Click
      end
    end
    object View1: TMenuItem
      Caption = 'View'
      object Zoom1: TMenuItem
        Caption = 'Zoom'
        object N501: TMenuItem
          Caption = '50%'
          ShortCut = 16500
          OnClick = N501Click
        end
        object N1001: TMenuItem
          Caption = '100%'
          Checked = True
          ShortCut = 16496
          OnClick = N1001Click
        end
        object N2001: TMenuItem
          Caption = '200%'
          ShortCut = 16497
          OnClick = N2001Click
        end
        object N4001: TMenuItem
          Caption = '400%'
          ShortCut = 16499
          OnClick = N4001Click
        end
        object UserDefined1: TMenuItem
          Caption = 'User Defined'
          OnClick = UserDefined1Click
        end
      end
      object BorderSize1: TMenuItem
        Caption = 'Border Size'
        object None1: TMenuItem
          Caption = 'None         (256x192)'
          OnClick = None1Click
        end
        object Small1: TMenuItem
          Caption = 'Small         (264x200)'
          OnClick = Small1Click
        end
        object Normal1: TMenuItem
          Caption = 'Normal      (320x240)'
          Checked = True
          OnClick = Normal1Click
        end
        object Large1: TMenuItem
          Caption = 'Large        (400x300)'
          OnClick = Large1Click
        end
        object FullImage1: TMenuItem
          Caption = 'Full Frame (414x312)'
          OnClick = FullImage1Click
        end
      end
      object InverseVideo: TMenuItem
        Caption = 'Inverse Video'
        OnClick = InverseVideoClick
      end
      object StatusBar2: TMenuItem
        Caption = 'Status Bar'
        Checked = True
        OnClick = StatusBar2Click
      end
    end
    object Control1: TMenuItem
      Caption = 'Control'
      object ResetZX811: TMenuItem
        Caption = 'Soft Reset'
        ShortCut = 113
        OnClick = ResetZX811Click
      end
      object HardReset1: TMenuItem
        Caption = 'Hard Reset'
        ShortCut = 8305
        OnClick = HardReset1Click
      end
      object GenerateNMI1: TMenuItem
        Caption = 'NMI'
        ShortCut = 8306
        OnClick = GenerateNMI1Click
      end
      object PauseZX81: TMenuItem
        Caption = 'Pause'
        ShortCut = 114
        OnClick = PauseZX81Click
      end
      object MemotechReset: TMenuItem
        Caption = 'Memotech Reset'
        OnClick = MemotechResetClick
      end
      object RamPackWobble1: TMenuItem
        Caption = 'RamPack Wobble'
        object RPWobble1: TMenuItem
          Caption = 'Enabled'
          OnClick = RPWobble1Click
        end
        object WobbleNow1: TMenuItem
          Caption = 'Wobble Now'
          ShortCut = 16507
          OnClick = WobbleNow1Click
        end
      end
    end
    object Options1: TMenuItem
      Caption = 'Options'
      object Config1: TMenuItem
        Caption = 'Configuration'
        object SaveCurrentConfig: TMenuItem
          Caption = 'Save Current Config'
          OnClick = SaveCurrentConfigClick
        end
        object N7: TMenuItem
          Caption = '-'
        end
        object ConfigItem1: TMenuItem
          Caption = 'ConfigItem'
          OnClick = ConfigItem1Click
        end
      end
      object N5: TMenuItem
        Caption = '-'
      end
      object Tape2: TMenuItem
        Caption = 'Tape'
        object Source1: TMenuItem
          Caption = 'Source'
          object InTZXManager: TMenuItem
            Caption = 'TZX Manager'
            Checked = True
            OnClick = InTZXManagerClick
          end
          object InWaveLoader: TMenuItem
            Caption = 'Wave Loader'
            OnClick = InWaveLoaderClick
          end
          object InAudioIn: TMenuItem
            Caption = 'Audio In'
            Enabled = False
          end
        end
        object Destination1: TMenuItem
          Caption = 'Destination'
          object OutTZXManager: TMenuItem
            Caption = 'TZX Manager'
            Checked = True
            OnClick = OutTZXManagerClick
          end
          object OutWaveLoader: TMenuItem
            Caption = 'Wave Loader'
            OnClick = OutWaveLoaderClick
          end
          object OutAudioOut: TMenuItem
            Caption = 'Audio Out'
            OnClick = OutAudioOutClick
          end
        end
      end
      object Ports: TMenuItem
        Caption = 'Ports'
        object IFace1: TMenuItem
          Caption = 'Interface 1'
          OnClick = IFace1Click
        end
        object PrinterPort1: TMenuItem
          Caption = 'Printer Port'
          OnClick = PrinterPort1Click
        end
        object TS20501: TMenuItem
          Caption = 'Timex TS2050'
          OnClick = TS20501Click
        end
      end
      object Joystick1: TMenuItem
        Caption = 'Joystick...'
        Enabled = False
      end
      object Sound1: TMenuItem
        Caption = 'Beeper Sound'
        ShortCut = 115
        OnClick = Sound1Click
      end
      object FullScreenSettings1: TMenuItem
        Caption = 'Display Settings...'
        ShortCut = 116
        OnClick = FullScreenSettings1Click
      end
      object Display1: TMenuItem
        Caption = 'Hardware...'
        ShortCut = 117
        OnClick = Display1Click
      end
      object Keyboard1: TMenuItem
        Caption = 'Keyboard...'
        ShortCut = 118
        OnClick = Keyboard1Click
      end
      object Midi1: TMenuItem
        Caption = 'Sound...'
        OnClick = Midi1Click
      end
      object Speed1: TMenuItem
        Caption = 'Speed...'
        ShortCut = 119
        OnClick = Speed1Click
      end
      object DisplayArt: TMenuItem
        Caption = 'TV Emulation...'
        ShortCut = 120
        OnClick = DisplayArtClick
      end
      object Zx97Config: TMenuItem
        Caption = 'ZX97 Config...'
        ShortCut = 121
        OnClick = Zx97ConfigClick
      end
      object DBG1: TMenuItem
        Caption = 'Debug Opt 1'
        Visible = False
        OnClick = DBG1Click
      end
      object DBG2: TMenuItem
        Caption = 'Debug Opt 2'
        Visible = False
      end
    end
    object Tools1: TMenuItem
      Caption = 'Tools'
      object DebugWin: TMenuItem
        Caption = 'Debug Window...'
        ShortCut = 16501
        OnClick = DebugWinClick
      end
      object ViewPrinter: TMenuItem
        Caption = 'ZX Printer Output...'
        ShortCut = 16502
        OnClick = ViewPrinterClick
      end
      object SoundOutput1: TMenuItem
        Caption = 'Sound Output...'
        ShortCut = 16503
        OnClick = SoundOutput1Click
      end
      object TZXMan: TMenuItem
        Caption = 'Tape Manager...'
        ShortCut = 16504
        OnClick = TZXManClick
      end
      object WavLoadBtn: TMenuItem
        Caption = 'Wav Loader...'
        ShortCut = 16505
        OnClick = WavLoadBtnClick
      end
    end
    object Help1: TMenuItem
      Caption = 'Help'
      object HelpTopics2: TMenuItem
        Caption = 'Help Topics'
        OnClick = HelpTopics2Click
      end
      object KeyboardMap1: TMenuItem
        Caption = 'Keyboard Map'
        ShortCut = 112
        OnClick = KeyboardMap1Click
      end
      object N1: TMenuItem
        Caption = '-'
      end
      object AboutEightyOne1: TMenuItem
        Caption = 'About EightyOne'
        OnClick = AboutEightyOne1Click
      end
    end
  end
  object LoadSnapDialog: TOpenDialog
    Filter = '.Z81 Snapshot|*.z81|.ACE Snapshot|*.ace'
    Top = 40
  end
  object SaveSnapDialog: TSaveDialog
    OnClose = SaveSnapDialogTypeChange
    DefaultExt = 'Z81'
    Filter = '.Z81 Snapshot|*.z81'
    OnTypeChange = SaveSnapDialogTypeChange
    Left = 32
    Top = 72
  end
  object Timer2: TTimer
    Enabled = False
    OnTimer = Timer2Timer
    Left = 32
    Top = 8
  end
  object OpenTape1: TOpenDialog
    Filter = 
      'All ZX Files (TZX,WAV,TAP,T81,P,O,A83)|*.tzx;*.wav;*.csw;*.tap;*' +
      '.t81;*.p;*.o,*.a83;*.zip|Tape Files (T81, P, O,A83)|*.tap;*.t81;' +
      '*.p;*.o;*.a83|TZX Files (TZX)|*.tzx|TAP Files (TAP)|*.tap|Wave F' +
      'iles (WAV)|*.wav;*.csw|Compressed Files|*.zip'
    Left = 32
    Top = 40
  end
  object OpenDock: TOpenDialog
    Filter = 'Timex Dock Files|*.dck|Compressed Dock Files|*.zip'
    Left = 64
    Top = 40
  end
  object ImageList1: TImageList
    Height = 10
    Left = 96
    Top = 40
    Bitmap = {
      494C010102000400040010000A00FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000400000000A0000000100200000000000000A
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000940000009400000094
      0000009400000094000000940000009400000094000000940000009400000094
      0000009400000094000000940000000000000000000000F7630000F7630000F7
      630000F7630000F7630000F7630000F7630000F7630000F7630000F7630000F7
      630000F7630000F7630000F76300000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000940000009400000094
      0000009400000094000000940000009400000094000000940000009400000094
      0000009400000094000000940000000000000000000000F7630000F7630000F7
      630000F7630000F7630000F7630000F7630000F7630000F7630000F7630000F7
      630000F7630000F7630000F76300000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000940000009400000094
      0000009400000094000000940000009400000094000000940000009400000094
      0000009400000094000000940000000000000000000000F7630000F7630000F7
      630000F7630000F7630000F7630000F7630000F7630000F7630000F7630000F7
      630000F7630000F7630000F76300000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000940000009400000094
      0000009400000094000000940000009400000094000000940000009400000094
      0000009400000094000000940000000000000000000000F7630000F7630000F7
      630000F7630000F7630000F7630000F7630000F7630000F7630000F7630000F7
      630000F7630000F7630000F76300000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000940000009400000094
      0000009400000094000000940000009400000094000000940000009400000094
      0000009400000094000000940000000000000000000000F7630000F7630000F7
      630000F7630000F7630000F7630000F7630000F7630000F7630000F7630000F7
      630000F7630000F7630000F76300000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000940000009400000094
      0000009400000094000000940000009400000094000000940000009400000094
      0000009400000094000000940000000000000000000000F7630000F7630000F7
      630000F7630000F7630000F7630000F7630000F7630000F7630000F7630000F7
      630000F7630000F7630000F76300000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000940000009400000094
      0000009400000094000000940000009400000094000000940000009400000094
      0000009400000094000000940000000000000000000000F7630000F7630000F7
      630000F7630000F7630000F7630000F7630000F7630000F7630000F7630000F7
      630000F7630000F7630000F76300000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000940000009400000094
      0000009400000094000000940000009400000094000000940000009400000094
      0000009400000094000000940000000000000000000000F7630000F7630000F7
      630000F7630000F7630000F7630000F7630000F7630000F7630000F7630000F7
      630000F7630000F7630000F76300000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      28000000400000000A0000000100010000000000500000000000000000000000
      000000000000000000000000FFFFFF00FFFFFFFF000000008001800100000000
      8001800100000000800180010000000080018001000000008001800100000000
      800180010000000080018001000000008001800100000000FFFFFFFF00000000
      00000000000000000000000000000000000000000000}
  end
  object SaveConfigDialog: TSaveDialog
    DefaultExt = '.ini'
    Filter = 'Configuration Files (*.ini)|*.ini'
    Options = [ofHideReadOnly, ofNoChangeDir, ofEnableSizing]
    Top = 72
  end
  object AnimTimer1: TAnimTimer
    Enabled = False
    OnTimer = AnimTimer1Timer
    Left = 64
    Top = 8
  end
  object SaveScrDialog: TSaveDialog
    DefaultExt = 'bmp'
    Filter = 'Windows Bitmap (.bmp)|*.bmp'
    Left = 64
    Top = 72
  end
  object ThemeManager1: TThemeManager
    Left = 96
    Top = 8
  end
end
