\c :dbname 

--Furnishers rappresenta i fornitori
CREATE TABLE IF NOT EXISTS furnishers(
    id SERIAL PRIMARY KEY
);

--Items rappresenta le item in vendita
CREATE TABLE IF NOT EXISTS items(
    id SERIAL PRIMARY KEY,
    qta INTEGER,    --quantità dell'item
    furnisher_id INTEGER,
    FOREIGN KEY (furnisher_id) REFERENCES furnishers(id)
);

CREATE TABLE IF NOT EXISTS customers(
    id SERIAL PRIMARY KEY
);

CREATE TABLE IF NOT EXISTS transporters(
    id SERIAL PRIMARY KEY
);

-- Orders rappresenta gli ordini effettuati dai clienti.
CREATE TABLE IF NOT EXISTS orders(
    id SERIAL PRIMARY KEY,
    started_delivery TIMESTAMP, -- tempo in cui la consegna è stata presa in carico  
    delivery_time TIMESTAMP, -- tempo in cui la consegna è stata effettuata
    customer_id INTEGER NOT NULL,
    transporter_id INTEGER,
    FOREIGN KEY (customer_id) REFERENCES customers(id),
    FOREIGN KEY (transporter_id) REFERENCES transporters(id)
);

CREATE TABLE IF NOT EXISTS item_order(
    item_id INTEGER,
    order_id INTEGER,
    qta INTEGER,
    PRIMARY KEY (item_id, order_id),
    FOREIGN KEY (item_id) REFERENCES items(id),
    FOREIGN KEY (order_id) REFERENCES orders(id)
);

-- tutte le azioni che i client compiono
CREATE TYPE allowedActions AS ENUM('GET_FURNISHER_ID', 
                                   'GET_CUSTOMER_ID', 
                                   'GET_TRANSPORTER_ID', 
                                   'INSERT_ITEM', 
                                   'LIST_OF_ITEMS', 
                                   'UPDATE_ITEM',
                                   'BUY_ITEM', 
                                   'HAS_ORDERS', 
                                   'TAKE_ORDER', 
                                   'DELIVER_ORDER'); 

DROP TYPE IF EXISTS testType CASCADE;
-- CREATE TYPE testType AS ENUM('furn_10c_1s',
--                              'cust_10c_1s',
--                              'tran_10c_1s',
--                              'furn_50c_5s',
--                              'cust_50c_5s',
--                              'tran_50c_5s',
--                              'furn_10c_3s_cust_10c_3s_tran_10c_3s');
-- CREATE TYPE testType AS ENUM('furn_5c_1s_cust_5c_1s_tran_5c_1s',
--                              'furn_8c_2s_cust_8c_2s_tran_8c_2s',
--                              'furn_10c_3s_cust_10c_3s_tran_10c_3s',
--                              'furn_13c_4s_cust_13c_4s_tran_13c_4s',
--                              'furn_15c_5s_cust_15c_5s_tran_15c_5s');

-- tabella contenente i dati della simulazione
-- DROP TABLE IF EXISTS logtable CASCADE;
CREATE TABLE IF NOT EXISTS logtable(
    run_number INT NOT NULL,
    timevalue TIMESTAMP NOT NULL,
    nanoseconds BIGINT NOT NULL,
    pid INT NOT NULL,
    customer_id INT REFERENCES customers(id),
    furnisher_id INT REFERENCES furnishers(id),
    transporter_id INT REFERENCES transporters(id),
    actor_action allowedActions NOT NULL,
    order_id INT REFERENCES orders(id),
    item_id INT REFERENCES items(id),
    response_time_ms FLOAT,
    func_monitor_1 BOOLEAN,
    func_monitor_2 BOOLEAN,
    func_monitor_3 BOOLEAN,
    non_func_monitor_1 BOOLEAN,
    test_type VARCHAR(255) NOT NULL,
    PRIMARY KEY (timevalue, nanoseconds, pid),
    CONSTRAINT either_actor_id CHECK ((customer_id IS NULL <> (furnisher_id IS NULL AND transporter_id IS NULL)) AND
                                      (furnisher_id IS NULL <> (customer_id IS NULL AND transporter_id IS NULL)) AND
                                      (transporter_id IS NULL <> (customer_id IS NULL AND furnisher_id IS NULL))),
    CONSTRAINT either_object_id CHECK (((order_id IS NULL) <> (item_id IS NULL)) OR 
                                        (order_id IS NULL AND item_id IS NULL))
);

-- DROP TABLE IF EXISTS item_quantity_history CASCADE;
CREATE TABLE IF NOT EXISTS item_quantity_history(
    order_id INTEGER NOT NULL,
    item_id INTEGER NOT NULL,
    new_quantity INTEGER NOT NULL,
    PRIMARY KEY (order_id, item_id),
    FOREIGN KEY (order_id, item_id) REFERENCES item_order(order_id, item_id)
);

-- DROP TABLE IF EXISTS transactions_per_second CASCADE;
CREATE TABLE IF NOT EXISTS transactions_per_second(
    run_number INT NOT NULL,
    start_timestamp TIMESTAMP NOT NULL,
    start_nanoseconds BIGINT NOT NULL,
    end_timestamp TIMESTAMP NOT NULL,
    end_nanoseconds BIGINT NOT NULL,
    transaction_s FLOAT NOT NULL,
    monitor_response BOOLEAN,
    PRIMARY KEY (end_timestamp, end_nanoseconds)
);
