object frmChangelog: TfrmChangelog
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMaximize]
  Caption = 'xVST 2.0 Changelog'
  ClientHeight = 339
  ClientWidth = 607
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  DesignSize = (
    607
    339)
  PixelsPerInch = 96
  TextHeight = 13
  object btnClose: TButton
    Left = 495
    Top = 306
    Width = 104
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Close'
    Default = True
    TabOrder = 0
    OnClick = btnCloseClick
    ExplicitLeft = 448
    ExplicitTop = 308
  end
  object rchEdtChangelog: TRichEdit
    Left = 8
    Top = 8
    Width = 591
    Height = 292
    Anchors = [akLeft, akTop, akRight, akBottom]
    ScrollBars = ssBoth
    TabOrder = 1
    ExplicitWidth = 544
    ExplicitHeight = 294
  end
end
