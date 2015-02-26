object FormEditStr: TFormEditStr
  Left = 480
  Top = 333
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = #1053#1072#1079#1074#1072#1085#1080#1077' '#1093#1086#1089#1090#1072' ('#1090#1088#1077#1082#1077#1088#1072')'
  ClientHeight = 123
  ClientWidth = 345
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
    Width = 329
    Height = 65
    Caption = #1053#1072#1079#1074#1072#1085#1080#1077' '#1093#1086#1089#1090#1072' ('#1090#1088#1077#1082#1077#1088#1072')'
    TabOrder = 0
    object EditStr: TEdit
      Left = 16
      Top = 24
      Width = 297
      Height = 21
      TabOrder = 0
      Text = 'EditStr'
    end
  end
  object BtnCancel: TButton
    Left = 256
    Top = 88
    Width = 75
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    TabOrder = 1
    OnClick = BtnCancelClick
  end
  object BtnOk: TButton
    Left = 176
    Top = 88
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 2
    OnClick = BtnOkClick
  end
end
