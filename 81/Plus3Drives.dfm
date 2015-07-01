object P3Drive: TP3Drive
  Left = 692
  Top = 58
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsDialog
  Caption = 'Drives'
  ClientHeight = 576
  ClientWidth = 336
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
  object OK: TButton
    Left = 254
    Top = 544
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'OK'
    TabOrder = 0
    OnClick = OKClick
  end
  object FloppyGroup: TGroupBox
    Left = 8
    Top = 8
    Width = 321
    Height = 73
    Caption = 'Floppy Drives'
    TabOrder = 1
    object DriveALabel: TLabel
      Left = 8
      Top = 20
      Width = 38
      Height = 13
      Alignment = taRightJustify
      Caption = 'Drive A:'
    end
    object DriveBLabel: TLabel
      Left = 8
      Top = 44
      Width = 38
      Height = 13
      Alignment = taRightJustify
      Caption = 'Drive B:'
    end
    object DriveAText: TEdit
      Left = 56
      Top = 16
      Width = 185
      Height = 21
      Enabled = False
      ParentShowHint = False
      ReadOnly = True
      ShowHint = True
      TabOrder = 0
      Text = '< Empty >'
    end
    object DriveBText: TEdit
      Left = 56
      Top = 40
      Width = 185
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 1
      Text = '< Empty >'
    end
    object DriveAFSBtn: TButton
      Left = 244
      Top = 16
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 2
      OnClick = DriveAFSBtnClick
    end
    object DriveBFSBtn: TButton
      Left = 244
      Top = 40
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 3
      OnClick = DriveBFSBtnClick
    end
    object DriveAEjectBtn: TButton
      Left = 272
      Top = 16
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 4
      OnClick = DriveAEjectBtnClick
    end
    object DriveBEjectBtn: TButton
      Left = 272
      Top = 40
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 5
      OnClick = DriveBEjectBtnClick
    end
  end
  object HardGroup: TGroupBox
    Left = 8
    Top = 88
    Width = 321
    Height = 225
    Caption = 'Hard Drives'
    TabOrder = 2
    object HD1Label: TLabel
      Left = 18
      Top = 116
      Width = 28
      Height = 13
      Alignment = taRightJustify
      Caption = 'HD 1:'
    end
    object HD1CHS: TLabel
      Left = 12
      Top = 172
      Width = 35
      Height = 13
      Caption = 'C/H/S:'
    end
    object HD0Label: TLabel
      Left = 18
      Top = 28
      Width = 28
      Height = 13
      Alignment = taRightJustify
      Caption = 'HD 0:'
    end
    object HD0CHS: TLabel
      Left = 12
      Top = 84
      Width = 35
      Height = 13
      Caption = 'C/H/S:'
    end
    object HD1Text: TEdit
      Left = 56
      Top = 140
      Width = 185
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 0
      Text = '< Empty >'
    end
    object HD1FSBtn: TButton
      Left = 244
      Top = 138
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 1
      OnClick = HD1FSBtnClick
    end
    object HD1EjectBtn: TButton
      Left = 272
      Top = 138
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 2
      OnClick = HD1EjectBtnClick
    end
    object RedetectDrives: TButton
      Left = 176
      Top = 200
      Width = 137
      Height = 17
      Caption = 'Redetect Physical Drives'
      TabOrder = 3
      OnClick = RedetectDrivesClick
    end
    object HD1List: TComboBox
      Left = 56
      Top = 112
      Width = 145
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 4
      OnChange = HD1ListChange
    end
    object HD1C: TEdit
      Left = 56
      Top = 172
      Width = 41
      Height = 21
      Enabled = False
      TabOrder = 5
      Text = '0'
    end
    object HD1H: TEdit
      Left = 104
      Top = 172
      Width = 41
      Height = 21
      Enabled = False
      TabOrder = 6
      Text = '0'
    end
    object HD1HUD: TUpDown
      Left = 146
      Top = 172
      Width = 25
      Height = 21
      Min = 0
      Orientation = udHorizontal
      Position = 0
      TabOrder = 7
      Wrap = False
      OnClick = HD1HUDClick
    end
    object HD1S: TEdit
      Left = 176
      Top = 172
      Width = 41
      Height = 21
      Enabled = False
      TabOrder = 8
      Text = '0'
    end
    object HD1SUD: TUpDown
      Left = 218
      Top = 172
      Width = 25
      Height = 21
      Min = 0
      Orientation = udHorizontal
      Position = 0
      TabOrder = 9
      Wrap = False
      OnClick = HD1HUDClick
    end
    object HD0List: TComboBox
      Left = 56
      Top = 24
      Width = 145
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 10
      OnChange = HD0ListChange
    end
    object HD0Text: TEdit
      Left = 56
      Top = 52
      Width = 185
      Height = 21
      Enabled = False
      ParentShowHint = False
      ReadOnly = True
      ShowHint = True
      TabOrder = 11
      Text = '< Empty >'
    end
    object HD0FSBtn: TButton
      Left = 244
      Top = 50
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 12
      OnClick = HD0FSBtnClick
    end
    object HD0EjectBtn: TButton
      Left = 272
      Top = 50
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 13
      OnClick = HD0EjectBtnClick
    end
    object HD0C: TEdit
      Left = 56
      Top = 80
      Width = 41
      Height = 21
      Enabled = False
      TabOrder = 14
      Text = '0'
    end
    object HD0H: TEdit
      Left = 104
      Top = 80
      Width = 41
      Height = 21
      Enabled = False
      TabOrder = 15
      Text = '0'
    end
    object HD0HUD: TUpDown
      Left = 146
      Top = 80
      Width = 25
      Height = 21
      Min = 0
      Orientation = udHorizontal
      Position = 0
      TabOrder = 16
      Wrap = False
      OnClick = HD0HUDClick
    end
    object HD0S: TEdit
      Left = 176
      Top = 80
      Width = 41
      Height = 21
      Enabled = False
      TabOrder = 17
      Text = '0'
    end
    object HD0SUD: TUpDown
      Left = 218
      Top = 80
      Width = 25
      Height = 21
      Min = 0
      Orientation = udHorizontal
      Position = 0
      TabOrder = 18
      Wrap = False
      OnClick = HD0HUDClick
    end
    object HD0ReadOnly: TCheckBox
      Left = 228
      Top = 24
      Width = 73
      Height = 17
      Caption = 'Read Only'
      TabOrder = 19
      OnClick = HD0ReadOnlyClick
    end
    object HD1ReadOnly: TCheckBox
      Left = 228
      Top = 112
      Width = 77
      Height = 17
      Caption = 'Read Only'
      TabOrder = 20
      OnClick = HD1ReadOnlyClick
    end
  end
  object MicroGroup: TGroupBox
    Left = 8
    Top = 320
    Width = 321
    Height = 217
    Caption = 'Microdrives'
    TabOrder = 3
    object MDV0Label: TLabel
      Left = 9
      Top = 20
      Width = 37
      Height = 13
      Alignment = taRightJustify
      Caption = 'Drive 1:'
    end
    object MDV1Label: TLabel
      Left = 9
      Top = 44
      Width = 37
      Height = 13
      Alignment = taRightJustify
      Caption = 'Drive 2:'
    end
    object MDV3Label: TLabel
      Left = 9
      Top = 92
      Width = 37
      Height = 13
      Alignment = taRightJustify
      Caption = 'Drive 4:'
    end
    object MDV2Label: TLabel
      Left = 9
      Top = 68
      Width = 37
      Height = 13
      Alignment = taRightJustify
      Caption = 'Drive 3:'
    end
    object MDV4Label: TLabel
      Left = 9
      Top = 116
      Width = 37
      Height = 13
      Alignment = taRightJustify
      Caption = 'Drive 5:'
    end
    object MDV5Label: TLabel
      Left = 9
      Top = 140
      Width = 37
      Height = 13
      Alignment = taRightJustify
      Caption = 'Drive 6:'
    end
    object MDV6Label: TLabel
      Left = 9
      Top = 164
      Width = 37
      Height = 13
      Alignment = taRightJustify
      Caption = 'Drive 7:'
    end
    object MDV7Label: TLabel
      Left = 9
      Top = 188
      Width = 37
      Height = 13
      Alignment = taRightJustify
      Caption = 'Drive 8:'
    end
    object MDV0Text: TEdit
      Left = 56
      Top = 16
      Width = 185
      Height = 21
      Enabled = False
      ParentShowHint = False
      ReadOnly = True
      ShowHint = True
      TabOrder = 0
      Text = '< Empty >'
    end
    object MDV0FSBtn: TButton
      Left = 244
      Top = 16
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 1
      OnClick = MDV0FSBtnClick
    end
    object MDV0EjectBtn: TButton
      Left = 272
      Top = 16
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 2
      OnClick = MDV0EjectBtnClick
    end
    object MDV1EjectBtn: TButton
      Left = 272
      Top = 40
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 3
      OnClick = MDV0EjectBtnClick
    end
    object MDV1FSBtn: TButton
      Left = 244
      Top = 40
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 4
      OnClick = MDV0FSBtnClick
    end
    object MDV1Text: TEdit
      Left = 56
      Top = 40
      Width = 185
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 5
      Text = '< Empty >'
    end
    object MDV2Text: TEdit
      Left = 56
      Top = 64
      Width = 185
      Height = 21
      Enabled = False
      ParentShowHint = False
      ReadOnly = True
      ShowHint = True
      TabOrder = 6
      Text = '< Empty >'
    end
    object MDV3Text: TEdit
      Left = 56
      Top = 88
      Width = 185
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 7
      Text = '< Empty >'
    end
    object MDV3FSBtn: TButton
      Left = 244
      Top = 88
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 8
      OnClick = MDV0FSBtnClick
    end
    object MDV2FSBtn: TButton
      Left = 244
      Top = 64
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 9
      OnClick = MDV0FSBtnClick
    end
    object MDV2EjectBtn: TButton
      Left = 272
      Top = 64
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 10
      OnClick = MDV0EjectBtnClick
    end
    object MDV3EjectBtn: TButton
      Left = 272
      Top = 88
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 11
      OnClick = MDV0EjectBtnClick
    end
    object MDV7Text: TEdit
      Left = 56
      Top = 184
      Width = 185
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 12
      Text = '< Empty >'
    end
    object MDV6Text: TEdit
      Left = 56
      Top = 160
      Width = 185
      Height = 21
      Enabled = False
      ParentShowHint = False
      ReadOnly = True
      ShowHint = True
      TabOrder = 13
      Text = '< Empty >'
    end
    object MDV5Text: TEdit
      Left = 56
      Top = 136
      Width = 185
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 14
      Text = '< Empty >'
    end
    object MDV4Text: TEdit
      Left = 56
      Top = 112
      Width = 185
      Height = 21
      Enabled = False
      ParentShowHint = False
      ReadOnly = True
      ShowHint = True
      TabOrder = 15
      Text = '< Empty >'
    end
    object MDV4FSBtn: TButton
      Left = 244
      Top = 112
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 16
      OnClick = MDV0FSBtnClick
    end
    object MDV5FSBtn: TButton
      Left = 244
      Top = 136
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 17
      OnClick = MDV0FSBtnClick
    end
    object MDV6FSBtn: TButton
      Left = 244
      Top = 160
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 18
      OnClick = MDV0FSBtnClick
    end
    object MDV7FSBtn: TButton
      Left = 244
      Top = 184
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 19
      OnClick = MDV0FSBtnClick
    end
    object MDV7EjectBtn: TButton
      Left = 272
      Top = 184
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 20
      OnClick = MDV0EjectBtnClick
    end
    object MDV6EjectBtn: TButton
      Left = 272
      Top = 160
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 21
      OnClick = MDV0EjectBtnClick
    end
    object MDV5EjectBtn: TButton
      Left = 272
      Top = 136
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 22
      OnClick = MDV0EjectBtnClick
    end
    object MDV4EjectBtn: TButton
      Left = 272
      Top = 112
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 23
      OnClick = MDV0EjectBtnClick
    end
  end
  object OpenDialog1: TOpenDialog
    Filter = 
      'Disk Images|*.dsk;*.mgt;*.img;*.opd;*.opu;*.trd;*.zip|+3 Disk Im' +
      'ages|*.dsk|+D/Disciple Images|*.mgt;*.img|Opus Discovery Images|' +
      '*.opd;*.opu|TR-DOS Images|*.trd|Compressed Images|*.zip'
    Left = 224
    Top = 392
  end
  object OpenDialog2: TOpenDialog
    Filter = 'Hard Drive Images|*.hdf'
    Left = 192
    Top = 392
  end
  object OpenDialog3: TOpenDialog
    Filter = 'All Microdrive Cartridges (*.mdr, *.mdv)|*.mdr;*.mdv'
    Left = 160
    Top = 392
  end
end
