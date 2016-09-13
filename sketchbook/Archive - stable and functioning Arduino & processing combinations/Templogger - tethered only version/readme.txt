Stable combination of templogger (arduino) and LogReader (processing) sketches.
Major limitation is reset on serial 'feature' of Arduino, meaning that you
can set up Arduino to run as desired but when you disconnect to move the unit everything is
reset.

Next steps:
- start by doing hardware fix to disable serial reset 
- add restart button (and associated code) to allow settting up instrument before logging starts
- add indicator light(s)