#### To Run a NS3 File
student@MIT-ICT-LAB3-08:/opt/ns-allinone-3.35/ns-3.35$ **./waf --run scratch/first**
Waf: Entering directory `/opt/ns-allinone-3.35/ns-3.35/build'
Waf: Leaving directory `/opt/ns-allinone-3.35/ns-3.35/build'
Build commands will be stored in build/compile_commands.json
'build' finished successfully (0.420s)
At time +2s client sent 1024 bytes to 10.1.1.2 port 9
At time +2.00369s server received 1024 bytes from 10.1.1.1 port 49153
At time +2.00369s server sent 1024 bytes to 10.1.1.1 port 49153
At time +2.00737s client received 1024 bytes from 10.1.1.2 port 9

#### Running AWK Files 
```
awk -f pdf.awk l3r1.tr
```
Number of Packet Sent=1.000000  
Number of Packet Dropped=0.000000  
Number of Packet Received=1.000000  
Packet Delivery Fraction (PDF)=100.000000 percent  

#### Gnuplot
```
awk -f pdf-graph.awk l3q1.tr > abc
```
- copying the contents of trace file to another file 
- Edit pdf-graphcode
```
gnuplot pdf-graphcode
```


