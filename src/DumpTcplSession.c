/*
 * tcplstat - TCP packets monitor and statistical tool
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "tcplstat_in.h"

void DumpTcplSession( struct TcplStatEnv *p_env , const struct pcap_pkthdr *pcaphdr , struct TcplSession *p_tcpl_session )
{
	struct TcplPacket	*p_tcpl_packet = NULL ;
	struct TcplPacket	*p_next_tcpl_packet = NULL ;
	
	if( p_env->cmd_line_para.output_level >= OUTPUT_LEVEL_1 )
	{
		struct timeval	total_elapse ;
		double		avg_packet_elapse ;
		double		avg_oppo_packet_elapse ;
		
		COPY_TIMEVAL( total_elapse , p_env->fixed_timestamp );
		DIFF_TIMEVAL( total_elapse , p_tcpl_session->begin_timestamp )
		
		if( p_tcpl_session->total_packet_count > 0 )
			avg_packet_elapse = (p_tcpl_session->total_packet_elapse_for_avg.tv_sec*1000000+p_tcpl_session->total_packet_elapse_for_avg.tv_usec) / p_tcpl_session->total_packet_count ;
		else
			avg_packet_elapse = 0 ;
		if( p_tcpl_session->total_packet_count > 0 )
			avg_oppo_packet_elapse = (p_tcpl_session->total_oppo_packet_elapse_for_avg.tv_sec*1000000+p_tcpl_session->total_oppo_packet_elapse_for_avg.tv_usec) / p_tcpl_session->total_packet_count ;
		else
			avg_oppo_packet_elapse = 0 ;
		
		printf( "S [%s:%d]->[%s:%d] | %ld.%06ld | %ld.%06ld | %ld.%06ld %ld.%06ld , %ld.%06ld %.6lf %ld.%06ld %ld.%06ld %.6lf %ld.%06ld , %ld.%06ld %ld.%06ld %ld.%06ld | %u %u\n"
			, p_tcpl_session->tcpl_addr_hr.src_ip , p_tcpl_session->tcpl_addr_hr.src_port , p_tcpl_session->tcpl_addr_hr.dst_ip , p_tcpl_session->tcpl_addr_hr.dst_port
			
			, p_tcpl_session->begin_timestamp.tv_sec , p_tcpl_session->begin_timestamp.tv_usec
			
			, total_elapse.tv_sec , total_elapse.tv_usec
			
			, p_tcpl_session->wait_for_second_syn_and_first_ack_elapse.tv_sec , p_tcpl_session->wait_for_second_syn_and_first_ack_elapse.tv_usec
			, p_tcpl_session->wait_for_after_syn_and_second_ack_elapse.tv_sec , p_tcpl_session->wait_for_after_syn_and_second_ack_elapse.tv_usec
			
			, p_tcpl_session->min_packet_elapse.tv_sec , p_tcpl_session->min_packet_elapse.tv_usec
			, avg_packet_elapse/1000000
			, p_tcpl_session->max_packet_elapse.tv_sec , p_tcpl_session->max_packet_elapse.tv_usec
			
			, p_tcpl_session->min_oppo_packet_elapse.tv_sec , p_tcpl_session->min_oppo_packet_elapse.tv_usec
			, avg_oppo_packet_elapse/1000000
			, p_tcpl_session->max_oppo_packet_elapse.tv_sec , p_tcpl_session->max_oppo_packet_elapse.tv_usec
			
			, p_tcpl_session->wait_for_first_fin_elapse.tv_sec , p_tcpl_session->wait_for_first_fin_elapse.tv_usec
			, p_tcpl_session->wait_for_second_fin_and_first_ack_elapse.tv_sec , p_tcpl_session->wait_for_second_fin_and_first_ack_elapse.tv_usec
			, p_tcpl_session->wait_for_second_ack_elapse.tv_sec , p_tcpl_session->wait_for_second_ack_elapse.tv_usec
			
			, p_tcpl_session->total_packet_count , p_tcpl_session->total_packet_data_len );
	}
	
	if( p_env->cmd_line_para.output_level >= OUTPUT_LEVEL_2 )
	{
		list_for_each_entry_safe( p_tcpl_packet , p_next_tcpl_packet , & (p_tcpl_session->tcpl_packets_list.this_node) , struct TcplPacket , this_node )
		{
			char	*direct_string = NULL ;
			
			if( p_tcpl_packet->direction_flag == 1 )
				direct_string = "->" ;
			else
				direct_string = "<-" ;
			
			printf( "P    %ld.%06ld | %ld.%06ld %ld.%06ld | [%s:%d]%s[%s:%d] | %s %u\n"
				, p_tcpl_packet->timestamp.tv_sec , p_tcpl_packet->timestamp.tv_usec
				
				, p_tcpl_packet->last_packet_elapse.tv_sec , p_tcpl_packet->last_packet_elapse.tv_usec
				, p_tcpl_packet->last_oppo_packet_elapse.tv_sec , p_tcpl_packet->last_oppo_packet_elapse.tv_usec
				
				, p_tcpl_session->tcpl_addr_hr.src_ip , p_tcpl_session->tcpl_addr_hr.src_port
				, direct_string
				, p_tcpl_session->tcpl_addr_hr.dst_ip , p_tcpl_session->tcpl_addr_hr.dst_port
				
				, p_tcpl_packet->packet_flags
				, p_tcpl_packet->packet_data_len_actually );
			
			if( p_env->cmd_line_para.output_level >= OUTPUT_LEVEL_3 )
			{
				if( p_tcpl_packet->packet_data_len_intercepted > 0 )
				{
					DumpBuffer( "D    " , "#stdout" , p_tcpl_packet->packet_data_len_intercepted , p_tcpl_packet->packet_data_intercepted );
				}
			}
			
			list_del( & (p_tcpl_packet->this_node) );
			if( p_tcpl_packet->packet_data_intercepted )
				free( p_tcpl_packet->packet_data_intercepted );
			free( p_tcpl_packet );
		}
	}
	
	return;
}
