LoraPaper Arduino Library
===============================================================

Welcome to the docs! This is an Arduino Library for LoraPaper, a TTN-connected 1.1” E-Paper node which is powered by ambient light and thus energy-autark. 


![lorapaper2](https://user-images.githubusercontent.com/21104467/71319613-09bdf500-24a1-11ea-9b56-77f6731cf4ea.png) 
[*LoraPaper*](https://twitter.com/Paperino_io)

Documentation
--------------
This is the place to get started! We have divided the documentation into the following sections:


* **[Examples](https://github.com/RobPo/lorapaper/tree/master/examples)** - Ready to use examples for your own inspiration.
* **[Reference](https://github.com/RobPo/lorapaper/tree/master/datasheets)** - Datasheets.
* **[Hardware](https://github.com/RobPo/lorapaper/tree/master/schematics)** - Schematic.

How To Use
-------------------

### Installation

Please download and start the latest Arduino IDE. Select "Arduino Mini" in the menu Tools/Board; if not available please download this board from the Boards Manager. Please select "Atmega328p" in the Tools/Processor menu. Now please download and import the examples of this repository.

### Hardware hookup

Connect your FTTI programmer to LoraPaper through the pins exposed on the bottom side; either directly (left) or through a breadboard and with soldered pin row (right):
![ftdi2](https://user-images.githubusercontent.com/21104467/71321525-9de88600-24ba-11ea-95f3-0afbbc627986.jpg)


Examples
-------------------
These demo projects are for your inspiration! What will you implement with LoraPaper? Tell us, we’ll love to add your project here!

![weatherforecast](https://user-images.githubusercontent.com/21104467/71322107-f3756080-24c3-11ea-96c5-fdd6a71fff85.jpg)

License Information
-------------------

This library is **open source**!

Libraries used in this sketch are based on the LoRaWAN stack from IDEETRON/NEXUS, for more infos please check this great source: https://github.com/Ideetron/Nexus-Low-Power

Libraries used in this sketch around the ePaper and the Example sketches are created by Robert Poser, Dez 19th 2019, Dresden/Germany. 

Released under GNU Lesser General Public License, either version 3 of the License, or (at your option) any later version, check license.md for more information.

We invested time and resources providing this source code, please support open source hardware and software @Ideetron, @Adafruit, @Watterott and others.

If you like this project please [follow us on Twitter](https://twitter.com/paperino_io).
Having problems or have awesome suggestions? Contact us: paperino.display@gmail.com.
