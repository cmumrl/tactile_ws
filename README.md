# Microrobotics Lab Tactile Sensinsing Module

This Repository contains: 

1. Links to the previous works related to the sensors
2. Instructions for using the module along with the the microrobotics lab's 3 axix tactile sensors

## Install

This project uses the Arduino microcontroller to read capacitance data from the AD7746 chip. Please make sure that the Arduino IDE is installed and the microcontroller has the necessary rights.

### Windows and Mac users
For Windows and Mac users microcontrollers don't require any additional access. No additional packages are required for accessing the sensor data.


### Ubuntu users
For Ubuntu users, the microcontrollers need additional USB access. Use the command below to grant them the USB access.

```sudo chmod 666 /dev/ttyUSB0```

Please make sure to give the correct USB address. To check the USB connected to the Arduino use the following command.

```
lsusb
```

Ubuntu users can use ROS module to publish sensor data on a ROS topic. TO use the ROS module additional packages are required. The essentials.sh script will install all the essential packages required for using ROS module. Follow the command below to run the script successfully.

```
sudo chmod +x /path/to/script/essenitals.sh
sudo /path/to/script/essentials.sh
```

## Usage

### Windows and Mac users

Go to the Arduino IDE and upload the `AD7746_clean_standalone.ino` code on the controller. Run the code and the IDE will start publishing the sensor data on the serial monitor.

### Linux users (ROS specifically)

The `tactile_ws` is the catkin workspace for the ROS users. Go to the Arduino IDE and upload the `AD7746_clean.ino` code on the controller.

Either copy the `shear_sensor` package in your specific catkin_ws or use the `tactile_ws` itself. Catkin_make the package and source the `devel` folder. To publish the sensor data on the `/capacitance_val` topic follow the command below.
```
roslaunch shear_sensor shear.launch
```
The module should start publishing the unprocessed capacitance data on `/chatter` and the processed capacitance data on `/capacitance_val`.

## Maintainers
@rishabhagarwal880

