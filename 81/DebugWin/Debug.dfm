object Dbg: TDbg
  Left = 388
  Top = 113
  BorderIcons = [biSystemMenu]
  BorderStyle = bsToolWindow
  Caption = 'Debug Window'
  ClientHeight = 340
  ClientWidth = 494
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 2
    Top = 0
    Width = 305
    Height = 153
    Caption = 'Z80'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Pitch = fpFixed
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    object Label1: TLabel
      Left = 3
      Top = 56
      Width = 25
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'HL:'
    end
    object Label2: TLabel
      Left = 3
      Top = 24
      Width = 25
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'BC:'
    end
    object Label3: TLabel
      Left = 3
      Top = 40
      Width = 25
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'DE:'
    end
    object Label4: TLabel
      Left = 3
      Top = 72
      Width = 25
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'A:'
    end
    object Label5: TLabel
      Left = 3
      Top = 88
      Width = 25
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'F:'
    end
    object HL: TLabel
      Left = 32
      Top = 56
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = HLClick
    end
    object BC: TLabel
      Left = 32
      Top = 24
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = BCClick
    end
    object DE: TLabel
      Left = 32
      Top = 40
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = DEClick
    end
    object A: TLabel
      Left = 32
      Top = 72
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = AClick
    end
    object F: TLabel
      Left = 32
      Top = 88
      Width = 65
      Height = 13
      AutoSize = False
      Caption = '00000000'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = FClick
    end
    object Label6: TLabel
      Left = 110
      Top = 56
      Width = 30
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'HL'#39':'
    end
    object HL_: TLabel
      Left = 144
      Top = 56
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = HL_Click
    end
    object BC_: TLabel
      Left = 144
      Top = 24
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = BC_Click
    end
    object DE_: TLabel
      Left = 144
      Top = 40
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = DE_Click
    end
    object A_: TLabel
      Left = 144
      Top = 72
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = A_Click
    end
    object F_: TLabel
      Left = 144
      Top = 88
      Width = 65
      Height = 13
      AutoSize = False
      Caption = '00000000'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = F_Click
    end
    object Label12: TLabel
      Left = 110
      Top = 88
      Width = 30
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'F'#39':'
    end
    object Label13: TLabel
      Left = 110
      Top = 72
      Width = 30
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'A'#39':'
    end
    object Label14: TLabel
      Left = 110
      Top = 40
      Width = 30
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'DE'#39':'
    end
    object Label15: TLabel
      Left = 110
      Top = 24
      Width = 30
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'BC'#39':'
    end
    object Label16: TLabel
      Left = 224
      Top = 24
      Width = 20
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'IX:'
    end
    object IX: TLabel
      Left = 248
      Top = 24
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = IXClick
    end
    object Label18: TLabel
      Left = 224
      Top = 40
      Width = 20
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'IY:'
    end
    object IY: TLabel
      Left = 248
      Top = 40
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = IYClick
    end
    object Label20: TLabel
      Left = 224
      Top = 56
      Width = 20
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'IR:'
    end
    object IR: TLabel
      Left = 248
      Top = 56
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = IRClick
    end
    object Label22: TLabel
      Left = 224
      Top = 72
      Width = 20
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'SP:'
    end
    object SP: TLabel
      Left = 248
      Top = 72
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = SPClick
    end
    object Label24: TLabel
      Left = 224
      Top = 88
      Width = 20
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'PC:'
    end
    object PC: TLabel
      Left = 248
      Top = 88
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = PCClick
    end
    object Label7: TLabel
      Left = 222
      Top = 104
      Width = 23
      Height = 13
      AutoSize = False
      Caption = 'IM:'
    end
    object IM: TLabel
      Left = 248
      Top = 104
      Width = 17
      Height = 13
      AutoSize = False
      Caption = '0'
      OnClick = IMClick
    end
    object Label9: TLabel
      Left = 70
      Top = 104
      Width = 73
      Height = 13
      AutoSize = False
      Caption = 'Interupts:'
    end
    object Interupts: TLabel
      Left = 144
      Top = 104
      Width = 56
      Height = 14
      Caption = 'Disabled'
      OnClick = InteruptsClick
    end
    object Label11: TLabel
      Left = 4
      Top = 104
      Width = 37
      Height = 13
      AutoSize = False
      Caption = 'Halt:'
    end
    object Halt: TLabel
      Left = 40
      Top = 104
      Width = 25
      Height = 13
      AutoSize = False
      Caption = 'Yes'
    end
    object SingleStep: TButton
      Left = 64
      Top = 122
      Width = 65
      Height = 25
      Caption = 'Single Step'
      Enabled = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      OnClick = SingleStepClick
    end
    object StepOver: TButton
      Left = 144
      Top = 122
      Width = 65
      Height = 25
      Caption = 'Step Over'
      Enabled = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      TabOrder = 1
      OnClick = StepOverClick
    end
    object RunStop: TButton
      Left = 224
      Top = 122
      Width = 65
      Height = 25
      Caption = 'Stop'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      TabOrder = 2
      OnClick = RunStopClick
    end
  end
  object GroupBox2: TGroupBox
    Left = 2
    Top = 162
    Width = 225
    Height = 167
    Caption = 'Program'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    object Label8: TLabel
      Left = 8
      Top = 16
      Width = 21
      Height = 14
      Caption = 'PC:'
    end
    object Disass0: TLabel
      Left = 8
      Top = 32
      Width = 196
      Height = 14
      Caption = '0000 xxxxxxxx LD HL,(IX+$2f)'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Disass1: TLabel
      Left = 8
      Top = 48
      Width = 196
      Height = 14
      Caption = '0000 xxxxxxxx LD HL,(IX+$2f)'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Disass2: TLabel
      Left = 8
      Top = 64
      Width = 196
      Height = 14
      Caption = '0000 xxxxxxxx LD HL,(IX+$2f)'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Disass3: TLabel
      Left = 8
      Top = 80
      Width = 196
      Height = 14
      Caption = '0000 xxxxxxxx LD HL,(IX+$2f)'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Disass4: TLabel
      Left = 8
      Top = 96
      Width = 196
      Height = 14
      Caption = '0000 xxxxxxxx LD HL,(IX+$2f)'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Disass5: TLabel
      Left = 8
      Top = 112
      Width = 196
      Height = 14
      Caption = '0000 xxxxxxxx LD HL,(IX+$2f)'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Disass6: TLabel
      Left = 8
      Top = 128
      Width = 196
      Height = 14
      Caption = '0000 xxxxxxxx LD HL,(IX+$2f)'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Disass7: TLabel
      Left = 8
      Top = 144
      Width = 196
      Height = 14
      Caption = '0000 xxxxxxxx LD HL,(IX+$2f)'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object History: TButton
      Left = 64
      Top = 12
      Width = 65
      Height = 17
      Caption = 'History'
      TabOrder = 0
      OnClick = HistoryClick
    end
    object EnableHistory: TCheckBox
      Left = 136
      Top = 12
      Width = 81
      Height = 17
      Caption = 'Enable'
      TabOrder = 1
    end
  end
  object GroupBoxZX81: TGroupBox
    Left = 314
    Top = 162
    Width = 177
    Height = 167
    Caption = 'ZX81'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 2
    object Label39: TLabel
      Left = 5
      Top = 24
      Width = 105
      Height = 14
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'NMI Gen:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label10: TLabel
      Left = 5
      Top = 40
      Width = 105
      Height = 14
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'HSYNC Gen:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label19: TLabel
      Left = 5
      Top = 56
      Width = 105
      Height = 14
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'Scanline:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label21: TLabel
      Left = 5
      Top = 72
      Width = 105
      Height = 14
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'Row Counter:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label23: TLabel
      Left = 5
      Top = 88
      Width = 105
      Height = 14
      Alignment = taRightJustify
      Caption = 'Shift Register:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object NMIGen: TLabel
      Left = 115
      Top = 24
      Width = 57
      Height = 14
      AutoSize = False
      Caption = 'OFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = NMIGenClick
    end
    object Scanline: TLabel
      Left = 115
      Top = 56
      Width = 57
      Height = 14
      AutoSize = False
      Caption = '299'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object RowCount: TLabel
      Left = 115
      Top = 72
      Width = 57
      Height = 13
      AutoSize = False
      Caption = '7'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object ShiftReg: TLabel
      Left = 115
      Top = 88
      Width = 57
      Height = 13
      AutoSize = False
      Caption = '00000000'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object HSYNCGen: TLabel
      Left = 115
      Top = 40
      Width = 57
      Height = 14
      AutoSize = False
      Caption = 'OFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = HSYNCGenClick
    end
    object Label27: TLabel
      Left = 54
      Top = 104
      Width = 56
      Height = 14
      Alignment = taRightJustify
      Caption = 'TStates:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object TStates: TLabel
      Left = 115
      Top = 104
      Width = 57
      Height = 13
      AutoSize = False
      Caption = '00000000'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
  end
  object GroupBox4: TGroupBox
    Left = 314
    Top = 0
    Width = 177
    Height = 153
    Caption = 'Control'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 3
    object Label25: TLabel
      Left = 8
      Top = 16
      Width = 59
      Height = 13
      Caption = 'Breakpoints:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object Label26: TLabel
      Left = 114
      Top = 32
      Width = 34
      Height = 13
      Caption = 'Display'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object Continuous: TCheckBox
      Left = 96
      Top = 16
      Width = 73
      Height = 17
      Caption = 'Continuous'
      Checked = True
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      State = cbChecked
      TabOrder = 0
      OnClick = ContinuousClick
    end
    object AddBrkBtn: TButton
      Left = 96
      Top = 88
      Width = 73
      Height = 25
      Caption = 'Add'
      TabOrder = 1
      OnClick = AddBrkBtnClick
    end
    object DelBrkBtn: TButton
      Left = 96
      Top = 120
      Width = 73
      Height = 25
      Caption = 'Del'
      TabOrder = 2
      OnClick = DelBrkBtnClick
    end
    object BPList: TStringGrid
      Left = 8
      Top = 32
      Width = 81
      Height = 113
      ColCount = 1
      DefaultRowHeight = 14
      FixedCols = 0
      RowCount = 1
      FixedRows = 0
      Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goRowSelect]
      ScrollBars = ssVertical
      TabOrder = 3
      ColWidths = (
        77)
    end
    object SkipNMIBtn: TCheckBox
      Left = 96
      Top = 48
      Width = 73
      Height = 17
      Caption = 'Skip NMI'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 4
      OnClick = SkipNMIBtnClick
    end
    object SkipINTBtn: TCheckBox
      Left = 96
      Top = 64
      Width = 73
      Height = 17
      Caption = 'Skip INT'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 5
      OnClick = SkipINTBtnClick
    end
  end
  object GroupBox5: TGroupBox
    Left = 234
    Top = 162
    Width = 73
    Height = 167
    Caption = 'Stack'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
    object Stack0: TLabel
      Left = 19
      Top = 32
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = Stack0Click
    end
    object Stack1: TLabel
      Left = 19
      Top = 48
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack1Click
    end
    object Stack2: TLabel
      Left = 19
      Top = 64
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack2Click
    end
    object Stack3: TLabel
      Left = 19
      Top = 80
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack3Click
    end
    object Stack4: TLabel
      Left = 19
      Top = 96
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack4Click
    end
    object Stack5: TLabel
      Left = 19
      Top = 112
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack5Click
    end
    object Stack6: TLabel
      Left = 19
      Top = 128
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack6Click
    end
    object Stack7: TLabel
      Left = 19
      Top = 144
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack7Click
    end
    object Label17: TLabel
      Left = 19
      Top = 16
      Width = 21
      Height = 14
      Caption = 'SP:'
    end
  end
  object GroupBoxAce: TGroupBox
    Left = 314
    Top = 162
    Width = 177
    Height = 167
    Caption = 'Jupiter Ace'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 5
    object AceStk0: TLabel
      Left = 43
      Top = 32
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = Stack0Click
    end
    object AceStk1: TLabel
      Left = 43
      Top = 48
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack1Click
    end
    object AceStk2: TLabel
      Left = 43
      Top = 64
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack2Click
    end
    object AceStk3: TLabel
      Left = 43
      Top = 80
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack3Click
    end
    object AceStk4: TLabel
      Left = 43
      Top = 96
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack4Click
    end
    object AceStk5: TLabel
      Left = 43
      Top = 112
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack5Click
    end
    object AceStk6: TLabel
      Left = 43
      Top = 128
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack6Click
    end
    object AceStk7: TLabel
      Left = 43
      Top = 144
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack7Click
    end
    object Label36: TLabel
      Left = 8
      Top = 16
      Width = 84
      Height = 14
      Caption = 'Forth Stack:'
    end
    object Label37: TLabel
      Left = 83
      Top = 32
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = Stack0Click
    end
    object Label38: TLabel
      Left = 83
      Top = 48
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack1Click
    end
    object Label40: TLabel
      Left = 83
      Top = 64
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack2Click
    end
    object Label41: TLabel
      Left = 83
      Top = 80
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack3Click
    end
    object Label42: TLabel
      Left = 83
      Top = 96
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack4Click
    end
    object Label43: TLabel
      Left = 83
      Top = 112
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack5Click
    end
    object Label44: TLabel
      Left = 83
      Top = 128
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack6Click
    end
    object Label45: TLabel
      Left = 83
      Top = 144
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack7Click
    end
    object AceStkVal0: TLabel
      Left = 99
      Top = 32
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = Stack0Click
    end
    object AceStkVal1: TLabel
      Left = 99
      Top = 48
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack1Click
    end
    object AceStkVal2: TLabel
      Left = 99
      Top = 64
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack2Click
    end
    object AceStkVal3: TLabel
      Left = 99
      Top = 80
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack3Click
    end
    object AceStkVal4: TLabel
      Left = 99
      Top = 96
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack4Click
    end
    object AceStkVal5: TLabel
      Left = 99
      Top = 112
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack5Click
    end
    object AceStkVal6: TLabel
      Left = 99
      Top = 128
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack6Click
    end
    object AceStkVal7: TLabel
      Left = 99
      Top = 144
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = Stack7Click
    end
  end
end
