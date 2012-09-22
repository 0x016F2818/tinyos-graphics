delimiter ;;
drop   procedure sp_update_node_info;
create procedure sp_update_node_info(in id int,in parent_id int,
        in state varchar(10),in status varchar(10),
        in power int,in GPS point)
begin
    declare state_num smallint;
    declare status_num smallint;
    declare amount int;

    select count(*) from tb_work_state
    where state_name = state into amount;
    if amount = 0 then
        insert into tb_work_state(state_name) values(state);
        select work_state from tb_work_state
        where state_name = state into state_num;
    else
        select work_state from tb_work_state
        where state_name = state into state_num;
    end if;
    select count(*) from tb_node_status
    where status_name = status into amount;
    if amount = 0 then
        insert into tb_node_status(status_name) values(status);
        select node_status from tb_node_status
        where status_name = status into status_num;
    else
        select node_status from tb_node_status
        where status_name = status into status_num;
    end if; 
    select count(*) from tb_node
    where node_id = id into amount;
    if amount = 0 then
        insert into tb_node(node_id,parent_id,work_state,node_status,power,GPS) values(id,parent_id,state_num,status_num,power,GPS);
    else
        update tb_node
        set work_state = state_num,
            node_status = status_num,
            tb_node.parent_id = parent_id,
            tb_node.power = power,
            tb_node.GPS = GPS
        where node_id = id;
    end if;
end;
