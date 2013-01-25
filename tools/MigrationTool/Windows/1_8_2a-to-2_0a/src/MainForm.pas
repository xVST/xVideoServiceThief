unit MainForm;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ExtCtrls, ShellApi;

type
  TfrmMain = class(TForm)
    Image1: TImage;
    Label1: TLabel;
    Label2: TLabel;
    btnClose: TButton;
    btnDownload: TButton;
    btnChangelog: TButton;
    Label3: TLabel;
    Label4: TLabel;
    lblLink: TLabel;
    procedure btnChangelogClick(Sender: TObject);
    procedure btnDownloadClick(Sender: TObject);
    procedure lblLinkClick(Sender: TObject);
    procedure btnCloseClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  frmMain: TfrmMain;

implementation

uses ChangelogForm;

{$R *.dfm}

procedure TfrmMain.btnChangelogClick(Sender: TObject);
begin
  frmChangelog.ShowModal;
end;

procedure TfrmMain.btnCloseClick(Sender: TObject);
begin
  Close;
end;

procedure TfrmMain.btnDownloadClick(Sender: TObject);
begin
  ShellExecute(Handle, 'open', 'http://xviservicethief.sourceforge.net/index.php?action=downloads#Windows', '', '', SW_NORMAL);
  Close;
end;

procedure TfrmMain.lblLinkClick(Sender: TObject);
begin
  ShellExecute(Handle, 'open', 'http://xviservicethief.sourceforge.net/', '', '', SW_NORMAL);
end;

end.
