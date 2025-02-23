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

With this in mind, existing hardware components in an easily understood configuration was required. The only bespoke component was the PCB used but it is not required. Off the shelf components include ESP32, ds18b20 temperature probe, and 50kg load cells, HX711 analog- to-digital converter (ADC), 18650 rechargable cells, and a 18650 solar MPPT charger. 


