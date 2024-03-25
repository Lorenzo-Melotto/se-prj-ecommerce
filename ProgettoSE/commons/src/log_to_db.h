#ifndef LOG_TO_DB_H
#define LOG_TO_DB_H

#include <stdio.h>
#include <string.h>
#include "pgsql.h"
#include "time_utils.h"

/**
 * @brief logs to DB using connection `db` the infos about the actions taken by the actors including
 * the response time from the server
 * 
 * @param db connection to the db
 * @param run_number number of actual run
 * @param actor can be `"CUSTOMER"`, `"FURNISHER"` or `"TRANSPORTER"`
 * @param date date to log in 'YYYY-MM-DD HH:mm:ss-msms' format
 * @param nanos_day nanoesconds to append to date for more precision
 * @param timeadvance time passed between actor actions
 * @param pid pid of the executing program
 * @param actor_id id of the actor that executes the action
 * @param actor_action the action executed by the actor, can be LIST_OF_ITEMS, BUY_ITEM, ...
 * @param reponse_time_ms milliseconds taken from server to reply to the client
 * @param action_object_id the object that receives the action executed by the actor. Put -1 if the action does not require an object
 */
void log_to_db_with_response_time(Con2DB db, int run_number, char *actor, char *date, long nanos_day, double timeadvance, int pid, int actor_id, char *actor_action, int action_object_id, float response_time_ms, char *test_test);

/**
 * @brief logs to DB using connection `db` the infos about the actions taken by the actors including
 * the response time from the server
 * 
 * @param db connection to the db
* @param run_number number of actual run
 * @param actor can be `"CUSTOMER"`, `"FURNISHER"` or `"TRANSPORTER"`
 * @param date date to log in 'YYYY-MM-DD HH:mm:ss-msms' format
 * @param nanos_day nanoesconds to append to date for more precision
 * @param timeadvance time passed between actor actions
 * @param pid pid of the executing program
 * @param actor_id id of the actor that executes the action
 * @param actor_action the action executed by the actor, can be LIST_OF_ITEMS, BUY_ITEM, ...
 * @param reponse_time_ms milliseconds taken from server to reply to the client
 * @param action_object_id the object that receives the action executed by the actor. Put -1 if the action does not require an object
 */
void log_to_db_with_response_time(Con2DB db, int run_number, char *actor, char *date, long nanos_day, double timeadvance, int pid, int actor_id, char *actor_action, float response_time_ms, char *test_type);

#endif