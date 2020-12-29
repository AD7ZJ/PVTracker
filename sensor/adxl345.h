/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the Free Software            *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111 USA    *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 *               (c) Copyright, 2020, AD7ZJ                                *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 * Filename:     adxl345.h                                                 *
 *                                                                         *
 ***************************************************************************/

#ifndef ADXL345_H
#define ADXL345_H

#include <stdint.h>
#include <stdbool.h>

void Adxl345Init(void);

void Adxl345ReadData(int16_t* xyz);

bool Adxl345SelfTest(void);

#endif /* ADXL345_H */
