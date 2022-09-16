# Stop-Watch-System

Implementation of a Stopwatch system with the specifications listed below: 
1.	Use ATmega16 Microcontroller with frequency 1Mhz. 
2.	Configure Timer1 in ATmega16 with CTC mode to count the Stopwatch time. 
3.	Use six Common Anode 7-segments. 
4.	Connect the six 7-segments in the project using the multiplexed technique. 
5.	You should use one 7447 decoder for all 7-segments and control the enable/disable for each 7-segement using a NPN BJT transistor connect to one of the MCU pins. 
6.	Connect 7447 decoder 4-pins to the first 4-pins in PORTC. 
7.	Use first 6-pins in PORTA as the enable/disable pins for the six 7-segments. 
8.	Stopwatch counting should start once the power is connected to the MCU. 
9.	Configure External Interrupt INT0 with falling edge. Connect a push button with the internal pull-up resistor. If a falling edge detected the Stopwatch time should be reset. 
10.	Configure External Interrupt INT1 with raising edge. Connect a push button with the external pull-down resistor. If a raising edge detected the Stopwatch time should be paused. 
11.	Configure External Interrupt INT2 with falling edge. Connect a push button with the internal pull-up resistor. If a falling edge detected the Stopwatch time should be resumed.
