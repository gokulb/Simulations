# awk -f arkAna.awk tracefile_Name
# This is an awk program that summarizes information about Network Simulation
#
BEGIN {
   highest_packet_id = 0;
   highest_flow_id = 0;
}
############################################################################################
##################### Packet Delivery Fraction #############################################
##########################################################################################

//                 { total++ }
/AGT/||/RTR/||/MAC/ || /ARP/     { totalNL++ }          	# total packets (Network trace Level)
/AGT/            {agt++}         				# total packets   AGT
/RTR/            {rtr++}         				# total packets   RTR
/MAC/            {mac++}         				# total packets   MAC
/ARP/            {arp++}         				# total packets   ARP
/AGT/ && /-It cbr/ {data_agt++}					# total data packets
/^s/		 {sent++}        				# sent packets total
/^s/ && /AGT/    {agt_sent++}          				# sent packets AGT
/^s/ && /RTR/    {rtr_sent++}          				# sent packets RTR
/^s/ && /MAC/    {mac_sent++}          				# sent packets MAC
/^s/ && /AGT/ && /-It cbr/ {data_agt_sent++}			# sent data packets
/^r/		 {rec++}         				# received packets total
/^r/ && /AGT/    {agt_rec++}           				# received packets AGT
/^r/ && /RTR/    {rtr_rec++}           				# received packets RTR
/^r/ && /MAC/    {mac_rec++}           				# received packets MAC
/^r/ && /AGT/ && /-It cbr/ {data_agt_rec++}			# received data packets
/^d/		 {drop++}        				# dropped packets total
/^d/ && /AGT/    {agt_drop++}          				# dropped packets AGT
/^d/ && /RTR/    {rtr_drop++}          				# dropped packets RTR
/^d/ && /MAC/    {mac_drop++}          				# dropped packets MAC
/^d/ && /AGT/ && /-It cbr/ {data_agt_drop++}			# dropped data packets
/^d/ && /IFQ/    {ifq_drop++}          				# ROP_IFQ_QFULL i.e no buffer space in IFQ.
/^f/             {forw++}        				# forwarded packets total
/^f/ && /AGT/    {agt_forw++}          				# forwarded packets AGT
/^f/ && /RTR/    {rtr_forw++}          				# forwarded packets RTR
/^f/ && /MAC/    {mac_forw++}          				# forwarded packets MAC
/^f/ && /AGT/ && /-It cbr/ {data_agt_forw++}			# forward data packets
/^f/ && /RTR/ && /\-It DSR/ {cp_forw++}				# Controll Packets forwarded
/^d/ && /RTR/ && /\-It DSR/ {cp_drop++}				# Controll Packets drop
/^r/ && /RTR/ && /\-It DSR/ {if ($7".255" == $33) cp_rec}	# Controll Packets received
/^s/ && /RTR/ && /\-It DSR/ && /-Pq 1/ {rreq++}			# Route Requests sent
/^s/ && /RTR/ && /\-It DSR/ && /-Pp 1/ {rrep++}			# Route Reply sent
/^s/ && /RTR/ && /\-It DSR/ && /-Pw 1/ {rerr++}			# Route Error sent

#######################################################################################
############## THROUGHPUT ANALYSE  ###################################################
######################################################################################
{  action = $1;
   time = $3;
   packet_size = $37;
   packet_id = $47;
   flow_id = $41;

   if ( packet_id > highest_packet_id ) highest_packet_id = packet_id;
   if ( flow_id > highest_flow_id ) highest_flow_id = flow_id;

   if ( ($19 == "AGT") && (start_time[flow_id] == 0) ){
     start_time[flow_id] = time;
     size[flow_id] = packet_size;
   }

   if ( ($1 == "r") && ($19 == "AGT")) {
         end_time[flow_id] = time;
   }
   if ($1 == "d")
      end_time[flow_id] = -1;
}
END { 
######################################################################################
#################### Packet Delivery Fraction #######################################
######################################################################################

      cp_sent = rreq + rrep + rerr;     			# Controll Packets sent
      cp_lost = cp_sent - cp_rec;       			# Controll Packets lost
      if (data_agt_sent > 0) pdf = (data_agt_rec/data_agt_sent)*100;        # Packet Delivery Fraction in percentage
      if (data_agt_rec > 0) nrl = ((cp_sent + cp_forw)/data_agt_rec)*100;  # Normalized Routing Load
      lost = sent - rec;                			# Packets lost
      data_lost = data_agt_sent - data_agt_rec; 		# Data Packets lost
      agt_lost = agt_sent - agt_rec;     			# Packets lost by Agent
      mac_lost = mac_sent - mac_rec;     			# Packets lost by Mac
      rtr_lost = agt_lost + cp_lost;     			# Packets lost by Router
      print "Data Analysis:";  # Print results.
      printf ("\n");
      printf ("   Packets total:        			%2d\n", totalNL);
      printf ("   Packets RTR:          			%2d\n", rtr);
      printf ("   Packets MAC:          			%2d\n", mac);
      printf ("   Packets Data AGT:         			%2d\n", data_agt);
      printf ("\n");
      printf ("   Packets sent by Agent:        		%2d\n", agt_sent);
      printf ("   Packets received by Agent:    		%2d\n", agt_rec);
      printf ("   Packets forwarded by Agent:   		%2d\n", agt_forw);
      printf ("   Packets dropped by Agent:     		%2d\n", agt_drop);
      printf ("   Packets lost by Agent:        		%2d\n", agt_lost);
      printf ("\n");
      printf ("   Packets sent by Router:       		%2d\n", rtr_sent);
      printf ("   Packets received by Router:   		%2d\n", rtr_rec);
      printf ("   Packets forwarded by Routers: 		%2d\n", rtr_forw);
      printf ("   Packets dropped by Routers:   		%2d\n", rtr_drop);
      printf ("   Packets lost by Routers:      		%2d\n", rtr_lost);
      printf ("\n");
      printf ("   Packets sent by Mac-Layer:    		%2d\n", mac_sent);
      printf ("   Packets received by Mac-Layer:		%2d\n", mac_rec);
      printf ("   Packets forwarded by Mac-Laye:		%2d\n", mac_forw);
      printf ("   Packets dropped by Mac-Layer: 		%2d\n", mac_drop);
      printf ("   Packets lost by Mac-Layer:    		%2d\n", mac_lost);
      printf ("\n");
      printf ("   Data Packets sent:    			%2d\n", data_agt_sent);
      printf ("   Data Packets received:        		%2d\n", data_agt_rec);
      printf ("   Data Packets forwarded:       		%2d\n", data_agt_forw);
      printf ("   Data Packets dropped:         		%2d\n", data_agt_drop);
      printf ("   Data Packets lost:            		%2d\n", data_lost);
      printf ("\n");
      printf ("   Controll Packets sent:          		%2d\n", cp_sent);
      printf ("   Controll Packets forwarded:     		%2d\n", cp_forw);
      printf ("   Controll Packets dropped:       		%2d\n", cp_drop);
      printf ("   Controll Packets received:      		%2d\n", cp_rec);
      printf ("   Controll Packets lost:          		%2d\n", cp_lost);
      printf ("\n");
      printf ("   Packet Delivery Fraction (Percentage):        %3.2f\n", pdf);
      printf ( "  Normalized Routing Load (Percentage):         %3.2f\n", nrl);
      
####################################################################################
##################### Throughput ###################################################
#####################################################################################

    for ( flow_id = 0; flow_id <= highest_flow_id; flow_id++ ) {
       start = start_time[flow_id];
       end = end_time[flow_id];
       if ( start < end ){
	 packet_duration = end - start;      # single distance
         duration_total += packet_duration;  # total duration
	 flow_number++;	                     # flow number
         bits_total += $37;                  # Bits Total
       }

   }
    thrgputPackets = flow_number / duration_total;
    thrgputBits    = bits_total / duration_total;

    printf ("Total Number of Flows: %d\n", flow_number);
    printf ("Average Duration of packets per sec.: %f\n", thrgputPackets);
    printf ("Average Duration of Bits per sec.: %f\n", thrgputBits);
}
