#
# nodes: 50, max conn: 10, send rate: 0.20000000000000001, seed: 4
#
#
# 4 connecting to 5 at time 35.390028588189757
#
set udp_(0) [new Agent/UDP]
$ns_ attach-agent $node_(4) $udp_(0)
set null_(0) [new Agent/Null]
$ns_ attach-agent $node_(5) $null_(0)
set cbr_(0) [new Application/Traffic/CBR]
$cbr_(0) set packetSize_ 512
$cbr_(0) set interval_ 0.20000000000000001
$cbr_(0) set random_ 1
$cbr_(0) set maxpkts_ 10000
$cbr_(0) attach-agent $udp_(0)
$ns_ connect $udp_(0) $null_(0)
$ns_ at 35.390028588189757 "$cbr_(0) start"
#
# 4 connecting to 6 at time 45.331526587405953
#
set udp_(1) [new Agent/UDP]
$ns_ attach-agent $node_(4) $udp_(1)
set null_(1) [new Agent/Null]
$ns_ attach-agent $node_(6) $null_(1)
set cbr_(1) [new Application/Traffic/CBR]
$cbr_(1) set packetSize_ 512
$cbr_(1) set interval_ 0.20000000000000001
$cbr_(1) set random_ 1
$cbr_(1) set maxpkts_ 10000
$cbr_(1) attach-agent $udp_(1)
$ns_ connect $udp_(1) $null_(1)
$ns_ at 45.331526587405953 "$cbr_(1) start"
#
# 6 connecting to 7 at time 99.981005219733802
#
set udp_(2) [new Agent/UDP]
$ns_ attach-agent $node_(6) $udp_(2)
set null_(2) [new Agent/Null]
$ns_ attach-agent $node_(7) $null_(2)
set cbr_(2) [new Application/Traffic/CBR]
$cbr_(2) set packetSize_ 512
$cbr_(2) set interval_ 0.20000000000000001
$cbr_(2) set random_ 1
$cbr_(2) set maxpkts_ 10000
$cbr_(2) attach-agent $udp_(2)
$ns_ connect $udp_(2) $null_(2)
$ns_ at 99.981005219733802 "$cbr_(2) start"
#
# 6 connecting to 8 at time 42.53692169791875
#
set udp_(3) [new Agent/UDP]
$ns_ attach-agent $node_(6) $udp_(3)
set null_(3) [new Agent/Null]
$ns_ attach-agent $node_(8) $null_(3)
set cbr_(3) [new Application/Traffic/CBR]
$cbr_(3) set packetSize_ 512
$cbr_(3) set interval_ 0.20000000000000001
$cbr_(3) set random_ 1
$cbr_(3) set maxpkts_ 10000
$cbr_(3) attach-agent $udp_(3)
$ns_ connect $udp_(3) $null_(3)
$ns_ at 42.53692169791875 "$cbr_(3) start"
#
# 8 connecting to 9 at time 30.809234516140648
#
set udp_(4) [new Agent/UDP]
$ns_ attach-agent $node_(8) $udp_(4)
set null_(4) [new Agent/Null]
$ns_ attach-agent $node_(9) $null_(4)
set cbr_(4) [new Application/Traffic/CBR]
$cbr_(4) set packetSize_ 512
$cbr_(4) set interval_ 0.20000000000000001
$cbr_(4) set random_ 1
$cbr_(4) set maxpkts_ 10000
$cbr_(4) attach-agent $udp_(4)
$ns_ connect $udp_(4) $null_(4)
$ns_ at 30.809234516140648 "$cbr_(4) start"
#
# 9 connecting to 10 at time 81.939091990673489
#
set udp_(5) [new Agent/UDP]
$ns_ attach-agent $node_(9) $udp_(5)
set null_(5) [new Agent/Null]
$ns_ attach-agent $node_(10) $null_(5)
set cbr_(5) [new Application/Traffic/CBR]
$cbr_(5) set packetSize_ 512
$cbr_(5) set interval_ 0.20000000000000001
$cbr_(5) set random_ 1
$cbr_(5) set maxpkts_ 10000
$cbr_(5) attach-agent $udp_(5)
$ns_ connect $udp_(5) $null_(5)
$ns_ at 81.939091990673489 "$cbr_(5) start"
#
# 11 connecting to 12 at time 125.8597827543783
#
set udp_(6) [new Agent/UDP]
$ns_ attach-agent $node_(11) $udp_(6)
set null_(6) [new Agent/Null]
$ns_ attach-agent $node_(12) $null_(6)
set cbr_(6) [new Application/Traffic/CBR]
$cbr_(6) set packetSize_ 512
$cbr_(6) set interval_ 0.20000000000000001
$cbr_(6) set random_ 1
$cbr_(6) set maxpkts_ 10000
$cbr_(6) attach-agent $udp_(6)
$ns_ connect $udp_(6) $null_(6)
$ns_ at 125.8597827543783 "$cbr_(6) start"
#
# 16 connecting to 17 at time 13.425234301679412
#
set udp_(7) [new Agent/UDP]
$ns_ attach-agent $node_(16) $udp_(7)
set null_(7) [new Agent/Null]
$ns_ attach-agent $node_(17) $null_(7)
set cbr_(7) [new Application/Traffic/CBR]
$cbr_(7) set packetSize_ 512
$cbr_(7) set interval_ 0.20000000000000001
$cbr_(7) set random_ 1
$cbr_(7) set maxpkts_ 10000
$cbr_(7) attach-agent $udp_(7)
$ns_ connect $udp_(7) $null_(7)
$ns_ at 13.425234301679412 "$cbr_(7) start"
#
# 17 connecting to 18 at time 138.4609193999604
#
set udp_(8) [new Agent/UDP]
$ns_ attach-agent $node_(17) $udp_(8)
set null_(8) [new Agent/Null]
$ns_ attach-agent $node_(18) $null_(8)
set cbr_(8) [new Application/Traffic/CBR]
$cbr_(8) set packetSize_ 512
$cbr_(8) set interval_ 0.20000000000000001
$cbr_(8) set random_ 1
$cbr_(8) set maxpkts_ 10000
$cbr_(8) attach-agent $udp_(8)
$ns_ connect $udp_(8) $null_(8)
$ns_ at 138.4609193999604 "$cbr_(8) start"
#
# 17 connecting to 19 at time 80.688442383282094
#
set udp_(9) [new Agent/UDP]
$ns_ attach-agent $node_(17) $udp_(9)
set null_(9) [new Agent/Null]
$ns_ attach-agent $node_(19) $null_(9)
set cbr_(9) [new Application/Traffic/CBR]
$cbr_(9) set packetSize_ 512
$cbr_(9) set interval_ 0.20000000000000001
$cbr_(9) set random_ 1
$cbr_(9) set maxpkts_ 10000
$cbr_(9) attach-agent $udp_(9)
$ns_ connect $udp_(9) $null_(9)
$ns_ at 80.688442383282094 "$cbr_(9) start"
#
#Total sources/connections: 7/10
#
