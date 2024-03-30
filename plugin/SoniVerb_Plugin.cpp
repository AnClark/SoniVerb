/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "DistrhoPlugin.hpp"
#include "eas_reverb.h"
#include "eas_reverbdata.h"
#include "eas_types.h"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

/**
  Simple plugin to demonstrate parameter usage (including UI).
  The plugin will be treated as an effect, but it will not change the host audio.
 */
class SoniVerbPlugin : public Plugin
{
public:
    SoniVerbPlugin()
        : Plugin(EAS_PARAM_COUNT, 0, 0) // 5 parameters, 0 programs, 0 states
        , fReverbData(NULL)
    {
        DISTRHO_SAFE_ASSERT(fReverbData == NULL)

        //auto ret = ReverbInit(fReverbData);
        //DISTRHO_SAFE_ASSERT(ret == EAS_SUCCESS)
        fReverbData = ReverbInit();

        fprintf(stderr, "reverb object addr in DPF: 0x%x\n", fReverbData);
    }

    ~SoniVerbPlugin()
    {
        DISTRHO_SAFE_ASSERT(fReverbData != NULL)

        free(fReverbData);
        fReverbData = NULL;
    }

protected:
   /* --------------------------------------------------------------------------------------------------------
    * Information */

   /**
      Get the plugin label.
      A plugin label follows the same rules as Parameter::symbol, with the exception that it can start with numbers.
    */
    const char* getLabel() const override
    {
        return DISTRHO_PLUGIN_NAME;
    }

   /**
      Get an extensive comment/description about the plugin.
    */
    const char* getDescription() const override
    {
        return "Simple plugin to demonstrate parameter usage (including UI).\n\
The plugin will be treated as an effect, but it will not change the host audio.";
    }

   /**
      Get the plugin author/maker.
    */
    const char* getMaker() const override
    {
        return "AnClark Liu";
    }

   /**
      Get the plugin homepage.
    */
    const char* getHomePage() const override
    {
        return DISTRHO_PLUGIN_URI;
    }

   /**
      Get the plugin license name (a single line of text).
      For commercial plugins this should return some short copyright information.
    */
    const char* getLicense() const override
    {
        return "GPLv3";
    }

   /**
      Get the plugin version, in hexadecimal.
    */
    uint32_t getVersion() const override
    {
        return d_version(0, 1, 0);
    }

   /**
      Get the plugin unique Id.
      This value is used by LADSPA, DSSI and VST plugin formats.
    */
    int64_t getUniqueId() const override
    {
        return d_cconst('s', 'x', 'r', 'b');
    }

   /* --------------------------------------------------------------------------------------------------------
    * Init */

   /**
      Initialize the audio port @a index.@n
      This function will be called once, shortly after the plugin is created.
    */
    void initAudioPort(bool input, uint32_t index, AudioPort& port) override
    {
        // treat meter audio ports as stereo
        port.groupId = kPortGroupStereo;

        // everything else is as default
        Plugin::initAudioPort(input, index, port);
    }

   /**
      Initialize the parameter @a index.
      This function will be called once, shortly after the plugin is created.
    */
    void initParameter(uint32_t index, Parameter& parameter) override
    {
       /**
          All parameters in this plugin are similar except for name.
          As such, we initialize the common details first, then set the unique name later.
        */

       /**
          Changing parameters does not cause any realtime-unsafe operations, so we can mark them as automatable.
          Also set as boolean because they work as on/off switches.
        */
        parameter.hints = kParameterIsAutomatable;

       /**
          Set the (unique) parameter name.
          @see fParamGrid
        */
        switch (index)
        {
        case EAS_PARAM_REVERB_DRY:
            parameter.name = "Dry";
            parameter.ranges.min = (float)EAS_REVERB_DRY_MIN;
            parameter.ranges.max = (float)EAS_REVERB_DRY_MAX;
            parameter.ranges.def = (float)REVERB_DEFAULT_DRY;
            break;
        case EAS_PARAM_REVERB_WET:
            parameter.name = "Wet";
            parameter.ranges.min = (float)EAS_REVERB_WET_MIN;
            parameter.ranges.max = (float)EAS_REVERB_WET_MAX;
            parameter.ranges.def = (float)REVERB_DEFAULT_WET;
            break;
        case EAS_PARAM_REVERB_PRESET:
            parameter.name = "Preset";
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = (float)EAS_PARAM_PRESET_COUNT;
            parameter.ranges.def = 0.0f;
            {
                parameter.enumValues.count = EAS_PARAM_PRESET_COUNT;
                parameter.enumValues.restrictedMode = true;
                {
                    ParameterEnumerationValue* const values = new ParameterEnumerationValue[EAS_PARAM_PRESET_COUNT];
                    parameter.enumValues.values = values;
                    values[0].label = "Large Hall";
                    values[0].value = EAS_PARAM_REVERB_LARGE_HALL;
                    values[1].label = "Hall";
                    values[1].value = EAS_PARAM_REVERB_HALL;
                    values[2].label = "Chamber";
                    values[2].value = EAS_PARAM_REVERB_CHAMBER;
                    values[3].label = "Room";
                    values[3].value = EAS_PARAM_REVERB_ROOM;
                }
            }
            break;
        case EAS_PARAM_REVERB_BYPASS:
            parameter.name = "Bypass";
            parameter.hints |= kParameterIsBoolean;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            parameter.ranges.def = 0.0f;
            break;
        }

       /**
          Our parameter names are valid symbols except for "-".
        */
        parameter.symbol = parameter.name;
        parameter.symbol.replace('-', '_');

       /**
          Apply default param values.
        */
        setParameterValue(index, parameter.ranges.def);
    }

   /* --------------------------------------------------------------------------------------------------------
    * Internal data */

   /**
      Get the current value of a parameter.
    */
    float getParameterValue(uint32_t index) const override
    {
        DISTRHO_SAFE_ASSERT(index >= 0 && index < EAS_PARAM_COUNT)

        EAS_I32 pValue;
        ReverbGetParam(fReverbData, index, &pValue);
        return (float)pValue;
    }

   /**
      Change a parameter value.
    */
    void setParameterValue(uint32_t index, float value) override
    {
        DISTRHO_SAFE_ASSERT(index >= 0 && index < EAS_PARAM_COUNT)

        ReverbSetParam(fReverbData, index, (EAS_I32) value);
    }

   /* --------------------------------------------------------------------------------------------------------
    * Process */

   /**
      Run/process function for plugins without MIDI input.
    */
    void run(const float** inputs, float** outputs, uint32_t frames) override
    {
#if 0
        EAS_I16 pSrc[frames * 2], pDst[frames * 2];
        uint32_t current_sample_index_L = 0, current_sample_index_R = 1;

        // Must clean up output buffers, otherwise you will hear terrible noise!
        memset(pDst, 0, frames * 2 * sizeof(EAS_I16));

        for (uint32_t i = 0; i < frames; i++) {
            pSrc[current_sample_index_L] = clipToShort(inputs[0][i]);
            pSrc[current_sample_index_R] = clipToShort(inputs[1][i]);

            current_sample_index_L += 2;
            current_sample_index_R += 2;
        }

        ReverbProcess(fReverbData, pSrc, pDst, frames);

        current_sample_index_L = 0, current_sample_index_R = 1;
        for (uint32_t i = 0; i < frames; i++) {
            outputs[0][i] = shortToFloat(pDst[current_sample_index_L]);
            outputs[1][i] = shortToFloat(pDst[current_sample_index_R]);

            current_sample_index_L += 2;
            current_sample_index_R += 2;
        }
#else
        EAS_I16 pSrc[frames * 2], pDst[frames * 2];
        uint32_t current_sample_index = 0;

        // Must clean up output buffers, otherwise you will hear terrible noise!
        memset(pDst, 0, frames * 2 * sizeof(EAS_I16));

        for (uint32_t i = 0; i < frames; i++) {
            pSrc[current_sample_index] = clipToShort(inputs[0][i]);
            pSrc[current_sample_index + 1] = clipToShort(inputs[1][i]);

            current_sample_index += 2;
        }

        ReverbProcess(fReverbData, pSrc, pDst, frames);

        current_sample_index = 0;
        for (uint32_t i = 0; i < frames; i++) {
            outputs[0][i] = shortToFloat(pDst[current_sample_index]);
            outputs[1][i] = shortToFloat(pDst[current_sample_index + 1]);

            current_sample_index += 2;
        }
#endif
    }

    // -------------------------------------------------------------------------------------------------------

private:
    /**
      SoniVOX EAS reverb data.
    */
    S_REVERB_OBJECT *fReverbData;

    /**
      Clip float sample data to EAS_I16 (short).
    */
    EAS_I16 clipToShort(const float value) {
        if (value > 1.0f) {
            return 32767;
        } else if (value < -1.0f) {
            return -32767;
        } else [[likely]] {
            return static_cast<EAS_I16>(value * 32767.0f);
        }
    }

    float shortToFloat(EAS_I16 value) {
        return static_cast<float>(value) / 32767.0f;
    }

   /**
      Set our plugin class as non-copyable and add a leak detector just in case.
    */
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoniVerbPlugin)
};

/* ------------------------------------------------------------------------------------------------------------
 * Plugin entry point, called by DPF to create a new plugin instance. */

Plugin* createPlugin()
{
    return new SoniVerbPlugin();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
