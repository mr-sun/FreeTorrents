object FormVirtualMgr: TFormVirtualMgr
  Left = 358
  Top = 189
  Width = 600
  Height = 427
  Caption = #1053#1072#1089#1090#1088#1086#1081#1082#1072' '#1074#1080#1088#1090#1091#1072#1083#1100#1085#1099#1093' '#1090#1086#1088#1088#1077#1085#1090#1086#1074
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  DesignSize = (
    584
    389)
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 8
    Top = 8
    Width = 579
    Height = 321
    Anchors = [akLeft, akTop, akRight, akBottom]
    Caption = #1042#1080#1088#1090#1091#1072#1083#1100#1085#1099#1077' '#1090#1086#1088#1088#1077#1085#1090#1099
    TabOrder = 0
    DesignSize = (
      579
      321)
    object ListViewVirtual: TListView
      Left = 8
      Top = 16
      Width = 467
      Height = 297
      Anchors = [akLeft, akTop, akRight, akBottom]
      Checkboxes = True
      Columns = <
        item
          Caption = #1053#1072#1079#1074#1072#1085#1080#1077' '#1090#1086#1088#1088#1077#1085#1090#1072
          Width = 150
        end
        item
          Caption = #1048#1084#1080#1090#1072#1094#1080#1103' '#1088#1072#1079#1076#1072#1095#1080
          Width = 120
        end
        item
          Caption = #1058#1088#1077#1082#1077#1088#1099
          Width = 150
        end>
      GridLines = True
      ReadOnly = True
      RowSelect = True
      TabOrder = 0
      ViewStyle = vsReport
      OnClick = ListViewVirtualClick
      OnDblClick = ListViewVirtualDblClick
      OnKeyPress = ListViewVirtualKeyPress
      OnSelectItem = ListViewVirtualSelectItem
    end
    object BtnAdd: TButton
      Left = 490
      Top = 16
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = #1044#1086#1073#1072#1074#1080#1090#1100
      TabOrder = 1
      OnClick = BtnAddClick
    end
    object BtnEdit: TButton
      Left = 490
      Top = 48
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = #1048#1079#1084#1077#1085#1080#1090#1100
      TabOrder = 2
      OnClick = BtnEditClick
    end
    object BtnDelete: TButton
      Left = 490
      Top = 80
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = #1059#1076#1072#1083#1080#1090#1100
      TabOrder = 3
      OnClick = BtnDeleteClick
    end
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 367
    Width = 584
    Height = 22
    Panels = <>
    SimplePanel = False
  end
  object BtnCancel: TButton
    Left = 504
    Top = 341
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #1054#1090#1084#1077#1085#1072
    TabOrder = 2
    OnClick = BtnCancelClick
  end
  object BtnOk: TButton
    Left = 416
    Top = 341
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'OK'
    TabOrder = 3
    OnClick = BtnOkClick
  end
end
