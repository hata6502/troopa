#include <core/core.hpp>
#include <com/com.hpp>

#include <exception>
#include <emscripten/emscripten.h>
#include <sstream>
#include <string.h>
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>

using namespace std;

extern "C"
{
    int main(int argc, char **argv);
    EMSCRIPTEN_KEEPALIVE void onSimStart();
    EMSCRIPTEN_KEEPALIVE int onAudioProcess(double dt, float *buffer, int length);
    EMSCRIPTEN_KEEPALIVE void onSimEnd();
    EMSCRIPTEN_KEEPALIVE const char *export_();
}

int main(int argc, char **argv)
{
    initCom();

    Component_p speaker1 = Component_p(new Speaker());
    g_sketch.appendCom(speaker1);
    Component_p input1 = Component_p(new Input());
    g_sketch.appendCom(input1);
    Component_p sine1 = Component_p(new Sine());
    g_sketch.appendCom(sine1);

    speaker1->ins[speaker1->getIn()["sound"]]->connect(sine1->outs[sine1->getOut()["sine"]]);
    sine1->ins[sine1->getIn()["freq"]]->connect(input1->outs[input1->getOut()["value"]]);

    static_cast<Input *>(input1.get())->setValue(440.0);

    printf("speaker: %p\n", speaker1.get());
    printf("input: %p\n", input1.get());
    printf("sine: %p\n", sine1.get());

    return 0;
}

// エラー番号を作り、インタフェースの関数には try catch を作る。

EMSCRIPTEN_KEEPALIVE void onSimStart()
{
    g_sketch.onSimStart();
}

EMSCRIPTEN_KEEPALIVE int onAudioProcess(double dt, float *buffer, int length)
{
    // スキップフレームは未実装。
    try
    {
        for (int i = 0; i < length; i++)
        {
            g_spout = 0.0;
            g_spcount = 0;
            g_sketch.onChangeTime(dt);

            printf("%p\n", buffer);
            *(buffer++) = g_spcount != 0 ? g_spout / g_spcount : 0.0;
        }
    }
    catch (exception &e)
    {
        return 1;
    }

    return 0;
}

EMSCRIPTEN_KEEPALIVE void onSimEnd()
{
    g_sketch.onSimEnd();
}

EMSCRIPTEN_KEEPALIVE const char *export_()
{
    g_sketch.exportExtends();

    stringstream stream;
    {
        cereal::JSONOutputArchive archive(stream);
        archive(cereal::make_nvp("sketch", g_sketch));
    }
    return stream.str().c_str();
}
