delimiter ;;
drop procedure if exists sp_get_latest_record;
create procedure sp_get_latest_record(in net_name char(50),in nod_id int,in sense char(50))
top:begin
    declare net_id  int;
    declare amount  int;
    select count(*),network_id from viw_network_segment
    where network_name = net_name into amount,net_id;
    if amount = 0 then 
        leave top;
    end if;

    if nod_id >= 0 and sense is not null then
        case sense    
        when 'temperature' then
            select temperature,insert_time from viw_temperature
            where network_id = net_id and node_id = nod_id
            order by order_num desc
            limit 1;
        when 'brightness' then
            select brightness,insert_time from viw_brightness
            where node_id = nod_id and network_id = net_id
            order by order_num desc
            limit 1;
        when 'microphone' then
            select microphone,insert_time from viw_microphone
            where node_id = nod_id and network_id = net_id
            order by order_num desc
            limit 1;
        when 'accelerate' then 
            select accelerate_x,accelerate_y,insert_time from viw_accelerate
            where node_id = nod_id and network_id = net_id
            order by order_num desc
            limit 1;
        when 'terrestrial_magnetism' then
            select terre_mag_x,terre_mag_y,insert_time from viw_terre_mag
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
end;
