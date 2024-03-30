/*----------------------------------------------------------------------------
 *
 * File:
 * eas_reverb.h
 *
 * Contents and purpose:
 * Contains parameter enumerations for the Reverb effect
 *
 *
 * Copyright Sonic Network Inc. 2006

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *----------------------------------------------------------------------------
 * Revision Control:
 *   $Revision: 300 $
 *   $Date: 2006-09-11 17:37:20 -0700 (Mon, 11 Sep 2006) $
 *----------------------------------------------------------------------------
*/

#ifndef _EAS_REVERB_H
#define _EAS_REVERB_H

#include "eas_types.h"

/* Forward declarations */
struct S_REVERB_OBJECT;

/* enumerated parameter settings for Reverb effect */

typedef enum
{
    EAS_PARAM_REVERB_BYPASS,
    EAS_PARAM_REVERB_PRESET,
    EAS_PARAM_REVERB_WET,
    EAS_PARAM_REVERB_DRY,
    EAS_PARAM_COUNT,
} E_REVERB_PARAMS;


typedef enum
{
    EAS_PARAM_REVERB_LARGE_HALL,
    EAS_PARAM_REVERB_HALL,
    EAS_PARAM_REVERB_CHAMBER,
    EAS_PARAM_REVERB_ROOM,
    EAS_PARAM_PRESET_COUNT,
} E_REVERB_PRESETS;

#if 0
class EAS_Reverb : public S_REVERB_OBJECT
{
public:
    EAS_Reverb();
    ~EAS_Reverb();

private:
    S_REVERB_OBJECT *pReverbData;
};
#endif

/* prototypes for effects interface */
//EAS_RESULT ReverbInit (S_REVERB_OBJECT *pReverbData);
S_REVERB_OBJECT* ReverbInit();
void ReverbProcess (S_REVERB_OBJECT *pReverbData, EAS_PCM *pSrc, EAS_PCM *pDst, EAS_I32 numSamples);
//EAS_RESULT ReverbShutdown (S_REVERB_OBJECT *pReverbData);     // // This DOES NOT work in C++!
EAS_RESULT ReverbGetParam (S_REVERB_OBJECT *pReverbData, EAS_I32 param, EAS_I32 *pValue);
EAS_RESULT ReverbSetParam (S_REVERB_OBJECT *pReverbData, EAS_I32 param, EAS_I32 value);

#endif /* _REVERB_H */
