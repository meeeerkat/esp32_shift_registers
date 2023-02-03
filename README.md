
## A shift register component to drive shift registers on an ESP32
It provides a simple interface to drive many shift registers.  
Here a "shift register" can actually be multiple physical components chained together (2 x 8bits shift registers = 1 x 16bits shift register for the software).  

### Limitations
- For now, shift register chain that over 64bits long aren't supported.  
- /!\ The shift\_register variables shouldn't be local (pointer references are kept when sending values, once the values are send the variable may be "deallocated").


### TODO
- Throw an event when values have been sent
- Allow configuration of shift register clk (SRCLK) period ??
