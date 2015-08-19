SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for `rbac_default_permissions`
-- ----------------------------
DROP TABLE IF EXISTS `rbac_default_permissions`;
CREATE TABLE `rbac_default_permissions` (
  `secId` int(10) unsigned NOT NULL COMMENT 'Security Level id',
  `permissionId` int(10) unsigned NOT NULL COMMENT 'permission id',
  PRIMARY KEY (`secId`,`permissionId`),
  KEY `fk__rbac_default_permissions__rbac_permissions` (`permissionId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Default permission to assign to different account security levels';

-- ----------------------------
-- Records of rbac_default_permissions
-- ----------------------------
INSERT INTO `rbac_default_permissions` VALUES ('3', '192');
INSERT INTO `rbac_default_permissions` VALUES ('2', '193');
INSERT INTO `rbac_default_permissions` VALUES ('1', '194');
