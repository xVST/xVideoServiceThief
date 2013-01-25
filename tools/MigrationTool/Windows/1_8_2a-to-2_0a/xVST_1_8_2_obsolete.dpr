program xVST_1_8_2_obsolete;

uses
  Forms,
  MainForm in 'src\MainForm.pas' {frmMain},
  ChangelogForm in 'src\ChangelogForm.pas' {frmChangelog},
  NativeWindowsCursors in 'src\NativeWindowsCursors.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  Application.Title := 'xVideoServiceThief Migration';
  Application.CreateForm(TfrmMain, frmMain);
  Application.CreateForm(TfrmChangelog, frmChangelog);
  Application.Run;
end.
