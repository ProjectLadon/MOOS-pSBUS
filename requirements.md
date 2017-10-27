# 1.0 SRD005 {#SRD005 }

This package **shall** be implemented at a MOOS Appcasting App as descibed in the (http://oceanai.mit.edu/moos-ivp/pmwiki/pmwiki.php)[MOOS-IvP wiki].

## 1.1 SRD001 {#SRD001 }

This package *shall** read and interpret an SBUS serial stream. A valid SBUS stream **shall** have the following properties:

* **Serial Settings**: 100000, 8E2. Note that the native stream is inverted; this package **shall** assume that the inversion is done in external hardware.
* The stream **shall** be divided into 25 byte (200 bit) packets.
* The highest bit of each byte **shall** be transmited first
* Each valid packet **shall** begin with 0x0f and end with 0x00.
* Bytes 1 through 22 **shall** be divided into 16 11-bit fields. Each field **shall** be a big endian unsigned integer representing the channel position in microseconds.
* Byte 23 **shall** be a flag byte.
  * Bit 7 **shall** be digital channel 17
  * Bit 6 **shall** be digital channel 18
  * Bit 5 **shall** be the lost frame flag
  * Bit 4 **shall** be the failsafe flag (true if receiver connection lost)
  * Bits 0-3 **shall** be ignored.

## 1.2 SRD002 {#SRD002 }

This package **shall** maintain and publish goodFrame and badFrame counters. The goodFrame counter **shall** be incremented on the receipt of any frame that conforms to SRD001. The badFrame counter **shall** be incremented on the receipt of any frame or partial frame that does not conform.

> `requirements/SRD001.yml`

## 1.3 SRD003 {#SRD003 }

This package **shall** maintain and publish the following outputs, in addition to the goodFrame and badFrame counters mentioned in SRD002.

* **SBUS_json** *shall* be a JSON object containing all of the output data
* **SBUS_Channels** *shall* be a STRING containing a JSON array of the sixteen proportional channels, expressed in microseconds.
* **SBUS_Scaled_Channels** *shall* be a STRING containing a JSON array of the sixteen proportional channels scaled to the range -1.0f to 1.0f, inclusive.
* **SBUS_Ch17** *shall* be a BINARY containing the state of digital cahnnel 17.
* **SBUS_Ch18** *shall* be a BINARY containing the state of digital cahnnel 18.
* **SBUS_Failsafe** *shall* be a BINARY that is true if the R/C receiver is either not receiving a signal or out of range of the transmitter.
* **SBUS_GoodFrame** *shall* be a DOUBLE that reports the number of good SBUS frames received.
* **SBUS_BadFrame** *shall* be a DOUBLE that reports the number of bad SBUS frames received.

> `requirements/SRD002.yml`

## 1.3 SRD004 {#SRD004 }

This packge **shall** provide a MOOS AppCast containing the scaled values of all sixteen proportional channels, both digital channels, and the failsafe signal.

> `requirements/SRD003.yml`

## 1.5 SRD006 {#SRD006 }

This package **shall** listen for the SBUS stream on a port defined by the SBUS_Port configuration option. This option **shall** default to /dev/ttyO4 if it is not present.

## 1.6 SRD007 {#SRD007 }

The scaled channels **shall** be scaled to values of -1.0f to 1.0f where:
* -1.0f **shall** be equal to the SBUS_MinValue parameter set in the configuration file. This parameter **shall** default to 1000 microseconds if it is not present.
* 1.0f **shall** be equal to the SBUS_MaxValue parameter set in the configuration file. This parameter **shall** default to 2000 microseconds if it is not present.

> `requirements/SRD003.yml`

