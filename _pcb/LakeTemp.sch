EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "LakeTempESP"
Date "2021-06-05"
Rev "2.0"
Comp "(c) 2021 foorschtbar"
Comment1 "https://github.com/foorschtbar/LakeTempESP"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Interface_Expansion:PCF8574 U3
U 1 1 5F5982DF
P 7050 3850
F 0 "U3" H 7050 3900 50  0000 C CNN
F 1 "PCF8574" H 7050 4000 50  0000 C CNN
F 2 "Package_SO:SOIC-16W_7.5x10.3mm_P1.27mm" H 7050 3850 50  0001 C CNN
F 3 "http://www.nxp.com/documents/data_sheet/PCF8574_PCF8574A.pdf" H 7050 3850 50  0001 C CNN
	1    7050 3850
	1    0    0    -1  
$EndComp
$Comp
L Regulator_SwitchedCapacitor:ICL7660 U2
U 1 1 5F5A1379
P 4750 1400
F 0 "U2" H 4750 1967 50  0000 C CNN
F 1 "ICL7660" H 4750 1876 50  0000 C CNN
F 2 "Package_SO:SOP-8_3.9x4.9mm_P1.27mm" H 4850 1300 50  0001 C CNN
F 3 "http://datasheets.maximintegrated.com/en/ds/ICL7660-MAX1044.pdf" H 4850 1300 50  0001 C CNN
	1    4750 1400
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR04
U 1 1 5F5A7DE9
P 4150 1100
F 0 "#PWR04" H 4150 950 50  0001 C CNN
F 1 "+5V" H 4150 1250 50  0000 C CNN
F 2 "" H 4150 1100 50  0001 C CNN
F 3 "" H 4150 1100 50  0001 C CNN
	1    4150 1100
	1    0    0    -1  
$EndComp
$Comp
L Device:R_POT_TRIM RV1
U 1 1 5F5C44AE
P 6400 1500
F 0 "RV1" H 6600 1750 50  0000 R CNN
F 1 "10K" H 6600 1650 50  0000 R CNN
F 2 "Potentiometer_THT:Potentiometer_Runtron_RM-065_Vertical" H 6400 1500 50  0001 C CNN
F 3 "~" H 6400 1500 50  0001 C CNN
	1    6400 1500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR028
U 1 1 5F636D67
P 7050 4550
F 0 "#PWR028" H 7050 4300 50  0001 C CNN
F 1 "GND" H 7055 4377 50  0000 C CNN
F 2 "" H 7050 4550 50  0001 C CNN
F 3 "" H 7050 4550 50  0001 C CNN
	1    7050 4550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR043
U 1 1 5F63EF8C
P 1250 6000
F 0 "#PWR043" H 1250 5750 50  0001 C CNN
F 1 "GND" V 1255 5872 50  0000 R CNN
F 2 "" H 1250 6000 50  0001 C CNN
F 3 "" H 1250 6000 50  0001 C CNN
	1    1250 6000
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR045
U 1 1 5F6434F7
P 1250 6100
F 0 "#PWR045" H 1250 5950 50  0001 C CNN
F 1 "+5V" V 1265 6228 50  0000 L CNN
F 2 "" H 1250 6100 50  0001 C CNN
F 3 "" H 1250 6100 50  0001 C CNN
	1    1250 6100
	0    -1   -1   0   
$EndComp
Text GLabel 4650 3850 2    50   Input ~ 0
SDA
Text GLabel 4650 3950 2    50   Input ~ 0
SCL
$Comp
L power:+3V3 #PWR020
U 1 1 5F6B1844
P 2800 3000
F 0 "#PWR020" H 2800 2850 50  0001 C CNN
F 1 "+3V3" H 2815 3173 50  0000 C CNN
F 2 "" H 2800 3000 50  0001 C CNN
F 3 "" H 2800 3000 50  0001 C CNN
	1    2800 3000
	1    0    0    -1  
$EndComp
Text GLabel 6550 3450 0    50   Input ~ 0
SCL
$Comp
L Regulator_Linear:NCP1117-3.3_SOT223 U1
U 1 1 5F6D700E
P 2150 1100
F 0 "U1" H 2150 1342 50  0000 C CNN
F 1 "NCP1117-3.3_SOT223" H 2150 1251 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 2150 1300 50  0001 C CNN
F 3 "http://www.onsemi.com/pub_link/Collateral/NCP1117-D.PDF" H 2250 850 50  0001 C CNN
	1    2150 1100
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR02
U 1 1 5F6E1730
P 1100 1100
F 0 "#PWR02" H 1100 950 50  0001 C CNN
F 1 "+5V" H 1000 1250 50  0000 L CNN
F 2 "" H 1100 1100 50  0001 C CNN
F 3 "" H 1100 1100 50  0001 C CNN
	1    1100 1100
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 5F6E4208
P 1600 1350
F 0 "C1" H 1715 1396 50  0000 L CNN
F 1 "10uF" H 1715 1305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 1638 1200 50  0001 C CNN
F 3 "~" H 1600 1350 50  0001 C CNN
	1    1600 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C2
U 1 1 5F6E5524
P 2650 1350
F 0 "C2" H 2765 1396 50  0000 L CNN
F 1 "10uF" H 2765 1305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 2688 1200 50  0001 C CNN
F 3 "~" H 2650 1350 50  0001 C CNN
	1    2650 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 5F7023F1
P 5350 1500
F 0 "C3" H 5465 1546 50  0000 L CNN
F 1 "10uF" H 5465 1455 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 5388 1350 50  0001 C CNN
F 3 "~" H 5350 1500 50  0001 C CNN
	1    5350 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 1300 5350 1300
Wire Wire Line
	5350 1300 5350 1350
Wire Wire Line
	5350 1650 5350 1700
Wire Wire Line
	5350 1700 5150 1700
NoConn ~ 4350 1400
NoConn ~ 4350 1600
Wire Wire Line
	6400 1350 6400 1100
Text GLabel 6700 1500 2    50   Input ~ 0
LCD_CONTRAST
Text GLabel 1250 6200 0    50   Input ~ 0
LCD_CONTRAST
Wire Wire Line
	2800 3000 2800 3250
Text GLabel 1250 6300 0    50   Input ~ 0
LCD_RS
Text GLabel 7550 3450 2    50   Input ~ 0
LCD_RS
Text GLabel 7550 3550 2    50   Input ~ 0
LCD_RW
Text GLabel 1250 6400 0    50   Input ~ 0
LCD_RW
Text GLabel 1250 6500 0    50   Input ~ 0
LCD_E
Text GLabel 7550 3650 2    50   Input ~ 0
LCD_E
Text GLabel 7550 3850 2    50   Input ~ 0
LCD_DB4
Text GLabel 7550 3950 2    50   Input ~ 0
LCD_DB5
Text GLabel 7550 4050 2    50   Input ~ 0
LCD_DB6
Text GLabel 7550 4150 2    50   Input ~ 0
LCD_DB7
Text GLabel 1250 7100 0    50   Input ~ 0
LCD_DB5
Text GLabel 1250 7300 0    50   Input ~ 0
LCD_DB7
Text GLabel 1250 7200 0    50   Input ~ 0
LCD_DB6
Text GLabel 1250 7000 0    50   Input ~ 0
LCD_DB4
$Comp
L power:+3V3 #PWR025
U 1 1 5F769040
P 7050 3150
F 0 "#PWR025" H 7050 3000 50  0001 C CNN
F 1 "+3V3" H 7065 3323 50  0000 C CNN
F 2 "" H 7050 3150 50  0001 C CNN
F 3 "" H 7050 3150 50  0001 C CNN
	1    7050 3150
	1    0    0    -1  
$EndComp
Text GLabel 7550 3750 2    50   Input ~ 0
LCD_LED
Text GLabel 8200 5750 0    50   Input ~ 0
LCD_LED
$Comp
L Device:R R15
U 1 1 5F7989C1
P 8350 5600
F 0 "R15" H 8420 5646 50  0000 L CNN
F 1 "4,7K" H 8420 5555 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 8280 5600 50  0001 C CNN
F 3 "~" H 8350 5600 50  0001 C CNN
	1    8350 5600
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR037
U 1 1 5F798F42
P 8350 5450
F 0 "#PWR037" H 8350 5300 50  0001 C CNN
F 1 "+5V" H 8365 5623 50  0000 C CNN
F 2 "" H 8350 5450 50  0001 C CNN
F 3 "" H 8350 5450 50  0001 C CNN
	1    8350 5450
	1    0    0    -1  
$EndComp
Text GLabel 6550 3950 0    50   Input ~ 0
PCF_A2
Text GLabel 6550 3850 0    50   Input ~ 0
PCF_A1
Text GLabel 6550 3750 0    50   Input ~ 0
PCF_A0
Text GLabel 6550 3550 0    50   Input ~ 0
SDA
$Comp
L power:GND #PWR048
U 1 1 5F7B0AD1
P 1250 6600
F 0 "#PWR048" H 1250 6350 50  0001 C CNN
F 1 "GND" V 1255 6472 50  0000 R CNN
F 2 "" H 1250 6600 50  0001 C CNN
F 3 "" H 1250 6600 50  0001 C CNN
	1    1250 6600
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR050
U 1 1 5F7B0DD7
P 1250 6700
F 0 "#PWR050" H 1250 6450 50  0001 C CNN
F 1 "GND" V 1255 6572 50  0000 R CNN
F 2 "" H 1250 6700 50  0001 C CNN
F 3 "" H 1250 6700 50  0001 C CNN
	1    1250 6700
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR053
U 1 1 5F7B0FFA
P 1250 6800
F 0 "#PWR053" H 1250 6550 50  0001 C CNN
F 1 "GND" V 1255 6672 50  0000 R CNN
F 2 "" H 1250 6800 50  0001 C CNN
F 3 "" H 1250 6800 50  0001 C CNN
	1    1250 6800
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR054
U 1 1 5F7B11C6
P 1250 6900
F 0 "#PWR054" H 1250 6650 50  0001 C CNN
F 1 "GND" V 1255 6772 50  0000 R CNN
F 2 "" H 1250 6900 50  0001 C CNN
F 3 "" H 1250 6900 50  0001 C CNN
	1    1250 6900
	0    1    1    0   
$EndComp
$Comp
L Transistor_BJT:S8050 Q1
U 1 1 5F7B1F36
P 8700 5750
F 0 "Q1" H 8890 5796 50  0000 L CNN
F 1 "S8050" H 8890 5705 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 8900 5675 50  0001 L CIN
F 3 "http://www.unisonic.com.tw/datasheet/S8050.pdf" H 8700 5750 50  0001 L CNN
	1    8700 5750
	1    0    0    -1  
$EndComp
Wire Wire Line
	8500 5750 8350 5750
Connection ~ 8350 5750
Wire Wire Line
	8350 5750 8200 5750
$Comp
L power:GND #PWR041
U 1 1 5F7BFECF
P 8800 6000
F 0 "#PWR041" H 8800 5750 50  0001 C CNN
F 1 "GND" H 8805 5827 50  0000 C CNN
F 2 "" H 8800 6000 50  0001 C CNN
F 3 "" H 8800 6000 50  0001 C CNN
	1    8800 6000
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR038
U 1 1 5F7CBA39
P 10000 5450
F 0 "#PWR038" H 10000 5300 50  0001 C CNN
F 1 "+5V" H 10015 5623 50  0000 C CNN
F 2 "" H 10000 5450 50  0001 C CNN
F 3 "" H 10000 5450 50  0001 C CNN
	1    10000 5450
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Male J2
U 1 1 600EFAD5
P 8250 1250
F 0 "J2" H 8200 1250 50  0000 R CNN
F 1 "Conn_01x06_Male" H 8750 1600 50  0000 R CNN
F 2 "Connector_JST:JST_PH_S6B-PH-K_1x06_P2.00mm_Horizontal" H 8250 1250 50  0001 C CNN
F 3 "~" H 8250 1250 50  0001 C CNN
	1    8250 1250
	-1   0    0    -1  
$EndComp
$Comp
L power:+5V #PWR05
U 1 1 600F9B98
P 8050 1150
F 0 "#PWR05" H 8050 1000 50  0001 C CNN
F 1 "+5V" V 8065 1278 50  0000 L CNN
F 2 "" H 8050 1150 50  0001 C CNN
F 3 "" H 8050 1150 50  0001 C CNN
	1    8050 1150
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Conn_01x03_Male J10
U 1 1 600FAEB7
P 3400 7350
F 0 "J10" H 3372 7374 50  0000 R CNN
F 1 "Conn_01x03_Male" H 3372 7283 50  0000 R CNN
F 2 "Connector_JST:JST_PH_B3B-PH-K_1x03_P2.00mm_Vertical" H 3400 7350 50  0001 C CNN
F 3 "~" H 3400 7350 50  0001 C CNN
	1    3400 7350
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR056
U 1 1 600FD67A
P 3200 7250
F 0 "#PWR056" H 3200 7000 50  0001 C CNN
F 1 "GND" V 3205 7122 50  0000 R CNN
F 2 "" H 3200 7250 50  0001 C CNN
F 3 "" H 3200 7250 50  0001 C CNN
	1    3200 7250
	0    1    1    0   
$EndComp
Text GLabel 3200 7450 0    50   Input ~ 0
TOUCH_IO
Text GLabel 3400 4450 2    50   Input Italic 0
TOUCH_IO
Text GLabel 3400 4050 2    50   Input ~ 0
ONEWIRE_BUS
$Comp
L power:+3.3V #PWR057
U 1 1 600FE286
P 3200 7350
F 0 "#PWR057" H 3200 7200 50  0001 C CNN
F 1 "+3.3V" V 3215 7478 50  0000 L CNN
F 2 "" H 3200 7350 50  0001 C CNN
F 3 "" H 3200 7350 50  0001 C CNN
	1    3200 7350
	0    -1   -1   0   
$EndComp
Text GLabel 8050 1450 0    50   Input ~ 0
ONEWIRE_BUS
$Comp
L Sensor_Temperature:DS18B20 U5
U 1 1 60113B80
P 9350 4300
F 0 "U5" H 9120 4346 50  0000 R CNN
F 1 "DS18B20" H 9120 4255 50  0000 R CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 8350 4050 50  0001 C CNN
F 3 "http://datasheets.maximintegrated.com/en/ds/DS18B20.pdf" H 9200 4550 50  0001 C CNN
	1    9350 4300
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR026
U 1 1 60119A26
P 8750 3550
F 0 "#PWR026" H 8750 3400 50  0001 C CNN
F 1 "+5V" H 8765 3723 50  0000 C CNN
F 2 "" H 8750 3550 50  0001 C CNN
F 3 "" H 8750 3550 50  0001 C CNN
	1    8750 3550
	1    0    0    -1  
$EndComp
$Comp
L Device:R R11
U 1 1 60124FE5
P 9850 4100
F 0 "R11" H 9920 4146 50  0000 L CNN
F 1 "4,7K" H 9920 4055 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 9780 4100 50  0001 C CNN
F 3 "~" H 9850 4100 50  0001 C CNN
	1    9850 4100
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR027
U 1 1 60125338
P 10000 3550
F 0 "#PWR027" H 10000 3400 50  0001 C CNN
F 1 "+3V3" H 10000 3700 50  0000 C CNN
F 2 "" H 10000 3550 50  0001 C CNN
F 3 "" H 10000 3550 50  0001 C CNN
	1    10000 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 3300 4050 3850
Wire Wire Line
	4350 3300 4350 3950
$Comp
L power:+3V3 #PWR023
U 1 1 5F71EA26
P 4350 3000
F 0 "#PWR023" H 4350 2850 50  0001 C CNN
F 1 "+3V3" H 4365 3173 50  0000 C CNN
F 2 "" H 4350 3000 50  0001 C CNN
F 3 "" H 4350 3000 50  0001 C CNN
	1    4350 3000
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR022
U 1 1 5F71E77D
P 4050 3000
F 0 "#PWR022" H 4050 2850 50  0001 C CNN
F 1 "+3V3" H 4065 3173 50  0000 C CNN
F 2 "" H 4050 3000 50  0001 C CNN
F 3 "" H 4050 3000 50  0001 C CNN
	1    4050 3000
	1    0    0    -1  
$EndComp
$Comp
L Device:R R9
U 1 1 5F71BC8D
P 4350 3150
F 0 "R9" H 4420 3196 50  0000 L CNN
F 1 "4,7K" H 4420 3105 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 4280 3150 50  0001 C CNN
F 3 "~" H 4350 3150 50  0001 C CNN
	1    4350 3150
	1    0    0    -1  
$EndComp
$Comp
L Device:R R8
U 1 1 5F71AEB5
P 4050 3150
F 0 "R8" H 4120 3196 50  0000 L CNN
F 1 "4,7K" H 4120 3105 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 3980 3150 50  0001 C CNN
F 3 "~" H 4050 3150 50  0001 C CNN
	1    4050 3150
	1    0    0    -1  
$EndComp
Connection ~ 4050 3850
Wire Wire Line
	4050 3850 4650 3850
Connection ~ 4350 3950
Wire Wire Line
	4350 3950 4650 3950
$Comp
L Connector:Conn_01x06_Male J3
U 1 1 609C7E34
P 8650 1250
F 0 "J3" H 8600 1250 50  0000 R CNN
F 1 "Conn_01x06_Male" H 9150 1600 50  0000 R CNN
F 2 "Connector_JST:JST_PH_B6B-PH-K_1x06_P2.00mm_Vertical" H 8650 1250 50  0001 C CNN
F 3 "~" H 8650 1250 50  0001 C CNN
	1    8650 1250
	1    0    0    -1  
$EndComp
$Comp
L power:GND1 #PWR015
U 1 1 609D08D9
P 8950 2600
F 0 "#PWR015" H 8950 2350 50  0001 C CNN
F 1 "GND1" H 8955 2427 50  0000 C CNN
F 2 "" H 8950 2600 50  0001 C CNN
F 3 "" H 8950 2600 50  0001 C CNN
	1    8950 2600
	1    0    0    -1  
$EndComp
$Comp
L Device:R_PHOTO R14
U 1 1 609FAB45
P 7000 5500
F 0 "R14" H 7070 5546 50  0000 L CNN
F 1 "GL5528" H 7070 5455 50  0000 L CNN
F 2 "OptoDevice:R_LDR_5.1x4.3mm_P3.4mm_Vertical" V 7050 5250 50  0001 L CNN
F 3 "~" H 7000 5450 50  0001 C CNN
	1    7000 5500
	1    0    0    -1  
$EndComp
Text GLabel 6700 5650 0    50   Input ~ 0
LDR
$Comp
L power:+3V3 #PWR035
U 1 1 60A02581
P 7000 5350
F 0 "#PWR035" H 7000 5200 50  0001 C CNN
F 1 "+3V3" H 7015 5523 50  0000 C CNN
F 2 "" H 7000 5350 50  0001 C CNN
F 3 "" H 7000 5350 50  0001 C CNN
	1    7000 5350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R16
U 1 1 60A039E3
P 7000 5800
F 0 "R16" H 7070 5846 50  0000 L CNN
F 1 "10K" H 7070 5755 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 6930 5800 50  0001 C CNN
F 3 "~" H 7000 5800 50  0001 C CNN
	1    7000 5800
	1    0    0    -1  
$EndComp
Wire Wire Line
	6700 5650 7000 5650
Connection ~ 7000 5650
$Comp
L Connector:Conn_01x04_Male J7
U 1 1 60A0F106
P 3450 6150
F 0 "J7" H 3422 6032 50  0000 R CNN
F 1 "Conn_01x04_Male" H 3422 6123 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 3450 6150 50  0001 C CNN
F 3 "~" H 3450 6150 50  0001 C CNN
	1    3450 6150
	-1   0    0    1   
$EndComp
$Comp
L Device:R R12
U 1 1 60A116EF
P 4050 4600
F 0 "R12" H 3980 4554 50  0000 R CNN
F 1 "10K" H 3980 4645 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 3980 4600 50  0001 C CNN
F 3 "~" H 4050 4600 50  0001 C CNN
	1    4050 4600
	-1   0    0    1   
$EndComp
$Comp
L Device:R R10
U 1 1 60A1453F
P 4700 3150
F 0 "R10" H 4630 3104 50  0000 R CNN
F 1 "10K" H 4630 3195 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 4630 3150 50  0001 C CNN
F 3 "~" H 4700 3150 50  0001 C CNN
	1    4700 3150
	-1   0    0    1   
$EndComp
Text GLabel 3250 6250 0    50   Input ~ 0
ESP_TX
Text GLabel 3400 3550 2    50   Input ~ 0
ESP_TX
Text GLabel 3400 3750 2    50   Input ~ 0
ESP_RX
Text GLabel 3250 6150 0    50   Input ~ 0
ESP_RX
$Comp
L power:+3V3 #PWR039
U 1 1 60A1AD37
P 3250 5950
F 0 "#PWR039" H 3250 5800 50  0001 C CNN
F 1 "+3V3" V 3250 6200 50  0000 C CNN
F 2 "" H 3250 5950 50  0001 C CNN
F 3 "" H 3250 5950 50  0001 C CNN
	1    3250 5950
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR044
U 1 1 60A1C481
P 3250 6050
F 0 "#PWR044" H 3250 5800 50  0001 C CNN
F 1 "GND" V 3255 5922 50  0000 R CNN
F 2 "" H 3250 6050 50  0001 C CNN
F 3 "" H 3250 6050 50  0001 C CNN
	1    3250 6050
	0    1    1    0   
$EndComp
$Comp
L Device:R R5
U 1 1 60A26CF4
P 1650 3150
F 0 "R5" H 1580 3104 50  0000 R CNN
F 1 "10K" H 1580 3195 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 1580 3150 50  0001 C CNN
F 3 "~" H 1650 3150 50  0001 C CNN
	1    1650 3150
	-1   0    0    1   
$EndComp
$Comp
L power:+3V3 #PWR018
U 1 1 60A27D59
P 1650 3000
F 0 "#PWR018" H 1650 2850 50  0001 C CNN
F 1 "+3V3" H 1665 3173 50  0000 C CNN
F 2 "" H 1650 3000 50  0001 C CNN
F 3 "" H 1650 3000 50  0001 C CNN
	1    1650 3000
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW1
U 1 1 60A29704
P 1000 3450
F 0 "SW1" H 1000 3735 50  0000 C CNN
F 1 "RESET" H 1000 3644 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm_H5mm" H 1000 3650 50  0001 C CNN
F 3 "~" H 1000 3650 50  0001 C CNN
	1    1000 3450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR030
U 1 1 60A2A171
P 800 4700
F 0 "#PWR030" H 800 4450 50  0001 C CNN
F 1 "GND" H 805 4527 50  0000 C CNN
F 2 "" H 800 4700 50  0001 C CNN
F 3 "" H 800 4700 50  0001 C CNN
	1    800  4700
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR024
U 1 1 60A2B393
P 4700 3000
F 0 "#PWR024" H 4700 2850 50  0001 C CNN
F 1 "+3V3" H 4715 3173 50  0000 C CNN
F 2 "" H 4700 3000 50  0001 C CNN
F 3 "" H 4700 3000 50  0001 C CNN
	1    4700 3000
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW2
U 1 1 60A2D342
P 5150 3450
F 0 "SW2" H 5150 3265 50  0000 C CNN
F 1 "FLASH" H 5150 3356 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm_H5mm" H 5150 3650 50  0001 C CNN
F 3 "~" H 5150 3650 50  0001 C CNN
	1    5150 3450
	-1   0    0    1   
$EndComp
$Comp
L power:+3V3 #PWR021
U 1 1 60A2FAE2
P 3700 3000
F 0 "#PWR021" H 3700 2850 50  0001 C CNN
F 1 "+3V3" H 3715 3173 50  0000 C CNN
F 2 "" H 3700 3000 50  0001 C CNN
F 3 "" H 3700 3000 50  0001 C CNN
	1    3700 3000
	1    0    0    -1  
$EndComp
$Comp
L Device:R R7
U 1 1 60A333CA
P 3700 3150
F 0 "R7" H 3630 3104 50  0000 R CNN
F 1 "10K" H 3630 3195 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 3630 3150 50  0001 C CNN
F 3 "~" H 3700 3150 50  0001 C CNN
	1    3700 3150
	-1   0    0    1   
$EndComp
Wire Wire Line
	3700 3300 3700 3450
Connection ~ 3700 3450
Wire Wire Line
	3700 3450 4950 3450
Wire Wire Line
	4700 3650 4700 3300
Wire Wire Line
	1650 3300 1650 3450
$Comp
L Device:R R6
U 1 1 60A5AB2C
P 2050 3150
F 0 "R6" H 1980 3104 50  0000 R CNN
F 1 "10K" H 1980 3195 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 1980 3150 50  0001 C CNN
F 3 "~" H 2050 3150 50  0001 C CNN
	1    2050 3150
	-1   0    0    1   
$EndComp
$Comp
L power:+3V3 #PWR019
U 1 1 60A5B15A
P 2050 3000
F 0 "#PWR019" H 2050 2850 50  0001 C CNN
F 1 "+3V3" H 2065 3173 50  0000 C CNN
F 2 "" H 2050 3000 50  0001 C CNN
F 3 "" H 2050 3000 50  0001 C CNN
	1    2050 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	1650 3450 2200 3450
Connection ~ 1650 3450
Wire Wire Line
	2050 3300 2050 3650
Wire Wire Line
	2050 3650 2200 3650
NoConn ~ 2200 4050
NoConn ~ 2200 4150
NoConn ~ 2200 4250
NoConn ~ 2200 4350
NoConn ~ 2200 4450
NoConn ~ 2200 4550
Wire Wire Line
	5350 4750 5350 3450
Wire Wire Line
	800  4700 800  3450
NoConn ~ 5250 5800
$Comp
L power:GND #PWR046
U 1 1 60A70257
P 4950 6100
F 0 "#PWR046" H 4950 5850 50  0001 C CNN
F 1 "GND" H 4955 5927 50  0000 C CNN
F 2 "" H 4950 6100 50  0001 C CNN
F 3 "" H 4950 6100 50  0001 C CNN
	1    4950 6100
	1    0    0    -1  
$EndComp
$Comp
L LED:WS2812B D5
U 1 1 60A6E239
P 4950 5800
F 0 "D5" H 5294 5846 50  0000 L CNN
F 1 "WS2812B" H 5294 5755 50  0000 L CNN
F 2 "LED_SMD:LED_WS2812B_PLCC4_5.0x5.0mm_P3.2mm" H 5000 5500 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf" H 5050 5425 50  0001 L TNN
	1    4950 5800
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR036
U 1 1 60A71082
P 4950 5400
F 0 "#PWR036" H 4950 5250 50  0001 C CNN
F 1 "+5V" H 4965 5573 50  0000 C CNN
F 2 "" H 4950 5400 50  0001 C CNN
F 3 "" H 4950 5400 50  0001 C CNN
	1    4950 5400
	1    0    0    -1  
$EndComp
$Comp
L Device:C C5
U 1 1 60A73068
P 5850 5800
F 0 "C5" H 5965 5846 50  0000 L CNN
F 1 "100nF" H 5965 5755 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 5888 5650 50  0001 C CNN
F 3 "~" H 5850 5800 50  0001 C CNN
	1    5850 5800
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR047
U 1 1 60A8189F
P 5850 6100
F 0 "#PWR047" H 5850 5850 50  0001 C CNN
F 1 "GND" H 5855 5927 50  0000 C CNN
F 2 "" H 5850 6100 50  0001 C CNN
F 3 "" H 5850 6100 50  0001 C CNN
	1    5850 6100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5850 5950 5850 6100
$Comp
L Jumper:SolderJumper_3_Bridged12 JP1
U 1 1 60AB888E
P 9350 3550
F 0 "JP1" H 9350 3755 50  0000 C CNN
F 1 "SolderJumper_3_Open" H 9350 3664 50  0000 C CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged12_Pad1.0x1.5mm" H 9350 3550 50  0001 C CNN
F 3 "~" H 9350 3550 50  0001 C CNN
	1    9350 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	8750 3550 9150 3550
Text GLabel 9200 3900 0    50   Input ~ 0
ONEWIRE_PWR
Text GLabel 8050 1350 0    50   Input ~ 0
ONEWIRE_PWR
$Comp
L Device:R R3
U 1 1 60AC7208
P 8950 1950
F 0 "R3" H 9020 1996 50  0000 L CNN
F 1 "150" H 9020 1905 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 8880 1950 50  0001 C CNN
F 3 "~" H 8950 1950 50  0001 C CNN
	1    8950 1950
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D3
U 1 1 60AC8901
P 8950 2250
F 0 "D3" V 8989 2132 50  0000 R CNN
F 1 "PWR 5V" V 8898 2132 50  0000 R CNN
F 2 "LED_SMD:LED_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 8950 2250 50  0001 C CNN
F 3 "~" H 8950 2250 50  0001 C CNN
	1    8950 2250
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R4
U 1 1 60AE757A
P 9450 1950
F 0 "R4" H 9520 1996 50  0000 L CNN
F 1 "68" H 9520 1905 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 9380 1950 50  0001 C CNN
F 3 "~" H 9450 1950 50  0001 C CNN
	1    9450 1950
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D4
U 1 1 60AE77C8
P 9450 2250
F 0 "D4" V 9489 2132 50  0000 R CNN
F 1 "PWR 3.3V" V 9398 2132 50  0000 R CNN
F 2 "LED_SMD:LED_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 9450 2250 50  0001 C CNN
F 3 "~" H 9450 2250 50  0001 C CNN
	1    9450 2250
	0    -1   -1   0   
$EndComp
Wire Wire Line
	9450 2600 9450 2400
$Comp
L Connector:Screw_Terminal_01x03 J4
U 1 1 60B07FFC
P 10300 1450
F 0 "J4" H 10380 1492 50  0000 L CNN
F 1 "Conn_01x03_Female" H 10380 1401 50  0000 L CNN
F 2 "TerminalBlock_Phoenix:TerminalBlock_Phoenix_MKDS-1,5-3-5.08_1x03_P5.08mm_Horizontal" H 10300 1450 50  0001 C CNN
F 3 "~" H 10300 1450 50  0001 C CNN
	1    10300 1450
	1    0    0    -1  
$EndComp
Text GLabel 4650 5800 0    50   Input ~ 0
WS2812B
Text GLabel 3400 4150 2    50   Input ~ 0
WS2812B
Text GLabel 4650 4250 2    50   Input ~ 0
WATER_LEVEL
Wire Wire Line
	4050 4350 4050 4450
$Comp
L Device:R R13
U 1 1 60B21F9D
P 4500 4600
F 0 "R13" H 4570 4646 50  0000 L CNN
F 1 "4,7K" H 4570 4555 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 4430 4600 50  0001 C CNN
F 3 "~" H 4500 4600 50  0001 C CNN
	1    4500 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 4450 4500 4250
Text GLabel 8050 1550 0    50   Input ~ 0
WATER_LEVEL
$Comp
L Connector:Screw_Terminal_01x02 J5
U 1 1 60B2AC7D
P 10300 1750
F 0 "J5" H 10380 1742 50  0000 L CNN
F 1 "Conn_01x02_Female" H 10380 1651 50  0000 L CNN
F 2 "TerminalBlock_Phoenix:TerminalBlock_Phoenix_MKDS-1,5-2-5.08_1x02_P5.08mm_Horizontal" H 10300 1750 50  0001 C CNN
F 3 "~" H 10300 1750 50  0001 C CNN
	1    10300 1750
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J1
U 1 1 60B06737
P 10300 1050
F 0 "J1" H 10380 1042 50  0000 L CNN
F 1 "Conn_01x02_Female" H 10380 951 50  0000 L CNN
F 2 "TerminalBlock_Phoenix:TerminalBlock_Phoenix_MKDS-1,5-2-5.08_1x02_P5.08mm_Horizontal" H 10300 1050 50  0001 C CNN
F 3 "~" H 10300 1050 50  0001 C CNN
	1    10300 1050
	1    0    0    -1  
$EndComp
Wire Wire Line
	8950 2400 8950 2600
$Comp
L power:+3V3 #PWR06
U 1 1 60B584B0
P 8050 1250
F 0 "#PWR06" H 8050 1100 50  0001 C CNN
F 1 "+3V3" V 8065 1378 50  0000 L CNN
F 2 "" H 8050 1250 50  0001 C CNN
F 3 "" H 8050 1250 50  0001 C CNN
	1    8050 1250
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8950 1150 8950 1800
Wire Wire Line
	8850 1250 9450 1250
Wire Wire Line
	8850 1350 10100 1350
Wire Wire Line
	9450 1250 9450 1800
Wire Wire Line
	8850 1450 10100 1450
Wire Wire Line
	8850 1050 9950 1050
Wire Wire Line
	9850 1250 9850 1750
Wire Wire Line
	9850 1750 10100 1750
Wire Wire Line
	8850 1550 9750 1550
Wire Wire Line
	9750 1550 9750 1850
Wire Wire Line
	9750 1850 10100 1850
Wire Wire Line
	10100 1550 9950 1550
Wire Wire Line
	9950 1550 9950 2600
$Comp
L Connector:Conn_01x04_Female J9
U 1 1 60CBD02D
P 2900 6700
F 0 "J9" H 2928 6676 50  0000 L CNN
F 1 "Conn_01x04_Female" H 2928 6585 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 2900 6700 50  0001 C CNN
F 3 "~" H 2900 6700 50  0001 C CNN
	1    2900 6700
	1    0    0    -1  
$EndComp
Text GLabel 2700 6900 0    50   Input ~ 0
SDA
Text GLabel 2700 6800 0    50   Input ~ 0
SCL
$Comp
L power:+3V3 #PWR049
U 1 1 60CBF54E
P 2700 6600
F 0 "#PWR049" H 2700 6450 50  0001 C CNN
F 1 "+3V3" V 2700 6850 50  0000 C CNN
F 2 "" H 2700 6600 50  0001 C CNN
F 3 "" H 2700 6600 50  0001 C CNN
	1    2700 6600
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR051
U 1 1 60CBF989
P 2700 6700
F 0 "#PWR051" H 2700 6450 50  0001 C CNN
F 1 "GND" V 2705 6572 50  0000 R CNN
F 2 "" H 2700 6700 50  0001 C CNN
F 3 "" H 2700 6700 50  0001 C CNN
	1    2700 6700
	0    1    1    0   
$EndComp
Connection ~ 8950 1150
Wire Wire Line
	8950 1150 8850 1150
Wire Wire Line
	8950 1150 10100 1150
Connection ~ 9450 1250
Wire Wire Line
	9450 1250 9850 1250
$Comp
L power:+3V3 #PWR03
U 1 1 5F6E21DA
P 3600 1100
F 0 "#PWR03" H 3600 950 50  0001 C CNN
F 1 "+3V3" H 3500 1250 50  0000 L CNN
F 2 "" H 3600 1100 50  0001 C CNN
F 3 "" H 3600 1100 50  0001 C CNN
	1    3600 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	2450 1100 2650 1100
Wire Wire Line
	2650 1200 2650 1100
Wire Wire Line
	1600 1200 1600 1100
Wire Wire Line
	1600 1100 1850 1100
$Comp
L Device:C C4
U 1 1 5F6FDD07
P 5900 1500
F 0 "C4" H 6015 1546 50  0000 L CNN
F 1 "10uF" H 6015 1455 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 5938 1350 50  0001 C CNN
F 3 "~" H 5900 1500 50  0001 C CNN
	1    5900 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 1100 5900 1100
Wire Wire Line
	5900 1350 5900 1100
Connection ~ 5900 1100
Wire Wire Line
	5900 1100 6400 1100
Wire Wire Line
	5900 1650 5900 1900
Wire Wire Line
	6400 1650 6400 1900
Wire Wire Line
	4150 1100 4350 1100
Text GLabel 10100 4300 2    50   Input ~ 0
ONEWIRE_BUS
$Comp
L power:GND #PWR058
U 1 1 600D863B
P 5850 7400
F 0 "#PWR058" H 5850 7150 50  0001 C CNN
F 1 "GND" V 5855 7272 50  0000 R CNN
F 2 "" H 5850 7400 50  0001 C CNN
F 3 "" H 5850 7400 50  0001 C CNN
	1    5850 7400
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR055
U 1 1 600D809F
P 5850 7050
F 0 "#PWR055" H 5850 6800 50  0001 C CNN
F 1 "GND" V 5855 6922 50  0000 R CNN
F 2 "" H 5850 7050 50  0001 C CNN
F 3 "" H 5850 7050 50  0001 C CNN
	1    5850 7050
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR052
U 1 1 600D7402
P 5850 6700
F 0 "#PWR052" H 5850 6450 50  0001 C CNN
F 1 "GND" V 5855 6572 50  0000 R CNN
F 2 "" H 5850 6700 50  0001 C CNN
F 3 "" H 5850 6700 50  0001 C CNN
	1    5850 6700
	0    -1   -1   0   
$EndComp
$Comp
L Jumper:SolderJumper_2_Open ADR-A3
U 1 1 600D65E7
P 5700 7400
F 0 "ADR-A3" H 5700 7605 50  0000 C CNN
F 1 "SolderJumper_2_Open" H 5700 7514 50  0000 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_TrianglePad1.0x1.5mm" H 5700 7400 50  0001 C CNN
F 3 "~" H 5700 7400 50  0001 C CNN
	1    5700 7400
	1    0    0    -1  
$EndComp
$Comp
L Jumper:SolderJumper_2_Open ADR-A2
U 1 1 600D5BDF
P 5700 7050
F 0 "ADR-A2" H 5700 7255 50  0000 C CNN
F 1 "SolderJumper_2_Open" H 5700 7164 50  0000 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_TrianglePad1.0x1.5mm" H 5700 7050 50  0001 C CNN
F 3 "~" H 5700 7050 50  0001 C CNN
	1    5700 7050
	1    0    0    -1  
$EndComp
$Comp
L Jumper:SolderJumper_2_Open ADR-A1
U 1 1 600D49D4
P 5700 6700
F 0 "ADR-A1" H 5700 6905 50  0000 C CNN
F 1 "SolderJumper_2_Open" H 5700 6814 50  0000 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_TrianglePad1.0x1.5mm" H 5700 6700 50  0001 C CNN
F 3 "~" H 5700 6700 50  0001 C CNN
	1    5700 6700
	1    0    0    -1  
$EndComp
Text GLabel 5550 7400 0    50   Input ~ 0
PCF_A2
Text GLabel 5550 7050 0    50   Input ~ 0
PCF_A1
Text GLabel 5550 6700 0    50   Input ~ 0
PCF_A0
$Comp
L power:GND #PWR01
U 1 1 600F9331
P 8050 1050
F 0 "#PWR01" H 8050 800 50  0001 C CNN
F 1 "GND" V 8055 922 50  0000 R CNN
F 2 "" H 8050 1050 50  0001 C CNN
F 3 "" H 8050 1050 50  0001 C CNN
	1    8050 1050
	0    1    1    0   
$EndComp
$Comp
L Device:R R1
U 1 1 60B54615
P 1100 1350
F 0 "R1" H 1170 1396 50  0000 L CNN
F 1 "150" H 1170 1305 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 1030 1350 50  0001 C CNN
F 3 "~" H 1100 1350 50  0001 C CNN
	1    1100 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D1
U 1 1 60B5461B
P 1100 1650
F 0 "D1" V 1139 1532 50  0000 R CNN
F 1 "PWR 5V" V 1048 1532 50  0000 R CNN
F 2 "LED_SMD:LED_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 1100 1650 50  0001 C CNN
F 3 "~" H 1100 1650 50  0001 C CNN
	1    1100 1650
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R2
U 1 1 60B54621
P 3600 1350
F 0 "R2" H 3670 1396 50  0000 L CNN
F 1 "68" H 3670 1305 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 3530 1350 50  0001 C CNN
F 3 "~" H 3600 1350 50  0001 C CNN
	1    3600 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D2
U 1 1 60B54627
P 3600 1650
F 0 "D2" V 3639 1532 50  0000 R CNN
F 1 "PWR 3.3V" V 3548 1532 50  0000 R CNN
F 2 "LED_SMD:LED_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 3600 1650 50  0001 C CNN
F 3 "~" H 3600 1650 50  0001 C CNN
	1    3600 1650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	300  2350 300  2550
Wire Wire Line
	1600 1500 1600 1900
Wire Wire Line
	2150 1400 2150 1900
Wire Wire Line
	2650 1500 2650 1900
Wire Wire Line
	3600 1100 3600 1200
Wire Wire Line
	2650 1100 3100 1100
Connection ~ 3600 1100
Wire Wire Line
	1600 1100 1400 1100
Connection ~ 1600 1100
Wire Wire Line
	1100 1100 1100 1200
Connection ~ 1100 1100
Wire Wire Line
	1100 1800 1100 1900
Wire Wire Line
	4950 5400 4950 5450
Wire Wire Line
	4950 5450 5850 5450
Connection ~ 4950 5450
Wire Wire Line
	4950 5450 4950 5500
Wire Wire Line
	5850 5450 5850 5650
$Comp
L Jumper:Jumper_3_Bridged12 JP2
U 1 1 60BB76AD
P 9750 5750
F 0 "JP2" V 9650 5550 50  0000 C CNN
F 1 "SolderJumper_3_Open" V 9550 5200 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 9750 5750 50  0001 C CNN
F 3 "~" H 9750 5750 50  0001 C CNN
	1    9750 5750
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Conn_01x02_Female J6
U 1 1 5F5C5063
P 10300 5650
F 0 "J6" H 10328 5626 50  0000 L CNN
F 1 "Conn_01x02_Female" H 10328 5535 50  0000 L CNN
F 2 "Connector_JST:JST_PH_B2B-PH-K_1x02_P2.00mm_Vertical" H 10300 5650 50  0001 C CNN
F 3 "~" H 10300 5650 50  0001 C CNN
	1    10300 5650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR042
U 1 1 60BC69F4
P 9750 6000
F 0 "#PWR042" H 9750 5750 50  0001 C CNN
F 1 "GND" H 9755 5827 50  0000 C CNN
F 2 "" H 9750 6000 50  0001 C CNN
F 3 "" H 9750 6000 50  0001 C CNN
	1    9750 6000
	1    0    0    -1  
$EndComp
Wire Wire Line
	10000 5450 10000 5650
Wire Wire Line
	10000 5650 10100 5650
Wire Wire Line
	10100 5750 9900 5750
$Comp
L power:GND #PWR040
U 1 1 60A019A9
P 7000 5950
F 0 "#PWR040" H 7000 5700 50  0001 C CNN
F 1 "GND" H 7005 5777 50  0000 C CNN
F 2 "" H 7000 5950 50  0001 C CNN
F 3 "" H 7000 5950 50  0001 C CNN
	1    7000 5950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR029
U 1 1 6011912E
P 9350 4600
F 0 "#PWR029" H 9350 4350 50  0001 C CNN
F 1 "GND" H 9355 4427 50  0000 C CNN
F 2 "" H 9350 4600 50  0001 C CNN
F 3 "" H 9350 4600 50  0001 C CNN
	1    9350 4600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR031
U 1 1 5F6A9851
P 2800 4750
F 0 "#PWR031" H 2800 4500 50  0001 C CNN
F 1 "GND" H 2805 4577 50  0000 C CNN
F 2 "" H 2800 4750 50  0001 C CNN
F 3 "" H 2800 4750 50  0001 C CNN
	1    2800 4750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR032
U 1 1 60A2AF62
P 4050 4750
F 0 "#PWR032" H 4050 4500 50  0001 C CNN
F 1 "GND" H 4055 4577 50  0000 C CNN
F 2 "" H 4050 4750 50  0001 C CNN
F 3 "" H 4050 4750 50  0001 C CNN
	1    4050 4750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR034
U 1 1 60A31EE3
P 5350 4750
F 0 "#PWR034" H 5350 4500 50  0001 C CNN
F 1 "GND" H 5355 4577 50  0000 C CNN
F 2 "" H 5350 4750 50  0001 C CNN
F 3 "" H 5350 4750 50  0001 C CNN
	1    5350 4750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR033
U 1 1 60B227F3
P 4500 4750
F 0 "#PWR033" H 4500 4500 50  0001 C CNN
F 1 "GND" H 4505 4577 50  0000 C CNN
F 2 "" H 4500 4750 50  0001 C CNN
F 3 "" H 4500 4750 50  0001 C CNN
	1    4500 4750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR014
U 1 1 5F624AC7
P 6400 1900
F 0 "#PWR014" H 6400 1650 50  0001 C CNN
F 1 "GND" H 6405 1727 50  0000 C CNN
F 2 "" H 6400 1900 50  0001 C CNN
F 3 "" H 6400 1900 50  0001 C CNN
	1    6400 1900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR013
U 1 1 5F5AA1FB
P 5900 1900
F 0 "#PWR013" H 5900 1650 50  0001 C CNN
F 1 "GND" H 5905 1727 50  0000 C CNN
F 2 "" H 5900 1900 50  0001 C CNN
F 3 "" H 5900 1900 50  0001 C CNN
	1    5900 1900
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR012
U 1 1 5F5A66A2
P 4750 1900
F 0 "#PWR012" H 4750 1650 50  0001 C CNN
F 1 "GND" H 4755 1727 50  0000 C CNN
F 2 "" H 4750 1900 50  0001 C CNN
F 3 "" H 4750 1900 50  0001 C CNN
	1    4750 1900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR07
U 1 1 60B5460F
P 1100 1900
F 0 "#PWR07" H 1100 1650 50  0001 C CNN
F 1 "GND" H 1105 1727 50  0000 C CNN
F 2 "" H 1100 1900 50  0001 C CNN
F 3 "" H 1100 1900 50  0001 C CNN
	1    1100 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 1800 3600 1900
$Comp
L power:GND #PWR0101
U 1 1 60C04707
P 1600 1900
F 0 "#PWR0101" H 1600 1650 50  0001 C CNN
F 1 "GND" H 1605 1727 50  0000 C CNN
F 2 "" H 1600 1900 50  0001 C CNN
F 3 "" H 1600 1900 50  0001 C CNN
	1    1600 1900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 60C04ACF
P 2150 1900
F 0 "#PWR0102" H 2150 1650 50  0001 C CNN
F 1 "GND" H 2155 1727 50  0000 C CNN
F 2 "" H 2150 1900 50  0001 C CNN
F 3 "" H 2150 1900 50  0001 C CNN
	1    2150 1900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 60C04D65
P 2650 1900
F 0 "#PWR0103" H 2650 1650 50  0001 C CNN
F 1 "GND" H 2655 1727 50  0000 C CNN
F 2 "" H 2650 1900 50  0001 C CNN
F 3 "" H 2650 1900 50  0001 C CNN
	1    2650 1900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 60C05098
P 3600 1900
F 0 "#PWR0104" H 3600 1650 50  0001 C CNN
F 1 "GND" H 3605 1727 50  0000 C CNN
F 2 "" H 3600 1900 50  0001 C CNN
F 3 "" H 3600 1900 50  0001 C CNN
	1    3600 1900
	1    0    0    -1  
$EndComp
NoConn ~ 6550 4250
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 60C0AE80
P 1400 1100
F 0 "#FLG0101" H 1400 1175 50  0001 C CNN
F 1 "PWR_FLAG" H 1400 1273 50  0000 C CNN
F 2 "" H 1400 1100 50  0001 C CNN
F 3 "~" H 1400 1100 50  0001 C CNN
	1    1400 1100
	1    0    0    -1  
$EndComp
Connection ~ 1400 1100
Wire Wire Line
	1400 1100 1100 1100
Wire Wire Line
	4650 4250 4500 4250
Connection ~ 4500 4250
Wire Wire Line
	9200 3900 9350 3900
Connection ~ 9350 3900
Wire Wire Line
	9350 3900 9350 4000
$Comp
L Connector:Conn_01x14_Female J8
U 1 1 5F59BA48
P 1450 6600
F 0 "J8" H 1478 6576 50  0000 L CNN
F 1 "Conn_01x14_Female" H 1478 6485 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x14_P2.54mm_Vertical" H 1450 6600 50  0001 C CNN
F 3 "~" H 1450 6600 50  0001 C CNN
	1    1450 6600
	1    0    0    -1  
$EndComp
Wire Wire Line
	9950 1550 9950 1050
Connection ~ 9950 1550
Connection ~ 9950 1050
Wire Wire Line
	9950 1050 10100 1050
Wire Wire Line
	6550 1500 6700 1500
Connection ~ 2650 1100
$Comp
L Device:C C6
U 1 1 60E487EC
P 3100 1350
F 0 "C6" H 3215 1396 50  0000 L CNN
F 1 "1000uF" H 3215 1305 50  0000 L CNN
F 2 "Capacitor_SMD:CP_Elec_10x10.5" H 3138 1200 50  0001 C CNN
F 3 "~" H 3100 1350 50  0001 C CNN
	1    3100 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	3100 1200 3100 1100
Connection ~ 3100 1100
Wire Wire Line
	3100 1100 3600 1100
$Comp
L power:GND #PWR0105
U 1 1 60E4C455
P 3100 1900
F 0 "#PWR0105" H 3100 1650 50  0001 C CNN
F 1 "GND" H 3105 1727 50  0000 C CNN
F 2 "" H 3100 1900 50  0001 C CNN
F 3 "" H 3100 1900 50  0001 C CNN
	1    3100 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	3100 1900 3100 1500
Wire Wire Line
	8800 5950 8800 6000
Wire Wire Line
	9750 5500 8800 5500
Wire Wire Line
	8800 5500 8800 5550
Wire Wire Line
	9550 3550 10000 3550
Wire Wire Line
	9350 3900 9850 3900
Wire Wire Line
	9850 3900 9850 3950
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 60C1218A
P 10100 3900
F 0 "#FLG0102" H 10100 3975 50  0001 C CNN
F 1 "PWR_FLAG" V 10100 4027 50  0000 L CNN
F 2 "" H 10100 3900 50  0001 C CNN
F 3 "~" H 10100 3900 50  0001 C CNN
	1    10100 3900
	0    1    1    0   
$EndComp
Wire Wire Line
	10100 3900 9850 3900
Connection ~ 9850 3900
Wire Wire Line
	9350 3700 9350 3900
Wire Wire Line
	9650 4300 9850 4300
Wire Wire Line
	9850 4250 9850 4300
Connection ~ 9850 4300
Wire Wire Line
	9850 4300 10100 4300
$Comp
L power:GND1 #PWR0106
U 1 1 60C85E01
P 9450 2600
F 0 "#PWR0106" H 9450 2350 50  0001 C CNN
F 1 "GND1" H 9455 2427 50  0000 C CNN
F 2 "" H 9450 2600 50  0001 C CNN
F 3 "" H 9450 2600 50  0001 C CNN
	1    9450 2600
	1    0    0    -1  
$EndComp
$Comp
L power:GND1 #PWR0107
U 1 1 60C86117
P 9950 2600
F 0 "#PWR0107" H 9950 2350 50  0001 C CNN
F 1 "GND1" H 9955 2427 50  0000 C CNN
F 2 "" H 9950 2600 50  0001 C CNN
F 3 "" H 9950 2600 50  0001 C CNN
	1    9950 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 4350 4050 4350
Wire Wire Line
	4500 4250 3400 4250
Wire Wire Line
	3400 3950 4350 3950
Wire Wire Line
	3400 3850 4050 3850
Wire Wire Line
	3400 3650 4700 3650
Wire Wire Line
	3400 3450 3700 3450
Text GLabel 1450 3550 0    50   Input ~ 0
LDR
$Comp
L RF_Module:ESP-12E U4
U 1 1 5F6A3FB9
P 2800 4050
F 0 "U4" H 2800 4000 50  0000 C CNN
F 1 "ESP-12E" H 2800 4150 50  0000 C CNN
F 2 "RF_Module:ESP-12E" H 2800 4050 50  0001 C CNN
F 3 "http://wiki.ai-thinker.com/_media/esp8266/esp8266_series_modules_user_manual_v1.1.pdf" H 2450 4150 50  0001 C CNN
	1    2800 4050
	1    0    0    -1  
$EndComp
$Comp
L Device:R R18
U 1 1 60D71AC2
P 1650 4000
F 0 "R18" H 1580 3954 50  0000 R CNN
F 1 "100K" H 1580 4045 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 1580 4000 50  0001 C CNN
F 3 "~" H 1650 4000 50  0001 C CNN
	1    1650 4000
	-1   0    0    1   
$EndComp
$Comp
L Device:R R17
U 1 1 60D7219E
P 1650 3700
F 0 "R17" H 1580 3654 50  0000 R CNN
F 1 "220K" H 1580 3745 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 1580 3700 50  0001 C CNN
F 3 "~" H 1650 3700 50  0001 C CNN
	1    1650 3700
	-1   0    0    1   
$EndComp
Wire Wire Line
	2200 3850 1650 3850
Connection ~ 1650 3850
$Comp
L power:GND #PWR0108
U 1 1 60D82323
P 1650 4700
F 0 "#PWR0108" H 1650 4450 50  0001 C CNN
F 1 "GND" H 1655 4527 50  0000 C CNN
F 2 "" H 1650 4700 50  0001 C CNN
F 3 "" H 1650 4700 50  0001 C CNN
	1    1650 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	1650 4700 1650 4150
Wire Wire Line
	1450 3550 1650 3550
Wire Wire Line
	1200 3450 1650 3450
$Comp
L power:+3V3 #PWR0109
U 1 1 60E575B5
P 4050 6600
F 0 "#PWR0109" H 4050 6450 50  0001 C CNN
F 1 "+3V3" V 4050 6850 50  0000 C CNN
F 2 "" H 4050 6600 50  0001 C CNN
F 3 "" H 4050 6600 50  0001 C CNN
	1    4050 6600
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0110
U 1 1 60E57B7D
P 4050 6700
F 0 "#PWR0110" H 4050 6450 50  0001 C CNN
F 1 "GND" V 4055 6572 50  0000 R CNN
F 2 "" H 4050 6700 50  0001 C CNN
F 3 "" H 4050 6700 50  0001 C CNN
	1    4050 6700
	0    1    1    0   
$EndComp
Text GLabel 4050 6800 0    50   Input ~ 0
SCL
Text GLabel 4050 6900 0    50   Input ~ 0
SDA
$Comp
L Connector:Conn_01x04_Male J11
U 1 1 60E5634D
P 4250 6700
F 0 "J11" H 4222 6674 50  0000 R CNN
F 1 "Conn_01x04_Female" H 4222 6583 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 4250 6700 50  0001 C CNN
F 3 "~" H 4250 6700 50  0001 C CNN
	1    4250 6700
	-1   0    0    -1  
$EndComp
$EndSCHEMATC
