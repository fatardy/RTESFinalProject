# RTES Final Project
Final Project for ECE-GY 6143 Real Time Embedded Systems by Dr. Matthew Campisi in New York University.
* Rituraj Singh ()
* Aradhya Alamuru (aa9405@nyu.edu)  

## Setup
* Do a search and change all `wait_ms` to `wait_us`, or you'll get this error:
 ```stm32f429i_discovery.c:(.text.IOE_Delay+0x0): undefined reference to `wait_ms'```

## Notes
* All the examples are from here https://platformio.org/lib/show/2208/BSP_DISCO_F429ZI/examples
* Checkout the installation tab to use the inbuilt terminal in vscode.
* Code for the touchscreen is in `touchscreen_demo.cpp`
