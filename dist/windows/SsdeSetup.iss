[Setup]
AppId={{5D1BF4DD-8593-48B3-96CA-1225513733D7}
AppName=Ssde
AppVersion=1.0
AppContact=jocelyn.serot@uca.fr
AppCopyright=Copyright (C) 2019 J. Serot
AppPublisher=J. Serot / Clermont-Auvergne University
AppPublisherURL=http://github.com/jserot/ssde
LicenseFile=E:/Qt/ssde/LICENSE
UsePreviousAppDir=false
DefaultDirName={pf}/Ssde
DefaultGroupName=Ssde
Compression=none
;Compression=lzma2
;SolidCompression=yes
OutputBaseFilename=Ssde_setup
OutputDir=.
WizardImageFile=E:\Qt\ssde\build\ssde.bmp
WizardSmallImageFile=E:\Qt\ssde\build\ssde.bmp

[Files]
Source: "E:\Qt\ssde\build\*"; DestDir: "{app}"; Excludes:"examples"; Flags: recursesubdirs
Source: "E:\Qt\ssde\build\examples\*"; DestDir: "{code:GetDirs|0}\SsdeExamples"; Flags: recursesubdirs
 
[Icons]
Name: "{group}\Ssde"; Filename: "{app}\ssde.exe";IconFilename: "E:\Qt\ssde\build\ssde.ico"
Name: "{group}\{cm:UninstallProgram,Ssde}"; Filename: "{uninstallexe}";IconFilename: "E:\Qt\ssde\build\ssdeun.ico"
Name: "{commondesktop}\Ssde"; Filename: "{app}\Ssde.exe";    IconFilename: "E:\Qt\ssde\build\ssde.ico"

[Run]
Filename: "{app}\ssde.exe"; Description: "Launch application"; Flags: postinstall nowait skipifsilent unchecked

[UninstallDelete]
Type: files; Name: "{app}\ssde.ini"

[Code]
var
  DirPage: TInputDirWizardPage;

function GetDirs(Param: String): String;
begin
  Result := DirPage.Values[StrToInt(Param)];
end;

procedure InitializeWizard;
begin
  { Directory input page for specifying where to store examples }
  DirPage := CreateInputDirPage(
    wpSelectDir,
    'Select Destination Location for the directory containing the examples',
    'Please choose a directory with read-write permissions',
    'Setup will install examples in the following directory',
    True,
    '');
  DirPage.Add('');
  DirPage.Values[0] := GetPreviousData('Directory1', ExpandConstant('{userdocs}'));
end;

procedure RegisterPreviousData(PreviousDataKey: Integer);
begin
  { store chosen directories for the next run of the setup }
  SetPreviousData(PreviousDataKey, 'Directory1', DirPage.Values[0]);
end;

procedure CurPageChanged(CurPageID: Integer);
begin
  if CurPageID=wpReady then
  begin
   Wizardform.ReadyMemo.Lines.Add(''); 
   Wizardform.ReadyMemo.Lines.Add('Directory containing the examples:');
   Wizardform.ReadyMemo.Lines.Add('    ' + DirPage.Values[0] + '\SsdeExamples');
 end;
end;
