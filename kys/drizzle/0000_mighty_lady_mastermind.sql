CREATE TABLE `corpusTicks` (
	`id` int AUTO_INCREMENT NOT NULL,
	`tick` int NOT NULL,
	`cycle` int NOT NULL,
	`steps` json NOT NULL,
	`triad` json NOT NULL,
	`u` json NOT NULL,
	`interfaces` json NOT NULL,
	`modeBalance` varchar(8) NOT NULL,
	`createdAt` timestamp NOT NULL DEFAULT (now()),
	CONSTRAINT `corpusTicks_id` PRIMARY KEY(`id`),
	CONSTRAINT `corpusTicks_tick_unique` UNIQUE(`tick`)
);
--> statement-breakpoint
CREATE TABLE `engineConfigs` (
	`id` int AUTO_INCREMENT NOT NULL,
	`version` varchar(32) NOT NULL,
	`word` json NOT NULL,
	`u1` json NOT NULL,
	`u2` json NOT NULL,
	`active` int NOT NULL DEFAULT 0,
	`note` text,
	`createdAt` timestamp NOT NULL DEFAULT (now()),
	CONSTRAINT `engineConfigs_id` PRIMARY KEY(`id`),
	CONSTRAINT `engineConfigs_version_unique` UNIQUE(`version`)
);
--> statement-breakpoint
CREATE TABLE `invariantChecks` (
	`id` int AUTO_INCREMENT NOT NULL,
	`invariantId` varchar(16) NOT NULL,
	`name` varchar(128) NOT NULL,
	`ticksChecked` int NOT NULL,
	`ticksPassed` int NOT NULL,
	`faults` int NOT NULL DEFAULT 0,
	`summary` text,
	`runLabel` varchar(64) NOT NULL DEFAULT 'seed-36',
	`checkedAt` timestamp NOT NULL DEFAULT (now()),
	CONSTRAINT `invariantChecks_id` PRIMARY KEY(`id`)
);
--> statement-breakpoint
CREATE TABLE `storedFiles` (
	`id` int AUTO_INCREMENT NOT NULL,
	`fileKey` varchar(512) NOT NULL,
	`url` text NOT NULL,
	`filename` varchar(256) NOT NULL,
	`mimeType` varchar(128),
	`sizeBytes` int,
	`kind` enum('export','upload') NOT NULL DEFAULT 'upload',
	`uploadedBy` int,
	`createdAt` timestamp NOT NULL DEFAULT (now()),
	CONSTRAINT `storedFiles_id` PRIMARY KEY(`id`),
	CONSTRAINT `storedFiles_fileKey_unique` UNIQUE(`fileKey`)
);
--> statement-breakpoint
CREATE TABLE `users` (
	`id` int AUTO_INCREMENT NOT NULL,
	`openId` varchar(64) NOT NULL,
	`name` text,
	`email` varchar(320),
	`loginMethod` varchar(64),
	`role` enum('user','admin') NOT NULL DEFAULT 'user',
	`createdAt` timestamp NOT NULL DEFAULT (now()),
	`updatedAt` timestamp NOT NULL DEFAULT (now()) ON UPDATE CURRENT_TIMESTAMP,
	`lastSignedIn` timestamp NOT NULL DEFAULT (now()),
	CONSTRAINT `users_id` PRIMARY KEY(`id`),
	CONSTRAINT `users_openId_unique` UNIQUE(`openId`)
);
