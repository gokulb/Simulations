BEGIN {
	recvdSize = 0
	startTime = 1e6
	stopTime = 0
}

{
	# Trace line format: normal
	if ($2 != "-t") {
		event = $1
		time = $2
		if (event == "+" || event == "-") node_id = $3
		if (event == "r" || event == "d") node_id = $4
		flow_id = $8
		pkt_id = $12
		pkt_size = $6
		flow_t = $5
		level = "AGT"
	}
	# Trace line format: new
	if ($2 == "-t") {
		event = $1
		time = $3
		node_id = $5
		flow_id = $39
		pkt_id = $41
		pkt_size = $37
		flow_t = $45
		level = $19
	}

	# Store packets send time
	if (level == "AGT" && flow_id == flow && node_id == src &&
	    sendTime[pkt_id] == 0 && (event == "+" || event == "s") && pkt_size >= pkt) {
		if (time < startTime) {
			startTime = time
		}
		sendTime[pkt_id] = time
		this_flow = flow_t
	}

	# Update total received packets' size and store packets arrival time
	if (level == "AGT" && flow_id == flow && node_id == dst &&
	    event == "r" && pkt_size >= pkt) {
		if (time > stopTime) {
			stopTime = time
		}
		# Rip off the header
		hdr_size = pkt_size % pkt
		pkt_size -= hdr_size
		# Store received packet's size
		recvdSize += pkt_size
		# Store packet's reception time
		recvTime[pkt_id] = time
	}

}

END {
	# Compute average delay
	delay = avg_delay = recvdNum = 0
	for (i in recvTime) {
		if (sendTime[i] == 0) {
			printf("\nError in delay.awk: receiving a packet that wasn't sent %g\n",i)
		}
		delay += recvTime[i] - sendTime[i]
		recvdNum ++
	}
	if (recvdNum != 0) {
		avg_delay = delay / recvdNum
	} else {
		avg_delay = 0
	}

	# Compute average jitters
	jitter1 = jitter2 = jitter3 = jitter4 = jitter5 = 0
	prev_time = delay = prev_delay = processed = deviation = 0
	prev_delay = -1
	for (i=0; processed<recvdNum; i++) {
		if(recvTime[i] != 0) {
			if(prev_time != 0) {
				delay = recvTime[i] - prev_time
				e2eDelay = recvTime[i] - sendTime[i]
				if(delay < 0) delay = 0
				if(prev_delay != -1) {
					jitter1 += abs(e2eDelay - prev_e2eDelay)
					jitter2 += abs(delay-prev_delay)
					jitter3 += (abs(e2eDelay-prev_e2eDelay) - jitter3) / 16
					jitter4 += (abs(delay-prev_delay) - jitter4) / 16
				 }
#				 deviation += (e2eDelay-avg_delay)*(e2eDelay-avg_delay)
				 prev_delay = delay
				 prev_e2eDelay = e2eDelay
			}
			prev_time = recvTime[i]
			processed++
		}
	}
	if (recvdNum != 0) {
		jitter1 = jitter1*1000/recvdNum
		jitter2 = jitter2*1000/recvdNum
	}
#	if (recvdNum > 1) {
#		jitter5 = sqrt(deviation/(recvdNum-1))
#	}

	# Output
	if (recvdNum == 0) {
		printf("####################################################################\n" \
		       "#  Warning: no packets were received, simulation may be too short  #\n" \
		       "####################################################################\n\n")
	}
	printf("\n")
	printf(" %15s:  %g\n", "flowID", flow)
	printf(" %15s:  %s\n", "flowType", this_flow)
	printf(" %15s:  %d\n", "srcNode", src)
	printf(" %15s:  %d\n", "destNode", dst)
	printf(" %15s:  %d\n", "startTime", startTime)
	printf(" %15s:  %d\n", "stopTime", stopTime)
	printf(" %15s:  %g\n", "receivedPkts", recvdNum)
	printf(" %15s:  %g\n", "avgTput[kbps]", (recvdSize/(stopTime-startTime))*(8/1000))
	printf(" %15s:  %g\n", "avgDelay[ms]", avg_delay*1000)
	printf(" %15s:  %g\n", "avgJitter1[ms]", jitter1)
	printf(" %15s:  %g\n", "avgJitter2[ms]", jitter2)
	printf(" %15s:  %g\n", "avgJitter3[ms]", jitter3*1000)
	printf(" %15s:  %g\n", "avgJitter4[ms]", jitter4*1000)
#	printf(" %15s:  %g\n", "avgJitter5[ms]", jitter5*1000)

#	%9s %4s %4s %6s %5s %13s %14s %13s %15s %15s %15s %15s %15s\n\n",	\
#	       "flow","flowType","src","dst","start","stop","receivedPkts",		\
#	       "avgTput[kbps]","avgDelay[ms]","avgJitter1[ms]","avgJitter2[ms]",	\
#	       "avgJitter3[ms]","avgJitter4[ms]","avgJitter5[ms]")
#	printf("  %6g %9s %4d %4d %6d %5d %13g %14s %13s %15s %15s %15s %15s\n\n",	\
#	       flow,this_flow,src,dst,startTime, stopTime, recvdNum,			\
#	       (recvdSize/(stopTime-startTime))*(8/1000),avg_delay*1000,		\
#	       jitter1,jitter2,jitter3*1000,jitter4*1000,jitter5*1000)
}

function abs(value) {
	if (value < 0) value = 0-value
	return value
}
