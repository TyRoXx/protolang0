How to install p0 syntax highlighting for the Geany text editor
---------------------------------------------------------------

- place filetypes.Protolang.conf in ~/.config/geany/filedefs/
- edit ~/.config/geany/filetype_extensions.conf
- if the file already exists:
  - below [Extensions] add the following line to associate the .p0 file ending
    with filetypes.Protolang.conf :
      Protolang=*.p0;
  - if filetype_extensions.conf does not exist:
    create filetype_extensions.conf with the following content:

[Extensions]
Protolang=*.p0;

- restart geany and check if p0 files are displayed in color now
