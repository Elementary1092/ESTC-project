<h1 align="center"><b>ESTC Project</b></h1>

<p>
	This is an application that may be used to control LEDs on a board. 
	Value of a LEDs may be set by using CLI interface, BLE interface or a button.
</p>

# Prerequisites
<ul>
	<li> C compiler [arm-none-eabi-gcc]
		(<a href="https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/gcc-arm-none-eabi-9-2019-q4-major-win32-sha2.exe?revision=ba95cefa-1880-4932-94d4-ebf30ad3f619&rev=ba95cefa1880493294d4ebf30ad3f619&hash=B19DC5BA5F85FC88399DE2F78BB7FBFD">Windows</a>) 
		(<a href="https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2?revision=108bd959-44bd-4619-9c19-26187abf5225&rev=108bd95944bd46199c1926187abf5225&hash=E367D388F6429B67D5D6BECF691B9521">Linux</a>)
		(<a href="https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/gcc-arm-none-eabi-9-2019-q4-major-mac.tar.bz2?revision=c2c4fe0e-c0b6-4162-97e6-7707e12f2b6e&rev=c2c4fe0ec0b6416297e67707e12f2b6e&hash=5129A9817C72711155CCB2EC11E049A2">Mac OS</a>)
	</li>
	<li> make </li>
	<li> nRF52840 (pca10059 board) </li>
	<li> <a href="https://github.com/DSRCorporation/esl-nsdk">nRF SDK 17</a> </li>
	<li> nRF Util - to flash the app (<a href="https://www.nordicsemi.com/Products/Development-tools/nrf-util">download link</a>) </li>
	<li> picocom or alternatives - to use CLI module or view logs </li>
	<li> nRF Connect </li>
</ul>

# Building
Unpack nRF SDK, go to armgcc directory, open Makefile and assign path to nRF SDK SDK_ROOT variable in the Makefile. Now you are ready to start building an applicaion. Type the following command to build and flash an application without CLI module enabled:

```
make clean
make dfu
```

To build and flash an application with CLI module enabled type:

```
make clean
make dfu ESTC_USB_CLI_ENABLED=1
```

# Usage
### BLE
To connect to a device, locate a device (named "SomeLongName" by default) in nRF Connect.
![image](https://github.com/Elementary1092/ESTC-project/assets/82331423/b4fe2ef0-41ed-4f36-a215-008d972612e0)

After connecting to a device there will be available 4 services.
![image](https://github.com/Elementary1092/ESTC-project/assets/82331423/76d50926-68d1-4286-844a-62abfcc6aa8c)

Generic Access and Generic Attirubute services are added automatically by the softdevice. 
The core service of the application is "Unknown Service". This service handles all requests regarding LEDs. User may try to change LED color, read current value of LEDs, and request notifications from the application about current LED values.
Bond Managment Service may be used to request an application to delete bonding information if the device is bonded. This may be done by writing to Bond Management Control Point a sequence starting with "03" and preceded by bond management service secret password ("41424344" for example). 
![image](https://github.com/Elementary1092/ESTC-project/assets/82331423/05c46a59-5b2c-4d29-b5b3-3d1345b003b3)
![image](https://github.com/Elementary1092/ESTC-project/assets/82331423/8bfa908a-fda3-419d-b8f3-29748321bbb1)

Every characteristic in "Unknown Service" is provided with its brief description which may be issued by reading from the corresponding "Characteristic User Description".
![image](https://github.com/Elementary1092/ESTC-project/assets/82331423/caa4283f-f7ed-447b-9ab0-8c4959ce1247)

A characteristic with ID 0x069B stores current RGB values.
It is possible to read and request notifications from this characteristic.
![image](https://github.com/Elementary1092/ESTC-project/assets/82331423/d05509da-1804-453b-a337-d2ff27d4c491)

Other three characteristic are used for seeting Red, Green, or Blue values. Device should be bonded and nRF Connect automically requires pairing if device was not bonded before 


### Button
The are several modes of editing the color of LEDs by using button. Modes:
* No input - all actions except double-click are ignored. 
* Edit hue - in "No input" mode double-click the button to move to "Edit hue" mode. Indicator LED should start blinking slowly. Hold the button to change LEDs' color.
* Edit saturation - in "Edit hue" mode double-click the button to move to "Edit saturation" mode. Indicator LED should start blinking faster. Hold the button to change saturation.
* Edit brightness - in "Edit saturation" mode double-click the button to move to "Edit brightness" mode. Indicator LED should stop blinking and should emit light constantly. Hold the button to change saturation. Double-click the button to move to "No input". Indicator should turn off if this transition was successful.

When the button is used to change color, the information about color is saved to the flash 
only when application moves from "Edit brightness" mode to "No input" mode.

### Logs
To view logs using picocom it is needed to know the port to which the board is connected.
For example, if the board is connected to ttyACM0, on Linux machine type the following command to view logs:
```
picocom /dev/ttyACM0
```

To exit picocom press Ctrl + A followed by Ctrl + X.

cat utility also may be used to view logs:
```
cat /dev/ttyACM0
```
