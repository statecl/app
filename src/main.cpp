#include <framework.hpp>

using namespace framework;

int main(int argc, char *argv[])
{
    int port = 8080;
    if (argc > 1)
        port = std::stoi(argv[1]);

    app _app;

    _app.register_endpoint(
        http_verb_t::get, "/api/hello",
        [](const clients::http::http_request &_req) -> http_response_t {
            http_response_t _res(http_status_t::ok, _req.raw_request_.version());
            _res.set(http_field_t::server, BOOST_BEAST_VERSION_STRING);
            _res.set(http_field_t::content_type, "application/json");
            _res.body() = R"({"status":200,"message":"Hello from State \u00A9 Framework"})";
            _res.prepare_payload();
            return _res;
        });

    LOG_INFO_IMPL("Starting app on port {}", port);
    _app.run_http_service(port);
    _app.run();

    return 0;
}
