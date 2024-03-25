\c :dbname postgres

GRANT ALL PRIVILEGES ON DATABASE :dbname to :username;

ALTER TABLE furnishers OWNER TO :username;
ALTER TABLE items OWNER TO :username;
ALTER TABLE customers OWNER TO :username;
ALTER TABLE transporters OWNER TO :username;
ALTER TABLE orders OWNER TO :username;
ALTER TABLE item_order OWNER TO :username;
ALTER TABLE item_quantity_history OWNER TO :username;
ALTER TABLE logtable OWNER TO :username;
ALTER TABLE transactions_per_second OWNER TO :username;

GRANT ALL ON SCHEMA public TO :username;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO :username;