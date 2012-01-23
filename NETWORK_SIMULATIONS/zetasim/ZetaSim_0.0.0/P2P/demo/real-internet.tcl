# Set Global Parameter
set num 100 ;# Num of nodes.
set bServerNum 1 ;#num of boot server
set DL 3 ;#debugLevel
set duration 130 ;# duration of the simulation

Agent/UDP set packetSize_ 1000
Application/ZetaApp set packetSize_ 1000
Agent/NSSocket set packetSize_ 50000

#Set Simulation Parameters
Application/ZetaApp set tracker_threshold_ 3
Application/ZetaApp set maxUploadSpeed_ 100kb
Application/ZetaApp set isBootServer_ false
Application/ZetaApp set updatePeersInterval_ 0.5
Application/ZetaApp set codeWordSize_ 1
Application/ZetaApp set blockSize_  2000
Application/ZetaApp set dataSize_ 300000
Application/ZetaApp set startSeederTh_ 0.5


set ns [new Simulator]
#open a nam trace file
set nf [open out.nam w]
$ns namtrace-all $nf

set tf [open out.tr w]
$ns trace-all $tf

proc finish {} {
	global nf tf
	puts finished
	close $tf
	close $nf
	exec nam out.nam &
	
	exit 0
}

# Set the Toplogy
puts "--starts to generate the topology"
source ts200.tcl
create-topology ns node 1.5Mb 

# Choose the node of the P2P network
puts "--starts to choose p2p node"
set rng [new RNG]
set rseq [new RandomVariable/Uniform]
$rseq set min_ 0
$rseq set max_ 199
$rseq use-rng $rng
for {set i 0} {$i < $num} {incr i} {
    $rng seed i
    set tmp [$rseq value]
    puts [expr round($tmp)]
    set seq($i) [expr round($tmp)]
}

puts "--starts to create sock and attach it to  node"
for {set i 0} {$i < $num} {incr i} {
    set sock($i) [new Agent/NSSocket]
    $ns attach-agent $node($seq($i)) $sock($i)
	puts "attach node $i and sock $i"
}

puts "--starts to create zeta application"
for {set i 0} {$i < $num} {incr i} {
    set app($i) [new Application/ZetaApp]
}

puts "--starts to set zeta application" 
for {set i 0} {$i < $num} {incr i} {
    $app($i)	set_alias	[expr $i]
    $app($i)	init_msger	$sock($i)
    $app($i)	set isDebug	$DL
    if { $i < $bServerNum } {
	$app($i)	set	isBootServer_	true
    } else {
	for { set j 0 } { $j < $bServerNum } { incr j } {
		$app($i)	add_boot_server		$app($j)
	}
    }

}

puts "--starts to set the shared file in each application"
$app(1) shared_files add 1 /home 2 /jing/home/desktop 4 /jing/home/desktop 3 /jing/home/desktop
$app(2) shared_files replace 2 /homei
$app(3) shared_files add 2 /home3

for {set i 0 } { $i < $bServerNum } { incr i } {
		$ns at 0 "$app($i) join"
}

for { set i $bServerNum } { $i < $num } { incr i} {
    $ns at [expr 1 + $i/10 ] "$app($i) join"
}

for { set i 4 } { $i < $num } { incr i } {
	$ns at [expr 10 + $i/5] "$app($i) query 2"
	puts "app($i) query 2"
}
$ns at $duration "finish"
$ns run

