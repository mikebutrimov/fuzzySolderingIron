# fuzzySolderingIron
Arduino sketch for soldering iron station based on fuzzy  logic.

#Dependencies:

Fuzzy Logic library - https://github.com/mikebutrimov/eFLL 
(original library is here - https://github.com/zerokol/eFLL , 
but author still don't accept very necessary pull request, that is
already in my fork)

#Usage:
A0 input - analog read for thermosensor<br>
A1 and A2 inputs - for up and down buttons<br>
digitalOutput on pin 5 - for pwm control of power supply
