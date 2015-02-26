object FormVirtualTorrent: TFormVirtualTorrent
  Left = 423
  Top = 205
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = #1042#1080#1088#1090#1091#1072#1083#1100#1085#1099#1081' '#1090#1086#1088#1088#1077#1085#1090
  ClientHeight = 388
  ClientWidth = 394
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 8
    Top = 8
    Width = 377
    Height = 65
    Caption = #1058#1086#1088#1088#1077#1085#1090' '#1092#1072#1081#1083
    TabOrder = 0
    object EditFName: TEdit
      Left = 16
      Top = 24
      Width = 273
      Height = 21
      ReadOnly = True
      TabOrder = 0
      Text = 'EditFName'
    end
    object BtnBrowse: TButton
      Left = 296
      Top = 23
      Width = 65
      Height = 23
      Caption = #1054#1073#1079#1086#1088'...'
      TabOrder = 1
      OnClick = BtnBrowseClick
    end
  end
  object GroupBox2: TGroupBox
    Left = 8
    Top = 80
    Width = 377
    Height = 81
    Caption = #1048#1085#1092#1086#1088#1084#1072#1094#1080#1103' '#1086' '#1090#1086#1088#1088#1077#1085#1090#1077
    TabOrder = 1
    object Label1: TLabel
      Left = 16
      Top = 24
      Width = 32
      Height = 13
      Caption = #1060#1072#1081#1083':'
    end
    object Label4: TLabel
      Left = 23
      Top = 48
      Width = 24
      Height = 13
      Caption = #1061#1101#1096':'
    end
    object EditName: TEdit
      Left = 56
      Top = 24
      Width = 305
      Height = 16
      BorderStyle = bsNone
      Color = clBtnFace
      Ctl3D = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentCtl3D = False
      ParentFont = False
      TabOrder = 0
      Text = 'EditName'
    end
    object EditHash: TEdit
      Left = 56
      Top = 48
      Width = 305
      Height = 16
      BorderStyle = bsNone
      Color = clBtnFace
      Ctl3D = False
      ParentCtl3D = False
      TabOrder = 1
      Text = 'Edit1'
    end
  end
  object GroupBox3: TGroupBox
    Left = 8
    Top = 168
    Width = 377
    Height = 169
    Caption = #1048#1084#1080#1090#1072#1094#1080#1103' '#1088#1072#1079#1076#1072#1095#1080
    TabOrder = 2
    object Label2: TLabel
      Left = 16
      Top = 120
      Width = 85
      Height = 13
      Caption = #1057#1082#1086#1088#1086#1089#1090#1100' '#1086#1090#1076#1072#1095#1080
    end
    object Label3: TLabel
      Left = 120
      Top = 144
      Width = 47
      Height = 13
      Caption = #1050#1073#1080#1090'/'#1089#1077#1082
    end
    object ListViewTrackers: TListView
      Left = 16
      Top = 24
      Width = 345
      Height = 89
      Checkboxes = True
      Columns = <
        item
          Caption = #1058#1088#1077#1082#1077#1088
          Width = 310
        end>
      GridLines = True
      ReadOnly = True
      RowSelect = True
      TabOrder = 0
      ViewStyle = vsReport
    end
    object EditUpSpeed: TEdit
      Left = 16
      Top = 136
      Width = 97
      Height = 21
      TabOrder = 1
      Text = '0'
    end
    object StaticText1: TStaticText
      Left = 192
      Top = 120
      Width = 169
      Height = 41
      AutoSize = False
      Caption = 
        #1053#1077' '#1091#1089#1090#1072#1085#1072#1074#1083#1080#1074#1072#1081#1090#1077' '#1086#1095#1077#1085#1100' '#1073#1086#1083#1100#1096#1091#1102' '#1089#1082#1086#1088#1086#1089#1090#1100' '#1086#1090#1076#1072#1095#1080' '#8212' '#1101#1090#1086' '#1084#1086#1078#1077#1090' '#1074#1099#1079#1074 +
        #1072#1090#1100' '#1087#1086#1076#1086#1079#1088#1077#1085#1080#1077'!'
      TabOrder = 2
    end
  end
  object BtnCancel: TButton
    Left = 304
    Top = 352
    Width = 75
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    TabOrder = 3
    OnClick = BtnCancelClick
  end
  object BtnOk: TButton
    Left = 224
    Top = 352
    Width = 75
    Height = 25
    Caption = #1054#1050
    TabOrder = 4
    OnClick = BtnOkClick
  end
  object OpenDialog1: TOpenDialog
    DefaultExt = '.torrent'
    Filter = 'Torrent-'#1092#1072#1081#1083#1099' (*.torrent)|*.torrent'
    Title = #1042#1099#1073#1086#1088' '#1090#1086#1088#1088#1077#1085#1090'-'#1092#1072#1081#1083#1072
    Left = 360
    Top = 56
  end
end
