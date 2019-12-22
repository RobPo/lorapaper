Example: MinimalTemplate
-------------------
This MinimalTemplate demonstrates a simple counter, being updated everytime if there is sufficient energy harvested. The supercap voltage v_scap is measured minutely while the ATmega328p processor is in deep sleep all remaining time. Triggering is done via external RTC to minimize current consumption during deep sleep phase. IF the voltage is charged above a certain limit (ie 4.2V), an image update is triggered. 

Example: WeatherForecast
-------------------

### 1) Register LoraPaper at your TTN Console

Now its time to add LoraPaper as new device to the TTN Console. After login, please select 'application' and 'register device'. Please add the Device ID and Device EUI  in the fields as provided with LoraPaper and select 'Register'. Then please reselect your newly added device and open the 'settings' view again. You will now see the new generated keys, please copy Device Adress, Network Session Key and the App Session Key since they will be added into the source code lateron. Finally, select 'ABP' as activation methode and deactivate 'frame counter checks'.

### 2) Schedule downloadable data at your TTN console with NodeRED

There are several ways to add data to the TTN console, which can then be downloaded to your node once it comes online the next time. In this example the shown Node-RED flow is implemented; you can continue to use it or build your own flow.

![noderead2](https://user-images.githubusercontent.com/21104467/71321637-c1accb80-24bc-11ea-906a-3cce3634e421.jpg)

Requesting a new weather API call can be triggered via a predefined timer (timer, i.e. hourly) or after the node came online and just downloaded the present weather data (ttn uplink). With this setup we assure that there is always weather data available, no matter when the node came back online the last time. The orange box extract payload now parses the received html code and extracts only the relevant weather data we are interested in. As a result, the data size decreases from several kilobytes down to less than ten bytes. This payload is now configured in ttn-downlink to be downloaded to our weather station when it is online next time.

![scheduled](https://user-images.githubusercontent.com/21104467/71321656-2a944380-24bd-11ea-957f-c4cb6a82c611.jpg)

### 3) Add the Encoder function to your TTN backend

Next we need to add an encoder function, which converts the weather data to a bytestream which can be sent during the next donwlink message. Please fill in the data from the file 'encoder.h' to the field in Application/Payload Format/Encoder.

### 4) Modify & Upload The Source Code

Open the example 'WeatherForecast' and select the tab 'lorawan_def.h'. Here you will need to add your previously generated keys:

![slora](https://user-images.githubusercontent.com/21104467/71319850-f1030e80-24a3-11ea-84f9-7d1ee86cc57c.jpg)

After compiling and successful upload you should be able to receive the first data!
