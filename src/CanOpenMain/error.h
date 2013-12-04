/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *
 *                                                       *
 *  LIVIC : Laboratoire Interractions Véhicule           * 
 *          Infrastructure Conducteur                    *
 *                       ----                            *
 *  INRETS/LIVIC : http://www.inrets.fr                  *
 *      Institut National de Recherche sur               *
 *      les Transports                                   *
 *      et leur Sécurité                                 *
 *  LCPC Laboratoire Central des Ponts et Chaussées      *
 *  Laboratoire Interractions Véhicule Infrastructure    *
 *  Conducteur                                           *
 *                                                       *
 *  Authors  : Camille BOSSARD                           *
 *             Francis DUPIN                             *
 *             Laurent Romieux                           *
 *                                                       *
 *  Contact : bossard.ca@voila.fr                        *
 *            francis.dupin@inrets.fr                    *
 *                                                       *
 *  Date    : 2003                                       *
 * This work is based on                                 *
 * -     CanOpenMatic by  Edouard TISSERANT              *
 *       http://sourceforge.net/projects/canfestival/    * 
 * -     slavelib by    Raphael Zulliger                 *
 *       http://sourceforge.net/projects/canopen/        *
 *********************************************************
 *                                                       *
 *********************************************************
 * This program is free software; you can redistribute   *
 * it and/or modify it under the terms of the GNU General*
 * Public License as published by the Free Software      *
 * Foundation; either version 2 of the License, or (at   *
 * your option) any later version.                       *
 *                                                       *
 * This program is distributed in the hope that it will  *
 * be useful, but WITHOUT ANY WARRANTY; without even the *
 * implied warranty of MERCHANTABILITY or FITNESS FOR A  *
 * PARTICULAR PURPOSE.  See the GNU General Public       *
 * License for more details.                             *
 *                                                       *
 * You should have received a copy of the GNU General    *
 * Public License along with this program; if not, write *
 * to 	The Free Software Foundation, Inc.               *
 *	675 Mass Ave                                     *
 *	Cambridge                                        *
 *	MA 02139                                         *
 * 	USA.                                             *
 *********************************************************
           File : error.h
 *-------------------------------------------------------*
 * For Microcontroler Motorola MC9S12  (HCS12)           *      
 *                                                       *
 *********************************************************/



#ifndef __ERROR__
#define __ERROR__
 

#define ERR_CAN_ADD_ID_TO_FILTER        "1      Not in init mode"
#define ERR_CAN_INIT_CLOCK              "4      Not in init mode"
#define ERR_CAN_INIT_1_FILTER           "5      Not in init mode"
#define ERR_CAN_INIT_FILTER             "6      Not in init mode" 
#define ERR_CAN_MSG_TRANSMIT            "7      No buffer free "
#define ERR_CAN_SLEEP_MODE              "8      Is in init mode"
#define ERR_CAN_SLEEP_MODE_Q            "9      Is in init mode"
#define ERR_CAN_SLEEP_WUP_MODE          "10     Is in init mode"
#define ERR_CAN0HDLRCV_STACK_FULL       "11     Stack R full"

#endif /* __ERROR__ */ 
