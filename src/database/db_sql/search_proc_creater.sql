delimiter ;;
/*##########################################################################*/
drop procedure if exists sp_get_sgl_node_info;
create procedure sp_get_sgl_node_info(in net_name char(50),in id int)
begin
    select viw_network_segment.network_id,
            viw_network_segment.network_name,
            viw_node.node_id,viw_network.parent_id,
            viw_work_state.state_name as work_state,
            viw_node_status.status_name as node_status,
            power, x(GPS),y(GPS)
    from viw_node,viw_work_state,viw_node_status,viw_network
            where viw_node.node_id      = id
            and viw_node.node_id        = viw_network.node_id
            and viw_node.network_id     = viw_network.network_id
            and viw_node.work_state     = viw_work_state.work_state
            and viw_node.node_status    = viw_node_status.node_status
            and viw_node.network_id     = viw_network_segment.network_id
            and viw_node.work_state     = viw_work_state.work_state
            and viw_node.node_status    = viw_node_status.node_status
            and viw_node.network_id     = viw_network_segment.network_id;
end;

/*##########################################################################*/
drop procedure if exists sp_get_all_node_info;
create procedure  sp_get_all_node_info()
begin
    select viw_network_segment.network_id,
            viw_network_segment.network_name,
            viw_node.node_id,viw_network.parent_id,
            viw_work_state.state_name as work_state,
            viw_node_status.status_name as node_status,
            power, x(GPS),y(GPS)
    from viw_node,viw_work_state,viw_node_status,viw_network,viw_network_segment
            where viw_node.node_id      = viw_network.node_id 
            and viw_node.network_id     = viw_network.network_id
            and viw_node.work_state     = viw_work_state.work_state
            and viw_node.node_status    = viw_node_status.node_status
            and viw_node.network_id     = viw_network_segment.network_id
            order by viw_node.node_id;
end;

/*##########################################################################*/
drop procedure if exists sp_get_network_info;
create procedure sp_get_network_info()
begin
    select * from viw_network;
end;

/*##########################################################################*/
drop procedure if exists sp_get_all_record;
create procedure sp_get_all_record()
begin
    select network_id,network_name,node_id,temperature,brightness,microphone,
    accelerate_x,accelerate_y, terre_mag_x,terre_mag_y,
    humidity,pressure,shoke,sense_time from viw_sense,viw_network_segment
    where viw_sense.network_id = viw_network_segment.network_id;
end;

/*##########################################################################*/
drop procedure if exists sp_get_sgl_node_all_sense;
create procedure sp_get_sgl_node_all_sense(in id int)
begin
    select temperature,humidity,brightness,microphone,
    terre_mag_x,terre_mag_y,accelerate_x,accelerate_y,
    pressure,shoke,sense_time from viw_sense
    where node_id = id;
end;

/*##########################################################################*/
drop procedure if exists sp_get_sgl_node_sgl_sense;
create procedure sp_get_sgl_node_sgl_sense(in id int,in sense varchar(30))
top:begin
    case sense
    when 'temperature' then
        select temperature from viw_temperature
        where node_id = id;
    when 'humidity' then
        select humidity from viw_humidity
        where node_id = id;
    when 'brightness' then
        select brightness from viw_brightness
        where node_id = id;
    when 'microphone' then
        select microphone from viw_microphone
        where node_id = id;
    when 'terrestrial_magnetism' then
        select terre_mag_x,terre_mag_y from viw_terre_mag
        where node_id = id;
    when 'pressure' then
        select pressure from viw_pressure
        where node_id = id;
    when 'accelerate' then 
        select accelerate_x,accelerate_y from viw_accelerate
        where node_id = id;
    when 'shoke' then
        select shoke from viw_shoke
        where node_id = id;
    else 
        leave top;
    end case;
end;

/*##########################################################################*/
/*TODO:improve*/
drop procedure if exists sp_get_max_record;
create procedure sp_get_max_record(in sense varchar(30))
top:begin
    case sense 
    when 'temperature' then
        select min(temperature) from viw_temperature;
    when 'brightness' then
        select max(brightness) from viw_brightness;
    when 'microphone' then
        select max(microphone) from viw_microphone;
    when 'terrestrial_magnetism' then
        select terre_mag_x,terre_mag_y from viw_terre_mag
        having terre_mag_x * terre_mag_x + terre_mag_y * terre_mag_y = max(terre_mag_x * terre_mag_x + terre_mag_y * terre_mag_y);
    when 'accelerate' then 
        select accelerate_x,accelerate_y from viw_accelerate
        having accelerate_x * accelerate_x + accelerate_y * accelerate_y = max( accelerate_x * accelerate_x + accelerate_y * accelerate_y);
    when 'pressure' then
        select pressure from viw_pressure;
    when 'humidity' then
        select max(humidity) from viw_humidity;
    when 'shoke' then
        select shoke from viw_shoke;
    else 
        leave top;
    end case;
end; 

/*##########################################################################*/
drop procedure if exists sp_get_latest_record;
create procedure sp_get_latest_record(in network_name,in id int,in sense varchar(30))
top:begin
if id < 0 and sense is null then
    select temperature,humidity,brightness,microphone,terre_mag_x,terre_mag_y,
    pressure,accelerate_x,accelerate_y,shoke,sense_time,insert_time from viw_sense
    order by order_num desc
    limit 1;

elseif id > 0 and sense is null then
    select temperature,humidity,brightness,microphone,terre_mag_x,terre_mag_y,
    pressure,accelerate_x,accelerate_y,shoke,sense_time,insert_time from viw_sense
    where node_id = id 
    order by order_num desc
    limit 1;

elseif id < 0 and sense is not null then
    case sense    
    when "temperature" then
        select temperature,insert_time from viw_temperature
        order by order_num desc
        limit 1;
    when 'brightness' then
        select brightness,insert_time from viw_brightness
        order by order_num desc
        limit 1;
    when 'microphone' then
        select microphone ,insert_time from viw_microphone
        order by order_num desc
        limit 1;
    when 'terrestrial_magnetism' then
        select terre_mag_x,terre_mag_y,insert_time from viw_terre_mag
        order by order_num desc
        limit 1;
    when 'accelerate' then 
        select accelerate_x,accelerate_y,insert_time from viw_accelerate
        order by order_num desc
        limit 1;
    when 'pressure' then
        select pressure,insert_time from viw_pressure
        order by order_num desc
        limit 1;
    when 'humidity' then
        select humidity,insert_time from viw_humidity
        order by order_num desc
        limit 1;
    when 'shoke' then
        select shoke,insert_time from viw_shoke
        order by order_num desc
        limit 1;
    else 
        leave top;
    end case;

elseif id > 0 and sense is not null then
    case sense    
    when 'temperature' then
        select temperature,insert_time from viw_temperature
        where node_id = id
        order by order_num desc
        limit 1;
    when 'brightness' then
        select brightness,insert_time from viw_brightness
        where node_id = id
        order by order_num desc
        limit 1;
    when 'microphone' then
        select microphone,insert_time from viw_microphone
        where node_id = id
        order by order_num desc
        limit 1;
    when 'accelerate' then 
        select accelerate_x,accelerate_y,insert_time from viw_accelerate
        where node_id = id
        order by order_num desc
        limit 1;
    when 'terrestrial_magnetism' then
        select terre_mag_x,terre_mag_y,insert_time from viw_terre_mag
        where node_id = id
        order by order_num desc
        limit 1;
    when 'pressure' then
        select pressure,insert_time from viw_pressure
        where node_id = id
        order by order_num desc
        limit 1;
    when 'humidity' then
        select humidity,insert_time from viw_humidity
        where node_id = id
        order by order_num desc
        limit 1;
    when 'shoke' then
        select shoke,insert_time from viw_shoke
        where node_id = id
        order by order_num desc
        limit 1;
    else
        leave top;
    end case;
    end if;
end;

/*##########################################################################*/
drop procedure if exists sp_get_absolute_record;
create procedure sp_get_absolute_record(in id int,in sense varchar(30),in start_time timestamp,in end_time timestamp)
top:begin 
    if id < 0 and sense is null then
        select temperature,humidity,brightness,microphone,terre_mag_x,terre_mag_y,
                pressure,accelerate_x,accelerate_y,shoke,insert_time from viw_sense
        where viw_sense.insert_time >= start_time and viw_sense.insert_time <= end_time;

    elseif id > 0 and sense is null then
        select temperature,humidity,brightness,microphone,terre_mag_x,terre_mag_y,
                pressure,accelerate_x,accelerate_y,shoke,insert_time from viw_sense
        where node_id = id and viw_sense.insert_time >= start_time and viw_sense.insert_time <= end_time;

    elseif id < 0 and sense is not null then
        case sense
        when 'temperature' then
            select temperature ,insert_time from viw_temperature
            where insert_time >= start_time and insert_time <= end_time;
        when 'brightness' then
            select brightness ,insert_time from viw_brightness
            where insert_time >= start_time and insert_time <= end_time;
        when 'microphone' then
            select microphone ,insert_time from viw_microphone
            where insert_time >= start_time and insert_time <= end_time;
        when 'terrestrial_magnetism' then
            select terre_mag_x,terre_mag_y ,insert_time from viw_terre_mag
            where insert_time >= start_time and insert_time <= end_time;
        when 'accelerate' then 
            select accelerate_x,accelerate_y ,insert_time from viw_accelerate
            where insert_time >= start_time and insert_time <= end_time;
        when 'humidity' then
            select humidity ,insert_time from viw_humidity
            where insert_time >= start_time and insert_time <= end_time;
        when 'pressure' then
            select pressure ,insert_time from viw_pressure
            where insert_time >= start_time and insert_time <= end_time;
        when 'shoke' then
            select shoke ,insert_time from viw_shoke
            where insert_time >= start_time and insert_time <= end_time;
        else
            leave top;
        end case;

    elseif id > 0 and sense is not null then
	    case sense    
        when 'temperature' then
            select temperature ,insert_time from viw_temperature
            where node_id = id and insert_time >= start_time and insert_time <= end_time;
        when 'brightness' then
            select brightness ,insert_time from viw_brightness
            where node_id = id and insert_time >= start_time and insert_time <= end_time;
        when 'microphone' then
            select microphone ,insert_time from viw_microphone
            where node_id = id and insert_time >= start_time and insert_time <= end_time;
        when 'terrestrial_magnetism' then
            select terre_mag_x,terre_mag_y ,insert_time from viw_terre_mag
            where node_id = id and insert_time >= start_time and insert_time <= end_time;
        when 'accelerate' then 
            select accelerate_x,accelerate_y ,insert_time from viw_accelerate
            where node_id = id and insert_time >= start_time and insert_time <= end_time;
        when 'humidity' then
            select humidity ,insert_time from viw_humidity
            where node_id = id and insert_time >= start_time and insert_time <= end_time;
        when 'pressure' then
            select pressure ,insert_time from viw_pressure
            where node_id = id and insert_time >= start_time and insert_time <= end_time;
        when 'shoke' then
            select shoke ,insert_time from viw_shoke
            where node_id = id and insert_time >= start_time and insert_time <= end_time;
        else 
            leave top;
        end case;
    end if;
end;

/*##########################################################################*/
drop procedure if exists sp_get_relative_record;
create procedure sp_get_relative_record(in id int,in sense varchar(30),in start_time timestamp,in numbers bigint)
top:begin
    set @id             = id;
    set @sense          = sense;
    set @start_time     = start_time;
    set @numbers        = numbers;
    set @neg_numbers    = -numbers;

    prepare a1 from 'select temperature,insert_time from viw_temperature where node_id = ? and insert_time >= ? limit ?';
    prepare a2 from 'select temperature,insert_time from viw_temperature where node_id = ? and insert_time <= ? limit ?';
    prepare a3 from 'select temperature,insert_time from viw_temperature where insert_time >= ? limit ?';
    prepare a4 from 'select temperature,insert_time from viw_temperature where insert_time <= ? limit ?';

    prepare b1 from 'select brightness,insert_time from viw_brightness where node_id = ? and insert_time >= ? limit ?';
    prepare b2 from 'select brightness,insert_time from viw_brightness where node_id = ? and insert_time <= ? limit ?';
    prepare b3 from 'select brightness,insert_time from viw_brightness where insert_time >= ? limit ?';
    prepare b4 from 'select brightness,insert_time from viw_brightness where insert_time <= ? limit ?';

    prepare c1 from 'select microphone,insert_time from viw_microphone where node_id = ? and insert_time >= ? limit ?';
    prepare c2 from 'select microphone,insert_time from viw_microphone where node_id = ? and insert_time <= ? limit ?';
    prepare c3 from 'select microphone,insert_time from viw_microphone where insert_time >= ? limit ?';
    prepare c4 from 'select microphone,insert_time from viw_microphone where insert_time <= ? limit ?';

    case sense
    when "temperature" then
        if(numbers >= 0) then
            execute a1 using @id,@start_time,@numbers;
        else
            execute a2 using @id,@start_time,@neg_numbers;
        end if;
    when "brightness" then
        if(numbers >= 0) then
            execute b1 using @id,@start_time,@numbers;
        else
            execute b2 using @id,@start_time,@neg_numbers;
        end if;
    when "microphone" then
        if(numbers >= 0) then
            execute c1 using @id,@start_time,@numbers;
        else
            execute c2 using @id,@start_time,@neg_numbers;
        end if;
    else 
        leave top;
    end case;
end;
