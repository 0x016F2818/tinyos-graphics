drop view if exists viw_node;
drop view if exists viw_network;
drop view if exists viw_work_state;
drop view if exists viw_node_status;
drop view if exists viw_network_segment;

drop view if exists viw_sense;
drop view if exists viw_temperature;
drop view if exists viw_humidity;
drop view if exists viw_brightness;
drop view if exists viw_microphone;
drop view if exists viw_terre_mag;
drop view if exists viw_pressure;
drop view if exists viw_accelerate;
drop view if exists viw_shoke;

create or replace view viw_node as
    select * from tb_node;
create or replace view viw_work_state as
    select * from tb_work_state;
create or replace view viw_node_status as
    select * from tb_node_status;
create or replace view viw_network as
    select * from tb_network;
create or replace view viw_network_segment as
    select * from tb_network_segment;
create or replace view viw_sense as
    select * from tb_sense;

create or replace view viw_temperature as
    select order_num,network_id,node_id,temp,sense_time,insert_time from tb_sense;
create or replace view viw_humidity as
    select order_num,network_id,node_id,humidity,sense_time,insert_time from tb_sense;
create or replace view viw_brightness as
    select order_num,network_id,node_id,photo,sense_time,insert_time from tb_sense;
create or replace view viw_microphone as
    select order_num,network_id,node_id,sound,sense_time,insert_time from tb_sense;
create or replace view viw_terre_mag as
    select order_num,network_id,node_id,x_mag,y_mag,sense_time,insert_time from tb_sense;
create or replace view viw_pressure as
    select order_num,network_id,node_id,pressure,sense_time,insert_time from tb_sense;
create or replace view viw_accelerate as
    select order_num,network_id,node_id,x_acc,y_acc,sense_time,insert_time from tb_sense;
create or replace view viw_shoke as
    select order_num,network_id,node_id,shoke,sense_time,insert_time from tb_sense;

