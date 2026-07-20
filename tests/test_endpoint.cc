#include <framework.hpp>
#include <gtest/gtest.h>

#include <chrono>
#include <future>
#include <thread>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>

using namespace framework;

struct app_test : ::testing::Test
{
    int port_ = 18080;

    http_response_t do_get(const std::string &_path)
    {
        boost::asio::io_context _ioc;
        boost::asio::ip::tcp::resolver _resolver(_ioc);
        boost::asio::ssl::context _ssl_ctx(boost::asio::ssl::context::tlsv12_client);
        _ssl_ctx.set_verify_mode(boost::asio::ssl::verify_none);
        boost::beast::ssl_stream<boost::beast::tcp_stream> _stream(_ioc, _ssl_ctx);

        auto const _results = _resolver.resolve("127.0.0.1", std::to_string(port_));
        boost::beast::get_lowest_layer(_stream).connect(_results);
        _stream.handshake(boost::asio::ssl::stream_base::client);

        boost::beast::http::request<boost::beast::http::string_body> _req{
            boost::beast::http::verb::get, _path, 11};
        _req.set(boost::beast::http::field::host, "127.0.0.1");
        _req.set(boost::beast::http::field::user_agent, "app_test");
        boost::beast::http::write(_stream, _req);

        boost::beast::flat_buffer _buffer;
        http_response_t _res;
        boost::beast::http::read(_stream, _buffer, _res);

        _stream.shutdown();
        return _res;
    }
};

TEST_F(app_test, hello_endpoint_returns_200)
{
    app _app;
    std::promise<void> _booted;
    std::future<void> _booted_fut = _booted.get_future();

    _app.on_boot([&](const boost::system::error_code &) { _booted.set_value(); });

    _app.register_endpoint(
        http_verb_t::get, "/api/hello",
        [](const clients::http::http_request &_req) -> http_response_t {
            http_response_t _res(http_status_t::ok, _req.raw_request_.version());
            _res.set(http_field_t::content_type, "application/json");
            _res.body() = R"({"status":200,"message":"Hello from State \u00A9 Framework"})";
            _res.prepare_payload();
            return _res;
        });

    _app.run_http_service(port_);

    std::thread _t([&]() { _app.run(); });

    ASSERT_EQ(_booted_fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);

    auto _res = do_get("/api/hello");

    EXPECT_EQ(_res.result(), http_status_t::ok);
    EXPECT_EQ(_res.body(), R"({"status":200,"message":"Hello from State \u00A9 Framework"})");

    _app.stop();
    _t.join();
}

TEST_F(app_test, hello_endpoint_has_json_content_type)
{
    app _app;
    std::promise<void> _booted;
    std::future<void> _booted_fut = _booted.get_future();

    _app.on_boot([&](const boost::system::error_code &) { _booted.set_value(); });

    _app.register_endpoint(
        http_verb_t::get, "/api/hello",
        [](const clients::http::http_request &_req) -> http_response_t {
            http_response_t _res(http_status_t::ok, _req.raw_request_.version());
            _res.set(http_field_t::content_type, "application/json");
            _res.body() = R"({"status":200,"message":"Hello from State \u00A9 Framework"})";
            _res.prepare_payload();
            return _res;
        });

    _app.run_http_service(port_);

    std::thread _t([&]() { _app.run(); });

    ASSERT_EQ(_booted_fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);

    auto _res = do_get("/api/hello");

    EXPECT_EQ(_res.at(http_field_t::content_type), "application/json");

    _app.stop();
    _t.join();
}

TEST_F(app_test, unknown_path_returns_404)
{
    app _app;
    std::promise<void> _booted;
    std::future<void> _booted_fut = _booted.get_future();

    _app.on_boot([&](const boost::system::error_code &) { _booted.set_value(); });

    _app.register_endpoint(
        http_verb_t::get, "/api/hello",
        [](const clients::http::http_request &_req) -> http_response_t {
            http_response_t _res(http_status_t::ok, _req.raw_request_.version());
            _res.set(http_field_t::content_type, "application/json");
            _res.body() = R"({"status":200,"message":"Hello from State \u00A9 Framework"})";
            _res.prepare_payload();
            return _res;
        });

    _app.run_http_service(port_);

    std::thread _t([&]() { _app.run(); });

    ASSERT_EQ(_booted_fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);

    auto _res = do_get("/api/nonexistent");

    EXPECT_EQ(_res.result(), http_status_t::not_found);

    _app.stop();
    _t.join();
}
