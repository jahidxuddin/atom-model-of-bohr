#include "engine.h"

int main(void) {
    Engine *engine = engine_new();

    engine_run(engine);

    return 0;
}
