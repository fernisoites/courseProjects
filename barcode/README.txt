=============================================
QR code scanner Windows interface description
=============================================

This program reads in input from QR code scanner, and triger the webpage only if the input is a valid url.

This is a program written in python, aiming at deploying on Windows machine, this version is not compatible with other operating system yet. To make it work on Windows machine without python dependency, the program was packaged by a library in python called "py2exe".

=============================================
                Installation
=============================================

Theoretically, nothing specific needs to be done to install. Just copy the whole 'dist' folder and put it anywhere convenient. Make a shortcut of 'read_qrcode.exe' in this folder on Desktop.

=============================================
                    Usage
=============================================

Open webpage:
1. Double click 'read_qrcode.exe' either on Desktop or in 'dist' folder, a prompt window would show up.
2. Scan QRcode, the webpage would be opened automatically.
3. For more QR code, just bring the prompt window and scan again.

Shut Down program:
1. type 'c' and press enter.
   or
   press 'Ctrl' and 'c' at the same time.

=============================================
           source code maintenance
=============================================

Original source codes are under root folder. 

'read_barcode.py':
    Main body of the program, simultaneously read from stdin, and call up webpage.

'setup.py':
    packaging scripts, need to be executed every time new changes were made in 'read_barcode.py'.
    under command line, type in:
      $ python setup.py py2exe
