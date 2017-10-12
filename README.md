# MOOS-S.Bus
A connector between an S.BUS input (as from a remote control) and MOOS. This code is based on https://github.com/zendes/SBUS and https://os.mbed.com/users/Digixx/notebook/futaba-s-bus-controlled-by-mbed/. It is designed to run on a Beaglebone Blue, but it should work on any Linux machine with at least one TTL serial input. It has only been tested with the Beaglebone Blue, however. 

# S.BUS Fundamentals
S.BUS is the odd serial bus that Futaba (and compatible) R/C receivers and transceivers use for local communications. It runs at 100 kbps, inverted, 8 bits, even parity, 2 stop bits (8E2). Since, to the best of my knolwedge, there's no way to invert the serial input to the Beaglebone in software, an external inverter must be used. A single NPN BJT or N-channel MOSFET wired open collector/drain with a 3.3V pullup on the output will do the the trick.

Each message is 25 bytes long. This consistes of a start byte, 16 proportional channels, 2 binary channels, and an end byte. 

# Configuration Parameters
* SbusPort

# Published Variables
* JSONPacket
* Ch{00-15}
* Ch16
* Ch17
* Failsafe
