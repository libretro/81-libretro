object TZX: TTZX
  Left = 764
  Top = 110
  Width = 239
  Height = 300
  Caption = 'Tape'
  Color = clBtnFace
  Constraints.MinHeight = 200
  Constraints.MinWidth = 231
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnResize = FormResize
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel1: TBevel
    Left = 6
    Top = 34
    Width = 219
    Height = 200
    Anchors = [akLeft, akTop, akRight, akBottom]
  end
  object Table: TStringGrid
    Left = 10
    Top = 38
    Width = 192
    Height = 191
    Anchors = [akLeft, akTop, akRight, akBottom]
    BorderStyle = bsNone
    ColCount = 2
    DefaultRowHeight = 16
    FixedCols = 0
    RowCount = 2
    GridLineWidth = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRowSelect]
    PopupMenu = PopupMenu1
    ScrollBars = ssNone
    TabOrder = 0
    OnContextPopup = TableContextPopup
    OnDblClick = EditBlock1Click
    OnSelectCell = TableSelectCell
    ColWidths = (
      163
      64)
  end
  object Panel1: TPanel
    Left = 37
    Top = 241
    Width = 161
    Height = 25
    Anchors = [akBottom]
    BevelOuter = bvNone
    TabOrder = 1
    object RewEnd: TOffice97Button
      Left = 4
      Top = 0
      Width = 25
      Height = 23
      Hint = 'Rewind to Start'
      Color = 13434879
      HoverFont.Charset = DEFAULT_CHARSET
      HoverFont.Color = clWindowText
      HoverFont.Height = -11
      HoverFont.Name = 'MS Sans Serif'
      HoverFont.Style = []
      InactiveColor = clBtnFace
      InactiveGlyph.Data = {
        36050000424D3605000000000000360400002800000010000000100000000100
        08000000000000010000120B0000120B00000200000002000000FFFFFF000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000010100000000000100000000000000000101000000000101000000
        0000000000010100000001010100000000000000000101000001010101000000
        0000000000010100010101010100000000000000000101000001010101000000
        0000000000010100000001010100000000000000000101000000000101000000
        0000000000010100000000000100000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000}
      ParentShowHint = False
      ShowGlyph = True
      ShowHint = True
      TabOrder = 0
      Transparent = True
      UseCustomGlyphs = True
      OnClick = RewEndClick
    end
    object Rewind: TOffice97Button
      Left = 28
      Top = 0
      Width = 25
      Height = 23
      Hint = 'Rewind to Previous Block'
      Color = 13434879
      HoverFont.Charset = DEFAULT_CHARSET
      HoverFont.Color = clWindowText
      HoverFont.Height = -11
      HoverFont.Name = 'MS Sans Serif'
      HoverFont.Style = []
      InactiveColor = clBtnFace
      InactiveGlyph.Data = {
        36050000424D3605000000000000360400002800000010000000100000000100
        08000000000000010000120B0000120B00000200000002000000FFFFFF000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000010000000000010000000000000000010100000000010100
        0000000000000101010000000101010000000000000101010100000101010100
        0000000001010101010001010101010000000000000101010100000101010100
        0000000000000101010000000101010000000000000000010100000000010100
        0000000000000000010000000000010000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000}
      ParentShowHint = False
      ShowGlyph = True
      ShowHint = True
      TabOrder = 1
      Transparent = True
      UseCustomGlyphs = True
      OnClick = RewindClick
    end
    object StopBtn: TOffice97Button
      Left = 52
      Top = 0
      Width = 25
      Height = 23
      Hint = 'Stop'
      ActiveColor = clBlack
      Color = clBtnFace
      HoverFont.Charset = DEFAULT_CHARSET
      HoverFont.Color = clWindowText
      HoverFont.Height = -11
      HoverFont.Name = 'MS Sans Serif'
      HoverFont.Style = []
      InactiveColor = clBtnFace
      InactiveGlyph.Data = {
        36050000424D3605000000000000360400002800000010000000100000000100
        08000000000000010000120B0000120B00000200000002000000FFFFFF000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000001010101010101000000
        0000000000000101010101010100000000000000000001010101010101000000
        0000000000000101010101010100000000000000000001010101010101000000
        0000000000000101010101010100000000000000000001010101010101000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000}
      ParentShowHint = False
      ShowGlyph = True
      ShowHint = True
      TabOrder = 2
      UseCustomGlyphs = True
      Down = True
      Toggle = True
      OnClick = StopBtnClick
    end
    object PlayBtn: TOffice97Button
      Left = 76
      Top = 0
      Width = 25
      Height = 23
      Hint = 'Play'
      Color = 13434879
      HoverFont.Charset = DEFAULT_CHARSET
      HoverFont.Color = clWindowText
      HoverFont.Height = -11
      HoverFont.Name = 'MS Sans Serif'
      HoverFont.Style = []
      InactiveColor = clBtnFace
      InactiveGlyph.Data = {
        36050000424D3605000000000000360400002800000010000000100000000100
        08000000000000010000120B0000120B00000200000002000000FFFFFF000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000010100000000000000000000000000000101010100000000000000
        0000000000010101010101000000000000000000000101010101010101000000
        0000000000010101010101010101010000000000000101010101010101000000
        0000000000010101010101000000000000000000000101010100000000000000
        0000000000010100000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000}
      ParentShowHint = False
      ShowGlyph = True
      ShowHint = True
      TabOrder = 3
      Transparent = True
      UseCustomGlyphs = True
      Toggle = True
      OnClick = PlayBtnClick
    end
    object FForward: TOffice97Button
      Left = 100
      Top = 0
      Width = 25
      Height = 23
      Hint = 'Fast Forward to Next Block'
      Color = 13434879
      HoverFont.Charset = DEFAULT_CHARSET
      HoverFont.Color = clWindowText
      HoverFont.Height = -11
      HoverFont.Name = 'MS Sans Serif'
      HoverFont.Style = []
      InactiveColor = clBtnFace
      InactiveGlyph.Data = {
        36050000424D3605000000000000360400002800000010000000100000000100
        08000000000000010000120B0000120B00000200000002000000FFFFFF000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000010000000000010000000000000000000101000000000101000000
        0000000000010101000000010101000000000000000101010100000101010100
        0000000000010101010100010101010100000000000101010100000101010100
        0000000000010101000000010101000000000000000101000000000101000000
        0000000000010000000000010000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000}
      ParentShowHint = False
      ShowGlyph = True
      ShowHint = True
      TabOrder = 4
      Transparent = True
      UseCustomGlyphs = True
      OnClick = FForwardClick
    end
    object FFEnd: TOffice97Button
      Left = 124
      Top = 0
      Width = 25
      Height = 23
      Hint = 'Fast Forward to End'
      Color = 13434879
      HoverFont.Charset = DEFAULT_CHARSET
      HoverFont.Color = clWindowText
      HoverFont.Height = -11
      HoverFont.Name = 'MS Sans Serif'
      HoverFont.Style = []
      InactiveColor = clBtnFace
      InactiveGlyph.Data = {
        36050000424D3605000000000000360400002800000010000000100000000100
        08000000000000010000120B0000120B00000200000002000000FFFFFF000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000001000000000001010000000000000000010100000000010100
        0000000000000001010100000001010000000000000000010101010000010100
        0000000000000001010101010001010000000000000000010101010000010100
        0000000000000001010100000001010000000000000000010100000000010100
        0000000000000001000000000001010000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000}
      ParentShowHint = False
      ShowGlyph = True
      ShowHint = True
      TabOrder = 5
      Transparent = True
      UseCustomGlyphs = True
      OnClick = FFEndClick
    end
  end
  object ScrollBar1: TScrollBar
    Left = 205
    Top = 39
    Width = 16
    Height = 190
    Anchors = [akTop, akRight, akBottom]
    Kind = sbVertical
    PageSize = 0
    Position = 1
    TabOrder = 2
    OnChange = ScrollBar1Change
  end
  object Panel2: TPanel
    Left = 8
    Top = 4
    Width = 217
    Height = 25
    Anchors = [akLeft, akTop, akRight]
    BevelOuter = bvNone
    TabOrder = 3
    object AutoStartBtn: TOffice97Button
      Left = 152
      Top = 0
      Width = 25
      Height = 25
      Hint = 'Auto Start/Stop'
      Anchors = [akRight]
      Color = clBtnFace
      HoverFont.Charset = DEFAULT_CHARSET
      HoverFont.Color = clWindowText
      HoverFont.Height = -11
      HoverFont.Name = 'MS Sans Serif'
      HoverFont.Style = []
      InactiveColor = clBtnFace
      InactiveGlyph.Data = {
        32010000424D320100000000000042000000280000000F0000000F0000000100
        080000000000F0000000120B0000120B00000300000003000000FFFFFF00E9E9
        E900000000000101010101010101010101010101010001020201010101010101
        0101010101000102020202010101010101010101010001020202020202010101
        0101010101000102020202020202020101010101010001020202020202020202
        0201010101000102020202020202020202020201010001020202020202000202
        0201010101000102000202020002000202020102010001020200020002000200
        0000020201000102020200020002000000000002010001020201020002000000
        0000000201000101010101020000000000000202010001010101010102020202
        02020102010001010101010101010101010101010100}
      ParentShowHint = False
      ShowGlyph = True
      ShowHint = True
      TabOrder = 0
      UseCustomGlyphs = True
      Down = True
      Toggle = True
      OnClick = AutoStartStopClick
    end
    object AutoLoadBtn: TOffice97Button
      Left = 176
      Top = 0
      Width = 41
      Height = 25
      Hint = 'Auto LOAD in Insert'
      Anchors = [akRight]
      Color = clBtnFace
      HoverFont.Charset = DEFAULT_CHARSET
      HoverFont.Color = clWindowText
      HoverFont.Height = -11
      HoverFont.Name = 'MS Sans Serif'
      HoverFont.Style = []
      InactiveColor = clBtnFace
      InactiveGlyph.Data = {
        4A010000424D4A010000000000004E000000280000001A000000090000000100
        080000000000FC000000120B0000120B00000600000006000000CECBCE00CDCD
        CF0086868700FFFFFF0063636300000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000005040402000000000500050505000000000000000000000000000000
        0503030100000000050005000005000000000000000000000000000005030402
        0000020405000500000005000000000000000000000000000503040200000000
        0500050000000500000000000000000000000000050304020000000005000500
        0005000000000500050000000500050005030402000002040000050505000000
        0000050005000000050005000503040200000000000000000000000000000000
        0000000000000000050404020000}
      ParentShowHint = False
      Position = bsBottom
      ShowGlyph = True
      ShowHint = True
      TabOrder = 1
      UseCustomGlyphs = True
      Down = True
      Toggle = True
      OnClick = AutoLoadonInsert1Click
    end
    object FlashLoadBtn: TOffice97Button
      Left = 128
      Top = 0
      Width = 25
      Height = 25
      Hint = 'Flash Load'
      Anchors = [akRight]
      Color = 13434879
      HoverFont.Charset = DEFAULT_CHARSET
      HoverFont.Color = clWindowText
      HoverFont.Height = -11
      HoverFont.Name = 'MS Sans Serif'
      HoverFont.Style = []
      InactiveColor = clBtnFace
      InactiveGlyph.Data = {
        FA000000424DFA0000000000000046000000280000000B0000000F0000000100
        080000000000B4000000120B0000120B0000040000000400000000FFFF00018A
        8A00FFFFFF000000000002030302020202020202020002030103020202020202
        0200020300010302020202020200020300000103020202020200020203000001
        0302020202000202030000000103020202000202020300000001030202000203
        0303000303030303020002030000000302020202020002030000000103020202
        0200020203000000010302020200020203000000000103020200020202030000
        000001030200020202030000000000010300020202020303030303030300}
      ParentShowHint = False
      ShowGlyph = True
      ShowHint = True
      TabOrder = 2
      Transparent = True
      TransparentColor = clWhite
      UseCustomGlyphs = True
      Down = True
      Toggle = True
      OnClick = FlashLoad1Click
    end
    object NewBtn: TOffice97Button
      Left = 0
      Top = 0
      Width = 25
      Height = 25
      Hint = 'New Tape'
      Anchors = [akLeft]
      Color = clBtnFace
      HoverFont.Charset = DEFAULT_CHARSET
      HoverFont.Color = clWindowText
      HoverFont.Height = -11
      HoverFont.Name = 'MS Sans Serif'
      HoverFont.Style = []
      InactiveColor = clBtnFace
      InactiveGlyph.Data = {
        22010000424D220100000000000042000000280000000D0000000E0000000100
        080000000000E0000000120B0000120B00000300000003000000FFFFFF00C0C0
        C000000000000101010101010101010101010100000001020202020202020202
        0202010000000102000000000000000000020100000001020000000000000000
        0002010000000102000000000000000000020100000001020000000000000000
        0002010000000102000000000000000000020100000001020000000000000000
        0002010000000102000000000000000000020100000001020000000000000000
        0002010000000102000000000000020202020100000001020000000000000200
        0201010000000102000000000000020201010100000001020202020202020201
        010101000000}
      ParentShowHint = False
      Position = bsTop
      ShowGlyph = True
      ShowHint = True
      TabOrder = 3
      UseCustomGlyphs = True
      OnClick = NewTZXClick
    end
    object OpenBtn: TOffice97Button
      Left = 24
      Top = 0
      Width = 25
      Height = 25
      Hint = 'Open Tape File'
      Anchors = [akLeft]
      Color = clBtnFace
      HoverFont.Charset = DEFAULT_CHARSET
      HoverFont.Color = clWindowText
      HoverFont.Height = -11
      HoverFont.Name = 'MS Sans Serif'
      HoverFont.Style = []
      InactiveColor = clBtnFace
      InactiveGlyph.Data = {
        5E010000424D5E010000000000004600000028000000110000000E0000000100
        08000000000018010000120B0000120B00000400000004000000FFFFFF00C0C0
        C000808080000000000001010101010101010101010101010101010000000103
        0303030303030303030301010101010000000103030202020202020202020301
        0101010000000103000302020303020202020203010101000000010300000302
        0202020202020202030101000000010300000003020202020202020202030100
        0000010300000000030303030303030303030100000001030000000000000000
        0003010101010100000001030000000000000000000301010101010000000103
        0000000303030303030301010101010000000101030303010101010101010103
        0303010000000101010101010101010101010101030301000000010101010101
        0101010301010103010301000000010101010101010101010303030101010100
        0000}
      ParentShowHint = False
      Position = bsTop
      ShowGlyph = True
      ShowHint = True
      TabOrder = 4
      UseCustomGlyphs = True
      OnClick = Open1Click
    end
    object SaveBtn: TOffice97Button
      Left = 48
      Top = 0
      Width = 25
      Height = 25
      Hint = 'Save Tape File'
      Anchors = [akLeft]
      Color = clBtnFace
      HoverFont.Charset = DEFAULT_CHARSET
      HoverFont.Color = clWindowText
      HoverFont.Height = -11
      HoverFont.Name = 'MS Sans Serif'
      HoverFont.Style = []
      InactiveColor = clBtnFace
      InactiveGlyph.Data = {
        32010000424D32010000000000004200000028000000100000000F0000000100
        080000000000F0000000120B0000120B00000300000003000000C0C0C0008080
        8000000000000000000000000000000000000000000000000202020202020202
        0202020202000002010102020202020200000201020000020101020202020202
        0000020102000002010102020202020200000201020000020101020202020202
        0202020102000002010101010101010101010101020000020101020202020202
        0202010102000002010200000000000000000201020000020102000000000000
        0000020102000002010200000000000000000201020000020102000000000000
        0000020102000002010200000000000000000202020000020102000000000000
        00000200020000020202020202020202020202020200}
      ParentShowHint = False
      Position = bsTop
      ShowGlyph = True
      ShowHint = True
      TabOrder = 5
      UseCustomGlyphs = True
      OnClick = SaveAs1Click
    end
    object SoundOn: TOffice97Button
      Left = 102
      Top = 0
      Width = 27
      Height = 25
      Hint = 'Loading Sounds'
      Anchors = [akTop, akRight]
      Color = 13434879
      DownActiveGlyph.Data = {
        BA010000424DBA01000000000000520000002800000012000000120000000100
        08000000000068010000120B0000120B0000070000000700000000FFFF000082
        82009CAAAD00B0B7B900FAFAFB00FFFFFF000000000005050505050505050505
        0505050505050505000005050505050505050606050505050505050500000505
        0505050505060106050505050505050500000505050505050601060505030505
        0503050500000505050505060103030505030505030505050000050505050601
        0006050505030503050505050000050506060100040605050503050505050505
        0000050601020004000606050503050505050505000005010004040004060406
        0503050303030305000005010004000400060306050305050505050500000501
        0404040004060605050305050505050500000505010100040006050505030503
        0505050500000505050501000406050505030505030505050000050505050501
        0003030505030505050305050000050505050505010006050505050505050505
        0000050505050505050103060505050505050505000005050505050505050606
        050505050505050500000505050505050505050505050505050505050000}
      HoverFont.Charset = DEFAULT_CHARSET
      HoverFont.Color = clWindowText
      HoverFont.Height = -11
      HoverFont.Name = 'MS Sans Serif'
      HoverFont.Style = []
      InactiveColor = clBtnFace
      InactiveGlyph.Data = {
        BA010000424DBA01000000000000520000002800000012000000120000000100
        08000000000068010000120B0000120B0000070000000700000000FFFF000082
        82009CAAAD00B0B7B900FAFAFB00FFFFFF000000000005050505050505050505
        0505050505050505000005050505050505050606050505050505050500000505
        0505050505060106050505050505050500000505050505050601060505050505
        0505050500000505050505060103030505050505050505050000050505050601
        0006050505050505050505050000050506060100040605050505050505050505
        0000050601020004000606050505050505050505000005010004040004060406
        0505050505050505000005010004000400060306050505050505050500000501
        0404040004060605050505050505050500000505010100040006050505050505
        0505050500000505050501000406050505050505050505050000050505050501
        0003030505050505050505050000050505050505010006050505050505050505
        0000050505050505050103060505050505050505000005050505050505050606
        050505050505050500000505050505050505050505050505050505050000}
      ParentShowHint = False
      ShowGlyph = True
      ShowHint = True
      TabOrder = 6
      Transparent = True
      UseCustomGlyphs = True
      Down = True
      Toggle = True
      OnClick = SoundOnClick
    end
  end
  object OpenDialog: TOpenDialog
    DefaultExt = 'tzx'
    Filter = 
      'All Tape Files|*.tzx;*.tap;*.t81;*.p;*.o;*.a83;*.zip|Spectrum Fi' +
      'les|*.tzx;*.tap|ZX81 Files|*.t81;*.p|ZX80 Files|*.o|Lambda Files' +
      '|*.a83|Compresses Files|*.zip'
    Options = [ofHideReadOnly, ofAllowMultiSelect, ofEnableSizing]
    Left = 104
    Top = 120
  end
  object SaveDialog: TSaveDialog
    DefaultExt = 'tzx'
    Filter = '.TZX File|*.tzx'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableIncludeNotify, ofEnableSizing]
    OnTypeChange = SaveDialogTypeChange
    Left = 72
    Top = 120
  end
  object PopupMenu1: TPopupMenu
    Left = 40
    Top = 120
    object EditBlock1: TMenuItem
      Caption = 'Edit...'
      OnClick = EditBlock1Click
    end
    object N4: TMenuItem
      Caption = '-'
    end
    object InsertBlock1: TMenuItem
      Caption = 'Insert'
      object Information1: TMenuItem
        Caption = 'Information'
        object ArchiveInfo1: TMenuItem
          Caption = 'Archive Information'
          OnClick = ArchiveInfo1Click
        end
        object HardwareInformation1: TMenuItem
          Caption = 'Hardware Information'
          OnClick = HardwareInformation1Click
        end
        object Message1: TMenuItem
          Caption = 'Message'
          Enabled = False
        end
        object TextDescription1: TMenuItem
          Caption = 'Text Description'
          OnClick = TextDescription1Click
        end
      end
      object Group1: TMenuItem
        Caption = 'Group'
        object GroupStart1: TMenuItem
          Caption = 'Group Start'
          OnClick = GroupStart1Click
        end
        object GroupEnd1: TMenuItem
          Caption = 'Group End'
          OnClick = GroupEnd1Click
        end
      end
      object Loop1: TMenuItem
        Caption = 'Loop'
        Enabled = False
        object LoopStart1: TMenuItem
          Caption = 'Loop Start'
        end
        object LoopEnd1: TMenuItem
          Caption = 'Loop End'
        end
      end
      object Data1: TMenuItem
        Caption = 'Data'
        OnClick = Data1Click
      end
      object Pause1: TMenuItem
        Caption = 'Pause'
        OnClick = Pause1Click
      end
    end
    object ExtractBlock1: TMenuItem
      Caption = 'Extract'
      OnClick = ExtractBlock1Click
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object MoveBlockUp1: TMenuItem
      Caption = 'Move Up'
      OnClick = MoveBlockUp1Click
    end
    object MoveBlockDown1: TMenuItem
      Caption = 'Move Down'
      OnClick = MoveBlockDown1Click
    end
    object N3: TMenuItem
      Caption = '-'
    end
    object DeleteBlock1: TMenuItem
      Caption = 'Delete'
      OnClick = DeleteBlock1Click
    end
    object N10: TMenuItem
      Caption = '-'
    end
    object ConvertBlocktoWave1: TMenuItem
      Caption = 'Convert Block to Wave'
      Enabled = False
      OnClick = ConvertBlocktoWave1Click
    end
    object ConvertTapetoWave1: TMenuItem
      Caption = 'Convert Tape to Wave'
      OnClick = ConvertTapetoWave1Click
    end
  end
  object ExtractDialog1: TSaveDialog
    DefaultExt = '.p'
    Filter = 'ZX81 Files (.p)|*.p|ZX80 Files (.o)|*.o'
    Left = 72
    Top = 88
  end
end
