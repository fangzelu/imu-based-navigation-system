
#ifndef __MTSL_DEF_H__
#define __MTSL_DEF_H__

// options
#define __START_POSITION_MODE__
#define __SIMULATION_MODE__
#define __MSP430_MATH_LIB__
//#define __DEBUG_MODE__
//#define __INT_MODE__

// Filtering option
#define __DIST_FILTER__
#define __POS_FILTER__

// Set neighbors
#define MAX_NB_CNT	4


// Thread control
#define ADVERT_BASE_PERIOD	500
#define RANGING_BASE_PERIOD	500
#define RANGING_LATENCY	20

// Define Communication Ports
#define RADIO_PORT	0xfe
#define SERIAL_PORT	0x0a

// status
#define MTSL_LEVEL0	0
#define MTSL_LEVEL1	1
#define MTSL_LEVEL2	2
#define MTSL_LEVEL3	3

// filtering 
#define DIST_THRES	1.0
#define POS_THRES	1.0


// my information
typedef struct _my_info_t{
	uint16_t id;
	double x;
	double y;
	uint8_t level;
} my_info_t;

// neighbor table information
typedef struct _nb_info_t{
	double x;
	double y;
	double d;
	uint8_t life;
} nb_info_t;

// history of neighbor
typedef struct _nb_past_t{
	double x;
	double y;
	double d;
} nb_past_t;

// temp. anchors
typedef struct _anchor_t{
	uint16_t id;
	double x;
	double y;
	double d;
} anchor_t;

// report data type
typedef struct _report_type{
	uint16_t id;
	double x;
	double y;
	double d;
} report_type_t;

my_info_t my_info;
nb_info_t nb_table[ MAX_NB_CNT ];
nb_past_t nb_past[ MAX_NB_CNT ];
anchor_t anchor[ 3 ];
report_type_t report_data[ MAX_NB_CNT ];

void mtsl_init();
void mtsl_ranging();
void set_anchors();

#ifdef __DIST_FILTER__
void dist_filter( uint8_t i );
#endif
#ifdef __POS_FILTER__
//void pos_filter();
#endif


// trilateration functions
double delta_x, delta_y;
uint8_t trilaterate( uint8_t i1, uint8_t i2, uint8_t i3 );
void self_pos();

#define ERROR_NONINTERSECT	10
#define ERROR_BASELINE		11

// filters and helpers
double ptod( double ax, double ay, double bx, double by );


// communication
void report();

#ifdef __START_POSITION_MODE__
void init_position();
#endif

double square( double A ){
	return A * A;
}

#ifndef __MSP430_MATH_LIB__
double sqrtd( double A ){
	
	double ans, i;
	for( ans = 0.0, i = 1.0; ans != i; ){
		ans = i;
		i = (ans + (A/ans)) / 2.0;
	}
	return ans;

}

double dabs( double A ){
	if( A < 0 ){ A = A * (-1.0); }
	return A;
}
#else

#define sqrtd(x)	(double)sqrtf((x))
#define dabs(x)		(double)fabs((x))

#endif


#ifdef __START_POSITION_MODE__
void init_position();
#endif

#ifdef __SIMULATION_MODE__
uint8_t sim_node_id;
void do_step();
#endif

#ifdef __DEBUG_MODE__

report_type_t debug_data[ MAX_NB_CNT ];
uint16_t dbg_cnt = 10;
void dbg_out();

#define dbg( i, line, v1, v2, v3 )	\
	do {	\
		debug_data[(i)].id = (line);	\
		debug_data[(i)].x = (double)(v1);	\
		debug_data[(i)].y = (double)(v2);	\
		debug_data[(i)].d = (double)(v3);	\
	}	while(0);

#endif



#endif

