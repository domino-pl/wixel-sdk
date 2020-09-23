Kompilacja pod Windowsem:
1. Zainstalować "Wixel Windows Drivers and Software" oraz "Wixel 
Development Bundle" for Windows dostępne pod adresem:
https://www.pololu.com/product/1336/resources
2. Pobrać jako ZIP(pod przyciskiem "Code") lub sklonować to 
repozytorium.
3. Uruchomić plik wsadowy make_all.bat

Kompilacja pod linuksem (testowane na Debian 10):
1. Zainstalować:
sudo apt install libusb-1.0-0-dev libqtgui4 sdcc
2. Pobrać jako ZIP(pod przyciskiem "Code") lub sklonować to 
repozytorium.
3. Wejść w terminalu do katalogu repozytorium.
4. Wydać polecenie:
make

Aby wgrać skompilowany program(plik wxl dostępny w podkatalogu danej 
aplikacji w katalogu apps) na moduł wixel posłużyć się
programem z pakietu "Wixel Software" dostępnym pod adresem:
https://www.pololu.com/product/1336/resources
dla danej platformy.


------------------------------------------------------------------------


Pololu Wixel Software Development Kit (SDK)

This package contains code and Makefiles that will help you create
your own applications for the Pololu Wixel.  The Wixel is a general-
purpose programmable module featuring a 2.4 GHz radio and USB.  The
Wixel is based on the CC2511F32 microcontroller from Texas Instruments,
which has an integrated radio transceiver, 32 KB of flash memory, 4 KB
of RAM, and a full-speed USB interface.

For documentation of this SDK, see:
http://pololu.github.com/wixel-sdk

(You can also generate the documentation yourself by running
Doxygen on libraries/Doxyfile.)

For more information about the Wixel, see the Wixel User's Guide:
http://www.pololu.com/docs/0J46
