const char* CUSTOMER_STREAM_CLIENT = "customerstreamclient";
const char* CUSTOMER_STREAM_SERVER = "customerstreamserver";
const char* FURNISHER_STREAM_CLIENT = "furnisherstreamclient";
const char* FURNISHER_STREAM_SERVER = "furnisherstreamserver";
const char* TRANSPORTER_STREAM_CLIENT = "transporterstreamclient";
const char* TRANSPORTER_STREAM_SERVER = "transporterstreamserver";

long int nanos = 0;
long int last_nanos = 0;
double timeadvance;
long int nanos_day;
int HORIZON = 30;
bool DEBUG = true;