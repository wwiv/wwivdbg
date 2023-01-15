/*
 * Copyright (C)2023 WWIV Software Services
 *
 * Licensed  under the  Apache License, Version  2.0 (the "License");
 * you may not use this  file  except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless  required  by  applicable  law  or agreed to  in  writing,
 * software  distributed  under  the  License  is  distributed on an
 * "AS IS"  BASIS, WITHOUT  WARRANTIES  OR  CONDITIONS OF ANY  KIND,
 * either  express  or implied.  See  the  License for  the specific
 * language governing permissions and limitations under the License.
 */

#ifndef INCLUDED_WWIVDBG_COMMANDS_H
#define INCLUDED_WWIVDBG_COMMANDS_H


constexpr int cmStartUserCommands = 100;
constexpr int cmChangeDrct = 102;
// View
constexpr int cmViewBreakpoints = 103;
constexpr int cmViewStack = 104;
constexpr int cmViewSource = 105;
constexpr int cmViewVars = 106;

// View
constexpr int cmDebugRun = 110;
constexpr int cmDebugTraceIn = 111;
constexpr int cmDebugStepOver = 112;
constexpr int cmDebugAttach = 113;
constexpr int cmDebugDetach = 114;

// Other debug
constexpr int cmBreakpointAdd = 120;
constexpr int cmBreakpointWindowRemove = 121;
constexpr int cmBreakpointWindowProperties = 122;

// Help
constexpr int cmHelpAbout = 130;
constexpr int cmHelpFoo = 131;

// Other commands
constexpr int cmFindWindow = 1000;

// Debug information is available
constexpr int cmDebugAvail = 2000;
constexpr int cmDebugAttached = 2001;
constexpr int cmDebugDetached = 2002;

constexpr int cmDebugSourceChanged = 2003;
constexpr int cmDebugStackChanged = 2004;
constexpr int cmDebugBreakpointsChanged = 2005;
constexpr int cmBroadcastDebugStateChanged = 2006;
constexpr int cmBreakpointsChanged = 2007;

// Debug local commands


// Other settings
constexpr int eventDebugStateChangedNormal = 0x0;
constexpr int eventDebugStateChangedNoUpdateLocation= 0x1;

#endif 
