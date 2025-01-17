# udpIpPg
1GbE GMII UDP/IPv4 packet generator logic simulation component

The `udpIpPg` project is a set of verification IP for generating and receiving 1GbE UDP/IPv4 Ethernet packets over an GMII interface in a Verilog or VHDL test environment. A GMII/RGMII convertor module is also provide to support IP with the RGMII interface. The generation environment is a set of C++ classes, to generate packets in to a buffer and then send that buffer over the HDL GMII interface. The connection between the HDL and the C++ domain is done using the Virtual Processor, [VProc](https://github.com/wyvernSemi/vproc)&mdash;a piece of VIP that allows C and C++ code, compiled for the local machine, to run and access the Verilog or VHDL simulation environment, and VProc is freely available on github.

<p align="center">
<img src="https://github.com/user-attachments/assets/6161e969-f274-4501-8e98-84d945cce58c" width=800>
</p>

The intent for this packet generator is to allow ease of test vector generation when verifying 1G Ethernet logic IP, such as a MAC, and/or a server or client for UDP and IPv4 protocols. The bulk of the functionality is defined in the provided C++ classes, making it easily extensible to allow support for other protocols. It is also meant to allow exploration of how these protocols function, as an educational vehicle.

An example test environment is provided for various simulators (Questa, Vivado xsim, Verilator, Icarus, GHDL and NVC), with two packet generators instantiated connected to one another&mdash;one acting as a client and one acting as a server. Formatted output of received packets can be displayed during the simulation.

## Features

The basic functionality provided is as listed below

* A Verilog module or VHDL component, `udp_ip_pg`
    *	Clock input, nominally 125MHz (1×10<sup>9</sup> ÷ 8)
    *	GMII interface, with TX and RX data and control bits
        * A GMII/RGMII convertor module is also supplied for RGMII IP support
    *	A halt output for use in test bench control
*	A class to generate a UDP/IPv4 packet into a buffer
*	A class to send a generated packet over the GMII interface
*	A means to receive UDP/IPv4 packets over the GMII interface and buffer them
*	A means to display, in a formatted manner, received packets
*	A means to request a halt of the simulation (when no more test data to send)
*	A means to read a clock tick counter from the software
