# MOOS-pSBSUS
This is a connector between an S.BUS input (as from a remote control) and MOOS. This code is based on https://github.com/zendes/SBUS and https://os.mbed.com/users/Digixx/notebook/futaba-s-bus-controlled-by-mbed/. It is designed to run on a Beaglebone Blue, but it should work on any Linux machine with at least one TTL serial input. It has only been tested with the Beaglebone Blue, however. 

## Dependencies
* MOOS-IvP
* https://github.com/Tencent/rapidjson/ -- provides JSON parse/deparse

## S.BUS Fundamentals
S.BUS is the odd serial bus that Futaba (and compatible) R/C receivers and transceivers use for local communications. It runs at 100 kbps, inverted, 8 bits, even parity, 2 stop bits (8E2). Since, to the best of my knolwedge, there's no way to invert the serial input to the Beaglebone in software, an external inverter must be used. A single NPN BJT or N-channel MOSFET wired open collector/drain with a 3.3V pullup on the output will do the the trick.

Each message is 25 bytes long. This consistes of a start byte, 16 proportional channels, 2 binary channels, and an end byte. 

## Configuration Parameters
* SBUS_Port -- a text field giving the UNIX device that this progam should listen to. It defaults to ```/dev/ttyO4```, which is the DSM port on a Beaglebone Blue. 
* SBUS_MaxValue -- the maximum raw value output on each channel -- scaled to 1.0f
* SBUS_MinValue -- the maximum raw value output on each channel -- scaled to -1.0f


## Published Variables
* SBUS_json -- each received frame as a STRING containing JSON with the following schema:
```
{
	"$schema": "http://json-schema.org/schema#",
	"id": "SBUS_output",
	"type": "object",
	"properties":{
		"proportional":{
			"type": "array",
			"items": {"type":"integer"},
			"minItems":16,
			"maxItems":16,
			"$comment":"This stores the 16 channels of the S.Bus signal"
		},
		"scaled":{
			"type": "array",
			"items": {"type":"number"},
			"minItems":16,
			"maxItems":16,
			"$comment":"This stores the 16 channels of the S.Bus signal"
		},
		"digital17":{"type":"boolean"},
		"digital18":{"type":"boolean"},
		"failsafe":{"type":"boolean"},
		"$comment":"failsafe is true if the S.Bus receiver is in failsafe mode"
	},
	"required": ["proportional", "scaled", digital16", "digital17", "failsafe"]
}
```
* SBUS_Channels -- a STRING containing a json array of the proportional values of the first sixteen channels in microseconds. 
* SBUS_Scaled_Channels -- a STRING containing a json array of the scaled values of the first sixteen channels in a range from -1.0f to 1.0f. 
* SBUS_Ch17 -- a BINARY containing the received value of the channel
* SBUS_Ch18 -- a BINARY containing the received value of the channel
* SBUS_Failsafe -- a BINARY that is true if the receiver is in failsafe mode and false otherwise. 
* SBUS_GoodFrames -- a DOUBLE containing the number of good frames received
* SBUS_BadFrames -- a DOUBLE containing the number of bad frames received
