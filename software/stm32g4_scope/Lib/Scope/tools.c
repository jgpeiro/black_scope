/*
 * tools.c
 *
 *  Created on: Jul 25, 2023
 *      Author: jgpei
 */

#include "tools.h"

uint16_t get_vmin( uint16_t *buffer, uint16_t len )
{
	uint16_t i = 0;
	uint16_t value = buffer[0];
	for( i = 0 ; i < len ; i++ )
	{
		if( buffer[i] < value )
		{
			value = buffer[i];
		}
	}
	return value;
}

uint16_t get_vmax( uint16_t *buffer, uint16_t len )
{
	uint16_t i = 0;
	uint16_t value = buffer[0];
	for( i = 0 ; i < len ; i++ )
	{
		if( buffer[i] > value )
		{
			value = buffer[i];
		}
	}
	return value;
}

uint16_t get_vavg( uint16_t *buffer, uint16_t len )
{
	uint16_t i = 0;
	uint32_t value = 0;

	for( i = 0 ; i < len ; i++ )
	{
		value += buffer[i];
	}

	return value/len;
}

uint16_t get_period( uint16_t *buffer, uint16_t len, uint16_t mx, uint16_t mn, uint16_t avg )
{
	uint16_t t0 = 0;
	uint16_t t1 = 0;
	uint16_t t2 = 0;
	uint16_t i = 0;

	uint16_t thr1 = mn + 3*(mx - mn)/4;
	uint16_t thr2 = mn + 1*(mx - mn)/4;

	if( abs(mx - mn) < 20 )
	{
		return 0;
	}

	for( i = 0 ; i < len-1 ; i++ )
	{
		if( buffer[i] > thr1 && buffer[i+1] <= thr1 )
		{
			t0 = i;
			for( i = t0+1 ; i < len-1 ; i++ )
			{
				if( buffer[i] < thr2 && buffer[i+1] >= thr2 )
				{
					t1 = i;
					for( i = t1+1 ; i < len-1 ; i++ )
					{
						if( buffer[i] > thr1 && buffer[i+1] <= thr1 )
						{
							t2 = i;
							return t2-t0;
						}
					}
				}
			}
		}
	}

	return 0;

}

uint16_t get_duty( uint16_t *buffer, uint16_t len, uint16_t mx, uint16_t mn, uint16_t avg )
{
	uint16_t t0 = 0;
	uint16_t t1 = 0;
	uint16_t t2 = 0;
	uint16_t i = 0;

	uint16_t thr1 = mn + 3*(mx - mn)/4;
	uint16_t thr2 = mn + 1*(mx - mn)/4;

	if( abs(mx - mn) < 20 )
	{
		return 0;
	}

	for( i = 0 ; i < len-1 ; i++ )
	{
		if( buffer[i] > thr1 && buffer[i+1] <= thr1 )
		{
			t0 = i;
			for( i = t0+1 ; i < len-1 ; i++ )
			{
				if( buffer[i] < thr2 && buffer[i+1] >= thr2 )
				{
					t1 = i;
					for( i = t1+1 ; i < len-1 ; i++ )
					{
						if( buffer[i] > thr1 && buffer[i+1] <= thr1 )
						{
							t2 = i;
							if( (t1-t0) == 0 )
							{
								return 0;
							}
							else
							{
								return 100-(100*(t1-t0))/(t2-t0);
							}
						}
					}
				}
			}
		}
	}

	return 0;
}
