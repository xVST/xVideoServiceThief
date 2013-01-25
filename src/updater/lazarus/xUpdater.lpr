(******************************************************************************)
(*                                                                            *)
(*           ToolsUnit v1.0 is part of xVideoServiceThief project             *)
(*                                                                            *)
(*                Powered by Xesc & Technology 1998/2010                      *)
(*                                                                            *)
(******************************************************************************)
(*                                                                            *)
(* Copyright (C) 2010 Xesc & Technology 1998-2010 xeskuu.xvst@gmail.com            *)
(*                                                                            *)
(* This source is free software; you can redistribute it and/or modify it     *)
(* under the terms of the GNU General Public License as published by the Free *)
(* Software Foundation; either version 2 of the License, or (at your option)  *)
(* any later version.                                                         *)
(*                                                                            *)
(* This code is distributed in the hope that it will be useful, but WITHOUT   *)
(* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      *)
(* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for  *)
(* more details.                                                              *)
(*                                                                            *)
(* A copy of the GNU General Public License is available on the World Wide    *)
(* Web at <http://www.gnu.org/copyleft/gpl.html>. You can also obtain it by   *)
(* writing to the:                                                            *)
(* Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,       *)
(* MA 02111-1307, USA.                                                        *)
(*                                                                            *)
(******************************************************************************)

program xUpdater;

{$mode objfpc}{$H+}

uses
  SysUtils,
  xUpdaterClass in '..\src\xUpdaterClass.pas',
  ToolsUnit in '..\src\ToolsUnit.pas';

{$IFDEF WIN32}
{$R ..\res\xUpdater_Icon.res}
{$R ..\res\xUpdater_VersionInfo.res}
{$ENDIF}

begin
  // about...
  WriteLn('Xesc & Technology (R) Updater Utility Version 1.0.0.6');
  WriteLn('Copyright (C) Xesc & Technology Corporation. All rights reserved.');
  // start updater
  if FileExists(ParamStr(1)) then
    with TxUpdater.Create(ParamStr(1)) do
      try
        WriteLn('Executing install script...');
        Start;
      finally
        Free;
        WriteLn('Install Script executed...');
      end
  else
    WriteLn('Install script file do not exists: "' + ParamStr(1) + '"');
end.

