# ledger-app-amity

Build from source:  
Install dependencies: 
apt install pcsc-tools pcscd libpcsclite1 libudev-dev libusb-1.0-0-dev python-dev virtualenv libusb-dev libudev-dev  

You have to have to add the Nano S to /etc/libccid_Info.plist  
In <key>ifdVendorID</key> add the entry <string>0x2C97</string>  
In <key>ifdProductID</key> add the entry <string>0x0001</string>  
In <key>ifdFriendlyName</key> add the entry <string>Ledger Token</string>  
These 3 entries must be added at the end of each list.  

Create the following udev rules /etc/udev/rules.d/99-ledger.rules
SUBSYSTEMS=="usb", ATTRS{idVendor}=="2c97", ATTRS{idProduct}=="0000", MODE="0660", TAG+="uaccess", TAG+="udev-acl" GROUP="plugdev"  
SUBSYSTEMS=="usb", ATTRS{idVendor}=="2c97", ATTRS{idProduct}=="0001", MODE="0660", TAG+="uaccess", TAG+="udev-acl" GROUP="plugdev"  
