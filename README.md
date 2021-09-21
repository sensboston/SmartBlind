# SmartBlind
**ESP32-based motorized "smart" window blind**

Motorized blinds are way to expensive (for that kind of home gadgets) so it's might be a good idea to build one by yourself.
Here is my implementation of motorized "smart" blind for living room window (my goal was to increase TV screen visibility at the daytime).
Of course, it's not an invention - you can find a lot of similar "arduino-based motorized blinds" guides and "how-to"'s on Google and YouTube; however, I provide you a good, clean and simple "you can do it" solution. 

Here is a BOM (bill of materials) I've used for hardware (with some exceptions: for example, I already bought esp32 SoC and IR kit on AliExpress much cheaper). Most pricey parts are blinds emselv and geared stepped motor. Probably, you can try to use cheap (but powerfull) DC motor but I'm unsure. DC motors are hard to work for that implementation, so I recommend you to use stepper. As for Nema 17 motor and A4988 driver, I strongly recommend you to read [this tutorial](https://lastminuteengineers.com/a4988-stepper-motor-driver-arduino-tutorial/).

| Part name | Amazon link |  Price |
| --- | --- | --- |
| Nema 17 Geared Stepper Motor (gear ratio 5.18:1) | https://www.amazon.com/gp/product/B07V359RFB | $28.29 |
| ESP32 development board | https://www.amazon.com/VKLSVAN-ESP-WROOM-32-Development-Dual-Mode-Microcontroller/dp/B07T6J3PXZ  |  $7.95 |
| A4988 stepper motor drivers (5 pieces) | https://www.amazon.com/gp/product/B07Q4J6LHT | $6.59 |
| 5mm to 8mm shaft coupler | https://www.amazon.com/gp/product/B07P6X8X7Y |  $5.99 |
| IR transmitter & receiver kit | https://www.amazon.com/Digital-Receiver-Transmitter-Arduino-Compatible/dp/B01E20VQD8 | $7.99 |
| PCB Board (two pieces, 8 x 12cm) | https://www.amazon.com/gp/product/B071135MB1 | $4.99 |
| DC converter 9V 12V (9V-22V) step down to DC 5V | https://www.amazon.com/gp/product/B0825TKY5M  | $5.86 |
| AC 100-240V to DC 12V 2A Switching Power Supply Adapter | https://www.amazon.com/gp/product/B07WH8T4BX | $6.95 |
| Blackout roller shade, 48"x72" | https://www.amazon.com/gp/product/B08GHLF6WT | $48.99 |
| **Total:** | | **$123.60** |

I'll not provide any schematics 'cause it pretty simple: you need to connect direction and step pins of A4988 driver to ESP32, also don't forget to connect IR receiver data pin to controller, what's all. You need to power A4988 driver and IR board from ESP32 3.3V or use 5V from the step-down converter (listed in BOM above).

For the Google Assitant integration, initially I've chosen [Arduino IO+IFTTT](https://iotdesignpro.com/projects/google-assistant-controlled-led-using-ESP32-and-adafruit-io) but pushed later to switch to the [SinricPro](https://help.sinric.pro/pages/googlehome.html). I strongly recommend you to get rid of IFTTT because of poor support and bugness, and use Sinric Pro solution! These guys are quick to respond, [available on the GitHub](https://github.com/sinricpro), and their software works as expected (and, overall, supposed to be!)

By the way, for additional advantages of my implementation, I've added IR remotes support (as you can easly see on schematics and source code), so your family members without smart phones and not added to your Google Assistant account still can use "smart blind" by old fashioned wasy, with the standard remote (what I'm really appreciate - it's ability to re-use existing physical remote and do not add new one to your TV stand! :smile:)

Here are some pics of hardware implementation. Initially I've tought to cover motor by cutted white polyesther tube but desided later to do not waste money (darn HomeDepot is way to expensive, even for small shit).

![20210915_140346](https://user-images.githubusercontent.com/1036158/134110373-5897b7f9-7d4d-437c-8361-a0b7c665e1c9.jpg)

LED strips on the top of the controller aren't beyond to this project - it's just my window highlighting gadget :smile: 

![20210915_140326](https://user-images.githubusercontent.com/1036158/134110374-2000402b-b4a0-45f8-a4c1-902ac7188a23.jpg)

Here is YouTube video of "smart" blinds in action:

[![YouTube video of "smart" blinds in action](http://img.youtube.com/vi/lWmYHm-vJgU/0.jpg)](http://www.youtube.com/watch?v=lWmYHm-vJgU)

**Enjoy!**
