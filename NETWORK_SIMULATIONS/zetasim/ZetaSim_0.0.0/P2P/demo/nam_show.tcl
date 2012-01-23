set ns [new Simulator] 
set nf [open out.nam w] 
$ns namtrace-all $nf 

source ts200.tcl 
create-topology ns node 1.5Mb 

$ns run 
