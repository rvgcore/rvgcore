DELETE FROM `command` WHERE `name` = 'mv';

INSERT INTO `command` (`name`, `security`, `help`) VALUES
('sakura', 3, 'Syntax: .mv $dir #value\r\n\r\nTeleport in the direction indicated by dir (f=forward,b=backward,u=up,d=down) by the number of coordinate units specified as value. You've got the moves like Sakura');
