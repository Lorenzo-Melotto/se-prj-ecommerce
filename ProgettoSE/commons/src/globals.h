#ifndef GLOBAL_H
#define GLOBAL_H

/**
 * @brief Returns the minimum between `a` and `b`
 * 
 */
#define get_min(a, b) ((a)<(b)?(a):(b))

// stream where the customer client writes and the server reads
extern const char* CUSTOMER_STREAM_CLIENT;
// stream where the customer server writes and the client reads
extern const char* CUSTOMER_STREAM_SERVER;

// stream where the furnisher client writes and the server reads
extern const char* FURNISHER_STREAM_CLIENT;
// stream where the furnisher server writes and the client reads
extern const char* FURNISHER_STREAM_SERVER;

// stream where the transporter client writes and the server reads
extern const char* TRANSPORTER_STREAM_CLIENT;
// stream where the transporter server writes and the client reads
extern const char* TRANSPORTER_STREAM_SERVER;

/**
 * @brief Nanoseconds of the simulation
 * 
 */
extern long int nanos;
/**
 * @brief Nanoseconds passed before the last update
 * 
 */
extern long int last_nanos;
/**
 * @brief Nanosecond passed between each update
 * 
 */
extern double timeadvance;
/**
 * @brief Used for more precision
 * 
 */
extern long int nanos_day;

/**
 * @brief Duration of the simulation in ticks
 * 
 */
extern int HORIZON;

/**
 * @brief Flag used to compile with debug prints
 * 
 */
extern bool DEBUG;

#endif