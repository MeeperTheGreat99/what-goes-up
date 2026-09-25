#include "accept.h"
#include <SDL3/SDL_messagebox.h>
#include <string>

static void buildMessage(exc e, std::string& msg) {
    if (msg.empty()) {
        msg += e.what();
    } else {
        msg = e.what() + (" because " + msg);
    }

    try {
        std::rethrow_if_nested(e);
    } catch (exc e) {
        buildMessage(e, msg);
    }
}

void reportException(exc e) {
    std::string msg;
    buildMessage(e, msg);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Ou shi...", msg.c_str(), nullptr);
}