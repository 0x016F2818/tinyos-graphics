create or replace view node_viw as
    select * from node_tb;
create or replace view work_state_viw as
    select * from work_state_tb;
create or replace view node_status_viw as
    select * from node_status_tb;
create or replace view network_viw as
    select * from network_tb
    order by node_id
    with cascaded check option;
create or replace view sense_viw as
    select * from sense_tb;

create or replace view temperature_viw as
    select order_num,node_id,temperature,sense_time,insert_time from sense_tb;
create or replace view humidity_viw as
    select order_num,node_id,humidity,sense_time,insert_time from sense_tb;
create or replace view brightness_viw as
    select order_num,node_id,brightness,sense_time,insert_time from sense_tb;
create or replace view microphone_viw as
    select order_num,node_id,microphone,sense_time,insert_time from sense_tb;
create or replace view terre_mag_viw as
    select order_num,node_id,terre_mag_x,terre_mag_y,sense_time,insert_time from sense_tb;
create or replace view pressure_viw as
    select order_num,node_id,pressure,sense_time,insert_time from sense_tb;
create or replace view accelerate_viw as
    select order_num,node_id,accelerate_x,accelerate_y,sense_time,insert_time from sense_tb;
create or replace view shoke_viw as
    select order_num,node_id,shoke,sense_time,insert_time from sense_tb;

