# oduqcux

This is the complete source code for the user interface to the ND ODU quality 
control package. This is written in c++ using WX Widgets and intended to run 
on a Raspberry Pi. This program communicates with the robot 
(https://github.com/karmgard/oduqc-robot-controller), requests moves, reads the 
response from the Arduino from LED flashes and diode reads, graphs the data 
for the operator, does some rudimentary analysis to detect faulty connections 
and saves the data for later statistical analysis.
  The robot CAD design is contained in ODU\ QC\ Robot.skp done in Sketchup 2017.