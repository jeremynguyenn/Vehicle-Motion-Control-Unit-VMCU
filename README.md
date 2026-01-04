# TeR_ECU
TeR Electronic Control Unit, which handles the behaivour of the cars dynamic system while gathering all the sensor and data neccesary for operation through its inputs and communication buses.

## Architecture
- UML SCU Firmware Sequence Diagram: System Initialization and Data Flow
<img width="3515" height="2069" alt="SCU_System_Diagram_UML" src="https://github.com/user-attachments/assets/94267902-22ec-490d-b3cf-36e15379422b" />


## Features
- STM32F405VGTx Microcontroller (Cortex™-M4 Core@168mHz with FPU)
- USB For Diagnosis Operation
- 2x CAN 2.0 For its communication with the Powertrain CAN and main sensors CAN
- NEO M9N GPS modules from u-blox, for determining cars position (Posibility to drive an active Antenna)
- 9DOF IMU consisting in Accelerometer, Gyroscope and Magnetometer for accurate posting and torque algorithms
- 4 Digital Inputs (0V-24V Range)
- 4 PWM Outputs(3.3V), for servo control, including actuators
- 4 Analog Inputs (0V-3.3V) Possibility of configurable Input divider
- 4 Digital Outputs(0V-24V) High side mosfet drivers
- 2 WS2812 RGB Led Channels using integrated SPI for FS-Spain LightShow Acceleration


