const char* sql_create_table_collected_eggs = R"_sql_(
DROP TABLE IF EXISTS "Collected Eggs";
CREATE TABLE IF NOT EXISTS "Collected Eggs" (
	"Date" TEXT PRIMARY KEY,
   	"Amount" INTEGER NOT NULL,
	"Small eggs" INTEGER
);
)_sql_";

const char* sql_create_table_buyers = R"_sql_(
DROP TABLE IF EXISTS "Buyers";
CREATE TABLE IF NOT EXISTS "Buyers" (
	"ID" INTEGER PRIMARY KEY,
   	"Name" TEXT UNIQUE NOT NULL
);
)_sql_";

const char* sql_create_table_sold_eggs = R"_sql_(
DROP TABLE IF EXISTS "Sold Eggs";
CREATE TABLE IF NOT EXISTS "Sold Eggs" (
	"ID" INTEGER PRIMARY KEY,
   	"Amount" INTEGER NOT NULL,
   	"Price" INTEGER NOT NULL,
   	"Date" TEXT NOT NULL,
   	"Buyer" INTEGER
);
)_sql_";

const char* sql_create_table_bought_resources = R"_sql_(
DROP TABLE IF EXISTS "Bought Resources";
CREATE TABLE IF NOT EXISTS "Bought Resources" (
	"ID" INTEGER PRIMARY KEY,
   	"Resource" TEXT NOT NULL,
   	"Price" INTEGER NOT NULL,
   	"Date" TEXT NOT NULL,
   	"Shop" TEXT NOT NULL
);
)_sql_";

const char* sql_create_view_sold_eggs_and_buyers = R"_sql_(
DROP VIEW IF EXISTS "Sold Eggs + Buyers";
CREATE VIEW IF NOT EXISTS "Sold Eggs + Buyers" AS
    SELECT Date, Amount, Price || " zł" AS 'Price (zł)', Name AS "Buyer"
    FROM "Sold Eggs" 
    LEFT JOIN Buyers 
    ON Buyer = Buyers.ID;
)_sql_";