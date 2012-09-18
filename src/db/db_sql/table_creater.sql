drop table if exists network_tb;
drop table if exists sense_tb;
drop table if exists node_tb;
drop table if exists work_state_tb;
drop table if exists node_status_tb;

create table if not exists node_status_tb(
    node_status     smallint     primary key auto_increment,
    status_name     varchar(10)    unique not null
    )engine = innodb;

create table if not exists work_state_tb(
    work_state      smallint     primary key auto_increment,
    state_name      varchar(10)    unique not null
    )engine=innodb;

create table if not exists node_tb(
    node_id         int             not null,
    work_state      smallint        not null,
    node_status     smallint        not null,
    power           int             not null,
    GPS             point           not null,
    update_time     timestamp       default current_timestamp on update current_timestamp,
    primary key(node_id),
    foreign key(work_state) references work_state_tb(work_state),
    foreign key(node_status) references node_status_tb(node_status)
    )engine = innodb;

create table if not exists network_tb(
    node_id         int             not null,
    parent_id       int             not null,
    quality         int,
    primary key(node_id,parent_id),
    foreign key(node_id) references node_tb(node_id),
    foreign key(parent_id) references node_tb(node_id)
    )engine = innodb;
    
create table if not exists sense_tb(
    order_num       bigint          auto_increment primary key,
    node_id         int,
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
    foreign key(node_id) references node_tb(node_id)
    )engine =innodb;
