while true; do
    read -p "Are you sure you want to drop the database? All data will be lost:" yn
    case $yn in
        [Yy]* ) sudo -u postgres psql postgres -f parameters.sql -f drop-db.sql; break;;
        [Nn]* ) exit;;
        * ) echo "Please answer yes or no.";;
    esac
done
