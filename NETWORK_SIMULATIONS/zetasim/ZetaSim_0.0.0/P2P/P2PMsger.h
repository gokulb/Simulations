/* P2PMsger.h
 * Jing Wang
 * June, 2010

ZetaSim - A NS2 based Simulation suit for Zeta Protocol
Copright (C) 2010 Jing Wang

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

Jing Wang
Department of Electronics and Information Engineering
Huazhong University of Science and Technology
Wuhan, Hubei, 430074
hbhzwj@gmail.com

*/
#ifndef P2P_MSGER_H
#define P2P_MSGER_H

#include "TypeDefine.h"

class P2PApp;
class P2PMsger : public TclObject
{
//class P2PMsger : public Application {
public:
        P2PMsger() : TclObject() {}

        //void process_data(int, AppData*) {};

        // functions called by P2PApp;
        //virtual void send(NodeAddr_t addr, std::string msg) = 0;
protected:
        virtual int command ( int argc, const char*const* argv ) {
                return TclObject::command ( argc, argv );
        }
};


#endif