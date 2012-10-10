delimiter ;;
drop procedure if exists sp_flush_network;;
create procedure sp_flush_network(in sec int)
begin
    delete tb_network from tb_network,tb_node
    where tb_network.node_id  = tb_node.node_id
    and tb_network.network_id = tb_node.network_id
    and unix_timestamp(now()) - unix_timestamp(update_time) > sec;
end;;

