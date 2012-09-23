drop table if exists tb_network;
drop table if exists tb_sense;
drop table if exists tb_node;
drop table if exists tb_work_state;
drop table if exists tb_node_status;
drop table if exists tb_network_segment;

create table if not exists tb_node_status(
    node_status     smallint     primary key auto_increment,
    status_name     char(50)     unique not null
    )engine = innodb;

create table if not exists tb_work_state(
    work_state      smallint     primary key auto_increment,
    state_name      char(50)     unique not null
    )engine = innodb;

create table if not exists tb_network_segment(
    network_id      int             unique not null auto_increment,
    network_name    char(50)     unique not null,
    primary key(network_id)
    )engine = innodb;

create table if not exists tb_node(
    network_id      int             not null,
    node_id         int             not null,
    work_state      smallint        not null,
    node_status     smallint        not null,
    power           int             not null,
    GPS             point           not null,
    update_time     timestamp       default current_timestamp on update current_timestamp,

    primary key(network_id,node_id),
    foreign key(work_state) references tb_work_state(work_state),
    foreign key(node_status) references tb_node_status(node_status)
    /*foreign key(network_id,node_id) references tb_network_segment(network_id)*/
    )engine = innodb;

create table if not exists tb_network(
    network_id      int             not null,
    node_id         int             not null,
    parent_id       int             not null,
    quality         int             not null default 0,
    primary key(network_id,node_id),
    foreign key(network_id,node_id) references tb_node(network_id,node_id),
    foreign key(network_id,parent_id) references tb_node(network_id,node_id)
    )engine = innodb;
    
create table if not exists tb_sense(
    order_num       bigint          auto_increment primary key,
    network_id      int             not null,
    node_id         int             not null,
    temperature     double(8,2),
    humidity        double(8,2),
    brightness      double(8,2),
    microphone      double(8,2),
    accelerate_x    double(8,2),
    accelerate_y    double(8,2),
    pressure        double(8,2),
    shoke           double(8,2),
    terre_mag_x     double(8,2),     #--terrestrial magnetism
    terre_mag_y     double(8,2),
    unknow1         double(8,2),
    unknow2         double(8,2),
    sense_time      datetime,
    insert_time     timestamp   default current_timestamp,
    foreign key(network_id,node_id) references tb_node(network_id,node_id)
    )engine = innodb;
