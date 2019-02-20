#include "core/core.hpp"
#include "com/com.hpp"

#include <stdio.h>
#include <stdexcept>
#include <emscripten/emscripten.h>

using namespace std;

extern "C"
{
    int main(int argc, char **argv)
    {
        return 0;
    }

    EMSCRIPTEN_KEEPALIVE void onSimStart()
    {
        g_sketch.onSimStart();
    }

    EMSCRIPTEN_KEEPALIVE int onAudioProcess(double dt, int length)
    {
        // スキップフレームは未実装。
        try
        {
            for (int i = 0; i < length; i++)
            {
                g_spout = 0.0;
                g_spcount = 0;
                g_sketch.onChangeTime(dt);

                /* out[i] = */ g_spout / g_spcount;
            }
        }
        catch (exception &e)
        {
            return 1;
        }

        return 0;
    }
}
