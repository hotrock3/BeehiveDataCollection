# BeehiveDataCollection
Overview: Data collection (wight and temp) via ESP32 and transmitting to another computer via wifi for display on Grafana dashboards.

## Inspiration and Context

### Inspiration
This is not a 100% original idea as many have done it before. Heavy influence comes from [HiveEyes Project](https://github.com/hiveeyes) and other similar projects within the beekeeping community. The HiveEyes project was far bigger and complicated than what was needed for my context but the [HiveEyes Forums](https://community.hiveeyes.org/) were extremely helpful in better understanding their system.

### Context
The beehives to be monitored are located on a south-facing exterior area located on the 5th floor of a school building. Wifi is easily accessable and reliable. Requirements for implementation of sensors were:
1. No installation of new wiring for electrical or data purposes due to limited funding.
2. Be able to add or remove more sensor packages as needed.
3. Be understandable/accessable for basic users to who may need to take over the project if the founder leaves.

With this in mind, existing hardware components in an easily understood configuration was required. The only bespoke component was the PCB used but it is not required. Off the shelf components include ESP32, ds18b20 temperature probe, and 50kg load cells, HX711 analog-to-digital converter (ADC), 18650 rechargable cells, and a 18650 solar MPPT charger. 


# Helpful Resources
- [Load cell explanation and setup](https://circuitjournal.com/50kg-load-cells-with-HX711)
- [Load cell bracket](https://www.thingiverse.com/thing:2624188)
- [IOT Stack guide](https://learnembeddedsystems.co.uk/easy-raspberry-pi-iot-server)
- [DS18B20 sensor setup and explanation](https://www.youtube.com/watch?v=Y1__vmkr8-g)
- [Calibration Process for DS18B20](https://www.instructables.com/Calibration-of-DS18B20-Sensor-With-Arduino-UNO/)
- [ESP32 Deep Sleep explanation](https://www.youtube.com/watch?v=YOjgZUg_skU)
  

# To-Do List
- [x] Create repo and upload arduino code
- [ ] Add description of data flow using a visual.
- [ ] Add photos of actual board and installation
- [ ] Include code and visuals from Node-Red
- [ ] Include screenshot of Grafana dashboard with hive data (currently only collecting weight and ambient temp)
- [x] Upload PCB diagram and/or files
- [x] Create parts list
- [ ] Upload .stl of housing (also post on printables.)
