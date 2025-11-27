

======== AUTO WATER MOTAR CONTROL V-2 =======

Function Modes

1. Auto Mode (No save voltage)
  - Motar Run when water level low than 100%  
  - Motar Stop when water level is 100%

2. Manual Mode
  - Motar relay always open, you need to control by on/off swithches of Motar
  - Not related with sensor 
  - This Mode is prefer for external usages that not related water level sensor

3. Auto Save Mode (Save voltage)
  - Motar Run when water level low and voltage is higher than save voltage
  - Motar Stop when water level is 100% or current voltage is lower than save voltage
  - This Mode is perfer for Motar's saving by using low voltage protection

  - *Low Voltage Protection*
    - When current ACV low than SV, Motar will Stop.
    - If this situation happens more than 2 time quickly within 1 min, system will sure that is low voltage Problem.
    - If low votage problem found, system will wait perfect safe voltage to restart the motar. 
    - Motar will restart when current ACV is equal or higher than safe voltage.


Protections and Adjustments

- If no water flow after 1 min of motar start in auto save and no save modes, motar will stop immediately and lcd will show "No Water Flowing"

- You can adjust the save voltage(SV) in auto save mode by pressing the adjust btn for 3s.
  Adjust the SV by Potentionmeter and then press the adjust button for 3s to release.

  
