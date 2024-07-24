## install ROOT [v6.30 currently checked] 

install libusb library:

`sudo apt-get install libusb-1.0-0-dev`

## download/copy CAEN libraries: 
https://www.caen.it/download/

CAENVMELib (v3.4.4), CAENComm (v1.6.0) and library for DT5770 (CAENDPPLib)


## install CAEN libraries 
`sudo ./install_64`

in /lib folder of every library (CAENVMELib, CAENComm and CAENDPPLib)

## copy and complie DSRoot
`git clone -b master https://github.com/DrKazlou/DSRoot.git`

- go to DSRoot folder and compile program by typing **make**
- launch DSRoot: ./DSRoot
- Have fun.
