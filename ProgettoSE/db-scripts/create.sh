

#invoco file che creano parametri, user e db, schema e privilegi
sudo -u postgres psql postgres -f parameters.sql -f create-db-user.sql  -f schema.sql  -f grant.sql

