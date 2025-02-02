/* ----------------------------------------------------------------------------
   SchedMCore - A MultiCore Scheduling Framework
   Copyright (C) 2009-2011, ONERA, Toulouse, FRANCE - LIFL, Lille, FRANCE

   This file is part of Prelude

   Prelude is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation ; either version 2 of
   the License, or (at your option) any later version.

   Prelude is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY ; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this program ; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA
  ---------------------------------------------------------------------------- */

/*

  This program is part of the TACLeBench benchmark suite.
  Version V 1.9

  Name: ros_th1

  Author: Dustin Green and others

  Function: executes the first thread of original threads.c

  Source: https://svn.onera.fr/schedmcore/branches/ROSACE_CaseStudy/c_posix_implementation/

  Original name: threads, rosace

  Changes: union of rosace.c and threads.c

  License: see above

*/

extern "C" {
#include "thread1/types.h"
#include "thread1/assemblage_includes.h"
#include "thread1/assemblage.h"
#include "thread1/common.h"
#include "gem5/m5ops.h"
}

#include <thread>

// Return variable
int ros_th1_check;


/*
  Forward declaration of functions
*/

void* ros_th1_thread1( void* arg );
int ros_th1_run_rosace( void );
int main( void );
void ros_th1_init( void );
void ros_th1_main( void );
int ros_th1_return( void );


/*
  Initialization function
*/

void ros_th1_init( void )
{
    ros_th1_check = -1;
    rosace_init();
}


/*
  Return function
*/

int ros_th1_return( void )
{
    return ros_th1_check;
}


/*
  Main functions
*/

void _Pragma( "entrypoint" ) ros_th1_main( void )
{
    ros_th1_check = ros_th1_run_rosace( );
}


int ros_th1_run_rosace( void ){
    // Set first command
    ROSACE_update_altitude_command( 11000.0 );

    // Exit
    ros_th1_thread1( NULL );
    return 0;
}


void* ros_th1_thread1( void* arg ) {
    uint64_t mystep_simu = step_simu;

    _Pragma( "loopbound min 60000 max 60000" )
    while( step_simu<max_step_simu ) {
        // --- 200 Hz ---
        CALL( ENGINE );
        // --- End 200 Hz ---

        // --- 100 Hz ---
        if( mystep_simu%2 == 0 ) {
            CALL( VZ_FILTER );
        }
        // --- End 100 Hz ---

        // --- 10 Hz ---
        if( mystep_simu%20 == 0 )
            CALL( VA_C0 );
        // --- End 10 Hz ---

        // --- 50 Hz ---
        if( mystep_simu%4 == 0 ) {
            CALL( VA_CONTROL );
        }
        if( mystep_simu%4 == 3 ) {
            CALL( DELTA_TH_C0 );
        }
        // --- End 50 Hz ---

        step_simu    = step_simu + 1;
        outs.t_simu += 5;

        // Print output
        copy_output_vars( &outs, step_simu );
        if ( step_simu%10 )
            ROSACE_write_outputs( &outs );

        mystep_simu++;
    }
    return NULL;
}

// Return variable
int ros_th2_check;


/*
  Forward declaration of functions
*/

void* ros_th2_thread2( void* arg );
int ros_th2_run_rosace( void );
int main( void );
void ros_th2_init( void );
void ros_th2_main( void );
int ros_th2_return( void );


/*
  Initialization function
*/

void ros_th2_init( void )
{
    ros_th2_check = -1;
    rosace_init();
}


/*
  Return function
*/

int ros_th2_return( void )
{
    return ros_th2_check;
}


/*
  Main functions
*/

void _Pragma( "entrypoint" ) ros_th2_main( void )
{
    ros_th2_check = ros_th2_run_rosace( );
}


int ros_th2_run_rosace( void ){
    // Set first command
    ROSACE_update_altitude_command( 11000.0 );

    // Exit
    ros_th2_thread2( NULL );
    return 0;
}



void* ros_th2_thread2( void* arg ) {
    uint64_t mystep_simu = step_simu;

    _Pragma( "loopbound min 60000 max 60000" )
    while( mystep_simu<max_step_simu ) {           //step_simu zu mystep_simu geändert
        // --- 200 Hz ---
        CALL( ELEVATOR );
        CALL( AIRCRAFT_DYN );
        // --- End 200 Hz ---

        // --- 100 Hz ---
        if( mystep_simu%2 == 0 ) {
            CALL(H_FILTER);
        }
        // --- End 100 Hz ---

        // --- 10 Hz ---
        if( mystep_simu%20 == 0 )
            CALL(H_C0);
        // --- End 10 Hz ---

        // --- 50 Hz ---
        if( mystep_simu%4 == 0 ) {
            CALL(ALTI_HOLD);
            CALL(VZ_CONTROL);
        }

        if( mystep_simu%4 == 3 ) {
            CALL(DELTA_E_C0);
        }
        // --- End 50 Hz ---

        mystep_simu++;
    }
    return NULL;
}


int main( void )
{
    max_step_simu = 300*200;

    ros_th1_init();
    ros_th2_init();
    m5_reset_stats(0, 0);
    std::thread t1([]{
        ros_th1_main();
    });

    std::thread t2([]{
        ros_th2_main();
    });

    t1.join();
    t2.join();

    m5_dump_stats(0, 0);

    return ros_th1_return() && ros_th2_return();

    ros_th1_init();
    m5_reset_stats(0, 0);
    ros_th1_main();
    m5_dump_stats(0, 0);

    printf("done th1\n");

    return ros_th1_return();
}
