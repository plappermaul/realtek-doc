The RTL8226B/RTL8221B SerDes Mode Setting Flow Application Note (Version 2.4) is essentially a "recipe book" for register writes. It moves away from the static descriptions found in the datasheet and provides state-machine logic that a driver must follow to ensure link stability.
Here are the specific sequences covered in that document:

SerDes Interface Mode Switching

This is the most important sequence. It explains how to move the PHY between different host-side protocols. The document specifies that you cannot just change a bit; you must follow a "Teardown and Rebuild" flow:

- The Flow: Power down SerDes $\rightarrow$ Change SDS_MODE_SEL (0x6A04) $\rightarrow$ Toggle PLL_RST $\rightarrow$ Wait for PATCH_READY status $\rightarrow$ Power up SerDes.
- Modes Covered: SGMII (1.25G), HiSGMII (3.125G), 2500Base-X, and 10G-Lite (for 5G/10G variants).

Rate Adapter (RA) Initialization

Since 10M, 100M, and 1000M traffic is physically sent over a 2.5G SerDes lane, the PHY must "stuff" the extra bandwidth with dummy bits.

- The Flow: The document provides the exact values for 0x6F10 and 0x6F11 (usually 0xD455 and 0x8020).
- Requirement: It details why the Rate Adapter must be enabled specifically when the MAC side is fixed at 2.5G but the Copper side is running at lower speeds.

Thermal Sensor Calibration and Monitoring

The Application Note provides the "Enablement Flow" that is often missing from the generic datasheet:

- Enablement: Sets the THS_EN and ADC_EN bits in 0x6601.
- Measurement: Instructions on reading the 12-bit ADC value from MMD 30, Reg 0x0020.
- Formula: The specific linear conversion constants for the B and VB silicon revisions.

LDO and Power Consumption Optimization

The RTL822x series can be power-hungry. The note includes a sequence for Low Power Mode:

- Voltage Adjustment: How to trim the internal LDO via 0x6601 to reduce heat if the PCB trace length allows for lower signal amplitude.
- EEE (Energy Efficient Ethernet): The sequence to advertise and negotiate LPI (Low Power Idle) states.

Signal Integrity Tuning (The "Eye" Diagram)

For hardware engineers, the document provides sequences to adjust the analog physical layer:
- Pre-emphasis: How to boost high-frequency signals for long backplanes.
- Tx Amplitude: Adjusting the millivolt swing of the SerDes differential pairs.

Auto-Downspeed (Reliability Flow)
A unique sequence that tells the PHY to automatically drop the link speed if the cable quality is poor or if the temperature exceeds a specific threshold (OTPS - Over Temperature Protection System).
