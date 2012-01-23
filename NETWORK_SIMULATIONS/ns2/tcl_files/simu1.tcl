# ======================================================================
# Define options
# ======================================================================

set val(chan)       Channel/WirelessChannel
set val(prop)       Propagation/TwoRayGround
set val(netif)      Phy/WirelessPhy
set val(mac)        Mac/802_11
set val(ifq)        Queue/DropTail/PriQueue
set val(ll)         LL
set val(ant)        Antenna/OmniAntenna
set val(x)             900   ;# X dimension of the topography
set val(y)             900   ;# Y dimension of the topography
set val(ifqlen)         50            ;# max packet in ifq
set val(seed)           0.0
set val(adhocRouting)   AOMDV
set val(nn)             50             ;# how many nodes are simulated
set val(energymodel)    EnergyModel     ;
set val(cp)             "/home/gokul/Documents/NS2/ns-allinone-2.34/WP/cbr_traf.tcl" 
set val(sc)             "/home/gokul/Documents/NS2/ns-allinone-2.34/WP/scene1a.tcl" 
set val(stop)           500.0           ;# simulation time


Mac/802_11 set RTSThreshold_ 150

# =====================================================================
# Main Program
# ======================================================================

#
# Initialize Global Variables
#

# create simulator instance

set ns_		[new Simulator]

# setup topography object

set topo	[new Topography]

# create trace object for ns and nam
#$ns_ use-newtrace
set tracefd	[open simu1a.tr w]
set namtrace    [open simu1.nam w]

$ns_ trace-all $tracefd
$ns_ namtrace-all-wireless $namtrace $val(x) $val(y)

# define topology
$topo load_flatgrid $val(x) $val(y)

#
# Create God
#
set god_ [create-god $val(nn)]

#
# define how node should be created
#

#global node setting

$ns_ node-config -adhocRouting $val(adhocRouting) \
                 -llType $val(ll) \
                 -macType $val(mac) \
                 -ifqType $val(ifq) \
                 -ifqLen $val(ifqlen) \
                 -antType $val(ant) \
                 -propType $val(prop) \
                 -phyType $val(netif) \
                 -channelType $val(chan) \
		 -topoInstance $topo \
		 -agentTrace ON \
                 -routerTrace OFF \
                 -macTrace ON \
 		 -movementTrace OFF   \
 	  -energyModel $val(energymodel) \
	  -initialEnergy 30.0 \
	  -txPower 0.6 \
	  -rxPower 0.2 

#
#  Create the specified number of nodes [$val(nn)] and "attach" them
#  to the channel. 

for {set i 0} {$i < $val(nn) } {incr i} {
	set node_($i) [$ns_ node]	
	$node_($i) random-motion 0		;# disable random motion
        $god_ new_node $node_($i)
}


# 
# Define node movement model
#
puts "Loading connection pattern..."
source $val(cp)

source $val(sc)


# TCP connection

#set tcp_(1) [$ns_ create-connection  TCP $node_(2) TCPSink $node_(7) 0]
#$tcp_(1) set window_ 32
#$tcp_(1) set packetSize_ 2312
#set ftp_(1) [$tcp_(1) attach-source FTP]
#$ns_ at 35.005172246603841 "$ftp_(1) start"
#

# UDP connection
#set udp_(0) [new Agent/UDP]
#$ns_ attach-agent $node_(2) $udp_(0)
#set null_(0) [new Agent/Null]
#$ns_ attach-agent $node_(7) $null_(0)
#set cbr_(0) [new Application/Traffic/CBR]
#$cbr_(0) set packetSize_ 512
#$cbr_(0) set interval_ 0.25
#$cbr_(0) set random_ 1
#$cbr_(0) set maxpkts_ 10000
#$cbr_(0) attach-agent $udp_(0)
#$ns_ connect $udp_(0) $null_(0)
#$ns_ at 35.00517224 "$cbr_(0) start"

# Tell nodes when the simulation ends
#
for {set i 0} {$i < $val(nn) } {incr i} {
    $ns_ at $val(stop).0 "$node_($i) reset";
}

$ns_ at  $val(stop).0002 "puts \"NS EXITING...\" ; $ns_ halt"

puts $tracefd "M 0.0 nn $val(nn) x $val(x) y $val(y) rp $val(adhocRouting)"
# puts $tracefd "M 0.0 cp $val(cp) seed $val(seed)"
puts $tracefd "M 0.0 prop $val(prop) ant $val(ant)"

puts "Starting Simulation..."
$ns_ run




