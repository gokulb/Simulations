import pprint

import xml.dom.minidom
from xml.dom.minidom import Node

doc = xml.dom.minidom.parse("results.xml")

lost_packets = 0
tx_packets = 0
rx_packets = 0
throughput = 0
stats = doc.getElementsByTagName("FlowStats")
node = stats[0].getElementsByTagName("Flow")
probe = doc.getElementsByTagName("FlowProbes")
probeElem = probe[0].getElementsByTagName("FlowStats")

delay = probeElem[1].getAttribute("delayFromFirstProbeSum")
numOfPackets = probeElem[1].getAttribute("packets")
rtt = float((delay[:-2]))*(10**-9)/float(numOfPackets)
Throughput = 1/(float((delay[:-2]))*(10**-9))

print "Throughput from rtt = ", Throughput
for i in range(0,(node.length)-1):
	tx_packets += int(node[i].getAttribute("txPackets"))
	rx_packets += int(node[i].getAttribute("rxPackets"))
	lost_packets += int(node[i].getAttribute("lostPackets"))

print "Tx Packets = ", tx_packets
print "Rx Packets = ", rx_packets
print "Lost Packets = ", lost_packets
throughput = float(rx_packets)/float(tx_packets)
print "Throughput = ", throughput
