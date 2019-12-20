LoraPaper Arduino Library
===============================================================

Welcome to the docs! This is an Arduino Library for LoraPaper, a TTN-connected 1.1” E-Paper node which is powered by harvesting ambient light and thus energy-autark. Say 
goodbye to recharging or replacing empty batteries!


![LoraPaper](https://user-images.githubusercontent.com/21104467/71291687-94a0d180-2373-11ea-8e2a-f3cf7f9ac0ab.png)  
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


License Information
-------------------

This library is _**open source**_!

Libraries used in this sketch are based on the LoRaWAN stack from IDEETRON/NEXUS, for more infos please check this great source: https://github.com/Ideetron/Nexus-Low-Power

Created by Robert Poser, Dez 19th 2019, Dresden/Germany. Released under GNU Lesser General Public License, either version 3 of the License, or (at your option) any later version), check license.md for more information.

We invested time and resources providing this  source code, please support Paperino and 
open source hardware @Ideetron, @Adafruit and @Watterott.

If you like this project please [follow us on Twitter](https://twitter.com/paperino_io).
Having problems or have awesome suggestions? Contact us: paperino.display@gmail.com.
