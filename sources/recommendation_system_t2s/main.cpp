#include "service/http_server/http_server.hpp"

int main( int argc, char* argv[] ) {

    recsys_t2s::HTTPWebServer app;
    return app.run(argc, argv);

}
