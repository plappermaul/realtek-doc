Warning! AI generated.

The RTL8226B/RTL8221B SerDes Mode Setting Flow Application Note (Version 2.4) is essentially a "recipe book" for register writes. It moves away from the static descriptions found in the datasheet and provides state-machine logic that a driver must follow to ensure link stability.
Here are the specific sequences covered in that document:

# SerDes Interface Mode Switching

This is the most important sequence. It explains how to move the PHY between different host-side protocols. The document specifies that you cannot just change a bit; you must follow a "Teardown and Rebuild" flow:

- The Flow: Power down SerDes $\rightarrow$ Change SDS_MODE_SEL (0x6A04) $\rightarrow$ Toggle PLL_RST $\rightarrow$ Wait for PATCH_READY status $\rightarrow$ Power up SerDes.
- Modes Covered: SGMII (1.25G), HiSGMII (3.125G), 2500Base-X, and 10G-Lite (for 5G/10G variants).

# Rate Adapter (RA) Initialization

Since 10M, 100M, and 1000M traffic is physically sent over a 2.5G SerDes lane, the PHY must "stuff" the extra bandwidth with dummy bits.

- The Flow: The document provides the exact values for 0x6F10 and 0x6F11 (usually 0xD455 and 0x8020).
- Requirement: It details why the Rate Adapter must be enabled specifically when the MAC side is fixed at 2.5G but the Copper side is running at lower speeds.

The Rate Adapter (RA) is the most critical logic block for 2.5G PHYs like the RTL8221B and RTL8226B when they are paired with a MAC (like your RTL9301) that is "locked" at a 2.5G SerDes frequency.

Since the SerDes lane runs at a fixed physical baud rate (3.125 GHz for 2.5G), but the copper side might be linked at 10/100/1000Mbps, the Rate Adapter performs Bit Stuffing to fill the gaps. Without proper RA initialization, the MAC will see "garbage" data at lower speeds.

According to the Application Note v2.4, the initialization must happen after the SerDes mode is selected but before the final SerDes reset is released.

Step 1: Enable the RA Clock logic. You must tell the PHY to prepare the Rate Adapter buffers.

Register: MMD 31, 0x6F10, Value: 0xD455 (This is a proprietary Realtek tuning value that stabilizes the internal FIFO depth).

Step 2: Configure RA Mode and Speed

Register: MMD 31, 0x6F11, Bit 15 (RA_EN): Must be 1 to enable the adapter, Bit 5 (RA_FIX_2500): Must be 1. This forces the Rate Adapter to always output 2.5G towards the MAC, regardless of the copper link speed. Value: 0x8020 (Standard for most 2.5G designs).

# Thermal Sensor Calibration and Monitoring

The Application Note provides the "Enablement Flow" that is often missing from the generic datasheet:

- Enablement: Sets the THS_EN and ADC_EN bits in 0x6601.
- Measurement: Instructions on reading the 12-bit ADC value from MMD 30, Reg 0x0020.
- Formula: The specific linear conversion constants for the B and VB silicon revisions.

LDO and Power Consumption Optimization

The RTL822x series can be power-hungry. The note includes a sequence for Low Power Mode:

- Voltage Adjustment: How to trim the internal LDO via 0x6601 to reduce heat if the PCB trace length allows for lower signal amplitude.
- EEE (Energy Efficient Ethernet): The sequence to advertise and negotiate LPI (Low Power Idle) states.

# Signal Integrity Tuning (The "Eye" Diagram)

For hardware engineers, the document provides sequences to adjust the analog physical layer:
- Pre-emphasis: How to boost high-frequency signals for long backplanes.
- Tx Amplitude: Adjusting the millivolt swing of the SerDes differential pairs.

# Auto-Downspeed (Reliability Flow)

A unique sequence that tells the PHY to automatically drop the link speed if the cable quality is poor or if the temperature exceeds a specific threshold (OTPS - Over Temperature Protection System).

# delta between the RTL8221B-CG (2nd Gen) and the RTL8221B-VB-CG (3rd Gen) 

This is vital for developers migrating their drivers. 
The "VB" revision introduces architectural refinements—primarily for power efficiency and thermal stability—that change the "Magic Numbers" used in the initialization flows.The following delta is based on the transition from Application Note v2.4 to v2.5.1. The LDO Voltage Delta (Reg 0x6601)The VB revision utilizes a more efficient internal regulator. Using the old CG-revision LDO settings on a VB chip can lead to unnecessary heat or signal instability.FeatureRTL8221B-CG (Old)RTL8221B-VB-CG (New)LDO Trim (Bits 5:0)0x10 (Standard 1.1V)0x08 (Optimized for 22nm)UsageDefault for 28nm silicon.Mandatory for VB to reduce power by ~15%.Note: If you see "Bit 6" being toggled in newer SDKs, this is the LDO_Low_Power_Mode enable, which is exclusive to the VB/VM series.2. Thermal Sensor Calibration Delta (MMD 30, 0x0020)While the register address remains the same, the internal analog diode characteristics changed. Using the old formula on a VB chip will result in an error of approximately 5-8°C.Conversion Formulas:CG Revision: $T(^\circ C) = (Raw \times 0.58) - 45$VB Revision: $T(^\circ C) = (Raw \times 0.56) - 42$3. SerDes Setting Flow Delta (MMD 31, 0x6A04)The VB revision is more "picky" about the SerDes Auto-Negotiation handshake with the SoC (like the RTL9301).SettingCG RevisionVB RevisionSDS Mode Select0x0503 (Typical)0x0505 or 0x0507LogicStandard 2500Base-X.Adds enhanced SGMII-Lite support for better 2.5G sync.4. Patch/Firmware ReadinessIn the Application Note v2.5, a new check was added to ensure the PHY's internal MCU has finished its "Self-Calibration" before the MAC starts sending data.Check Register: MMD 31, 0x6A40Bit 9 (PATCH_READY): In the VB revision, you must poll this bit until it is 1 before releasing the PHY from reset. In the older CG version, a simple 10ms delay was often sufficient, but the VB's internal state machine is more complex.Migration Summary for GPL DevelopersIf you are updating a driver to support both chips, your probe function should look like this:Read MMD 30, 0x6958 (Chip Revision).If value is 0x0001 $\rightarrow$ Use CG flows (LDO 0x10, Temp Offset -45).If value is 0x0002 $\rightarrow$ Use VB flows (LDO 0x08, Temp Offset -42).
