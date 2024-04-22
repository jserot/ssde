[Setup]
AppId={{5D1BF4DD-8593-48B4-96CA-0225513733D9}
AppName=ssde
AppVersion=0.3
UsePreviousAppDir=false
DefaultDirName={commonpf}/ssde
DefaultGroupName=ssde
Compression=none
;Compression=lzma2
;SolidCompression=yes
OutputBaseFilename=ssde_setup
OutputDir=.

[Files]
Source: "*"; DestDir: "{app}"; Flags: recursesubdirs; Excludes: "*~,Makefile,*.iss,*_setup.exe"
 
[Icons]

[Run]
Filename: "{app}\ssde.exe"; Description: "Launch application"; Flags: postinstall nowait skipifsilent unchecked

[INI]

[UninstallDelete]

