LoraPaper Arduino Library
===============================================================

Welcome to the docs! This is an Arduino Library for LoraPaper, a TTN-connected 1.1” E-Paper node which is powered by harvested ambient light and thus energy-autark. 


(![lorapaper2](https://user-images.githubusercontent.com/21104467/71319613-09bdf500-24a1-11ea-9b56-77f6731cf4ea.png) 
[*LoraPaper*](https://twitter.com/Paperino_io)

How To Use
-------------------

### Installation

Please download and start the latest Arduino IDE. Select "Arduino Mini" in the menu Tools/Board; if not available please download this board from the Boards Manager. Please select "Atmega328p" in the Tools/Processor menu. Now please download and import the examples of this repository.

### Hardware hookup

Connect your FTTI programmer to LoraPaper through the pins exposed on the bottom side; either directly (see below) or through a breadboard and with soldered pin row (see above).
![ftdi](https://user-images.githubusercontent.com/21104467/71294001-6f15c700-2377-11ea-91f3-f41ed5e0c1be.jpg)

### Example: MinimalTemplate

This MinimalTemplate demonstrates a simple counter, being updated everytime if there is sufficient energy harvested. The supercap voltage v_scap is measured minutely while the ATmega328p processor is in deep sleep all remaining time. Triggering is done via external RTC to minimize current consumption during deep sleep phase. IF the voltage is charged above a certain limit (ie 4.2V), an image update is triggered. 

### Example: WeatherForecast
(coming soon)

#### Register LoraPaper at your TTN Console

Now its time to add LoraPaper as new device to the TTN Console. After login, please select 'application' and 'register device'. Please add the Device ID and Device EUI  in the fields as provided with LoraPaper and select 'Register'. Then please reselect your newly added device and open the 'settings' view again. You will now see the new generated keys, please copy Device Adress, Network Session Key and the App Session Key since they will be added into the source code lateron. Finally, select 'ABP' as activation methode and deactivate 'frame counter checks'.

#### Modify & Upload The Source Code

Open the example 'WeatherForecast' and select the tab 'lorawan_def.h'. Here you will need to add your previously generated keys:

![slora](https://user-images.githubusercontent.com/21104467/71319850-f1030e80-24a3-11ea-84f9-7d1ee86cc57c.jpg)


License Information
-------------------

This library is **open source**!

Libraries used in this sketch are based on the LoRaWAN stack from IDEETRON/NEXUS, for more infos please check this great source: https://github.com/Ideetron/Nexus-Low-Power

Created by Robert Poser, Dez 19th 2019, Dresden/Germany. Released under GNU Lesser General Public License, either version 3 of the License, or (at your option) any later version, check license.md for more information.

We invested time and resources providing this  source code, please support Paperino and 
open source hardware @Ideetron, @Adafruit and @Watterott.

If you like this project please [follow us on Twitter](https://twitter.com/paperino_io).
Having problems or have awesome suggestions? Contact us: paperino.display@gmail.com.
