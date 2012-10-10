delimiter ;;

/*####################################################################*/
drop procedure if exists sp_update_node_info;;
create procedure sp_update_node_info(in net_name char(50),in nod_id int,
        in state char(50),in status char(50),
        in power int,in GPS point)
begin
    declare state_num smallint;
    declare status_num smallint;
    declare net_id int;
    declare amount int;

    select count(*),work_state from tb_work_state
    where state_name = state into amount,state_num;
    if amount = 0 then
        insert into tb_work_state(state_name) values(state);
        select work_state from tb_work_state
        where state_name = state into state_num;
    end if;

    select count(*),node_status from tb_node_status
    where status_name = status into amount,status_num;
    if amount = 0 then
        insert into tb_node_status(status_name) values(status);
        select node_status from tb_node_status
        where status_name = status into status_num;
    end if; 

    select count(*),network_id from tb_network_segment
    where network_name = net_name into amount,net_id;
    if amount = 0 then
        insert into tb_network_segment(network_name) values(net_name);
        select network_id from tb_network_segment
        where network_name = net_name into net_id;
    end if; 

    select count(*) from tb_node
    where node_id = nod_id and network_id = net_id into amount;
    if amount = 0 then
        insert into tb_node(network_id,node_id,work_state,node_status,power,GPS) values(net_id,nod_id,state_num,status_num,power,GPS);
    else
        update tb_node
        set 
            work_state          = state_num,
            node_status         = status_num,
            tb_node.power       = power,
            tb_node.GPS         = GPS
        where node_id           = nod_id
            and network_id      = net_id;
    end if;
end;;

/*####################################################################*/
drop procedure if exists sp_update_network;;
create procedure sp_update_network(in net_name char(50),in nod_id int,in par_id int,in quality int)
top:begin
    declare amount int;
    declare net_id int;

    /*judge the whether the net_mame exists in tb_network_segment*/
    select count(*),network_id from tb_network_segment
    where network_name = net_name into amount,net_id;
    if amount = 0 then 
        leave top;
    end if;
    select count(*) from tb_network
    where node_id = nod_id and parent_id = par_id into amount;
    if amount = 0 then
        insert into tb_network(network_id,node_id,parent_id,quality) values(net_id,nod_id,par_id,quality);
    else 
        update tb_network
        set parent_id = par_id,
        tb_network.quality = quality
        where network_id = net_id and node_id = nod_id;
    end if;
end;;

/*####################################################################*/
drop procedure if exists sp_insert_sense_record;;
create procedure sp_insert_sense_record(in net_name char(50),
    in nod_id        int,in temp  double(8,2),
    in photo double(8,2),in sound double(8,2),
    in x_acc double(8,2),in y_acc double(8,2),
    in x_mag double(8,2),in y_mag double(8,2),
    in pressure     double(8,2),in humidity     double(8,2),
    in shoke        double(8,2),in sense_time datetime)
top:begin
    declare amount int;
    declare net_id int;

    select count(*),network_id from tb_network_segment
    where network_name = net_name into amount,net_id;
    if amount = 0 then
        leave top;
    end if;

    insert into tb_sense(network_id,node_id,temp,
        photo,sound,x_acc,y_acc,x_mag,y_mag,
        pressure,shoke,humidity,sense_time) 
    values(net_id,nod_id,temp,photo,sound,
            x_acc,y_acc,x_mag,y_mag,pressure,
            shoke,humidity,sense_time);
end;;

/*####################################################################*/
