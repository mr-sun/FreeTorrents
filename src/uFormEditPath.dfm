object FormEditPath: TFormEditPath
  Left = 427
  Top = 304
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = #1056#1077#1076#1072#1082#1090#1080#1088#1086#1074#1072#1085#1080#1077' '#1087#1091#1090#1080
  ClientHeight = 124
  ClientWidth = 433
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
    Width = 417
    Height = 65
    Caption = #1042#1099#1073#1086#1088' '#1082#1072#1090#1072#1083#1086#1075#1072
    TabOrder = 0
    object EditPath: TEdit
      Left = 16
      Top = 24
      Width = 305
      Height = 21
      TabOrder = 0
      Text = 'EditPath'
      OnKeyPress = EditPathKeyPress
    end
    object BtnBrowse: TButton
      Left = 328
      Top = 22
      Width = 73
      Height = 25
      Caption = #1054#1073#1079#1086#1088'...'
      TabOrder = 1
      OnClick = BtnBrowseClick
    end
  end
  object BtnCancel: TButton
    Left = 344
    Top = 88
    Width = 75
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    TabOrder = 2
    OnClick = BtnCancelClick
  end
  object BtnOK: TButton
    Left = 264
    Top = 88
    Width = 75
    Height = 25
    Caption = #1054#1050
    TabOrder = 1
    OnClick = BtnOKClick
  end
end
