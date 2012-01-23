# Set Global Parameter
set num 10 ;# Num of nodes.
set bServerNum 1 ;#num of boot server
set DL 3 ;#debugLevel

# Set the Link Parameter

Agent/UDP set packetSize_ 10000
Application/ZetaApp set packetSize_ 1000
Agent/NSSocket set packetSize_ 10000

Application/ZetaApp set tracker_threshold_ 3
Application/ZetaApp set maxUploadSpeed_ 40kb
Application/ZetaApp set isBootServer_ false
Application/ZetaApp set updatePeersInterval_ 0.5
Application/ZetaApp set codeWordSize_ 1
Application/ZetaApp set blockSize_ 400
Application/ZetaApp set dataSize_ 2000000
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

for {set i 0} {$i < $num} {incr i} {
    set node($i) [$ns node]
}

# Generate the star topology
for {set i 1} {$i <  $num } {incr i} {
    $ns duplex-link $node($i) $node(0) 100Kb 10ms DropTail
}



for {set i 0} {$i < $num} {incr i} {
    set sock($i) [new Agent/NSSocket]
    $ns attach-agent $node($i) $sock($i)
}

for {set i 0} {$i < $num} {incr i} {
    set app($i) [new Application/ZetaApp]
}

for {set i 0} {$i < $num} {incr i} {
    set para1(sock) 		$sock($i)
    set para1(address)		[expr 10 *$i + 100]
    set para1(port)		[expr 10 *$i + 10]
    set para1(alias)		[expr $i]
    set para1(isDebug)		$DL
    set para1(type)		boot_server

    $app($i)	set_alias	$para1(alias)
    $app($i)	init_msger	$para1(sock)
    $app($i)	set isDebug	$para1(isDebug)
    if { $i < $bServerNum } {
	$app($i)	set	isBootServer_	true
    } else {
	for { set j 0 } { $j < $bServerNum } { incr j } {
		$app($i)	add_boot_server		$app($j)
	}
    }

}

$app(1) shared_files add 1 /home 2 /jing/home/desktop 4 /jing/home/desktop 3 /jing/home/desktop
$app(2) shared_files replace 2 /homei
$app(3) shared_files add 2 /home3
for {set i 0 } { $i < $bServerNum } { incr i } {
		$ns at 0 "$app($i) join"
}

for { set i $bServerNum } { $i < $num } { incr i} {
    $ns at [expr 1 + $i/4 ] "$app($i) join"
}

for { set i 4 } { $i < $num } { incr i } {
	$ns at [expr 2 + $i/2] "$app($i) query 2"
	puts "app($i) query 2"
}
$ns at 200 "finish"
$ns run

