# LED effects for 2024 CWC production of Joseph and the Amazong Technicolor Dreamcoat

This project was the first time I have dont anything significant with an Arduino and was done in a short timeframe. As such, the code could certainly be better.


## Arch

Used three runs of 24 volt individually addressible LED strips. The bottom tier was the longest run and served as the basis of all the effects. The top tier used the same LED pattern and the middle tier was reversed.

The strips were connected to 24 volt power supplies and a custom board. The board tied ground together across all strips and the Arduino. It also connected the controll lines to the arduino with appropriate resisters.

# Remote control

The code is setup to accept serial control input. Typing 'h' will print a help screen with control and scene descriptions. The Arduino was connected to a Raspberry Pi via USB which connected to the Arduino via the `screen` command. The RPi supplied an SSH connection to a remote computer via Ethernet.