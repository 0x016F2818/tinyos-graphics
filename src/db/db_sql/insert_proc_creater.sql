delimiter ;;

/*####################################################################*/
drop   procedure update_node_info_sp;
create procedure update_node_info_sp(in id int,
        in state varchar(10),in status varchar(10),
        in power int,in GPS point)
begin
    declare state_num smallint;
    declare status_num smallint;
    declare amount int;

    select count(*) from work_state_tb
    where state_name = state into amount;
    if amount = 0 then
        insert into work_state_tb(state_name) values(state);
        select work_state from work_state_tb
        where state_name = state into state_num;
    else
        select work_state from work_state_tb
        where state_name = state into state_num;
    end if;
    select count(*) from node_status_tb
    where status_name = status into amount;
    if amount = 0 then
        insert into node_status_tb(status_name) values(status);
        select node_status from node_status_tb
        where status_name = status into status_num;
    else
        select node_status from node_status_tb
        where status_name = status into status_num;
    end if; 
    select count(*) from node_tb
    where node_id = id into amount;
    if amount = 0 then
        insert into node_tb(node_id,work_state,node_status,power,GPS) values(id,state_num,status_num,power,GPS);
    else
        update node_tb
        set work_state = state_num,
            node_status = status_num,
            node_tb.power = power,
            node_tb.GPS = GPS
        where node_id = id;
    end if;
end;

/*####################################################################*/
drop procedure if exists update_network_sp;
create procedure update_network_sp(in id int,in p_id int)
begin
    declare amount int;
    select count(*) from network_tb
    where node_id = id and parent_id = p_id into amount;
    if amount = 0 then
        /*select count(*) from node_tb*/
        /*where node_id = id and */
        insert into network_tb(node_id,parent_id) values(id,p_id);
    else 
        update network_tb
        set parent_id = p_id;
    end if;
end;

/*####################################################################*/
drop procedure if exists insert_sense_record_sp;
create procedure insert_sense_record_sp(
    in id           int,        in temperature  double(8,2),
    in brightness   double(8,2),in microphone   double(8,2),
    in accelerate_x double(8,2),in accelerate_y double(8,2),
    in terre_mag_x  double(8,2),in terre_mag_y  double(8,2),
    in pressure     double(8,2),in humidity     double(8,2),
    in shoke        double(8,2),in sense_time datetime)
begin
    insert into sense_tb(node_id,temperature,humidity,
        brightness,microphone,accelerate_x,accelerate_y,
        pressure,shoke,terre_mag_x,terre_mag_y,sense_time) 
    values(id,temperature,humidity,brightness,microphone,
        accelerate_x,accelerate_y,pressure,shoke,terre_mag_x,terre_mag_y,sense_time);
end;

/*####################################################################*/
