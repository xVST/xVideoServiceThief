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

{ Support procedures and functions }

unit ToolsUnit;

interface

uses
{$IFNDEF FPC}
  Windows,
{$ENDIF}
  SysUtils, StrUtils, Classes;

  { Get the text between two tokens }
  function CopyBetween(Text: String; sFrom, sTo: String): String;
  { Return a valid URL }
  function FixURL(url: String): String;
  { Convert a URL into a Standard String (ASCII) }
  function URLtoAscii(url: String): String;
  { Extract the File name from a URL }
  function GetNameFromURL(URL: String): String;
  { Find a text into a TStringList, and return the line with the matched text }
  function FindIntoStringList(StringList: TStringList; Text: String; out Position: Integer): Boolean;
  { Compare two strings using simple Regular Expressions }
  function CompareStrWithRegExpr(Source, Pattern: String): Boolean;
  { Get a token from the String }
  function GetToken(Text, Separator: String; Token: Integer): String;
  { Get the token count from the String }
  function GetTokenCount(Text, Separator: String): Integer;
  { Get a with the all parameters }
  procedure GetParameterList(Params, Separator: String; const ParamsList: TStrings);
  { Get the url parameters }
  function GetUrlParameters(const URL: String): String;
  { Convert the File Size to String (Bytes, KBytes, MBytes... }
  function FileSizeToString(Bytes: Integer): String;
  { Convert seconds to Hours minutes seconds }
  function SecondsToHMS(Seconds: Integer; AllowNegative: Boolean; Compatct: Boolean): String;
  { Return the % }
  function CalculatePercent(const Value, Total: Real): Real;
  { Convert a Decimal HTML Char to the respective HTML Code }
  function HTMLCharToHTMLCode(HTML: String): String;
  { Convert a HTML Code to the respective Decimal HTML Char }
  function HTMLCodeToHTMLChar(HTML: String): String;
  { Convert standard html strings to strings }
  function CharToHTML(AChar: Char; HTMLCodes: Boolean = True): String;
  { Convert ascii chars to html chars }
  function HTMLToStr(HTML: String): String;
  { Convert strings to standard html strings }
  function HTMLToChar(HTML: String): Char;
  { Convert a html char to ascii char }
  function StrToHTML(Str: String; HTMLCodes: Boolean = True): String;
  { Return a valid File Name }
  function FixFileName(FileName: String; ReplaceFor: String = ''): String;
  { Return the file size }
  function GetFileSize(FileName: String): Int64;
  { Position to TPoint }
  function PositionToTPoint(Left, Top: Integer): TPoint;
  { Generates a UniqueFilename, makes sure the file does not exist before returning a result }
  function UniqueFilename(AFilename : String) : String;
  { Program directory }
  function ApplicationPath: String;
  { Universal Path (fix paths to make it cross-platform) }
  function UniversalPath(const Path: String): String;
{$IFNDEF FPC}
  { Execute application and wait to finish }
  function WinExecAndWait32(Path: PChar; Visibility: Cardinal): Cardinal;
{$ENDIF}

const
  HTML_Char_Conversion_Table: Array [1..100] of String =
    (
      '&#34;=&quot;',    // Quotation mark
      '&#38=&amp;',      // Ampersand
      '&#60=&lt;',       // Less than
      '&#62=&gt;',       // Greater than
      '&#160;=&nbsp;',	 // Non-breaking space
      '&#161;=&iexcl;',	 // Inverted exclamation
      '&#162;=&cent;',	 // Cent sign
      '&#163;=&pound;',	 // Pound sterling
      '&#164;=&curren;', // General currency sign
      '&#165;=&yen;',	   // Yen sign
      '&#166;=&brvbar;', // Broken vertical bar
      '&#167;=&sect;',	 // Section sign
      '&#168;=&uml;',	   // Umlaut (dieresis)
      '&#169;=&copy;',	 // Copyright
      '&#170;=&ordf;',	 // Feminine ordinal
      '&#171;=&laquo;',	 // Left angle quote, guillemotleft
      '&#172;=&not;',	   // Not sign
      '&#173;=&shy;',	   // Soft hyphen
      '&#174;=&reg;',	   // Registered trademark
      '&#175;=&macr;',	 // Macron accent
      '&#176;=&deg;',	   // Degree sign
      '&#177;=&plusmn;', // Plus or minus
      '&#178;=&sup2;',	 // Superscript two
      '&#179;=&sup3;',	 // Superscript three
      '&#180;=&acute;',	 // Acute accent
      '&#181;=&micro;',	 // Micro sign
      '&#182;=&para;',	 // Paragraph sign
      '&#183;=&middot;', // Middle dot
      '&#184;=&cedil;',	 // Cedilla
      '&#185;=&sup1;',	 // Superscript one
      '&#186;=&ordm;',	 // Masculine ordinal
      '&#187;=&raquo;',	 // Right angle quote, guillemotright
      '&#188;=&frac14;', // Fraction one-fourth
      '&#189;=&frac12;', // Fraction one-half
      '&#190;=&frac34;', // Fraction three-fourths
      '&#191;=&iquest;', // Inverted question mark
      '&#192;=&Agrave;', // Capital A, grave accent
      '&#193;=&Aacute;', // Capital A, acute accent
      '&#194;=&Acirc;',	 // Capital A, circumflex accent
      '&#195;=&Atilde;', // Capital A, tilde
      '&#196;=&Auml;',	 // Capital A, dieresis or umlaut mark
      '&#197;=&Aring;',	 // Capital A, ring
      '&#198;=&AElig;',	 // Capital AE dipthong (ligature)
      '&#199;=&Ccedil;', // Capital C, cedilla
      '&#200;=&Egrave;', // Capital E, grave accent
      '&#201;=&Eacute;', // Capital E, acute accent
      '&#202;=&Ecirc;',	 // Capital E, circumflex accent
      '&#203;=&Euml;',	 // Capital E, dieresis or umlaut mark
      '&#204;=&Igrave;', // Capital I, grave accent
      '&#205;=&Iacute;', // Capital I, acute accent
      '&#206;=&Icirc;',	 // Capital I, circumflex accent
      '&#207;=&Iuml;',	 // Capital I, dieresis or umlaut mark
      '&#208;=&ETH;',	   // Capital Eth, Icelandic
      '&#209;=&Ntilde;', // Capital N, tilde
      '&#210;=&Ograve;', // Capital O, grave accent
      '&#211;=&Oacute;', // Capital O, acute accent
      '&#212;=&Ocirc;',	 // Capital O, circumflex accent
      '&#213;=&Otilde;', // Capital O, tilde
      '&#214;=&Ouml;',	 // Capital O, dieresis or umlaut mark
      '&#215;=&times;',	 // Multiply sign
      '&#216;=&Oslash;', // Capital O, slash
      '&#217;=&Ugrave;', // Capital U, grave accent
      '&#218;=&Uacute;', // Capital U, acute accent
      '&#219;=&Ucirc;',	 // Capital U, circumflex accent
      '&#220;=&Uuml;',	 // Capital U, dieresis or umlaut mark
      '&#221;=&Yacute;', // Capital Y, acute accent
      '&#222;=&THORN;',	 // Capital THORN, Icelandic
      '&#223;=&szlig;',	 // Small sharp s, German (sz ligature)
      '&#224;=&agrave;', // Small a, grave accent
      '&#225;=&aacute;', // Small a, acute accent
      '&#226;=&acirc;',	 // Small a, circumflex accent
      '&#227;=&atilde;', // Small a, tilde
      '&#228;=&auml;',	 // Small a, dieresis or umlaut mark
      '&#229;=&aring;',	 // Small a, ring
      '&#230;=&aelig;',	 // Small ae dipthong (ligature)
      '&#231;=&ccedil;', // Small c, cedilla
      '&#232;=&egrave;', // Small e, grave accent
      '&#233;=&eacute;', // Small e, acute accent
      '&#234;=&ecirc;',	 // Small e, circumflex accent
      '&#235;=&euml;',	 // Small e, dieresis or umlaut mark
      '&#236;=&igrave;', // Small i, grave accent
      '&#237;=&iacute;', // Small i, acute accent
      '&#238;=&icirc;',	 // Small i, circumflex accent
      '&#239;=&iuml;',	 // Small i, dieresis or umlaut mark
      '&#240;=&eth;',	   // Small eth, Icelandic
      '&#241;=&ntilde;', // Small n, tilde
      '&#242;=&ograve;', // Small o, grave accent
      '&#243;=&oacute;', // Small o, acute accent
      '&#244;=&ocirc;',	 // Small o, circumflex accent
      '&#245;=&otilde;', // Small o, tilde
      '&#246;=&ouml;',	 // Small o, dieresis or umlaut mark
      '&#247;=&divide;', // Division sign
      '&#248;=&oslash;', // Small o, slash
      '&#249;=&ugrave;', // Small u, grave accent
      '&#250;=&uacute;', // Small u, acute accent
      '&#251;=&ucirc;',	 // Small u, circumflex accent
      '&#252;=&uuml;',	 // Small u, dieresis or umlaut mark
      '&#253;=&yacute;', // Small y, acute accent
      '&#254;=&thorn;',	 // Small thorn, Icelandic
      '&#255;=&yuml;'	   // Small y, dieresis or umlaut mark
    );

implementation

function CopyBetween(Text: String; sFrom, sTo: String): String;
var
  iStart, iEnd: integer;

begin
  Result:='';
  iStart:=Pos(sFrom,Text) + Length(sFrom);
  iEnd:=PosEx(sTo,Text,iStart);
  if (iStart > 0) and (iEnd > iStart) then
    Result:=Copy(Text, iStart, iEnd-iStart);
end;

function FixURL(url: String): String;
const
  CHARS_TO_FIX = '% "<>#{}|\^~[]`';

var
  n: Integer;

begin
  // convert '%25' for '%' (prevent future errors while the conversion process)
  url:=AnsiReplaceStr(url, '%25', '%');
  // move along the string
  for n:=1 to Length(CHARS_TO_FIX) do
    url:=AnsiReplaceStr(url, CHARS_TO_FIX[n], '%' + IntToHex(byte(CHARS_TO_FIX[n]), 2));
  // get the fixed url
  Result:=url;
end;

function URLtoAscii(URL: String): String;
var
  HexChar: String;
  n: Integer;

begin
  Result:='';
  while Pos('%', URL) > 0 do
    begin
      // copy text & delete it from URL
      n:=Pos('%', URL);
      Result:=Result + Copy(URL, 1, n - 1);
      Delete(URL, 1, n - 1);
      // get the HEx char & delete it from URL
      HexChar:=Copy(URL, 2, 2);
      Delete(URL, 1, 3);
      // convert
      Result:=Result + Char(StrToInt('$' + HexChar));
      // more?
      if Pos('%', URL) = 0 then if URL <> '' then
        Result:=Result + URL;
    end;
  // did something?
  if Result = '' then Result:=URL;
end;

function GetNameFromURL(URL: String): String;
var
  n: Integer;

begin
  Result:='';
  // we have something to analyze?
  if URL <> '' then
    for n:=Length(URL) downto 1 do
      if URL[n] <> '/' then
        Result:=URL[n] + Result
      else
        Exit;
end;

function FindIntoStringList(StringList: TStringList; Text: String; out Position: Integer): Boolean;
var
  lCount: IntegeR;
  n: Integer;
begin
  Result:=False;
  Position:=0;
  if StringList <> nil then
    begin
      lCount:=StringList.Count - 1;
      for n:=0 to lCount do
        if Pos(UpperCase(Text), UpperCase(StringList.Strings[n])) > 0  then
          begin
            Position:=n;
            Result:=True;
            Exit;
          end;
    end;
end;

function CompareStrWithRegExpr(Source, Pattern: String): Boolean;
var
  pSource: Array [0..255] of Char;
  pPattern: Array [0..255] of Char;

  function MatchPattern(element, pattern: PChar): Boolean;

    function IsPatternWild(pattern: PChar): Boolean;
    begin
      Result:=StrScan(pattern,'*') <> nil;
      if not Result then Result:=StrScan(pattern,'?') <> nil;
    end;

  begin
    if 0 = StrComp(pattern, '*') then
      Result:=True
    else if (element^ = Chr(0)) and (pattern^ <> Chr(0)) then
      Result:=False
    else if element^ = Chr(0) then
      Result:=True
    else
      begin
        case pattern^ of
          // * token
          '*': if MatchPattern(element, @pattern[1]) then
                 Result:=True
               else
                 Result:=MatchPattern(@element[1], pattern);
          // ? token
          '?': Result:=MatchPattern(@element[1], @pattern[1]);
          // el restu
          else
            if element^ = pattern^ then
              Result:=MatchPattern(@element[1], @pattern[1])
            else
              Result:=False;
        end;
      end;
  end;

begin
  StrPCopy(pSource,source);
  StrPCopy(pPattern,pattern);
  Result:=MatchPattern(pSource,pPattern);
end;

function GetToken(Text, Separator: String; Token: Integer): String;
var
  Position: Integer;

begin
  while Token > 1 do begin
    Delete(Text, 1, Pos(Separator,Text)+Length(Separator)-1);
    Dec(Token);
  end;
  Position:= Pos(Separator, Text);
  if Position = 0
  then  Result:= Text
  else  Result:= Copy(Text, 1, Position-1);
end;

function GetTokenCount(Text, Separator: String): Integer;
var
  Position: Integer;
  
begin
 if Text <> '' then begin
   Position:= Pos(Separator, Text);
   Result:= 1;
   while Position <> 0 do begin
     Inc(Result);
     Delete(Text, 1, Position+Length(Separator)-1);
     Position:= Pos(Separator, Text);
   end;
 end else
   Result:=0;
end;

procedure GetParameterList(Params, Separator: String; const ParamsList: TStrings);
var
  n, ParamsCount: Integer;

begin
  if Assigned(ParamsList) then
    begin
      // clear prev. params
      ParamsList.Clear;
      // get params count
      ParamsCount:=GetTokenCount(Params, Separator);
      // get all params
      for n:=1 to ParamsCount do
        ParamsList.Add(GetToken(Params, Separator, n));
    end;
end;

function GetUrlParameters(const URL: String): String;
begin
  Result:=Copy(URL, Pos('?', URL) + 1, Length(URL));
end;

function FileSizeToString(Bytes: Integer): String;
begin
  Result:=Format('%d bytes', [Bytes]); // Bytes
  // The file size is major than bytes
  if Bytes / 1024 < 1024 then
    Result:=Format('%.2f KB', [Bytes / 1024]) // KB
  else if Bytes / 1024 / 1024 < 1024 then
    Result:=Format('%.2f MB', [Bytes / 1024 / 1024]) // MB
  else if Bytes / 1024 / 1024 / 1024 < 1024 then
    Result:=Format('%.2f GB', [Bytes / 1024 / 1024 / 1024]); // GB
end;

function SecondsToHMS(Seconds: Integer; AllowNegative: Boolean; Compatct: Boolean): String;
begin
  // if user dont want negative values then
  if not AllowNegative and (Seconds < 0) then
    Seconds:=0;

  if not Compatct then
    // get the h m s format
    Result:=Format('%d%s %d%s %d%s', [Seconds div 3600, 'h',
                                      Seconds div 60 mod 60, 'm',
                                      Seconds mod 60, 's'])
  else
    begin
      // hours
      if Seconds div 3600 <> 0 then
        Result:=Format('%d%s', [Seconds div 3600, 'h ']);
      // minutes
      if (Seconds div 60 mod 60 <> 0) or (Result <> '') then
        Result:=Result + Format('%d%s', [Seconds div 60 mod 60, 'm ']);
      // seconds
      Result:=Result + Format('%d%s', [Seconds mod 60, 's']);
    end;
end;

function CalculatePercent(const Value, Total: Real): Real;
begin
  if Total <> 0.0 then
    Result:=Value / Total * 100
  else
    Result:=0.0;
end;

function HTMLCharToHTMLCode(HTML: String): String;
var
  Index: Integer;

begin
  // get the table index
  if TryStrToInt(CopyBetween(HTML, '#', ';'), Index) then
    case Index of
      34: Result:=GetToken(HTML_Char_Conversion_Table[1], '=', 2);
      38: Result:=GetToken(HTML_Char_Conversion_Table[2], '=', 2);
      60: Result:=GetToken(HTML_Char_Conversion_Table[3], '=', 2);
      62: Result:=GetToken(HTML_Char_Conversion_Table[4], '=', 2);
    else
      Result:=GetToken(HTML_Char_Conversion_Table[100 - (255 - index)], '=', 2);
    end
  else // ouch!
    Result:='';
end;

function HTMLCodeToHTMLChar(HTML: String): String;
var
  n: Integer;

begin
  for n:=1 to High(HTML_Char_Conversion_Table) do
    if GetToken(HTML_Char_Conversion_Table[n], '=', 2) = LowerCase(HTML) then
      begin
        Result:=GetToken(HTML_Char_Conversion_Table[n], '=', 1);
        Exit;
      end;
  // ouch!
  Result:='';
end;

function CharToHTML(AChar: Char; HTMLCodes: Boolean): String;
var
  HtmlChar: String;

begin
  if AChar in [' ', '!', '#'..'%', #39..';', '=', '?'..'~'] then // is a standard char
    Result:=AChar
  else // is a "rare" char (nonstandard)
    begin
      // decimal char
      Result:='&#' + IntToStr(Word(AChar)) + ';';
      // use html codes?
      if HTMLCodes then
        begin
          // get the html char code
          HtmlChar:=HTMLCharToHTMLCode(Result);
          // we have results?
          if HtmlChar <> '' then
            Result:=HtmlChar;
        end;
    end;
end;

function StrToHTML(Str: String; HTMLCodes: Boolean): String;
var
  n: Integer;

begin
  Result:='';
  for n:=1 to Length(Str) do
    Result:=Result + CharToHTML(Str[n], HTMLCodes);
end;

function HTMLToChar(HTML: String): Char;
var
  CharString: String;
  CharValue: Integer;

begin
  // try to convert the html code to html decimal
  CharString:=HTMLCodeToHTMLChar(HTML);
  // if we do not have results then get the original HTML passed by parameters
  if CharString = '' then
    CharString:=HTML;
  // start the conversion
  // delete -> &#
  //Delete(CharString, 1, 2);
  CharString:=CopyBetween(CharString, '#', ';');
  // convert
  if TryStrToInt(CharString, CharValue) then
    if CharValue < 255 then
      Result:=Char(CharValue)
    else
      Result:=' '
  else
    Result:=' ';
end;

function HTMLToStr(HTML: String): String;
var
  HTMLChar: String;
  n: Integer;

begin
  Result:='';
  // parse html text
  while Pos('&', HTML) > 0 do
    begin
      // copy text & delete it from HTML
      n:=Pos('&', HTML);
      Result:=Result + Copy(HTML, 1, n - 1);
      Delete(HTML, 1, n - 1);
      // get the HTML char & delete it from HTML
      n:=Pos(';', HTML);
      if n = 0 then begin Result:=Result + HTML; Exit; end;
      HTMLChar:=Copy(HTML, 1, n);
      Delete(HTML, 1, n);
      // convert
      Result:=Result + HTMLToChar(HTMLChar);
    end;
  // all has been parsed?
  if HTML <> '' then
    Result:=Result + HTML;
  // did something?
  if Result = '' then
    Result:=HTML;
end;

function FixFileName(FileName: String; ReplaceFor: String): String;
var
  n: Integer;

begin
  Result:='';
  for n:=1 to Length(FileName) do
    if FileName[n] in ['\', '/', ':', '*', '?', '"', '<', '>', '|', #$D, #$A] then
      Result:=Result + ReplaceFor
    else
      Result:=Result + FileName[n];
end;

function GetFileSize(FileName: String): Int64;
var
  TheFile: TFileStream;

begin
  if FileExists(FileName) then
    begin
      TheFile:=TFileStream.Create(FileName,  fmOpenRead);
      try
        Result:=TheFile.Size;
      finally
        TheFile.Free;
      end;
    end
  else
    Result:=0;
end;

function PositionToTPoint(Left, Top: Integer): TPoint;
begin
  Result.X:=Left;
  Result.Y:=Top;
end;

function UniqueFilename(AFilename : String) : String;
var
  Path,
  Filename,
  Ext: String;
  Index: Integer;

begin
  Result:=aFilename;
  if FileExists(aFilename) then begin
    Path:=ExtractFilepath(aFilename);
    Ext:=ExtractFileExt(aFilename);
    Filename:=ExtractFilename(aFilename);
    if Length(Ext) > 0 then
      Filename:=Copy(Filename, 1, Length(Filename) - Length(Ext));
    Index:=2;
    repeat
      Result:=Path + Filename + '(' + IntToStr(Index) + ')' + Ext;
      Inc(Index);
    until not FileExists(Result);
  end;
end;

function ApplicationPath: String;
begin
  Result:=IncludeTrailingPathDelimiter(ExtractFilePath(ParamStr(0)));
end;

function UniversalPath(const Path: String): String;
begin
  Result:=Path;
  // convert linux path delimeters to current path delimiter
  {$IFDEF FPC}AnsiReplaceStr{$ELSE}ReplaceStr{$ENDIF}(Result, '\', PathDelim);
  // convert windows path delimeters to current path delimiter
  {$IFDEF FPC}AnsiReplaceStr{$ELSE}ReplaceStr{$ENDIF}(Result, '/', PathDelim);
end;

{$IFNDEF FPC}
function WinExecAndWait32(Path: PChar; Visibility: Cardinal): Cardinal;
var
  WaitResult: Integer;
  StartupInfo: TStartupInfo;
  ProcessInfo: TProcessInformation;
  
begin
  FillChar(StartupInfo, SizeOf(TStartupInfo), 0);
  with StartupInfo do
    begin
      cb:=SizeOf(TStartupInfo);
      dwFlags:=STARTF_USESHOWWINDOW or STARTF_FORCEONFEEDBACK;
      wShowWindow:=visibility;
    end;
    if CreateProcess(nil,path,nil, nil, False, NORMAL_PRIORITY_CLASS, nil, nil,
		  StartupInfo, ProcessInfo) then
      begin
        WaitResult:=WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
        Result:=WaitResult;
      end
    else
      Result:=GetLastError;
end;
{$ENDIF}

end.
