delimiter ;;
/*##########################################################################*/
drop procedure if exists sp_get_sgl_node_info;;
create procedure sp_get_sgl_node_info(in net_name char(50),in nod_id int)
begin
    select tb_network_segment.network_id,
            tb_network_segment.network_name,
            tb_node.node_id,tb_network.parent_id,
            tb_work_state.state_name as work_state,
            tb_node_status.status_name as node_status,
            power, x(GPS),y(GPS)
    from tb_node,tb_work_state,tb_node_status,tb_network,tb_network_segment
            where tb_node.node_id      = nod_id
            and tb_node.node_id        = tb_network.node_id
            and tb_node.network_id     = tb_network.network_id
            and tb_node.work_state     = tb_work_state.work_state
            and tb_node.node_status    = tb_node_status.node_status
            and tb_node.network_id     = tb_network_segment.network_id
            and tb_node.work_state     = tb_work_state.work_state
            and tb_node.node_status    = tb_node_status.node_status
            and tb_node.network_id     = tb_network_segment.network_id;
end;;

/*##########################################################################*/
drop procedure if exists sp_get_all_node_info;;
create procedure  sp_get_all_node_info(in sec int)
begin
    /*TODO: the timeout should be static or not? */
    call sp_flush_network(sec);
    select tb_network_segment.network_id,
            tb_network_segment.network_name,
            tb_node.node_id,tb_network.parent_id,
            tb_work_state.state_name as work_state,
            tb_node_status.status_name as node_status,
            power, x(GPS),y(GPS)
    from tb_node,tb_work_state,tb_node_status,tb_network,tb_network_segment
            where tb_node.node_id      = tb_network.node_id 
            and tb_node.network_id     = tb_network.network_id
            and tb_node.work_state     = tb_work_state.work_state
            and tb_node.node_status    = tb_node_status.node_status
            and tb_node.network_id     = tb_network_segment.network_id
            order by tb_node.network_id,tb_node.node_id;
end;;

/*##########################################################################*/
drop procedure if exists sp_get_network_info;;
create procedure sp_get_network_info(in sec int)
begin
    /*TODO: the timeout should be static or not? */
    call sp_flush_network(sec);
    select tb_network.network_id,tb_network_segment.network_name, 
        node_id,parent_id,quality from tb_network,tb_network_segment
    where tb_network.network_id = tb_network_segment.network_id;
end;;

/*##########################################################################*/
drop procedure if exists sp_get_all_record;
create procedure sp_get_all_record()
begin
    select tb_sense.network_id,network_name,node_id,
    temp,photo,sound,
    x_acc,y_acc, x_mag,y_mag,
    humidity,pressure,shoke,sense_time,insert_time from tb_sense,tb_network_segment
    where tb_sense.network_id = tb_network_segment.network_id;
end;;

/*##########################################################################*/
drop procedure if exists sp_get_sgl_node_all_sense;;
create procedure sp_get_sgl_node_all_sense(in net_name char(50),in nod_id int)
top:begin
    declare net_id int;
    declare amount int;
    select count(*),network_id from tb_network_segment
    where network_name = net_name into amount,net_id;
    if amount = 0 then 
        leave top;
    end if;

    select temp,humidity,photo,sound,
    x_mag,y_mag,x_acc,y_acc,
    pressure,shoke,sense_time from tb_sense
    where network_id = net_id and node_id = nod_id;
end;;

/*##########################################################################*/
drop procedure if exists sp_get_sgl_node_sgl_sense;;
create procedure sp_get_sgl_node_sgl_sense(in net_name char(50),in nod_id int,in sense char(50))
top:begin
    declare net_id int;
    declare amount int;
    select count(*),network_id from tb_network_segment
    where network_name = net_name into amount,net_id;
    if amount = 0 then 
        leave top;
    end if;

    case sense
    when 'temp' then
        select temp,insert_time from viw_temp
        where network_id = net_id and node_id = nod_id;
    when 'humidity' then
        select humidity,insert_time from viw_humidity
        where network_id = net_id and node_id = nod_id;
    when 'photo' then
        select photo,insert_time from viw_photo
        where network_id = net_id and node_id = nod_id;
    when 'sound' then
        select sound,insert_time from viw_sound
        where network_id = net_id and node_id = nod_id;
    when 'magn' then
        select x_mag,y_mag,insert_time from viw_terre_mag
        where network_id = net_id and node_id = nod_id;
    when 'pressure' then
        select pressure,insert_time from viw_pressure
        where network_id = net_id and node_id = nod_id;
    when 'acce' then 
        select x_acc,y_acc,insert_time from viw_acce
        where network_id = net_id and node_id = nod_id;
    when 'shoke' then
        select shoke,insert_time from viw_shoke
        where network_id = net_id and node_id = nod_id;
    else 
        leave top;
    end case;
end;;

/*##########################################################################*/
/*TODO:improve*/
drop procedure if exists sp_get_max_record;;
create procedure sp_get_max_record(in sense char(50))
top:begin
    case sense 
    when 'temp' then
        select min(temp) from viw_temp;
    when 'photo' then
        select max(photo) from viw_photo;
    when 'sound' then
        select max(sound) from viw_sound;
    when 'magn' then
        select x_mag,y_mag from viw_terre_mag
        having x_mag * x_mag + y_mag * y_mag = max(x_mag * x_mag + y_mag * y_mag);
    when 'acce' then 
        select x_acc,y_acc from viw_acce
        having x_acc * x_acc + y_acc * y_acc = min(x_acc * x_acc + y_acc * y_acc);
    when 'pressure' then
        select pressure from viw_pressure;
    when 'humidity' then
        select max(humidity) from viw_humidity;
    when 'shoke' then
        select max(shoke) from viw_shoke;
    else 
        leave top;
    end case;
end;;

/*##########################################################################*/
/*TODO:improve*/
drop procedure if exists sp_get_min_record;;
create procedure sp_get_min_record(in sense char(50))
top:begin
    case sense 
    when 'temp' then
        select min(temp) from viw_temp;
    when 'photo' then
        select min(photo) from viw_photo;
    when 'sound' then
        select min(sound) from viw_sound;
    when 'magn' then
        select x_mag,y_mag from viw_terre_mag
        having x_mag * x_mag + y_mag * y_mag = min(x_mag * x_mag + y_mag * y_mag);
    when 'acce' then 
        select x_acc,y_acc from viw_acce
        having x_acc * x_acc + y_acc * y_acc = min(x_acc * x_acc + y_acc * y_acc);
    when 'pressure' then
        select pressure from viw_pressure;
    when 'humidity' then
        select min(humidity) from viw_humidity;
    when 'shoke' then
        select min(shoke) from viw_shoke;
    else 
        leave top;
    end case;
end;;
/*##########################################################################*/
drop procedure if exists sp_get_latest_record;;
create procedure sp_get_latest_record(in net_name char(50),in nod_id int,in sense char(50))
top:begin
    declare net_id  int;
    declare amount  int;
    select count(*),network_id from tb_network_segment
    where network_name = net_name into amount,net_id;
    if amount = 0 then 
        leave top;
    end if;

    if nod_id >= 0 and sense is not null then
        case sense    
        when 'temp' then
            select temp,insert_time from viw_temp
            where network_id = net_id and node_id = nod_id
            order by order_num desc
            limit 1;
        when 'photo' then
            select photo,insert_time from viw_photo
            where node_id = nod_id and network_id = net_id
            order by order_num desc
            limit 1;
        when 'sound' then
            select sound,insert_time from viw_sound
            where node_id = nod_id and network_id = net_id
            order by order_num desc
            limit 1;
        when 'acce' then 
            select x_acc,y_acc,insert_time from viw_acce
            where node_id = nod_id and network_id = net_id
            order by order_num desc
            limit 1;
        when 'magn' then
            select x_mag,y_mag,insert_time from viw_terre_mag
            where node_id = nod_id and network_id = net_id
            order by order_num desc
            limit 1;
        when 'pressure' then
            select pressure,insert_time from viw_pressure
            where node_id = nod_id and network_id = net_id
            order by order_num desc
            limit 1;
        when 'humidity' then
            select humidity,insert_time from viw_humidity
            where node_id = nod_id and network_id = net_id
            order by order_num desc
            limit 1;
        when 'shoke' then
            select shoke,insert_time from viw_shoke
            where node_id = nod_id and network_id = net_id
            order by order_num desc
            limit 1;
        else
            leave top;
        end case;
    else
        leave top;
    end if;
end;;
/*##########################################################################*/
drop procedure if exists sp_get_absolute_record;;
create procedure sp_get_absolute_record(in net_name char(50),in nod_id int,in sense char(50),in start_time timestamp,in end_time timestamp)
top:begin 
    declare net_id  int;
    declare amount  int;
    select count(*),network_id from tb_network_segment
    where network_name = net_name into amount,net_id;
    if amount = 0 then 
        leave top;
    end if;

    if nod_id >= 0 and sense is not null then
	    case sense    
        when 'temp' then
            select temp ,insert_time from viw_temp
            where net_id = network_id and node_id = nod_id and insert_time >= start_time and insert_time <= end_time;
        when 'photo' then
            select photo ,insert_time from viw_photo
            where net_id = network_id and node_id = nod_id and insert_time >= start_time and insert_time <= end_time;
        when 'sound' then
            select sound ,insert_time from viw_sound
            where net_id = network_id and node_id = nod_id and insert_time >= start_time and insert_time <= end_time;
        when 'magn' then
            select x_mag,y_mag ,insert_time from viw_terre_mag
            where net_id = network_id and node_id = nod_id and insert_time >= start_time and insert_time <= end_time;
        when 'acce' then 
            select x_acc,y_acc ,insert_time from viw_acce
            where net_id = network_id and node_id = nod_id and insert_time >= start_time and insert_time <= end_time;
        when 'humidity' then
            select humidity ,insert_time from viw_humidity
            where net_id = network_id and node_id = nod_id and insert_time >= start_time and insert_time <= end_time;
        when 'pressure' then
            select pressure ,insert_time from viw_pressure
            where net_id = network_id and node_id = nod_id and insert_time >= start_time and insert_time <= end_time;
        when 'shoke' then
            select shoke ,insert_time from viw_shoke
            where net_id = network_id and node_id = nod_id and insert_time >= start_time and insert_time <= end_time;
        else 
            leave top;
        end case;
    else 
        leave top;
    end if;
end;;

/*##########################################################################*/
drop procedure if exists sp_get_relative_record;;
create procedure sp_get_relative_record(in net_name char(50),in nod_id int,in sense char(50),in start_time timestamp,in numbers bigint)
top:begin
    declare net_id int;
    declare amount int;
    select count(*),network_id from tb_network_segment
    where network_name = net_name into amount,net_id;
    if amount = 0 then 
        leave top;
    end if;

    set @net_id         = net_id;
    set @nod_id         = nod_id;
    set @sense          = sense;
    set @start_time     = start_time;
    set @numbers        = numbers;
    set @neg_numbers    = -numbers;

    prepare a1 from 'select temp,UNIX_TIMESTAMP(insert_time) from viw_temp where network_id = ? and node_id = ? and insert_time >= ? order by order_num limit ?';
    prepare a2 from 'select temp,UNIX_TIMESTAMP(insert_time) from viw_temp where network_id = ? and node_id = ? and insert_time <= ? order by order_num desc limit ?';

    prepare b1 from 'select photo,UNIX_TIMESTAMP(insert_time) from viw_photo where network_id = ? and node_id = ? and insert_time >= ? order by order_num limit ?';
    prepare b2 from 'select photo,UNIX_TIMESTAMP(insert_time) from viw_photo where network_id = ? and node_id = ? and insert_time <= ? order by order_num desc limit ?';

    prepare c1 from 'select sound,UNIX_TIMESTAMP(insert_time) from viw_sound where network_id = ? and node_id = ? and insert_time >= ? order by order_num limit ?';
    prepare c2 from 'select sound,UNIX_TIMESTAMP(insert_time) from viw_sound where network_id = ? and node_id = ? and insert_time <= ? order by order_num desc limit ?';

    prepare d1 from 'select x_acc,UNIX_TIMESTAMP(insert_time) from viw_acce where network_id = ? and node_id = ? and insert_time >= ? order by order_num limit ?';
    prepare d2 from 'select x_acc,UNIX_TIMESTAMP(insert_time) from viw_acce where network_id = ? and node_id = ? and insert_time <= ? order by order_num desc limit ?';

    prepare e1 from 'select y_acc,UNIX_TIMESTAMP(insert_time) from viw_acce where network_id = ? and node_id = ? and insert_time >= ? order by order_num limit ?';
    prepare e2 from 'select y_acc,UNIX_TIMESTAMP(insert_time) from viw_acce where network_id = ? and node_id = ? and insert_time <= ? order by order_num desc limit ?';

    prepare f1 from 'select x_mag,UNIX_TIMESTAMP(insert_time) from viw_magn where network_id = ? and node_id = ? and insert_time >= ? order by order_num limit ?';
    prepare f2 from 'select x_mag,UNIX_TIMESTAMP(insert_time) from viw_magn where network_id = ? and node_id = ? and insert_time <= ? order by order_num desc limit ?';

    prepare g1 from 'select y_mag,UNIX_TIMESTAMP(insert_time) from viw_magn where network_id = ? and node_id = ? and insert_time >= ? order by order_num limit ?';
    prepare g2 from 'select y_mag,UNIX_TIMESTAMP(insert_time) from viw_magn where network_id = ? and node_id = ? and insert_time <= ? order by order_num desc limit ?';

    case sense
    when "temp" then
        if(numbers >= 0) then
            execute a1 using @net_id,@nod_id,@start_time,@numbers;
        else
            execute a2 using @net_id,@nod_id,@start_time,@neg_numbers;
        end if;
    when "photo" then
        if(numbers >= 0) then
            execute b1 using @net_id,@nod_id,@start_time,@numbers;
        else
            execute b2 using @net_id,@nod_id,@start_time,@neg_numbers;
        end if;
    when "sound" then
        if(numbers >= 0) then
            execute c1 using @net_id,@nod_id,@start_time,@numbers;
        else
            execute c2 using @net_id,@nod_id,@start_time,@neg_numbers;
        end if;
    when "x_acc" then
        if(numbers >= 0) then
            execute d1 using @net_id,@nod_id,@start_time,@numbers;
        else
            execute d2 using @net_id,@nod_id,@start_time,@neg_numbers;
        end if;
    when "y_acc" then
        if(numbers >= 0) then
            execute e1 using @net_id,@nod_id,@start_time,@numbers;
        else
            execute e2 using @net_id,@nod_id,@start_time,@neg_numbers;
        end if;
    when "x_mag" then
        if(numbers >= 0) then
            execute f1 using @net_id,@nod_id,@start_time,@numbers;
        else
            execute f2 using @net_id,@nod_id,@start_time,@neg_numbers;
        end if;
    when "y_mag" then
        if(numbers >= 0) then
            execute g1 using @net_id,@nod_id,@start_time,@numbers;
        else
            execute g2 using @net_id,@nod_id,@start_time,@neg_numbers;
        end if;
    else 
        leave top;
    end case;
end;;

/*##########################################################################*/
drop procedure if exists sp_get_absolute_record_num;;
create procedure sp_get_absolute_record_num(in net_name char(50),in nod_id int,in start_time timestamp,in end_time timestamp)
top:begin
    select count(*) from tb_sense,tb_network_segment 
    where tb_network_segment.network_name = net_name 
        and tb_sense.network_id = tb_network_segment.network_id 
        and node_id = nod_id 
        and insert_time >= start_time 
        and insert_time <= end_time;           
end;;

/*##########################################################################*/
drop procedure if exists sp_get_latest_network;;
create procedure sp_get_latest_network()
begin
    select * from tb_network left join ( select network_id, node_id, insert_time from (select network_id, node_id, insert_time from tb_sense  order by insert_time desc) as tb_temp group by node_id) as tb_temp using (network_id,node_id);
end;;

delimiter ;
