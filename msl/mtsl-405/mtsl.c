#include <stdlib.h>

#ifdef __MSP430_MATH_LIB__
#include <math.h>
#endif

#include <syscall.h>
#include <pthread.h>
#include "mtsl.h"

pthread_mutex_t mutex_lock;
void *th_advert( void *arg );
void *th_recv( void *arg );


#ifdef __START_POSITION_MODE__
void init_position(){

	nb_table[0].x = 0.0;
	nb_table[0].y = 0.0;

	nb_table[1].x = 3.0;
	nb_table[1].y = 0.0;
	
	nb_table[2].x = 3.0;
	nb_table[2].y = 3.0;

	nb_table[3].x = 0.0;
	nb_table[3].y = 3.0;

}
#endif

void mtsl_init(){

#ifdef __START_POSITION_MODE__
	init_position();
#endif
	my_info.id = LOCAL_ADDR;

	my_info.x = nb_table[LOCAL_ADDR].x;
	my_info.y = nb_table[LOCAL_ADDR].y;

	my_info.level = MTSL_LEVEL0;

}

int main(){

	mtsl_init();
#ifndef __SIMULATION_MODE__
	pthread_t th0, th1;
	pthread_mutex_init( &mutex_lock, NULL );

	pthread_create( &th0, NULL, (void*)th_advert, NULL );
	sleep( ADVERT_BASE_PERIOD );
	pthread_create( &th1, NULL, (void*)th_recv, NULL );
#endif
	while(1){	// this is the main thread

		led3_toggle();
#ifdef __SIMULATION_MODE__
		do_step();
		report();
#else
		if( my_info.level < MTSL_LEVEL3 ) continue;
		pthread_mutex_lock( &mutex_lock );
		mtsl_ranging();
		mtsl_dist_filtering();
		fill_anchor();
		self_pos();
		report();
		pthread_mutex_unlock( &mutex_lock );
#endif
		sleep( RANGING_BASE_PERIOD );
	}

	return 0;
}



void mtsl_ranging(){

	uint8_t i;
	double meas_d;

	for( i = 0; i < MAX_NB_CNT; i++ ){
		if( i == LOCAL_ADDR ) continue;	
	
		if( radio_send( (uint16_t)i, 0xff, 0xff, (rf_pkt*)&meas_d ) < 0 ){
			continue;
		}
		if( meas_d < 0 ) continue;

		nb_past[i].d = nb_table[i].d;
		nb_table[i].d = meas_d;
			
	}

}

void mtsl_dist_filtering(){
	
	uint8_t i;
	double gap;
	for( i = 0; i < MAX_NB_CNT; i++ ){
		
		if( i == LOCAL_ADDR ) continue;
		gap = 0.0;

		gap = dabs( nb_past[i].d - nb_table[i].d );
		if( nb_past[i].d > nb_table[i].d ){

			if( gap > DIST_THRES ){
				nb_table[i].d = nb_past[i].d - DIST_THRES;
			}

		}
		else{
			if( gap > DIST_THRES ){
				nb_table[i].d = nb_past[i].d + DIST_THRES;
			}
		}
		
	}
}

void fill_anchor(){
	
	uint8_t i;
	uint8_t _idx = 0;
	for( i = 0; i < MAX_NB_CNT; i++ ){

		if( i == LOCAL_ADDR ) continue;
		anchor[_idx].id = i;
		anchor[_idx].x = nb_table[i].x;
		anchor[_idx].y = nb_table[i].y;
		anchor[_idx].d = nb_table[i].d;
		_idx++;

	}
}



void self_pos(){

	uint8_t ret;
	double cnt = 0;

	double x = 0.0;	
	double y = 0.0;

	nb_past[ LOCAL_ADDR ].x = nb_table[ LOCAL_ADDR ].x;
	nb_past[ LOCAL_ADDR ].y = nb_table[ LOCAL_ADDR ].y;

#ifndef __SIMULATION_MODE__
	if( LOCAL_ADDR == 0 ){
		nb_table[ LOCAL_ADDR ].x = 0.0;
		nb_table[ LOCAL_ADDR ].y = 0.0;
	}
	else if( LOCAL_ADDR == 1 ){
		nb_table[ LOCAL_ADDR ].x = nb_table[0].d;
		nb_table[ LOCAL_ADDR ].y = 0.0;
	}
	else{
#endif
		ret = trilaterate( 0, 1, 2 );

		if( ret != ERROR_NONINTERSECT ){
			x = delta_x;
			y = delta_y;
			cnt += 1.0;
		}

		ret = trilaterate( 0, 2, 1 );
		if( ret != ERROR_NONINTERSECT ){
			x += delta_x;
			y += delta_y;
			cnt += 1.0;
		}

		ret = trilaterate( 1, 2, 0 );
		if( ret != ERROR_NONINTERSECT ){
			x += delta_x;
			y += delta_y;
			cnt += 1.0;
		}
		if( cnt > 0 ){
			x /= cnt;
			y /= cnt;

// position filtering
			if( nb_past[ LOCAL_ADDR ].x > x ){
				if( nb_past[ LOCAL_ADDR ].x - x > DIST_THRES ){
					x = nb_past[ LOCAL_ADDR ].x - DIST_THRES;
				}
			}
			else{
				if( x - nb_past[ LOCAL_ADDR ].x > DIST_THRES ){
					x = nb_past[ LOCAL_ADDR ].x + DIST_THRES;
				}
			}

			if( nb_past[ LOCAL_ADDR ].y > y ){
				if( nb_past[ LOCAL_ADDR ].y - y > DIST_THRES ){
					y = nb_past[ LOCAL_ADDR ].y - DIST_THRES;
				}
			}
			else{
				if( y - nb_past[ LOCAL_ADDR ].y > DIST_THRES ){
					y = nb_past[ LOCAL_ADDR ].y + DIST_THRES;
				}
			}

			nb_table[ LOCAL_ADDR ].x = x;
			nb_table[ LOCAL_ADDR ].y = y;
		}
		else{
			nb_table[ LOCAL_ADDR ].x = nb_past[ LOCAL_ADDR ].x;
			nb_table[ LOCAL_ADDR ].y = nb_past[ LOCAL_ADDR ].y;
		}
#ifndef __SIMULATION_MODE__
	}
#endif	
	my_info.x = nb_table[ LOCAL_ADDR ].x;
	my_info.y = nb_table[ LOCAL_ADDR ].y;

}

uint8_t trilaterate( uint8_t i1, uint8_t i2, uint8_t i3 ){

	
	double x_1, y_1;
	double x_2, y_2;
	double x_3, y_3;

	double r1, r2, r3;

	double dsq, gam;
	double xa, ya, xb, yb;
	double xt1, yt1;
	double xt2, yt2;
	double d1, d2;

	// get positions of anchor
	
	x_1 = anchor[i1].x;
	y_1 = anchor[i1].y;

	x_2 = anchor[i2].x;
	y_2 = anchor[i2].y;

	x_3 = anchor[i3].x;
	y_3 = anchor[i3].y;

	r1 = anchor[i1].d;
	r2 = anchor[i2].d;
	r3 = anchor[i3].d;

	// Find the two intersection points of the two circles

	dsq = square(x_2-x_1) + square(y_2-y_1);
	gam = (square(r2+r1) - dsq) * (dsq - square(r2-r1));
#ifdef __DEBUG_MODE__
	dbg( 1, anchor[i1].id, x_1, y_1, r1 );
	dbg( 2, anchor[i2].id, x_2, y_2, r2 );
	dbg( 3, anchor[i3].id, x_3, y_3, r3 );
	dbg_out();
//	dbg( 1, 364, r1, r2, r3 );
//	dbg( 2, 370, dsq, square(x_2-x_1), square(y_2-y_1) );
//	dbg( 3, 371, gam, (square(r2+r1) - dsq), (dsq - square(r2-r1)) );
//	dbg_out();
#endif
	
	if( gam < 0 || dsq <=0 ){
		return ERROR_NONINTERSECT;
	}

	gam = sqrtd( gam );

	xa = -(square(r2) - square(r1)) * (x_2-x_1)/(2.0 * dsq) + (x_1+x_2)*0.5;
	ya = -(square(r2) - square(r1)) * (y_2-y_1)/(2.0 * dsq) + (y_1+y_2)*0.5;

	xb = (y_2-y_1) * gam/(2.0*dsq);
	yb = (x_2-x_1) * gam/(2.0*dsq);

	xt1 = xa - xb;
	xt2 = xa + xb;
	yt1 = ya + yb;
	yt2 = ya - yb;


	// disambiguate between the two points using the third node
	d1 = sqrtd( square( xt1 - x_3 ) + square( yt1 - y_3 ));
	d2 = sqrtd( square( xt2 - x_3 ) + square( yt2 - y_3 ));

	if( dabs( d1 - r3 ) < dabs( d2 - r3 ) ){
		delta_x = xt1;
		delta_y = yt1;
	}
	else {
		delta_x = xt2;
		delta_y = yt2;
	}

	return 0;

}

void report(){

	uint8_t i;
	
	for( i = 0; i < MAX_NB_CNT; i++ ){
		report_data[i].id = i;
		report_data[i].x = nb_table[i].x;
		report_data[i].y = nb_table[i].y;
		report_data[i].d = nb_table[i].d;
	}

	serial_send(
			SERIAL_PORT,
			sizeof( report_data ),
			(uint8_t *)&report_data
	);

}


double ptod( double ax, double ay, double bx, double by ){

	double A;
	A = square( bx - ax ) + square( by - ay );
	A = sqrtd( A );
	return A;

}

void *th_advert( void *arg ){
	while(1){
		pthread_mutex_lock( &mutex_lock );
		led1_on();
	
		radio_send(	BROADCAST_ADDR, RADIO_PORT, sizeof( my_info_t ), (rf_pkt*)&my_info );

		led1_off();

		pthread_mutex_unlock( &mutex_lock );		
		sleep( ADVERT_BASE_PERIOD );
	}
}
void *th_recv( void *arg ){

	my_info_t *buf;
	rf_pkt recv_buf;
	uint8_t i;

	while(1){
		if( radio_recv(RADIO_PORT, (rf_pkt*)&recv_buf ) > 0 ){
			buf = (my_info_t*)&recv_buf;
			uint8_t idx = buf->id;
			pthread_mutex_lock( &mutex_lock );
			nb_table[idx].x = buf->x;
			nb_table[idx].y = buf->y;
			nb_table[idx].d = 
				ptod( 
						nb_table[LOCAL_ADDR].x, nb_table[LOCAL_ADDR].y,
						nb_table[idx].x, nb_table[idx].y
				);

			if( nb_table[idx].life < 5 ) nb_table[idx].life++;
			pthread_mutex_unlock( &mutex_lock );
			led2_toggle();
		}
		else
			continue;
		
		uint8_t cnt = 0;
		for( i = 0; i < MAX_NB_CNT; i++ ){
			if( nb_table[i].life > 0 ) cnt++;
		}
		if( cnt >= 3 ){
			my_info.level = MTSL_LEVEL3;
		}

	}

}
#ifdef __SIMULATION_MODE__


void do_step(){
	

	uint8_t i;
	for( i = 1; i < MAX_NB_CNT; i++ ){
		nb_table[i].d = ((double)(rand() % 100)/ 100.0) + 
			ptod( 
					nb_table[0].x, nb_table[0].y,
					nb_table[i].x, nb_table[i].y
				);
	}

	anchor[0].x = nb_table[1].x;
	anchor[0].y = nb_table[1].y;
	anchor[0].d = nb_table[1].d;
	
	anchor[1].x = nb_table[2].x;
	anchor[1].y = nb_table[2].y;
	anchor[1].d = nb_table[2].d;

	anchor[2].x = nb_table[3].x;
	anchor[2].y = nb_table[3].y;
	anchor[2].d = nb_table[3].d;
	self_pos();	

}


#endif
#ifdef __DEBUG_MODE__
void dbg_out(){
	dbg_cnt++;
	debug_data[0].id = dbg_cnt;
	serial_send(
			SERIAL_PORT,
			sizeof( debug_data ),
			(uint8_t *)&debug_data
	);
}
#endif



