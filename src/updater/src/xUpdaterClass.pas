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

{$IFDEF FPC}
  {$mode objfpc}{$H+}
{$ENDIF}

{ Reduced console command line: Script runner }

unit xUpdaterClass;

interface

uses
{$IFNDEF FPC}
  Windows,
{$ELSE}
  Process, FileUtil,
{$ENDIF}
  SysUtils, Classes, ToolsUnit;

type
  { Block information }
  TBlockID = packed record
    ID: String;    //< Block id
    Line: Integer; //< Start line
  end;

  { List of string parameters }
  TParams = Array of String;

  { Run scripts class }

  { TxUpdater }

  TxUpdater = class
    private
      FUpdateFilePath: String;   //< file with update instructions
      FUpdateFile: TStringList;  //< script content
      FMain_App: String; //< what application should execute after the update?
      FParametes: String; //< the main application will be executed with these parameters
      FRestart: Boolean; //< execute application? 
      FArrayOfBlocks: Array of TBlockID; // Stored Script Blocks
      Fline: Integer; //< Current Script line
      FIsInIf,        //< Is into a IF condition?
      FIsInElse: Boolean; //< Is into a ELSE condition?
      { Return if is a Reserved Word }
      function isReservedWord(KeyWord: String): Integer;
      { Get the blocks id's }
      procedure GetBlocksId;
      { Execute the Script }
      procedure ExecuteUpdateScript;
      { Execute Order }
      function ExecuteOrder(Order, Params: String): Boolean;
      { Go to the next "End" or "Else" }
      procedure GetEndOrElse;
      { Get the Block record by Block Id }
      function GetBlockByName(Name: String; var BlockID: TBlockID): Boolean;
      { Get an array of params }
      function GetParams(Params: String): TParams;
    public
      { TxUpdater Class constructor }
      constructor Create(UpdateFile: String);
      { TxUpdater Class destructor }
      destructor Destroy; override;
      { Run script }
      procedure Start;
  end;

implementation

{ TxUpdater }

(*
    Update Script - description

    IMPORTANT: ORDERS & BLOCKS ARE CASE SENSITIVE!!!

    Config Vars:
      #MAIN_APP=path+exe
      #PARAMS={extra application params to execute}
      #RESTART=true {true / false}

    Basic functions:
      del "file"
      copy "file"
      rename "file"
      exec "application" wait { the wait command is optional }
      run "update script" { run another update script }
      install "file" "destination"
      wait MILISECONDS
      exists "file"
      chdir "dir"
      rmdir "dir"
      mkdir "dir"
      mkdirs "dir"
      echo "message"

      if ORDER "file"
        ORDER
      else
        ORDER

      exit {finish the script}

      goto BLOCK_NAME

    How to define blocks
      :BLOCK_NAME

    How to coment line:
      Use '//' on line start
      example:
        // del "demo.exe"
        goto block1
      
*)

constructor TxUpdater.Create(UpdateFile: String);
begin
  FUpdateFilePath:=UpdateFile;
  FUpdateFile:=TStringList.Create;
  FUpdateFile.LoadFromFile(FUpdateFilePath);
  FMain_App:='';
  FRestart:=False;
  FIsInIf:=False;
  FIsInElse:=False;
end;

destructor TxUpdater.Destroy;
begin
  FUpdateFile.Free;
  inherited Destroy;
end;

function TxUpdater.ExecuteOrder(Order, Params: String): Boolean;
var
  Param01: String;
  Parameters: TParams;
  BlockID: TBlockID;
  Value: Integer;
  SubUpdaterScript: TxUpdater;

  { Get a parameter value }
  function GetParam(Index: Integer): String;
  begin
    if Parameters <> nil then
      if Index in [0..Length(Parameters)] then  // get parameter
        Result:=Parameters[Index]
      else // empty param
        Result:=''
    else // empty param
      Result:='';
  end;

begin
  Result:=False;
  // get function params
  Parameters:=GetParams(Params);
  // get order
  case isReservedWord(Order) of
    {goto}
    1:  begin
          Param01:=GetToken(Params, ' ', 2);
          if GetBlockByName(Param01, BlockID) then
            begin
              Fline:=BlockID.Line;
              FIsInIf:=False;
              Result:=True;
            end;
        end;
    {del}
    2: Result:=DeleteFile(GetParam(0));
    {copy}
    3:  begin
{$IFNDEF FPC}
          Result:=CopyFile(PChar(GetParam(0)), PChar(GetParam(1)), False);
{$ELSE}
          Result:=CopyFile(GetParam(0), GetParam(1));
{$ENDIF}
        end;
    {rename}
    4: Result:=RenameFile(GetParam(0), GetParam(1));
    {exec}
    5:  begin
          // check if has the "wait" command
          Param01:=GetToken(Params, ' ', GetTokenCount(Params, ' '));
{$IFNDEF FPC}
          if Param01 = 'wait' then
            Result:=WinExecAndWait32(PChar(GetParam(0)), SW_NORMAL) >= 31
          else // don't wait
            Result:=WinExec(PChar(GetParam(0)), SW_NORMAL) >= 31;
{$ELSE}
          with TProcess.Create(nil) do
            try
              CommandLine:=GetParam(0);
              if Param01 = 'wait' then Options:=[poWaitOnExit] else Options:=[];
              Execute;
            finally
              Free;
            end;
{$ENDIF}
        end;
    {run}
    6:  begin
          if FileExists(GetParam(0)) then
            begin
              SubUpdaterScript:=TxUpdater.Create(GetParam(0));
              SubUpdaterScript.Start;
              SubUpdaterScript.Free;
            end;
        end;
    {install}
    7:  begin
(*
          Param01:=GetToken(Params, '"', 2);
          Param02:=GetToken(Params, '"', 4);
          if FileExists(Param01) then
            begin
              Installer:=TPackUnCompress.Create;
              try
                Installer.OpenPackage(Param01);
                Installer.ExtractAll(Param02);
              finally
                Installer.Free;
              end;
            end;
*)
        end;
    {exit}
    8: Fline:=FUpdateFile.Count;
    {wait}
    9:  begin
          Param01:=GetToken(FUpdateFile[FLine], ' ', 2);
          if TryStrToInt(Param01, Value) then
            begin
              Sleep(Value);
              Result:=True;
            end;
        end;
    {exists}
    10: begin
          Result:=FileExists(GetParam(0)); // try file
          if not Result then // try folder
            Result:=DirectoryExists(GetParam(0));
        end;
    {chdir}
    11: begin
          if DirectoryExists(GetParam(0)) then
            begin
              ChDir(GetParam(0));
              Result:=True;
            end
          else
            Result:=False;
        end;
    {rmdir}
    12: begin
          if DirectoryExists(GetParam(0)) then
            begin
              RmDir(GetParam(0));
              Result:=True;
            end
          else
            Result:=False;
        end;
    {mkdir}
    13: begin
          if not DirectoryExists(GetParam(0)) then
            begin
              MkDir(GetParam(0));
              Result:=True;
            end
          else
            Result:=False;
        end;
    {mkdirs}
    14: begin
          if not DirectoryExists(GetParam(0)) then
            begin
              ForceDirectories(GetParam(0));
              Result:=True;
            end
          else
            Result:=False;
        end;
    {echo}
    15: WriteLn(GetParam(0));
  end;
end;

procedure TxUpdater.ExecuteUpdateScript;
var
  Count: Integer;
  isEnd: Boolean;
  KeyWord, Order, Params: String;

begin
  Count:=FUpdateFile.Count;
  Fline:=0;
  isEnd:=False;

  while not isEnd and (Fline < Count) do
    begin
      if FUpdateFile[Fline] <> '' then
        begin
          // Check for definition var
          if FUpdateFile[Fline][1] = '#' then
            begin
              KeyWord:=GetToken(FUpdateFile[Fline], '=', 1);
              // check what def is
              if KeyWord = '#MAIN_APP' then
                FMain_App:=GetToken(FUpdateFile[Fline], '=', 2)
              else if KeyWord = '#PARAMETERS' then
                FParametes:=GetToken(FUpdateFile[Fline], '=', 2)
              else if KeyWord = '#RESTART' then
                FRestart:=StrToBool(GetToken(FUpdateFile[Fline], '=', 2));
            end
          else // isnt a definition, so check what is it
            if FUpdateFile[Fline][1] <> ':' then // insnt a block def
              begin
                KeyWord:=GetToken(FUpdateFile[Fline], ' ', 1);
                if isReservedWord(KeyWord) > 0 then
                  ExecuteOrder(KeyWord, FUpdateFile[Fline])
                else
                  if not FIsInIf then // isnt in if
                    begin
                      if KeyWord = 'if' then  // is if
                        begin
                          FIsInIf:=True;
                          Params:=FUpdateFile[Fline];
                          Delete(Params, 1, Pos(' ', Params));
                          Order:=GetToken(Params, ' ', 1);
                          if not ExecuteOrder(Order, Params) then // if = false
                            begin
                              GetEndOrElse;
                              if Fline < Count then // isnt out of file
                                if FUpdateFile[Fline] = 'end'  then // is end if
                                  begin
                                    FIsInIf:=False;
                                    FIsInElse:=False;
                                  end
                                else // is in "else"
                                  FIsInElse:=True;
                            end
                          else // is true
                            FIsInElse:=False;
                        end
                    end
                  else // is in if
                    if not FIsInElse then // is in "true" case
                      if FUpdateFile[Fline] = 'end'  then
                        FIsInIf:=False // end of if
                      else
                        if FUpdateFile[Fline] = 'else'  then
                          begin
                            GetEndOrElse; // search -> end
                            FIsInIf:=False; // end if
                          end;
              end;
        end;
      Inc(FLine);
    end;
  // check def vars
  if FRestart then
    if FMain_App <> '' then    
      begin
        ChDir(ExtractFilePath(FMain_App));
        ExecuteOrder('exec', '"' + FMain_App + ' ' + FParametes + '"');
      end;
end;

function TxUpdater.GetBlockByName(Name: String; var BlockID: TBlockID): Boolean;
var
  n: Integer;

begin
  for n:=0 to Length(FArrayOfBlocks) do
    if FArrayOfBlocks[n].ID = Name then
      begin
        BlockID:=FArrayOfBlocks[n];
        Result:=True;
        Exit;
      end;
  Result:=False;
end;

function TxUpdater.GetParams(Params: String): TParams;
var
  n: Integer;
  str: String;
  preChar: Char;
  opened: Boolean;

begin
  Result:=nil;
  str:='';
  preChar:=#0;
  opened:=false;
  Params:=Trim(Params);
  // get params
  for n:=1 to Length(Params) do
    begin
      if (Params[n] <> '"') and opened then
        begin
          str:=str + Params[n];
          if Params[n] <> '\' then preChar:=#0 else preChar:='\';
        end
      else // is a " char
        if opened then
          begin
            if preChar = '\' then // is not a close token
              begin
                Delete(str, Length(str), 1); // delete the last char \
                str:=str + '"';
              end
            else // is a close token, so add this new param
              begin
                SetLength(Result, Length(Result) + 1);
                Result[Length(Result) - 1]:=str;
                str:='';
                opened:=False;
              end;
            preChar:=#0;
          end
        else // open new param
          opened:=Params[n] = '"';
    end;
end;

procedure TxUpdater.GetBlocksId;
var
  n, Count: Integer;
  NewBlock: TBlockID;

begin
  FArrayOfBlocks:=nil;
  Count:=FUpdateFile.Count;
  for n:=0 to Count - 1 do
    begin
      FUpdateFile[n]:=TrimLeft(FUpdateFile[n]);
      if FUpdateFile[n] <> '' then
        if FUpdateFile[n][1] = ':' then // is a block definition?
          begin
            NewBlock.ID:=GetToken(FUpdateFile[n], ':', 2);
            NewBlock.Line:=n;
            // Add this new block
            SetLength(FArrayOfBlocks, Length(FArrayOfBlocks) + 1);
            FArrayOfBlocks[Length(FArrayOfBlocks) - 1]:=NewBlock;
          end;
    end;
end;

procedure TxUpdater.GetEndOrElse;
var
  isEnd: Boolean;

begin
  isend:=False;
  Inc(FLine);
  while (FLine < FUpdateFile.Count) and not isEnd do
    if (FUpdateFile[FLine] <> 'else') and (FUpdateFile[FLine] <> 'end') then
      Inc(FLine)
    else
      isEnd:=True;
end;

function TxUpdater.isReservedWord(KeyWord: String): Integer;
begin
       if KeyWord = 'goto'     then Result:=1
  else if KeyWord = 'del'     then Result:=2
  else if KeyWord = 'copy'    then Result:=3
  else if KeyWord = 'rename'  then Result:=4
  else if KeyWord = 'exec'    then Result:=5
  else if KeyWord = 'run'     then Result:=6
  else if KeyWord = 'install' then Result:=7
  else if KeyWord = 'exit'    then Result:=8
  else if KeyWord = 'wait'    then Result:=9
  else if KeyWord = 'exists'  then Result:=10
  else if KeyWord = 'chdir'   then Result:=11
  else if KeyWord = 'rmdir'   then Result:=12
  else if KeyWord = 'mkdir'   then Result:=13
  else if KeyWord = 'mkdirs'  then Result:=14
  else if KeyWord = 'echo'    then Result:=15
  else Result:=0;
end;

procedure TxUpdater.Start;
begin

GetParams('"jojo\"pepe\"" "param 2" "\1\\2\\\3\""');

  GetBlocksId;
  // Start procces
  ExecuteUpdateScript;      
end;

end.
