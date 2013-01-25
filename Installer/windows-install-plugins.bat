set oldDir=%CD%
if not "%1" == "" cd "%1"
rmdir "..\bin\plugins" /s /q
mkdir "..\bin\plugins"
FOR /R "..\resources" %%v IN (*.js) DO copy "%%v" "..\bin\plugins"
cd %oldDir%