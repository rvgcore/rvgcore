DELETE FROM `command` WHERE `name` = 'sakura';

INSERT INTO `command` (`name`, `security`, `help`) VALUES
('sakura', 3, 'Syntax: .sakura $dir #value\r\n\r\nTeleport in the direction indicated by dir (f=forward,b=backward,u=up,d=down) by the number of coordinate units specified as value.');
