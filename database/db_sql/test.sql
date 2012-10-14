delimiter ;;
drop procedure if exists sp_test;;
create procedure sp_test()
begin
    select * from tb_network left join ( select network_id, node_id, insert_time from (select network_id, node_id, insert_time from tb_sense  order by insert_time desc) as tb_temp group by node_id) as tb_temp using (network_id,node_id);
end;;
delimiter ;
