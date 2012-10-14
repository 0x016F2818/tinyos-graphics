delimiter ;;
drop procedure if exists sp_flush_network;;
create procedure sp_flush_network(in sec int)
begin
    delete tb_network from tb_network,
    (select network_id,node_id,insert_time from tb_network left join ( select network_id, node_id, insert_time from (select network_id, node_id, insert_time from tb_sense  order by insert_time desc) as tb_temp group by node_id) as tb_temp using (network_id,node_id)) as tb_temp
    where tb_network.node_id  = tb_temp.node_id
    and tb_network.network_id = tb_temp.network_id
    and unix_timestamp(now()) - unix_timestamp(insert_time) > sec;
end;;
delimiter ;
