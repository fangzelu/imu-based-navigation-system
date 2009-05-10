#include <stdlib.h>

#ifdef __MSP430_MATH_LIB__
#include <math.h>
#endif

#include <syscall.h>
#include <pthread.h>
#include "mtsl.h"

#ifndef __SIMULATION_MODE__
pthread_mutex_t mutex_lock;
void *th_advert( void *arg );
void *th_recv( void *arg );
#endif

#ifdef __START_POSITION_MODE__
void init_position(){

	nb_table[0].x = 0.0;
	nb_table[0].y = 0.0;

	nb_table[1].x = 4.0;
	nb_table[1].y = 0.0;
	
	nb_table[2].x = 4.0;
	nb_table[2].y = 4.0;

	nb_table[3].x = 0.0;
	nb_table[3].y = 4.0;

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
	pthread_t th0, th1;
	pthread_mutex_init( &mutex_lock, NULL );

	pthread_create( &th0, NULL, (void*)th_advert, NULL );
	sleep( ADVERT_BASE_PERIOD );
	pthread_create( &th1, NULL, (void*)th_recv, NULL );
	while(1){	// this is the main thread

		led3_toggle();

		if( my_info.level < MTSL_LEVEL3 ) continue;
		pthread_mutex_lock( &mutex_lock );
		mtsl_ranging();

		set_anchors();
		self_pos();
		report();
		pthread_mutex_unlock( &mutex_lock );
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

		if( meas_d < 1.0 ) continue;

		// backup prev distance record
		nb_past[i].d = nb_table[i].d;
		nb_table[i].d = meas_d - 1.0;
		
#ifdef	__DIST_FILTER__
		dist_filter(i);
#endif
	}

}

#ifdef __DIST_FILTER__
void dist_filter( uint8_t i ){

#if 1
	double gap;
		
	gap = 0.0;

	gap = dabs( nb_past[i].d - nb_table[i].d );
	if( gap > DIST_THRES ){
		if( nb_past[i].d > nb_table[i].d ){
			nb_table[i].d = nb_past[i].d - DIST_THRES;

		}
		else{
			nb_table[i].d = nb_past[i].d + DIST_THRES;
		}
	}	

#else
	nb_table[i].d = 
		DIST_THRES * nb_past[i].d + 
		( 1.0 - DIST_THRES ) * nb_table[i].d;
#endif

}
#endif

#ifdef __POS_FILTER__
#endif

void set_anchors(){
	
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
	uint8_t cnt = 0;

	double x = 0.0;	
	double y = 0.0;

//	Backup old position

	if( LOCAL_ADDR == 0 ){
		nb_table[ LOCAL_ADDR ].x = 0.0;
		nb_table[ LOCAL_ADDR ].y = 0.0;
	}
	else if( LOCAL_ADDR == 1 ){
		nb_table[ LOCAL_ADDR ].x = nb_table[0].d;
		nb_table[ LOCAL_ADDR ].y = 0.0;
	}
	else{
		ret = trilaterate( 0, 1, 2 );

		if( ret != ERROR_NONINTERSECT ){
			x = delta_x;
			y = delta_y;
			cnt += 1;
		}
		else return;

		ret = trilaterate( 0, 2, 1 );
		if( ret != ERROR_NONINTERSECT ){
			x += delta_x;
			y += delta_y;
			cnt += 1;
		}
		else return;

		ret = trilaterate( 1, 2, 0 );
		if( ret != ERROR_NONINTERSECT ){
			x += delta_x;
			y += delta_y;
			cnt += 1;
		}
		else return;

		if( cnt > 0 ){
			x /= cnt;
			y /= cnt;

#ifdef	__POS_FILTER_SMOOTH__
		x = 
			POS_THRES * my_info.x + 
			( 1.0 - POS_THRES ) * x;

		y = 
			POS_THRES * my_info.y + 
			( 1.0 - POS_THRES ) * y;

#endif
#ifdef	__POS_FILTER__

			if( nb_past[ LOCAL_ADDR ].x > x ){
				if( nb_past[ LOCAL_ADDR ].x - x > POS_THRES ){
					x = nb_past[ LOCAL_ADDR ].x - POS_THRES;
				}
			}
			else{
				if( x - nb_past[ LOCAL_ADDR ].x > POS_THRES ){
					x = nb_past[ LOCAL_ADDR ].x + POS_THRES;
				}
			}

			if( nb_past[ LOCAL_ADDR ].y > y ){
				if( nb_past[ LOCAL_ADDR ].y - y > POS_THRES ){
					y = nb_past[ LOCAL_ADDR ].y - POS_THRES;
				}
			}
			else{
				if( y - nb_past[ LOCAL_ADDR ].y > POS_THRES ){
					y = nb_past[ LOCAL_ADDR ].y + POS_THRES;
				}
			}

#endif



			nb_table[ LOCAL_ADDR ].x = x;
			nb_table[ LOCAL_ADDR ].y = y;

		}
	}

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
			pthread_mutex_lock( &mutex_lock );
			buf = (my_info_t*)&recv_buf;
			uint8_t idx = buf->id;


			nb_past[idx].x = nb_table[idx].x;
			nb_past[idx].y = nb_table[idx].y;
			
			nb_table[idx].x = buf->x;
			nb_table[idx].y = buf->y;
			nb_table[idx].d = 
				ptod( 
						my_info.x, my_info.y,
						nb_table[idx].x, nb_table[idx].y
				);

			if( nb_table[idx].life < 5 ) nb_table[idx].life++;


			led2_toggle();
			uint8_t cnt = 0;
			for( i = 0; i < MAX_NB_CNT; i++ ){
				if( nb_table[i].life > 0 ) cnt++;
			}
			if( cnt >= 3 ){
				my_info.level = MTSL_LEVEL3;
			}
			pthread_mutex_unlock( &mutex_lock );

		}
	}

}

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



