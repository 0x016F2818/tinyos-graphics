drop view if exists viw_temp;
drop view if exists viw_humidity;
drop view if exists viw_photo;
drop view if exists viw_sound;
drop view if exists viw_magn;
drop view if exists viw_pressure;
drop view if exists viw_acce;
drop view if exists viw_shoke;
drop view if exists viw_last_update;

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

    /**//*create or replace view viw_last_update as*/
    /**//*select network_id,node_id,insert_time from tb_network,(select network_id,node_id,insert_time from (select * from tb_sense order by insert_time desc) group by network_id, node_id) as tb_temp*/
    /**//*where tb_network.node_id    = tb_temp.node_id*/
    /**//*and tb_network.network_id   = tb_temp.network_id;*/

    /*create or replace view viw_all_record_desc as select network_id,node_id,insert_time from tb_sense*/
    /*order by insert_time desc;*/

    /*create or replace view viw_ordered as*/
    /*select network_id, node_id, insert_time from viw_all_record_desc group by node_id;*/

    /*create or replace view viw_last_update as*/
    /*select distinct network_id,node_id,insert_time from tb_network left join viw_ordered */
    /*using (network_id,node_id)*/
    /*where insert_time in (select max(insert_time) from tb_sense group by network_id,node_id);*/
