/*
 * OpenSMACX - an open source clone of Sid Meier's Alpha Centauri.
 * Copyright (C) 2013-2019 Brendan Casey
 *
 * OpenSMACX is free software: you can redistribute it and / or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenSMACX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenSMACX. If not, see <http://www.gnu.org/licenses/>.
 */
#include "stdafx.h"
#include "time.h"

void Time::JACKAL_MsgClearQueue() {
	LPMSG lpMsg = 0;
	//g_MsgStatus |= 0x3F; // ?

	if(PeekMessage(lpMsg, NULL, WM_USER + 1, WM_USER + 1, PM_REMOVE)){ // WM_CREATE|WM_USER ?
		do {
			TranslateMessage(lpMsg);
			DispatchMessage(lpMsg);
		} while(PeekMessage(lpMsg, NULL, WM_USER + 1, WM_USER + 1, PM_REMOVE));
		//g_MsgStatus = 0;
	}else{
		//g_MsgStatus = 0; // no messages
	}
}

void Time::close() {
	Time::JACKAL_MsgClearQueue();
}