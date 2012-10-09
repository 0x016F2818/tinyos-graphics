drop view if exists viw_temp;
drop view if exists viw_humidity;
drop view if exists viw_photo;
drop view if exists viw_sound;
drop view if exists viw_magn;
drop view if exists viw_pressure;
drop view if exists viw_acce;
drop view if exists viw_shoke;

create or replace view viw_temp as
    select order_num,network_id,node_id,temp,sense_time,insert_time from tb_sense;
create or replace view viw_humidity as
    select order_num,network_id,node_id,humidity,sense_time,insert_time from tb_sense;
create or replace view viw_photo as
    select order_num,network_id,node_id,photo,sense_time,insert_time from tb_sense;
create or replace view viw_sound as
    select order_num,network_id,node_id,sound,sense_time,insert_time from tb_sense;
create or replace view viw_magn as
    select order_num,network_id,node_id,x_mag,y_mag,sense_time,insert_time from tb_sense;
create or replace view viw_pressure as
    select order_num,network_id,node_id,pressure,sense_time,insert_time from tb_sense;
create or replace view viw_acce as
    select order_num,network_id,node_id,x_acc,y_acc,sense_time,insert_time from tb_sense;
create or replace view viw_shoke as
    select order_num,network_id,node_id,shoke,sense_time,insert_time from tb_sense;
