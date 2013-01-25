unit NativeWindowsCursors;

interface

uses
  Windows, Controls, Forms;

  procedure ReplaceDelphiCursors;

implementation

procedure ReplaceDelphiCursors;

  procedure AddWindowsCursor(Index: Integer; ID: PAnsiChar); var hCur: HCURSOR;
  begin
    hCur:=LoadCursor(0, ID);
    // if this cursor exists
    if hCur <> 0 then
      Screen.Cursors[Index]:=hCur;
  end;

begin
  AddWindowsCursor(crCross, IDC_CROSS);
  AddWindowsCursor(crIBeam, IDC_IBEAM);
  AddWindowsCursor(crSize, IDC_SIZE);
  AddWindowsCursor(crSizeNESW, IDC_SIZENESW);
  AddWindowsCursor(crSizeNS, IDC_SIZENS);
  AddWindowsCursor(crSizeNWSE, IDC_SIZENWSE);
  AddWindowsCursor(crSizeWE, IDC_SIZEWE);
  AddWindowsCursor(crUpArrow, IDC_UPARROW);
  AddWindowsCursor(crHourGlass, IDC_WAIT);
  AddWindowsCursor(crNo, IDC_NO);
  AddWindowsCursor(crAppStart, IDC_APPSTARTING);
  AddWindowsCursor(crHelp, IDC_HELP);
  AddWindowsCursor(crHandPoint, IDC_HAND);
  AddWindowsCursor(crSizeAll, IDC_SIZEALL);
  //AddWindowsCursor(crDrag, 0);       // NOT DEFINED
  //AddWindowsCursor(crNoDrop, 0);     // NOT DEFINED
  //AddWindowsCursor(crHSplit, 0);     // NOT DEFINED
  //AddWindowsCursor(crVSplit, 0);     // NOT DEFINED
  //AddWindowsCursor(crMultiDrag, 0);  // NOT DEFINED
  //AddWindowsCursor(crSQLWait, 0);    // NOT DEFINED
end;

initialization
  ReplaceDelphiCursors;

end.
