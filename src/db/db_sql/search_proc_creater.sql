delimiter ;;
/*##########################################################################*/
drop procedure if exists sp_get_all_node_info;
create procedure  sp_get_all_node_info()
begin
    select node_viw.node_id,node_viw.parent_id,
            work_state_viw.state_name as work_state,
            node_status_viw.status_name as node_status,
            power, x(GPS),y(GPS)
    from node_viw,work_state_viw,node_status_viw
            where node_viw.work_state   = work_state_viw.work_state
            and node_viw.node_status    = node_status_viw.node_status
            order by node_viw.node_id;
end;

/*##########################################################################*/
drop procedure if exists sp_get_network_info;
create procedure sp_get_network_info()
begin
    select * from network_viw;
end;

/*##########################################################################*/
drop procedure if exists sp_get_all_record;
create procedure sp_get_all_record()
begin
    select node_id,temperature,brightness,microphone,
    accelerate_x,accelerate_y, terre_mag_x,terre_mag_y,
    humidity,pressure,shoke,sense_time from sense_tb;
end;

/*##########################################################################*/
drop procedure if exists sp_get_sgl_node_all_sense;
create procedure sp_get_sgl_node_all_sense(in id int)
begin
    select temperature,humidity,brightness,microphone,
    terre_mag_x,terre_mag_y,accelerate_x,accelerate_y,
    pressure,shoke,sense_time from sense_tb
    where node_id = id;
end;

/*##########################################################################*/
drop procedure if exists sp_get_sgl_node_info;
create procedure sp_get_sgl_node_info(in id int)
begin
    select node_viw.node_id,parent_id,
            work_state_viw.state_name as work_state,
            node_status_viw.status_name as node_status,
            power, x(GPS),y(GPS)
    from node_viw,work_state_viw,node_status_viw,network_viw
            where node_viw.node_id  = id
            and network_viw.node_id = id
            and node_viw.work_state = work_state_viw.work_state
            and node_viw.node_status = node_status_viw.node_status;
end;

/*##########################################################################*/
drop procedure if exists sp_get_sgl_node_sgl_sense;
create procedure sp_get_sgl_node_sgl_sense(in id int,in sense varchar(30))
top:begin
    case sense
    when 'temperature' then
        select temperature from temperature_viw
        where node_id = id;
    when 'humidity' then
        select humidity from humidity_viw
        where node_id = id;
    when 'brightness' then
        select brightness from brightness_viw
        where node_id = id;
    when 'microphone' then
        select microphone from microphone_viw
        where node_id = id;
    when 'terrestrial_magnetism' then
        select terre_mag_x,terre_mag_y from terre_mag_viw
        where node_id = id;
    when 'pressure' then
        select pressure from pressure_viw
        where node_id = id;
    when 'accelerate' then 
        select accelerate_x,accelerate_y from accelerate_viw
        where node_id = id;
    when 'shoke' then
        select shoke from shoke_viw
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
        select min(temperature) from temperature_viw;
    when 'brightness' then
        select max(brightness) from brightness_viw;
    when 'microphone' then
        select max(microphone) from microphone_viw;
    when 'terrestrial_magnetism' then
        select terre_mag_x,terre_mag_y from terre_mag_viw
        having terre_mag_x * terre_mag_x + terre_mag_y * terre_mag_y = max(terre_mag_x * terre_mag_x + terre_mag_y * terre_mag_y);
    when 'accelerate' then 
        select accelerate_x,accelerate_y from accelerate_viw
        having accelerate_x * accelerate_x + accelerate_y * accelerate_y = max( accelerate_x * accelerate_x + accelerate_y * accelerate_y);
    when 'pressure' then
        select pressure from pressure_viw;
    when 'humidity' then
        select max(humidity) from humidity_viw;
    when 'shoke' then
        select shoke from shoke_viw;
    else 
        leave top;
    end case;
end; 

/*##########################################################################*/
drop procedure if exists sp_get_latest_record;
create procedure sp_get_latest_record(in id int,in sense varchar(30))
top:begin
if id < 0 and sense is null then
    select temperature,humidity,brightness,microphone,terre_mag_x,terre_mag_y,
    pressure,accelerate_x,accelerate_y,shoke,sense_time,insert_time from sense_tb
    order by order_num desc
    limit 1;

elseif id > 0 and sense is null then
    select temperature,humidity,brightness,microphone,terre_mag_x,terre_mag_y,
    pressure,accelerate_x,accelerate_y,shoke,sense_time,insert_time from sense_tb
    where node_id = id 
    order by order_num desc
    limit 1;

elseif id < 0 and sense is not null then
    case sense    
    when "temperature" then
        select temperature,insert_time from temperature_viw
        order by order_num desc
        limit 1;
    when 'brightness' then
        select brightness,insert_time from brightness_viw
        order by order_num desc
        limit 1;
    when 'microphone' then
        select microphone ,insert_time from microphone_viw
        order by order_num desc
        limit 1;
    when 'terrestrial_magnetism' then
        select terre_mag_x,terre_mag_y,insert_time from terre_mag_viw
        order by order_num desc
        limit 1;
    when 'accelerate' then 
        select accelerate_x,accelerate_y,insert_time from accelerate_viw
        order by order_num desc
        limit 1;
    when 'pressure' then
        select pressure,insert_time from pressure_viw
        order by order_num desc
        limit 1;
    when 'humidity' then
        select humidity,insert_time from humidity_viw
        order by order_num desc
        limit 1;
    when 'shoke' then
        select shoke,insert_time from shoke_viw
        order by order_num desc
        limit 1;
    else 
        leave top;
    end case;

elseif id > 0 and sense is not null then
    case sense    
    when 'temperature' then
        select temperature,insert_time from temperature_viw
        where node_id = id
        order by order_num desc
        limit 1;
    when 'brightness' then
        select brightness,insert_time from brightness_viw
        where node_id = id
        order by order_num desc
        limit 1;
    when 'microphone' then
        select microphone,insert_time from microphone_viw
        where node_id = id
        order by order_num desc
        limit 1;
    when 'accelerate' then 
        select accelerate_x,accelerate_y,insert_time from accelerate_viw
        where node_id = id
        order by order_num desc
        limit 1;
    when 'terrestrial_magnetism' then
        select terre_mag_x,terre_mag_y,insert_time from terre_mag_viw
        where node_id = id
        order by order_num desc
        limit 1;
    when 'pressure' then
        select pressure,insert_time from pressure_viw
        where node_id = id
        order by order_num desc
        limit 1;
    when 'humidity' then
        select humidity,insert_time from humidity_viw
        where node_id = id
        order by order_num desc
        limit 1;
    when 'shoke' then
        select shoke,insert_time from shoke_viw
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
                pressure,accelerate_x,accelerate_y,shoke,insert_time from sense_viw
        where sense_viw.insert_time >= start_time and sense_viw.insert_time <= end_time;

    elseif id > 0 and sense is null then
        select temperature,humidity,brightness,microphone,terre_mag_x,terre_mag_y,
                pressure,accelerate_x,accelerate_y,shoke,insert_time from sense_tb
        where node_id = id and sense_viw.insert_time >= start_time and sense_viw.insert_time <= end_time;

    elseif id < 0 and sense is not null then
        case sense
        when 'temperature' then
            select temperature ,insert_time from temperature_viw
            where insert_time >= start_time and insert_time <= end_time;
        when 'brightness' then
            select brightness ,insert_time from brightness_viw
            where insert_time >= start_time and insert_time <= end_time;
        when 'microphone' then
            select microphone ,insert_time from microphone_viw
            where insert_time >= start_time and insert_time <= end_time;
        when 'terrestrial_magnetism' then
            select terre_mag_x,terre_mag_y ,insert_time from terre_mag_viw
            where insert_time >= start_time and insert_time <= end_time;
        when 'accelerate' then 
            select accelerate_x,accelerate_y ,insert_time from accelerate_viw
            where insert_time >= start_time and insert_time <= end_time;
        when 'humidity' then
            select humidity ,insert_time from humidity_viw
            where insert_time >= start_time and insert_time <= end_time;
        when 'pressure' then
            select pressure ,insert_time from pressure_viw
            where insert_time >= start_time and insert_time <= end_time;
        when 'shoke' then
            select shoke ,insert_time from shoke_viw
            where insert_time >= start_time and insert_time <= end_time;
        else
            leave top;
        end case;

    elseif id > 0 and sense is not null then
	    case sense    
        when 'temperature' then
            select temperature ,insert_time from temperature_viw
            where node_id = id and insert_time >= start_time and insert_time <= end_time;
        when 'brightness' then
            select brightness ,insert_time from brightness_viw
            where node_id = id and insert_time >= start_time and insert_time <= end_time;
        when 'microphone' then
            select microphone ,insert_time from microphone_viw
            where node_id = id and insert_time >= start_time and insert_time <= end_time;
        when 'terrestrial_magnetism' then
            select terre_mag_x,terre_mag_y ,insert_time from terre_mag_viw
            where node_id = id and insert_time >= start_time and insert_time <= end_time;
        when 'accelerate' then 
            select accelerate_x,accelerate_y ,insert_time from accelerate_viw
            where node_id = id and insert_time >= start_time and insert_time <= end_time;
        when 'humidity' then
            select humidity ,insert_time from humidity_viw
            where node_id = id and insert_time >= start_time and insert_time <= end_time;
        when 'pressure' then
            select pressure ,insert_time from pressure_viw
            where node_id = id and insert_time >= start_time and insert_time <= end_time;
        when 'shoke' then
            select shoke ,insert_time from shoke_viw
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

    prepare a1 from 'select temperature,insert_time from temperature_viw where node_id = ? and insert_time >= ? limit ?';
    prepare a2 from 'select temperature,insert_time from temperature_viw where node_id = ? and insert_time <= ? limit ?';
    prepare a3 from 'select temperature,insert_time from temperature_viw where insert_time >= ? limit ?';
    prepare a4 from 'select temperature,insert_time from temperature_viw where insert_time <= ? limit ?';

    prepare b1 from 'select brightness,insert_time from brightness_viw where node_id = ? and insert_time >= ? limit ?';
    prepare b2 from 'select brightness,insert_time from brightness_viw where node_id = ? and insert_time <= ? limit ?';
    prepare b3 from 'select brightness,insert_time from brightness_viw where insert_time >= ? limit ?';
    prepare b4 from 'select brightness,insert_time from brightness_viw where insert_time <= ? limit ?';

    prepare c1 from 'select microphone,insert_time from microphone_viw where node_id = ? and insert_time >= ? limit ?';
    prepare c2 from 'select microphone,insert_time from microphone_viw where node_id = ? and insert_time <= ? limit ?';
    prepare c3 from 'select microphone,insert_time from microphone_viw where insert_time >= ? limit ?';
    prepare c4 from 'select microphone,insert_time from microphone_viw where insert_time <= ? limit ?';

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
