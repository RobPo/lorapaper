LoraPaper Arduino Library
===============================================================

Welcome to the docs! This is a Library for LoraPaper, a Lora-connected 1.1” E-Paper node which is powered by PV cells and thus running battery-free!


![LoraPaper](https://user-images.githubusercontent.com/21104467/71291687-94a0d180-2373-11ea-8e2a-f3cf7f9ac0ab.png)  
[*LoraPaper*](https://twitter.com/Paperino_io)

### So, what is Paperino?

Paperino is an easy to use micro EPD breakout-board for the Photon or other Arduino-compatible microcontrollers. The eInk-based ePaper display mimics the appearance of natural paper and is capable of holding text and images indefinitely, even without electricity. This makes Paperino perfect for your next battery-driven, connected project where the display content changes rarely.

Paperino also integrates an easy to use accelerometer. It extends the EPD by tap-sensing functions and offers portrait/landscape detection. Furthermore you can wake-up your MCU after movement-based events from deep-sleeping, saving battery live during waiting phases.


Featured In
-------------------
![As Featured In](https://user-images.githubusercontent.com/21104467/29744858-a7aaefba-8aad-11e7-8f06-d41a8eaac7fb.png)

Documentation
--------------
This is the place to get started with your new hardware! We have divided the documentation into the following sections:


* **[Hookup Guide](https://robpo.github.io/Paperino/)** - Step-by-step instructions to get your Paperino setup and run within minutes.
* **[Examples](https://robpo.github.io/Paperino/exampleHelloWorld/)** - Ready to use examples for your own inspiration.
* **[Reference](https://github.com/RobPo/Paperino/tree/gh-pages/datasheets)** - Datasheets & Application Notes.
* **[Hardware](https://github.com/RobPo/Paperino/tree/gh-pages/hardware)** - Schematics & PCB layouts.


How To Use
-------------------

### Installation

This library is part of the Particle and Arduino **Library Manager**. Please search for `PL_microEPD` within the Manager and download the library, that’s it! Included are six example sketches, they are useful to learn the basics about this ePaper screen.

### Hardware hookup

To start communicating with the ePaper driver IC, you’ll need to supply 3.3V and four more wires for SPI communication (@3.3V voltage level). Please have a look at the [hook-up guide](https://robpo.github.io/Paperino/) for more detailed information. 

### Example: Hello World!

This is the [first](https://robpo.github.io/Paperino/exampleHelloWorld/), and shortest possible demo and shows how to address the ePaper (don’t forget to update the GPIO name if needed in line 3):


```cpp
#include "Adafruit_GFX.h"
#include "PL_microEPD.h"

#define EPD_CS      A2
PL_microEPD display(EPD_CS);  

void setup() {  
    SPI.begin();                    
    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  
    display.begin();                
    display.print("Hello World!");
    display.update();               
}

void loop() {              
}
```

You should now be able to see the ePaper screen updating. Congratulation! If you feel more like a pro’ this is now the time to [add the wiring](https://robpo.github.io/Paperino/) of two more GPIO lines (called ‘busy’ & ‘reset’). They are needed to run the image updates a bit faster and/or to reset the IC after having it set to deep sleep.


### More…

Once this example is successfully running, please have a look at the following sketches: [GFX demo](https://robpo.github.io/Paperino/exampleGFXdemo/) shows how to draw all the letters, rectangles and dots. The [Graylevel](https://robpo.github.io/Paperino/example4GLs/) sketch explains how to use the four different graylevels in your next project. [Update modes](https://robpo.github.io/Paperino/exampleUpdateMode/) describes three different ways of updating an ePaper screen.

Distributors
-------------------
Paperino ePaper displays are available at [Crowd Supply](https://www.crowdsupply.com/robert-poser/paperino), [Watterott electronic](http://www.watterott.com/de/Paperino-A-micro-ePaper-with-accelerometer), [Mouser](https://www2.mouser.com/Search/Refine.aspx?Keyword=paperino) and [DigiKey](https://www.digikey.com/products/en?keywords=paperino).

Projects
-------------------
These demo projects are for your inspiration! What will you implement with Paperino? Tell us, we’ll love to add your project here!


[![Project TrafficIndicator at Hackaday.io](https://user-images.githubusercontent.com/21104467/29744843-761172a8-8aad-11e7-8e60-a01fffbf1ea6.png)](https://hackaday.io/project/22002-trafficindicator-between-workhome-or-vice-versa) 
[![Project IoT Connected Desk Frame at Hackaday.io](https://user-images.githubusercontent.com/21104467/29744849-891f23a4-8aad-11e7-8b91-d1db8494546d.png)](https://hackaday.io/project/21638-iot-connected-picturedesk-frame) 
[![Project local rain forecast for fair weather cyclists at Hackster.io](https://user-images.githubusercontent.com/21104467/29744854-98f1e4b0-8aad-11e7-8356-7e5f55b207e4.png)](https://www.hackster.io/robert-poser/epaper-based-local-rain-forecast-for-fair-weather-cyclists-cb168c)  
[![Project Industrial Tap Tap Machine at Hackster.io](https://user-images.githubusercontent.com/21104467/32456267-4848ab9c-c325-11e7-8ec6-64f17dcb3cfc.png)](https://www.hackster.io/robert-poser/tap-tap-automat-958d9c)

Changelog
-------------------
- [v1.1.01 (03/2018)](https://github.com/RobPo/Paperino/archive/v1.1.01.zip) - Add support for more EPDs: 1.4", 2.1" & 3.1" (auto-detection) and for deepSleep() of driver IC
- [v1.0.20 (10/2017)](https://github.com/RobPo/Paperino/archive/v1.0.20.zip) - Add support for (optional) simplified wiring for MCUs with limited GPIOs
- [v1.0.10 (08/2017)](https://github.com/RobPo/Paperino/archive/v1.0.10.zip) -  Particle & Arduino Library Manager supported; tap sensitivity increased; ‘verified’ by Particle
- [v1.0.00 (07/2017)](https://github.com/RobPo/Paperino/archive/v1.0.06.zip) - Initial release


License Information
-------------------

This library is _**open source**_!

Created by Robert Poser, Mar 4th 2018, Dresden/Germany. Released under BSD license (3-clause BSD license), check license.md for more information.

We invested time and resources providing this open source code, please support Paperino and 
open source hardware @Adafruit and by purchasing this product @Crowdsupply @Watterott @Plasticlogic.

If you like this project please [follow us on Twitter](https://twitter.com/paperino_io).
Having problems or have awesome suggestions? Contact us: paperino.display@gmail.com.
