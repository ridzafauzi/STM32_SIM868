# STM32_SIM868
This project uses STM32 blue pill to send AT commands to SIM868 module to execute below tasks:-
1. configure and attach to GPRS network
2. send message to remote database (influxdb) via HTTP GET

Time series database (influxDB) is used to store the information received from SIM868 module.

This project was developed with :-<br /> 
1. STM32CubeMX 6.0.1<br /> 
2. Keil uVision 5.32.0.0<br />
3. InfluxDB v2

List of hardwares used :-<br />
1. STM32 blue pill 
2. DC-DC converter module
3. SIMCOM SIM868 GSM module
4. lithium polymer battery
5. ST-Link V2 programmer
6. CH340 USB to Serial


![My Image](images/testbed.jpg)

![My Image](images/debug_viewer.png)

![My Image](images/influxdb.png)


Reference: <br />
1. http://www.bepat.de/2020/12/02/stm32f103c8-uart-with-dma-buffer-and-idle-detection/ <br />
2. https://blog.csdn.net/as480133937/article/details/104827639?utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7Edefault-9.control&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7Edefault-9.control <br />
3. https://controllerstech.blogspot.com/2017/07/using-serial-communication-in-stm32.html <br />



