#ifndef _PIN_MACROS_H_
#define _PIN_MACROS_H_

#include <io.h>

// e.g. MAKE_PORT_MACRO(LED1, 3, 1);
#define MAKE_PORT_MACRO(__name, __port, __pin) \
	static inline void SET_PORT_OUT_##__name##_PIN()	{ P##__port##DIR |=  (1 << __pin); } \
	static inline void SET_PORT_IN_##__name##_PIN()		{ P##__port##DIR &= ~(1 << __pin); } \
	static inline void SET_PORT_HIGH_##__name##_PIN()	{ P##__port##OUT |=  (1 << __pin); } \
	static inline void SET_PORT_LOW_##__name##_PIN()	{ P##__port##OUT &= ~(1 << __pin); } \
	static inline void SET_PORT_MOD_##__name##_PIN()	{ P##__port##SEL |=  (1 << __pin); } \
	static inline void SET_PORT_IO_##__name##_PIN()		{ P##__port##SEL &= ~(1 << __pin); } \
	static inline uint8_t GET_PORT_##__name##_PIN()		{ return (P##__port##IN & (1 << __pin)); }

// e.g. MAKE_PORTINT_MACRO(CC2420_FIFOP, 1, 0);
#define MAKE_PORTINT_MACRO(__name, __port, __pin) \
	static inline void INIT_##__name##_INT_PIN()			{ SET_PORT_IN_##__name##_PIN(); SET_PORT_IO_##__name##_PIN(); } \
	static inline void ENABLE_##__name##_INT()				{ P##__port##IE |=  (1 << __pin); } \
	static inline void CLEAR_##__name##_INT()					{ P##__port##IFG &= ~(1 << __pin); } \
	static inline void DISABLE_##__name##_INT()				{ P##__port##IE &= ~(1 << __pin); CLEAR_##__name##_INT(); } \
	static inline void SET_POSEDGE_##__name##_INT()		{ \
		DISABLE_##__name##_INT(); \
		CLEAR_##__name##_INT(); \
		P##__port##IES &= ~(1 << __pin); \
		ENABLE_##__name##_INT(); \
	} \
	static inline void SET_NEGEDGE_##__name##_INT()		{ \
		DISABLE_##__name##_INT(); \
		CLEAR_##__name##_INT(); \
		P##__port##IES |= (1 << __pin); \
		ENABLE_##__name##_INT(); \
	} \

// e.g MAKE_TIMERCC_MACRO(CC2420_SFD, B, 1);
#define MAKE_TIMERCC_MACRO(__name, __port, __pin) \
	static inline void INIT_##__name##_INT_PIN()			{ SET_PORT_MOD_##__name##_PIN(); } \
	static inline void ENABLE_##__name##_INT()				{ T##__port##CCTL##__pin |= CCIE; } \
	static inline void CLEAR_##__name##_OVERFLOW()		{ T##__port##CCTL##__pin &= ~COV; } \
	static inline void CLEAR_##__name##_INT()					{ T##__port##CCTL##__pin &= ~CCIFG; CLEAR_CC2420_SFD_OVERFLOW(); } \
	static inline void DISABLE_##__name##_INT()				{ T##__port##CCTL##__pin &= ~OUT; CLEAR_##__name##_INT(); } \
	static inline void SET_POSEDGE_##__name##_INT()		{ \
		DISABLE_##__name##_INT(); \
		T##__port##CCTL##__pin = (SCS + CAP + CM_1); \
		CLEAR_##__name##_OVERFLOW(); \
		CLEAR_##__name##_INT(); \
		ENABLE_##__name##_INT(); \
	} \
	static inline void SET_NEGEDGE_##__name##_INT()		{ \
		DISABLE_##__name##_INT(); \
		T##__port##CCTL##__pin = (SCS + CAP + CM_2); \
		CLEAR_##__name##_OVERFLOW(); \
		CLEAR_##__name##_INT(); \
		ENABLE_##__name##_INT(); \
	} \

#endif

