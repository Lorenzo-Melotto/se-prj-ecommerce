# Progetto Ingegneria del Software e-commerce
## Esecuzione

Per poter eseguire il progetto seguire i seguenti step:

1. Creare il database: recarsi all'interno della directory
   `ProgettoSE/db-scripts` ed eseguire il comando:
   ```bash
    $ ./create.sh
   ```
   Con questo comando verrà creato il database che viene 
   utilizzato dal sistema.
2. Compilare: recarsi all'interno della directory 
   `ProgettoSE/Main/src` ed eseguire il comando:
   ```bash
   $ make
   ```
   Il comando compilerà tutto il necessario per poter 
   creare un eseguibile di tutto il sistema
3. Eseguire il sistema: recarsi all'interno della directory
   `ProgettoSE/Main/bin` ed eseguire il comando:
    ```bash
    $ ./main
    ```
   L'eseguibile farà partire i vari test creati e alla fine
   di tutti i test eseguirà i monitor.
   Durante l'esecuzione verranno create delle directory 
   all'interno di `ProgettoSE/Main/src` dove vengono 
   reindirizzati gli output dei processi mandati in esecuzione
   dai test. Per non avere nessun tipo di `print`, modificare
   la variabile `DEBUG` in `ProgettoSE/commons/globals.cpp` e
   ricompilare il progetto.
   > Nota: ogni test dura circa 20 minuti.

## Clean-up
Per eliminare il database, recarsi in `ProgettoSE/db-scripts`
ed eseguire il comando:
```bash
$ clean.sh
```
Al messaggio di conferma, digitare `y` e poi `Enter`.