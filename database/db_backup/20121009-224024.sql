-- MySQL dump 10.13  Distrib 5.5.24, for debian-linux-gnu (i686)
--
-- Host: 127.0.0.1    Database: tinyos
-- ------------------------------------------------------
-- Server version	5.5.24-0ubuntu0.12.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `tb_network`
--

DROP TABLE IF EXISTS `tb_network`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tb_network` (
  `network_id` int(11) NOT NULL,
  `node_id` int(11) NOT NULL,
  `parent_id` int(11) NOT NULL,
  `quality` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`network_id`,`node_id`),
  KEY `network_id` (`network_id`,`parent_id`),
  CONSTRAINT `tb_network_ibfk_1` FOREIGN KEY (`network_id`, `node_id`) REFERENCES `tb_node` (`network_id`, `node_id`),
  CONSTRAINT `tb_network_ibfk_2` FOREIGN KEY (`network_id`, `parent_id`) REFERENCES `tb_node` (`network_id`, `node_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tb_network`
--

LOCK TABLES `tb_network` WRITE;
/*!40000 ALTER TABLE `tb_network` DISABLE KEYS */;
/*!40000 ALTER TABLE `tb_network` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tb_network_segment`
--

DROP TABLE IF EXISTS `tb_network_segment`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tb_network_segment` (
  `network_id` int(11) NOT NULL AUTO_INCREMENT,
  `network_name` char(50) NOT NULL,
  PRIMARY KEY (`network_id`),
  UNIQUE KEY `network_id` (`network_id`),
  UNIQUE KEY `network_name` (`network_name`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tb_network_segment`
--

LOCK TABLES `tb_network_segment` WRITE;
/*!40000 ALTER TABLE `tb_network_segment` DISABLE KEYS */;
INSERT INTO `tb_network_segment` VALUES (1,'10.16.17.0');
/*!40000 ALTER TABLE `tb_network_segment` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tb_node`
--

DROP TABLE IF EXISTS `tb_node`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tb_node` (
  `network_id` int(11) NOT NULL,
  `node_id` int(11) NOT NULL,
  `work_state` smallint(6) NOT NULL,
  `node_status` smallint(6) NOT NULL,
  `power` int(11) NOT NULL,
  `GPS` point NOT NULL,
  `update_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`network_id`,`node_id`),
  KEY `work_state` (`work_state`),
  KEY `node_status` (`node_status`),
  CONSTRAINT `tb_node_ibfk_1` FOREIGN KEY (`work_state`) REFERENCES `tb_work_state` (`work_state`),
  CONSTRAINT `tb_node_ibfk_2` FOREIGN KEY (`node_status`) REFERENCES `tb_node_status` (`node_status`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tb_node`
--

LOCK TABLES `tb_node` WRITE;
/*!40000 ALTER TABLE `tb_node` DISABLE KEYS */;
INSERT INTO `tb_node` VALUES (1,0,1,1,0,'\0\0\0\0\0\0\0\0\0\0\0\0\0I@\0\0\0\0\0\0D@','2012-10-09 13:38:51'),(1,1,1,1,91,'\0\0\0\0\0\0\0\0\0\0\0\0€I@\0\0\0\0\0€D@','2012-10-09 13:38:51'),(1,2,1,1,92,'\0\0\0\0\0\0\0\0\0\0\0\0\0J@\0\0\0\0\0\0E@','2012-10-09 13:38:51'),(1,3,1,1,93,'\0\0\0\0\0\0\0\0\0\0\0\0€J@\0\0\0\0\0€E@','2012-10-09 13:38:51'),(1,4,1,1,94,'\0\0\0\0\0\0\0\0\0\0\0\0\0K@\0\0\0\0\0\0F@','2012-10-09 13:38:51'),(1,5,1,1,95,'\0\0\0\0\0\0\0\0\0\0\0\0€K@\0\0\0\0\0€F@','2012-10-09 13:38:51'),(1,6,1,1,96,'\0\0\0\0\0\0\0\0\0\0\0\0\0L@\0\0\0\0\0\0G@','2012-10-09 13:38:51'),(1,7,1,1,97,'\0\0\0\0\0\0\0\0\0\0\0\0€L@\0\0\0\0\0€G@','2012-10-09 13:38:51'),(1,8,1,1,98,'\0\0\0\0\0\0\0\0\0\0\0\0\0M@\0\0\0\0\0\0H@','2012-10-09 13:38:51'),(1,9,1,1,99,'\0\0\0\0\0\0\0\0\0\0\0\0€M@\0\0\0\0\0€H@','2012-10-09 13:38:51'),(1,10,1,1,90,'\0\0\0\0\0\0\0\0\0\0\0\0\0I@\0\0\0\0\0\0D@','2012-10-09 13:38:51');
/*!40000 ALTER TABLE `tb_node` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tb_node_status`
--

DROP TABLE IF EXISTS `tb_node_status`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tb_node_status` (
  `node_status` smallint(6) NOT NULL AUTO_INCREMENT,
  `status_name` char(50) NOT NULL,
  PRIMARY KEY (`node_status`),
  UNIQUE KEY `status_name` (`status_name`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tb_node_status`
--

LOCK TABLES `tb_node_status` WRITE;
/*!40000 ALTER TABLE `tb_node_status` DISABLE KEYS */;
INSERT INTO `tb_node_status` VALUES (1,'normal');
/*!40000 ALTER TABLE `tb_node_status` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tb_sense`
--

DROP TABLE IF EXISTS `tb_sense`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tb_sense` (
  `order_num` bigint(20) NOT NULL AUTO_INCREMENT,
  `network_id` int(11) NOT NULL,
  `node_id` int(11) NOT NULL,
  `temp` double(8,2) DEFAULT NULL,
  `photo` double(8,2) DEFAULT NULL,
  `sound` double(8,2) DEFAULT NULL,
  `x_acc` double(8,2) DEFAULT NULL,
  `y_acc` double(8,2) DEFAULT NULL,
  `x_mag` double(8,2) DEFAULT NULL,
  `y_mag` double(8,2) DEFAULT NULL,
  `pressure` double(8,2) DEFAULT NULL,
  `humidity` double(8,2) DEFAULT NULL,
  `shoke` double(8,2) DEFAULT NULL,
  `unknow1` double(8,2) DEFAULT NULL,
  `sense_time` datetime DEFAULT NULL,
  `insert_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`order_num`),
  KEY `network_id` (`network_id`,`node_id`),
  CONSTRAINT `tb_sense_ibfk_1` FOREIGN KEY (`network_id`, `node_id`) REFERENCES `tb_node` (`network_id`, `node_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tb_sense`
--

LOCK TABLES `tb_sense` WRITE;
/*!40000 ALTER TABLE `tb_sense` DISABLE KEYS */;
/*!40000 ALTER TABLE `tb_sense` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tb_work_state`
--

DROP TABLE IF EXISTS `tb_work_state`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tb_work_state` (
  `work_state` smallint(6) NOT NULL AUTO_INCREMENT,
  `state_name` char(50) NOT NULL,
  PRIMARY KEY (`work_state`),
  UNIQUE KEY `state_name` (`state_name`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tb_work_state`
--

LOCK TABLES `tb_work_state` WRITE;
/*!40000 ALTER TABLE `tb_work_state` DISABLE KEYS */;
INSERT INTO `tb_work_state` VALUES (1,'work');
/*!40000 ALTER TABLE `tb_work_state` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Temporary table structure for view `viw_acce`
--

DROP TABLE IF EXISTS `viw_acce`;
/*!50001 DROP VIEW IF EXISTS `viw_acce`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `viw_acce` (
  `order_num` bigint(20),
  `network_id` int(11),
  `node_id` int(11),
  `x_acc` double(8,2),
  `y_acc` double(8,2),
  `sense_time` datetime,
  `insert_time` timestamp
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `viw_humidity`
--

DROP TABLE IF EXISTS `viw_humidity`;
/*!50001 DROP VIEW IF EXISTS `viw_humidity`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `viw_humidity` (
  `order_num` bigint(20),
  `network_id` int(11),
  `node_id` int(11),
  `humidity` double(8,2),
  `sense_time` datetime,
  `insert_time` timestamp
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `viw_magn`
--

DROP TABLE IF EXISTS `viw_magn`;
/*!50001 DROP VIEW IF EXISTS `viw_magn`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `viw_magn` (
  `order_num` bigint(20),
  `network_id` int(11),
  `node_id` int(11),
  `x_mag` double(8,2),
  `y_mag` double(8,2),
  `sense_time` datetime,
  `insert_time` timestamp
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `viw_network`
--

DROP TABLE IF EXISTS `viw_network`;
/*!50001 DROP VIEW IF EXISTS `viw_network`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `viw_network` (
  `network_id` int(11),
  `node_id` int(11),
  `parent_id` int(11),
  `quality` int(11)
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `viw_network_segment`
--

DROP TABLE IF EXISTS `viw_network_segment`;
/*!50001 DROP VIEW IF EXISTS `viw_network_segment`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `viw_network_segment` (
  `network_id` int(11),
  `network_name` char(50)
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `viw_node`
--

DROP TABLE IF EXISTS `viw_node`;
/*!50001 DROP VIEW IF EXISTS `viw_node`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `viw_node` (
  `network_id` int(11),
  `node_id` int(11),
  `work_state` smallint(6),
  `node_status` smallint(6),
  `power` int(11),
  `GPS` point,
  `update_time` timestamp
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `viw_node_status`
--

DROP TABLE IF EXISTS `viw_node_status`;
/*!50001 DROP VIEW IF EXISTS `viw_node_status`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `viw_node_status` (
  `node_status` smallint(6),
  `status_name` char(50)
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `viw_photo`
--

DROP TABLE IF EXISTS `viw_photo`;
/*!50001 DROP VIEW IF EXISTS `viw_photo`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `viw_photo` (
  `order_num` bigint(20),
  `network_id` int(11),
  `node_id` int(11),
  `photo` double(8,2),
  `sense_time` datetime,
  `insert_time` timestamp
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `viw_pressure`
--

DROP TABLE IF EXISTS `viw_pressure`;
/*!50001 DROP VIEW IF EXISTS `viw_pressure`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `viw_pressure` (
  `order_num` bigint(20),
  `network_id` int(11),
  `node_id` int(11),
  `pressure` double(8,2),
  `sense_time` datetime,
  `insert_time` timestamp
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `viw_sense`
--

DROP TABLE IF EXISTS `viw_sense`;
/*!50001 DROP VIEW IF EXISTS `viw_sense`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `viw_sense` (
  `order_num` bigint(20),
  `network_id` int(11),
  `node_id` int(11),
  `temp` double(8,2),
  `photo` double(8,2),
  `sound` double(8,2),
  `x_acc` double(8,2),
  `y_acc` double(8,2),
  `x_mag` double(8,2),
  `y_mag` double(8,2),
  `pressure` double(8,2),
  `humidity` double(8,2),
  `shoke` double(8,2),
  `unknow1` double(8,2),
  `sense_time` datetime,
  `insert_time` timestamp
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `viw_shoke`
--

DROP TABLE IF EXISTS `viw_shoke`;
/*!50001 DROP VIEW IF EXISTS `viw_shoke`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `viw_shoke` (
  `order_num` bigint(20),
  `network_id` int(11),
  `node_id` int(11),
  `shoke` double(8,2),
  `sense_time` datetime,
  `insert_time` timestamp
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `viw_sound`
--

DROP TABLE IF EXISTS `viw_sound`;
/*!50001 DROP VIEW IF EXISTS `viw_sound`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `viw_sound` (
  `order_num` bigint(20),
  `network_id` int(11),
  `node_id` int(11),
  `sound` double(8,2),
  `sense_time` datetime,
  `insert_time` timestamp
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `viw_temp`
--

DROP TABLE IF EXISTS `viw_temp`;
/*!50001 DROP VIEW IF EXISTS `viw_temp`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `viw_temp` (
  `order_num` bigint(20),
  `network_id` int(11),
  `node_id` int(11),
  `temp` double(8,2),
  `sense_time` datetime,
  `insert_time` timestamp
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `viw_work_state`
--

DROP TABLE IF EXISTS `viw_work_state`;
/*!50001 DROP VIEW IF EXISTS `viw_work_state`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `viw_work_state` (
  `work_state` smallint(6),
  `state_name` char(50)
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Final view structure for view `viw_acce`
--

/*!50001 DROP TABLE IF EXISTS `viw_acce`*/;
/*!50001 DROP VIEW IF EXISTS `viw_acce`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `viw_acce` AS select `tb_sense`.`order_num` AS `order_num`,`tb_sense`.`network_id` AS `network_id`,`tb_sense`.`node_id` AS `node_id`,`tb_sense`.`x_acc` AS `x_acc`,`tb_sense`.`y_acc` AS `y_acc`,`tb_sense`.`sense_time` AS `sense_time`,`tb_sense`.`insert_time` AS `insert_time` from `tb_sense` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `viw_humidity`
--

/*!50001 DROP TABLE IF EXISTS `viw_humidity`*/;
/*!50001 DROP VIEW IF EXISTS `viw_humidity`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `viw_humidity` AS select `tb_sense`.`order_num` AS `order_num`,`tb_sense`.`network_id` AS `network_id`,`tb_sense`.`node_id` AS `node_id`,`tb_sense`.`humidity` AS `humidity`,`tb_sense`.`sense_time` AS `sense_time`,`tb_sense`.`insert_time` AS `insert_time` from `tb_sense` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `viw_magn`
--

/*!50001 DROP TABLE IF EXISTS `viw_magn`*/;
/*!50001 DROP VIEW IF EXISTS `viw_magn`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `viw_magn` AS select `tb_sense`.`order_num` AS `order_num`,`tb_sense`.`network_id` AS `network_id`,`tb_sense`.`node_id` AS `node_id`,`tb_sense`.`x_mag` AS `x_mag`,`tb_sense`.`y_mag` AS `y_mag`,`tb_sense`.`sense_time` AS `sense_time`,`tb_sense`.`insert_time` AS `insert_time` from `tb_sense` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `viw_network`
--

/*!50001 DROP TABLE IF EXISTS `viw_network`*/;
/*!50001 DROP VIEW IF EXISTS `viw_network`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `viw_network` AS select `tb_network`.`network_id` AS `network_id`,`tb_network`.`node_id` AS `node_id`,`tb_network`.`parent_id` AS `parent_id`,`tb_network`.`quality` AS `quality` from `tb_network` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `viw_network_segment`
--

/*!50001 DROP TABLE IF EXISTS `viw_network_segment`*/;
/*!50001 DROP VIEW IF EXISTS `viw_network_segment`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `viw_network_segment` AS select `tb_network_segment`.`network_id` AS `network_id`,`tb_network_segment`.`network_name` AS `network_name` from `tb_network_segment` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `viw_node`
--

/*!50001 DROP TABLE IF EXISTS `viw_node`*/;
/*!50001 DROP VIEW IF EXISTS `viw_node`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `viw_node` AS select `tb_node`.`network_id` AS `network_id`,`tb_node`.`node_id` AS `node_id`,`tb_node`.`work_state` AS `work_state`,`tb_node`.`node_status` AS `node_status`,`tb_node`.`power` AS `power`,`tb_node`.`GPS` AS `GPS`,`tb_node`.`update_time` AS `update_time` from `tb_node` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `viw_node_status`
--

/*!50001 DROP TABLE IF EXISTS `viw_node_status`*/;
/*!50001 DROP VIEW IF EXISTS `viw_node_status`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `viw_node_status` AS select `tb_node_status`.`node_status` AS `node_status`,`tb_node_status`.`status_name` AS `status_name` from `tb_node_status` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `viw_photo`
--

/*!50001 DROP TABLE IF EXISTS `viw_photo`*/;
/*!50001 DROP VIEW IF EXISTS `viw_photo`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `viw_photo` AS select `tb_sense`.`order_num` AS `order_num`,`tb_sense`.`network_id` AS `network_id`,`tb_sense`.`node_id` AS `node_id`,`tb_sense`.`photo` AS `photo`,`tb_sense`.`sense_time` AS `sense_time`,`tb_sense`.`insert_time` AS `insert_time` from `tb_sense` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `viw_pressure`
--

/*!50001 DROP TABLE IF EXISTS `viw_pressure`*/;
/*!50001 DROP VIEW IF EXISTS `viw_pressure`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `viw_pressure` AS select `tb_sense`.`order_num` AS `order_num`,`tb_sense`.`network_id` AS `network_id`,`tb_sense`.`node_id` AS `node_id`,`tb_sense`.`pressure` AS `pressure`,`tb_sense`.`sense_time` AS `sense_time`,`tb_sense`.`insert_time` AS `insert_time` from `tb_sense` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `viw_sense`
--

/*!50001 DROP TABLE IF EXISTS `viw_sense`*/;
/*!50001 DROP VIEW IF EXISTS `viw_sense`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `viw_sense` AS select `tb_sense`.`order_num` AS `order_num`,`tb_sense`.`network_id` AS `network_id`,`tb_sense`.`node_id` AS `node_id`,`tb_sense`.`temp` AS `temp`,`tb_sense`.`photo` AS `photo`,`tb_sense`.`sound` AS `sound`,`tb_sense`.`x_acc` AS `x_acc`,`tb_sense`.`y_acc` AS `y_acc`,`tb_sense`.`x_mag` AS `x_mag`,`tb_sense`.`y_mag` AS `y_mag`,`tb_sense`.`pressure` AS `pressure`,`tb_sense`.`humidity` AS `humidity`,`tb_sense`.`shoke` AS `shoke`,`tb_sense`.`unknow1` AS `unknow1`,`tb_sense`.`sense_time` AS `sense_time`,`tb_sense`.`insert_time` AS `insert_time` from `tb_sense` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `viw_shoke`
--

/*!50001 DROP TABLE IF EXISTS `viw_shoke`*/;
/*!50001 DROP VIEW IF EXISTS `viw_shoke`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `viw_shoke` AS select `tb_sense`.`order_num` AS `order_num`,`tb_sense`.`network_id` AS `network_id`,`tb_sense`.`node_id` AS `node_id`,`tb_sense`.`shoke` AS `shoke`,`tb_sense`.`sense_time` AS `sense_time`,`tb_sense`.`insert_time` AS `insert_time` from `tb_sense` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `viw_sound`
--

/*!50001 DROP TABLE IF EXISTS `viw_sound`*/;
/*!50001 DROP VIEW IF EXISTS `viw_sound`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `viw_sound` AS select `tb_sense`.`order_num` AS `order_num`,`tb_sense`.`network_id` AS `network_id`,`tb_sense`.`node_id` AS `node_id`,`tb_sense`.`sound` AS `sound`,`tb_sense`.`sense_time` AS `sense_time`,`tb_sense`.`insert_time` AS `insert_time` from `tb_sense` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `viw_temp`
--

/*!50001 DROP TABLE IF EXISTS `viw_temp`*/;
/*!50001 DROP VIEW IF EXISTS `viw_temp`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `viw_temp` AS select `tb_sense`.`order_num` AS `order_num`,`tb_sense`.`network_id` AS `network_id`,`tb_sense`.`node_id` AS `node_id`,`tb_sense`.`temp` AS `temp`,`tb_sense`.`sense_time` AS `sense_time`,`tb_sense`.`insert_time` AS `insert_time` from `tb_sense` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `viw_work_state`
--

/*!50001 DROP TABLE IF EXISTS `viw_work_state`*/;
/*!50001 DROP VIEW IF EXISTS `viw_work_state`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `viw_work_state` AS select `tb_work_state`.`work_state` AS `work_state`,`tb_work_state`.`state_name` AS `state_name` from `tb_work_state` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2012-10-09 22:40:24
