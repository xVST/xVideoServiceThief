unit ChangelogForm;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ComCtrls;

type
  TfrmChangelog = class(TForm)
    btnClose: TButton;
    rchEdtChangelog: TRichEdit;
    procedure btnCloseClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  frmChangelog: TfrmChangelog;

implementation

{$R *.dfm}
{$R ../rc/changelog.res}

procedure TfrmChangelog.btnCloseClick(Sender: TObject);
begin
  Close;
end;

procedure TfrmChangelog.FormCreate(Sender: TObject);
var
  rs: TResourceStream;

begin
  rs:=TResourceStream.Create(hinstance, 'CHANGELOG', RT_RCDATA);
  try
    rchEdtChangelog.Lines.LoadFromStream(rs);
  finally
    rs.Free;
  end;
end;

end.
