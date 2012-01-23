/* File: Header File for a new P2PApp for NS.
 * Author: Jing Wang
 * Date: 2010-5-8, today is my birthday, happy birthday to myself.
 */

#ifndef NS_P2PAPP_H
#define NS_P2PAPP_H

#include "agent.h"
#include "tclcl.h"
#include "packet.h"

#include <stdio.h>
#include <stdarg.h>

#include "TypeDefine.h"



class P2PApp : public Application
{
public:
        P2PApp ( NodeAddr_t addr ) {
                bind ( "isDebug", &debugLevel );
                srand ( ( unsigned ) time ( NULL ) );
        }
        P2PApp () {
                bind ( "isDebug", &debugLevel );
                srand ( ( unsigned ) time ( NULL ) );
        }
        virtual ~P2PApp () {}

        // User Command Interface
        virtual void join() = 0;
        virtual void leave() = 0;
        virtual void search ( FileTag_t tag ) = 0;

        // Debugging output for all App. By default, print to stdout
        void debug ( DebugLevel level, const char *fmt, ... ) {
                if ( level > debugLevel )
                        return;
                switch ( level ) {
                case ERROR:
                        printf ( "[ERROR]" );
                        break;
                case STAT:
                        printf ( "[STAT]" );
                        break;
                case WARNING:
                        printf ( "[WARNING]" );
                        break;
                case INFO:
                        printf ( "[INFO]" );
                        break;
                case DEBUG:
                        printf ( "[DEBUG]" );
                        break;
                }
                printf ( " "TIME_FORMAT"s ", Scheduler::instance().clock() );
                va_list ap;
                va_start ( ap, fmt );
                vprintf ( fmt, ap );
        }
        int debugLevel;
protected:
        virtual int command ( int argc, const char*const* argv ) {
                return Application::command ( argc, argv );
        };
};

#endif
