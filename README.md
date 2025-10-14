This repository serves as a well selected collection of Realtek switch SoC documentation and source. Focus is on the Otto platform with the RTL838x (maple), RTL839x (cypress), RTL930x (longan) and RTL931x (mango) chips. 

- [Roadmap](/datasheets/Roadmap.pdf) for the Realtek switch platform
- Additional infos can be found at https://svanheule.net/switches/start

What RTK (development kit) we have

- Some fragments from the TP-Link BE550. There are several drivers for Realtek PHYs that are used on non-Realtek platforms.
- The most recent SDK from 2022 is based on the D-Link DMS-1250 GPL source. It contains a lot of modern PHY code for RTL8218E, RTL8224, RTL8261 and RTL8264. Download from https://tsd.dlink.com.tw/GPL.asp
- An older SDK from 2016 is based on the Zyxel XGS1210-12 GPL source. It contains code for a totally unknown RTL8284 PHY. Download can be requested from https://www.zyxel.com/global/en/form/gpl-oss-software-notice

PHY Direct Links

PHY | BE550 | DMS-1250 | XGS-1210 XGS-210
--- | --- | --- | --- 
RTL8226x/RTL8221B | [U-Boot](/sources/uboot-be550/drivers/net/rtl8221b) | [RTK](/sources/rtk-dms1250/src/hal/phy) [U-Boot](/sources/uboot-xgs1210/board/Realtek/switch/sdk/src/hal/phy) | [RTK](/sources/rtk-xgs1210/src/hal/phy) [U-Boot](/sources/uboot-xgs1210/board/Realtek/switch/sdk/src/hal/phy)
RTL8251B | [U-Boot](/sources/uboot-be550/drivers/net/rtl8251b) | - | -
RTL8261x | [RTK](/sources/rtk-be550/src/hal/phy) | [RTK](/sources/rtk-dms1250/src/hal/phy) | 
... | ... | ...


The PDFs we have

- GPON/EON controller RTL9601 datasheets
- GPON/EON controller RTL9607C several application notes  
- Switch controller RTL8330 datasheet
- Switch controller RTL8332 datasheet
- Switch controller RTL8370MB datasheet
- Switch controller RTL8372 datasheet
- Switch controller RTL8376 datasheet
- Switch controller RTL8380 datasheet
- Switch controller RTL8382 datasheet
- Switch controller RTL8393 datasheet
- Switch controller RTL9301 datasheet
- Switch controller RTL9303 datasheet
- Switch controller RTL930x developer guide 
- Switch controller RTL9311 datasheet 
- Switch controller RTL9313 datasheet
- POE controller RTL8238B datasheet
- POE controller RTL8238B host command guide
- Transceiver RTL8218B, RTL8218D, RTL8218E
- Transceiver RTL8221B-VB-CG (3rd generation 2.5Gbps)
- Transceiver RTL8261N

What PDF datasheets are we missing

- Switch controller RTL8353M datasheet
- Switch controller RTL8373 datasheet
- Switch controller RTL838x register file
- Switch controller RTL839x register file
- Switch controller RTL930x register file
- Switch controller RTL9302B, RTL9302C, RTL930D datasheet
- POE controller RTL8239 datasheet
- POE controller RTL8239 host command guide
- Transceiver RTL8226 (1st generation 2.5Gbps)
- Transceiver RTL8226B-CG (2nd generation 2.5Gbps)
- Transceiver RTL8221B-CG (2nd generation 2.5Gbps)
- Transceiver RTL8224
- Transceiver RTL8264
- Transceiver RTL8261BE
