set oldDir=%CD%
if not "%1" == "" cd "%1"
rmdir "..\bin\languages" /s /q
mkdir "..\bin\languages"
FOR /R "..\resources\translations" %%v IN (*.qm) DO copy "%%v" "..\bin\languages"
FOR /R "..\resources\translations" %%v IN (*.language) DO copy "%%v" "..\bin\languages"
del "..\bin\languages\template_for_new_translations.qm"
cd %oldDir%