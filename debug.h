#ifdef DEBUG_PRINT
// Debugging
#define debug_begin(baud)           Serial.begin(baud)  
#define debug_wait(wait)            {unsigned long start = millis(); while(!Serial || ((millis()-start) < wait)){ delay(10); }}
#define debug_print(args...)        Serial.print(args)
#define debug_println(args...)      Serial.println(args)
#define debug_printf(args...)       Serial.printf(args)
#define debug_delay(ms)             delay(ms)                       

#ifdef _Time_h
// Time
#define debug_date(t)             debug_print(year(t)); \
                                  debug_print("-"); \
                                  debug_print(month(t) < 10 ? "0" : ""); \
                                  debug_print(month(t)); \
                                  debug_print("-"); \
                                  debug_print(day(t) < 10 ? "0" : ""); \
                                  debug_print(day(t))
#define debug_time(t)             debug_print(hour(t) < 10 ? "0" : ""); \
                                  debug_print(hour(t)); \
                                  debug_print(':'); \
                                  debug_print(minute(t) < 10 ? "0" : ""); \
                                  debug_print(minute(t)); \
                                  debug_print(':'); \
                                  debug_print(second(t) < 10 ? "0" : ""); \
                                  debug_print(second(t))
#define debug_datetime(t)         debug_date(t); debug_print(' '); debug_time(t)      
#else
#define debug_date(t)
#define debug_time(t)
#define debug_datetime(t)
#endif

#else
// No debugging.
#define debug_begin(baud)
#define debug_wait(wait)
#define debug_print(args...)    
#define debug_println(args...)  
#define debug_printf(args...)
#define debug_delay(ms)
#define debug_date(t)
#define debug_time(t)
#define debug_datetime(t)
#endif